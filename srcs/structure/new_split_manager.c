/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_split_manager.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 20:10:15 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 20:45:04 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int new_free_split_prompt(t_block *block)
{
	int i;

	i = 0;
	while (i < block->op_count)
	{
		if (block->child_prompts[i])
			token_list_destroy(&block->child_prompts[i]);
		i++;
	}
    if (block->child_prompts)
        ft_free_set_null(&block->child_prompts);
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
	int found_par;
	t_token_node *cur;

    count = 0;
	open_par = 0;
	found_par = 0;
	cur = block->prompt->head;
    while (cur)
    {
        if (cur->type == T_OPEN_PAR)
		{
			found_par = 1;
			open_par++;
		}
		if (cur->type == T_CLOSE_PAR)
			open_par--;
		if (!open_par && token_is_big_operator(cur))
			count++;
		cur = cur->next;
    }
	if (!count && !found_par)
		block->is_cmd = 1;
    return (count);
}



int new_setup_split_prompt_struct(t_block *block)
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


int new_split_prompt(t_block *block)
{
    int             i;
    t_split_prompt  split;

    if (!new_setup_split_prompt_struct(block))
        return (0);
    new_split_children_and_operators(block);
        return (new_free_split_prompt(&split));
    if (split.op_count == 0)
    {
        i = 0;
        while (split.prompt_copy[i] && split.prompt_copy[i] != '(')
            i++;
        if (!split.prompt_copy[i])
            block->is_cmd = 1;
        else
        {
            //printf("split [%s] must be forked cause it has parenthesis\n", split.prompt_orig);
            split.parenthesis_fork = 1;
            if (!new_split_extract_redirections(&split) \
            || !new_split_unnecessary_parenthesis(&split))
                return (new_free_split_prompt(&split));
            //printf("split [%s] has unnecessary? %d\n", split.prompt_orig, split.has_unnecessary_parenthesis);
        }
    }
    return (1);
}
