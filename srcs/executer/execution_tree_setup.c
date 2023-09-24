/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_tree_setup.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 20:13:11 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 20:13:29 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_all_here_docs(t_block *block)
{
	int	i;

	if (!block->is_worker)
	{
		i = 0;
		while (block->worker_list[i] \
		&& !block->has_arithmatic_parenthesis \
		&& g_signal != SIGINT)
		{
			get_all_here_docs(block->worker_list[i]);
			i++;
		}
	}
	open_here_docs_at_block(block);
	return (1);
}

int	setup_execution_tree(t_ms *ms, t_block *manager, \
						t_token_list *tasks, int my_id)
{
	int		i;
	t_block	*block;

	block = block_init(ms, manager, tasks, my_id);
	if (!block)
		return (0);
	if (!distribute_tasks_between_managers_and_workers(block))
		return (0);
	if (block->is_worker)
		return (0);
	if (!block->is_worker)
	{
		i = 0;
		while (block->worker_tasks[i])
		{
			setup_execution_tree(ms, block, block->worker_tasks[i], i);
			i++;
		}
		manager_destroy_worker_tasks(block);
	}
	return (1);
}
