/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 13:36:27 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 18:40:41 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	manage_infile(t_block *block, int index)
{
	t_token_node	*redir;

	redir = block->io_files->head;
	close_in_fds(block);
	if (redir->type == T_INDIR_HD)
	{
		if (index == block->here_doc_index)
			block->final_in = block->here_doc_fd;
	}
	else
	{
		if (block->here_doc && index > block->here_doc_index)
		{
			close(block->here_doc_fd);
			unlink(block->here_doc);
			ft_free_set_null(&block->here_doc);
		}
		block->final_in = open(redir->text, O_RDWR);
		if (block->final_in == -1)
			return (perror_msg_func(block, redir->text, CODE_OPEN, 1));
	}
	token_list_del_head(block->io_files);
	return (1);
}

static int	manage_outfile(t_block *block)
{
	t_token_node	*redir;

	redir = block->io_files->head;
	close_out_fds(block);
	if (redir->type == T_OUTDIR_TRUNC)
		block->final_out = open(redir->text, O_CREAT \
						| O_RDWR | O_TRUNC, 0644);
	else
		block->final_out = open(redir->text, O_CREAT \
						| O_RDWR | O_APPEND, 0644);
	if (block->final_out == -1)
		return (perror_msg_func(block, redir->text, CODE_OPEN, 1));
	token_list_del_head(block->io_files);
	return (1);
}

static int	take_inherited_fds(t_block *block)
{
	if (!block->my_manager)
	{
		block->final_out = block->ms->outfd;
		block->final_in = block->ms->infd;
		return (1);
	}
	block->final_out = block->my_manager->final_out;
	block->final_in = block->my_manager->final_in;
	if (block->my_id < block->my_manager->op_count \
	&& block->my_manager->op_id \
	&& block->my_manager->op_id[block->my_id] == T_OP_PIPE)
		block->final_out = block->my_manager->pipefd[1];
	if (block->my_id > 0 \
	&& block->my_manager->op_id \
	&& block->my_manager->op_id[block->my_id - 1] == T_OP_PIPE)
		block->final_in = block->my_manager->prev_pipe[0];
	return (1);
}

int	prepare_redirections(t_block *block)
{
	int	i;
	int	type;
	int	success;

	take_inherited_fds(block);
	if (!block->io_files || block->has_arithmatic_parenthesis)
		return (1);
	success = 1;
	i = 0;
	while (block->io_files->head && success)
	{
		type = block->io_files->head->type;
		success = manage_io_expansion(block);
		if (!success)
			break ;
		if (type == T_OUTDIR_TRUNC || type == T_OUTDIR_APPEND)
			success = manage_outfile(block);
		else
			success = manage_infile(block, i);
		i++;
	}
	token_list_destroy(&block->io_files);
	return (success);
}
