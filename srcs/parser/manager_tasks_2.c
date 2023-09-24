/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager_tasks_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 14:04:07 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 14:05:23 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	send_task_to_worker(t_block *manager, t_token_node **cur, \
			int *nodecount, int *all)
{
	token_list_move_top_to_new(manager->worker_tasks[*all],
		manager->prompt, (*cur)->prev, *nodecount);
	manager->op_id[*all] = manager->prompt->head->type;
	token_list_del_head(manager->prompt);
	*cur = manager->prompt->head;
	*nodecount = 0;
	(*all)++;
}

int	manager_gets_workers_and_operators(t_block *manager)
{
	t_token_node	*cur;
	int				all;
	int				i;
	int				open_par;

	i = 0;
	all = 0;
	open_par = 0;
	cur = manager->prompt->head;
	while (all < manager->op_count)
	{
		open_par += (cur->type == T_OPEN_PAR);
		open_par -= (cur->type == T_CLOSE_PAR);
		if (cur && token_is_big_operator(cur) && !open_par)
			send_task_to_worker(manager, &cur, &i, &all);
		else
		{
			i++;
			cur = cur->next;
		}
	}
	token_list_destroy(&manager->worker_tasks[all]);
	manager->worker_tasks[all] = manager->prompt;
	manager->prompt = NULL;
	return (1);
}
