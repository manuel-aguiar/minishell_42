/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block_setup.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 00:11:09 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/21 13:18:40 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	block_init_continued(t_block *block, int my_id);
static void	block_destroy_continued(t_block *block);

t_block	*block_init(t_ms *ms, t_block *manager, t_token_list *prompt, int my_id)
{
	t_block	*new;

	new = malloc(sizeof(*new));
	if (!new)
		return (perror_msg_ptr("malloc", NULL));
	new->ms = ms;
	new->my_manager = manager;
	new->i_am_forked = 0;
	new->prompt = prompt;
	if (manager)
	{
		new->my_manager->worker_tasks[my_id] = NULL;
		new->my_manager->worker_list[my_id] = new;
		new->my_level = new->my_manager->my_level + 1;
	}
	else
	{
		ms->prompt = NULL;
		ms->first = new;
		new->my_level = 0;
	}
	block_init_continued(new, my_id);
	return (new);
}

static void	block_init_continued(t_block *block, int my_id)
{
	block->worker_tasks = NULL;
	block->worker_list = NULL;
	block->worker_pids = NULL;
	block->worker_exit_status = NULL;
	block->op_count = 0;
	block->op_id = NULL;
	block->is_worker = 0;
	block->has_arithmatic_parenthesis = 0;
	block->must_subshell = 0;
	block->pipefd[0] = -1;
	block->pipefd[1] = -1;
	block->prev_pipe[0] = -1;
	block->prev_pipe[1] = -1;
	block->my_status = 0;
	block->cmd = NULL;
	block->cmd_args = NULL;
	block->io_files = NULL;
	block->final_in = -1;
	block->final_out = -1;
	block->here_doc = NULL;
	block->here_doc_fd = -1;
	block->here_doc_index = -1;
	block->my_id = my_id;
}

void	block_destroy(void *og_block)
{
	t_block	*block;

	block = (t_block *)og_block;
	if (!block)
		return ;
	if (block->worker_list)
		ft_free_sizemat_null(&block->worker_list, block->op_count + 1,
			block_destroy);
	if (block->worker_tasks)
		ft_free_charmat_null(&block->worker_tasks, free);
	if (block->worker_pids)
		ft_free_set_null(&block->worker_pids);
	if (block->worker_exit_status)
		ft_free_set_null(&block->worker_exit_status);
	if (block->op_id)
		ft_free_set_null(&block->op_id);
	if (block->my_manager)
		block->my_manager->worker_list[block->my_id] = NULL;
	else
		block->ms->first = NULL;
	block_destroy_continued(block);
	free(block);
}

static void	block_destroy_continued(t_block *block)
{
	if (block->prompt)
		token_list_destroy(&block->prompt);
	if (block->io_files)
		token_list_destroy(&block->io_files);
	if (block->here_doc)
	{
		close(block->here_doc_fd);
		unlink(block->here_doc);
		ft_free_set_null(&block->here_doc);
	}
	if (block->cmd)
		ft_free_set_null(&block->cmd);
	if (block->cmd_args)
		ft_free_charmat_null(&block->cmd_args, free);
}
