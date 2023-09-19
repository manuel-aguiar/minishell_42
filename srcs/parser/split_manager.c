/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_manager.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 20:10:15 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/18 17:06:31 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	destroy_worker_prompts(t_block *manager)
{
	int i;

	if (!manager->worker_prompts)
		return ;
	i = 0;
	while (i < manager->op_count + 1)
	{
		if (manager->worker_prompts[i])
			token_list_destroy(&manager->worker_prompts[i]);
		i++;
	}
	ft_free_set_null(&manager->worker_prompts);
}

int free_split_prompt(t_block *manager)
{
	destroy_worker_prompts(manager);
	if (manager->prompt)
		token_list_destroy(&manager->prompt);
	if (manager->worker_list)
		ft_free_set_null(&manager->worker_list);
    if (manager->op_id)
        ft_free_set_null(&manager->op_id);
    if (manager->worker_pids)
        ft_free_set_null(&manager->worker_pids);
    if (manager->worker_exit_status)
        ft_free_set_null(&manager->worker_exit_status);
    if (manager->io_files)
        token_list_destroy(&manager->io_files);
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
		block->is_worker = 1;
    return (count);
}



int setup_split_prompt_struct(t_block *block)
{
	int	i;

    block->op_count = check_if_cmd_and_count_operators(block);
	if (!block->is_worker)
	{
		//printf("i'm not a cmd, must subshell %d\n", block->must_subshell);
		block->op_id = malloc(sizeof(*(block->op_id)) * block->op_count);
		block->worker_prompts = ft_calloc((block->op_count + 2), sizeof(*(block->worker_prompts)));
		block->worker_list = ft_calloc((block->op_count + 2), sizeof(*(block->worker_list)));
		block->worker_pids = ft_calloc(block->op_count + 1, sizeof(*(block->worker_pids)));
		block->worker_exit_status = ft_calloc(block->op_count + 1, sizeof(*(block->worker_exit_status)));
		ft_memset(block->worker_exit_status, -1, (block->op_count + 1) * sizeof(*(block->worker_exit_status)));
		if (!block->op_id || !block->worker_prompts || !block->worker_pids)
		{
			perror_msg("malloc");
			return (free_split_prompt(block));
		}
		i = 0;
		while (i < block->op_count + 1)
		{
			block->worker_exit_status[i] = -1;
			block->worker_prompts[i] = token_list_new();
			if (!block->worker_prompts[i])
				return (free_split_prompt(block));
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

int manager_gets_workers_and_orders(t_block *manager)
{
	t_token_node	*cur;
    int all;
    int i;
	int	open_par;

	i = 0;
    all = 0;
	open_par = 0;
	cur = manager->prompt->head;
    while (all < manager->op_count)
    {
		open_par += (cur->type == T_OPEN_PAR);
		open_par -= (cur->type == T_CLOSE_PAR);
		if (cur && token_is_big_operator(cur) && !open_par)
		{
			token_list_move_top_to_new(manager->worker_prompts[all], \
			manager->prompt, cur->prev, i);
			manager->op_id[all] = manager->prompt->head->type;
			token_list_del_head(manager->prompt);
			cur = manager->prompt->head;
			//printf("printing args of child %d\n", all);
			//token_list_head_print(block->worker_prompts[all], print_token_args);
			//printf("finished printing args of child %d, list len is %ld\n", all, block->worker_prompts[all]->len);
			i = 0;
			all++;

		}
		else
		{
			i++;
			cur = cur->next;
		}
    }
	token_list_destroy(&manager->worker_prompts[all]);
	//printf("all is %d\n", all);
	manager->worker_prompts[all] = manager->prompt;
	manager->prompt = NULL;
	//printf("printing args of child %d\n", all);
	//token_list_head_print(block->worker_prompts[all], print_token_args);
	//printf("finished printing args of child %d, list len is %ld\n", all, block->worker_prompts[all]->len);
    return (1);
}

int manager_extract_redirections(t_block *manager)
{
    int     i;
	t_token_node *last;

	if (!manager->prompt)
		return (1);
	last = manager->prompt->tail;
	while (last && !token_is_redirection(last))
		last = last->prev;
	if (!token_is_redirection(last))
		return (1);
	manager->io_files = token_list_new();
	if (!manager->io_files)
		return (0);
	manager->io_files->head = last->next;
	last->next->prev = NULL;
	last->next = NULL;
	manager->prompt->tail = last;
    return (1);
}

int	worker_extract_redirections(t_block *worker)
{
	t_token_node *cur;

	//printf("cmd extraction\n");
	cur = worker->prompt->head;
	while (cur)
	{
		if (token_is_redirection(cur))
		{
			if (!worker->io_files)
				worker->io_files = token_list_new();
			if (!worker->io_files)
				return (0);
			cur = move_node_to_list_and_retrive_next(worker->io_files, worker->prompt, cur);
		}
		else
			cur = cur->next;
	}
	//printf("printing redirs\n");
	//token_list_head_print(block->io_files, print_token);
	//printf("printing cmd args\n");
	//token_list_head_print(block->prompt, print_token);
	//printf("\n");
	return (1);
}

int	check_arithmatic_parenthesis(t_block *block)
{
	t_token_node	*cur;
	int				open_par;

	open_par = 0;
	cur = block->prompt->head->next;
	while (cur)
	{
		open_par += (cur->type == T_OPEN_PAR);
		open_par -= (cur->type == T_CLOSE_PAR);
		if (open_par == 0)
		{
			block->has_arithmatic_parenthesis = \
			(cur->next && cur->next->type == T_CLOSE_PAR);
			return (1);
		}
		cur = cur->next;
	}
	return (0);
}

int remove_corner_parenthesis_and_arithmatic(t_block *block)
{
	int	remove_corner;

	t_token_node *cur;
	remove_corner = 0;
	if (block->prompt->len >= 2)
	{
		remove_corner = (block->prompt->head->type == T_OPEN_PAR \
		&& block->prompt->tail->type == T_CLOSE_PAR);
	}
	if (block->prompt->len >= 4 && remove_corner \
	&& block->prompt->head->next->type == T_OPEN_PAR \
	&& block->prompt->tail->prev->type == T_CLOSE_PAR)
		check_arithmatic_parenthesis(block);
	if (remove_corner)
	{
		token_list_del_head(block->prompt);
		token_list_del_tail(block->prompt);
	}
	return (1);
}

int split_prompt(t_block *block)
{
    int             i;
	int				has_parenthesis;

	//printf("i'm here \n");
	//token_list_head_print(block->prompt, print_token_args);
    if (!setup_split_prompt_struct(block))
        return (free_split_prompt(block));
	//token_list_head_print(block->prompt, print_token_args);
	//printf("block op count %d\n", block->op_count);
	if (!block->is_worker && block->op_count > 0)
    	manager_gets_workers_and_orders(block);
	//token_list_head_print(block->prompt, print_token_args);
    if (!block->op_count && block->must_subshell)
    {
        if (!manager_extract_redirections(block))
			return (free_split_prompt(block));
		remove_corner_parenthesis_and_arithmatic(block);
		token_list_destroy(&block->worker_prompts[0]);
		block->worker_prompts[0] = block->prompt;
		block->prompt = NULL;
    }
	//printf("i am command %d, [%s]\n", block->is_worker, block->prompt->head->text);
	if (block->is_worker && !worker_extract_redirections(block))
		return (0);
	//exit(0);
    return (1);
}
