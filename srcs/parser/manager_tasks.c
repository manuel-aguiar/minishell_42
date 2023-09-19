/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager_tasks.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:04:12 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 10:10:44 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int manager_gets_workers_and_operators(t_block *manager)
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
			token_list_move_top_to_new(manager->worker_tasks[all], \
			manager->prompt, cur->prev, i);
			manager->op_id[all] = manager->prompt->head->type;
			token_list_del_head(manager->prompt);
			cur = manager->prompt->head;
			//printf("printing args of child %d\n", all);
			//token_list_head_print(manager->worker_tasks[all], print_token_args);
			//printf("finished printing args of child %d, list len is %ld\n", all, manager->worker_tasks[all]->len);
			i = 0;
			all++;

		}
		else
		{
			i++;
			cur = cur->next;
		}
    }
	token_list_destroy(&manager->worker_tasks[all]);
	//printf("all is %d\n", all);
	manager->worker_tasks[all] = manager->prompt;
	manager->prompt = NULL;
	//printf("printing args of child %d\n", all);
	//token_list_head_print(manager->worker_tasks[all], print_token_args);
	//printf("finished printing args of child %d, list len is %ld\n", all, manager->worker_tasks[all]->len);
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



int manager_subshell_and_arithmatic(t_block *manager)
{
	int	remove_corner;

	t_token_node *cur;
	remove_corner = 0;
	if (manager->prompt->len >= 2)
	{
		remove_corner = (manager->prompt->head->type == T_OPEN_PAR \
		&& manager->prompt->tail->type == T_CLOSE_PAR);
	}
	if (manager->prompt->len >= 4 && remove_corner \
	&& manager->prompt->head->next->type == T_OPEN_PAR \
	&& manager->prompt->tail->prev->type == T_CLOSE_PAR)
		manager_check_arithmatic_parenthesis(manager);
	if (remove_corner)
	{
		token_list_del_head(manager->prompt);
		token_list_del_tail(manager->prompt);
	}
	return (1);
}

int	manager_check_arithmatic_parenthesis(t_block *manager)
{
	t_token_node	*cur;
	int				open_par;

	open_par = 0;
	cur = manager->prompt->head->next;
	while (cur)
	{
		open_par += (cur->type == T_OPEN_PAR);
		open_par -= (cur->type == T_CLOSE_PAR);
		if (open_par == 0)
		{
			manager->has_arithmatic_parenthesis = \
			(cur->next && cur->next->type == T_CLOSE_PAR);
			return (1);
		}
		cur = cur->next;
	}
	return (0);
}

void	manager_destroy_worker_tasks(t_block *manager)
{
	int i;

	if (!manager->worker_tasks)
		return ;
	i = 0;
	while (i < manager->op_count + 1)
	{
		if (manager->worker_tasks[i])
			token_list_destroy(&manager->worker_tasks[i]);
		i++;
	}
	ft_free_set_null(&manager->worker_tasks);
}
