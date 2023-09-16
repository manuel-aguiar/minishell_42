/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_manager.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 20:10:15 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 22:10:26 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	destroy_child_prompts(t_block *block)
{
	int i;

	while (i < block->op_count + 1)
	{
		if (block->child_prompts[i])
			token_list_destroy(&block->child_prompts[i]);
		i++;
	}
	ft_free_set_null(&block->child_list);
}

int free_split_prompt(t_block *block)
{
	destroy_child_prompts(block);
	if (block->child_list)
		ft_free_set_null(&block->child_list);
    if (block->op_id)
        ft_free_set_null(&block->op_id);
    if (block->child_pids)
        ft_free_set_null(&block->child_pids);
    if (block->child_exit_status)
        ft_free_set_null(&block->child_exit_status);
    if (block->io_files)
        vdmlist_destroy(&block->io_files, destroy_redir);
    return (0);
}

int	token_is_big_operator(t_token_node *token);

int check_if_cmd_and_count_operators(t_block *block)
{
	int open_par;
    int count;
	t_token_node *cur;

    count = 0;
	open_par = 0;
	cur = block->prompt->head;
    while (cur)
    {
        if (cur->type == T_OPEN_PAR)
		{
			block->must_subshell = 1;
			open_par++;
		}
		if (cur->type == T_CLOSE_PAR)
			open_par--;
		if (!open_par && token_is_big_operator(cur))
			count++;
		cur = cur->next;
    }
	if (!count && !block->must_subshell)
		block->is_cmd = 1;
    return (count);
}



int setup_split_prompt_struct(t_block *block)
{
	int	i;

    block->op_count = check_if_cmd_and_count_operators(block);
	if (!block->is_cmd)
	{
		block->op_id = malloc(sizeof(*(block->op_id)) * block->op_count);
		block->child_prompts = ft_calloc((block->op_count + 2), sizeof(*(block->child_prompts)));
		block->child_list = ft_calloc((block->op_count + 2), sizeof(*(block->child_list)));
		block->child_pids = ft_calloc(block->op_count + 1, sizeof(*(block->child_pids)));
		block->child_exit_status = ft_calloc(block->op_count + 1, sizeof(*(block->child_exit_status)));
		if (!block->op_id || !block->child_prompts || !block->child_pids)
		{
			perror_msg("malloc");
			return (new_free_split_prompt(block));
		}
		i = 0;
		while (i < block->op_count + 1)
		{
			block->child_prompts[i] = token_list_new();
			if (!block->child_prompts[i])
				return (new_free_split_prompt(block));
			i++;
		}
	}
    return (1);
}

int	token_is_big_operator(t_token_node *token)
{
	if (!token)
		return (0);
	if (token->type == T_OP_PIPE \
	|| token->type == T_OP_OR || token->type == T_OP_AND)
		return (1);
	return (0);
}

int	token_is_redirection(t_token_node *token)
{
	if (!token)
		return (0);
	if (token->type == T_INDIR_HD \
	|| token->type == T_INDIR_OPEN \
	|| token->type == T_OUTDIR_APPEND
	|| token->type == T_OUTDIR_TRUNC)
		return (1);
	return (0);
}

int split_children_and_operators(t_block *block)
{
	t_token_node	*cur;
    int all;
    int i;

    all = 0;
    i = 0;
	cur = block->prompt->head;
    while (all < block->op_count)
    {
		i = 0;
        while (cur && !token_is_big_operator(cur))
		{
			cur = cur->next;
			i++;
		}
		if (cur)
			token_list_move_top_to_new(block->child_prompts[all], \
			block->prompt, cur->prev, i);
		block->op_id[all] = cur->type;
		token_list_del_head(block->prompt);
		all++;
    }
    return (1);
}

int split_extract_redirections(t_block *block)
{
    int     i;
	t_token_node *last;

	last = block->prompt->tail;
	while (last && !token_is_redirection(last->type))
		last = last->prev;
	block->io_files = token_list_new();
	if (!block->io_files)
		return (0);
	block->io_files->head = last->next;
	last->next->prev = NULL;
	last->next = NULL;
	block->prompt->tail = last;
    return (1);
}

int	cmd_extract_redirections(t_block *block)
{
	t_token_node *cur;

	cur = block->prompt->head;
	while (cur)
	{
		if (token_is_redirection(cur->type))
		{
			if (!block->io_files)
				block->io_files = token_list_new();
			if (!block->io_files)
				return (0);
			cur = move_node_to_list_and_retrive_next(block->io_files, block->prompt, cur);
		}
		else
			cur = cur->next;
	}
}

int remove_corner_parenthesis_and_arithmatic(t_block *block)
{
	int	remove_corner;

	if (block->prompt->len < 4)
		return (0);
	if (block->prompt->head->type == T_OPEN_PAR \
	&& block->prompt->tail->type == T_CLOSE_PAR)
		remove_corner = 1;
	if (remove_corner \
	&& block->prompt->head->next->type == T_OPEN_PAR \
	&& block->prompt->tail->prev->type == T_CLOSE_PAR)
		block->has_arithmatic_parenthesis = 1;
	if (remove_corner)
	{
		token_list_del_head(block->prompt);
		token_list_del_tail(block->prompt);
	}
}

int split_prompt(t_block *block)
{
    int             i;
	int				has_parenthesis;
    t_split_prompt  split;

    if (!setup_split_prompt_struct(block))
        return (free_split_prompt(block));
    split_children_and_operators(block);
    if (!block->op_count && block->must_subshell)
    {
        if (!split_extract_redirections(block))
			return (free_split_prompt(block));
		remove_corner_parenthesis_and_arithmatic(block);
    }
	if (block->is_cmd && !cmd_extract_redirections(block))
		return (0);
    return (1);
}
