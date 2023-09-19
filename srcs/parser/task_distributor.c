/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   task_distributor.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 09:57:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 13:39:04 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	distribute_tasks_between_managers_and_workers(t_block *block)
{
	int	i;
	int	has_parenthesis;

	if (!setup_task_distributor(block))
		return (free_task_distributor(block));
	if (!block->is_worker && block->op_count > 0)
		manager_gets_workers_and_operators(block);
	if (!block->op_count && block->must_subshell)
	{
		if (!manager_extract_redirections(block))
			return (free_task_distributor(block));
		manager_subshell_and_arithmatic(block);
		token_list_destroy(&block->worker_tasks[0]);
		block->worker_tasks[0] = block->prompt;
		block->prompt = NULL;
	}
	if (block->is_worker && !worker_extract_redirections(block))
		return (0);
	return (1);
}

int	setup_task_distributor(t_block *block)
{
	int	i;

	block->op_count = check_if_worker_and_count_operators(block);
	if (!block->is_worker)
	{
		block->op_id = malloc(sizeof(*(block->op_id)) * block->op_count);
		block->worker_tasks = ft_calloc((block->op_count + 2),
				sizeof(*(block->worker_tasks)));
		block->worker_list = ft_calloc((block->op_count + 2),
				sizeof(*(block->worker_list)));
		block->worker_pids = ft_calloc(block->op_count + 1,
				sizeof(*(block->worker_pids)));
		block->worker_exit_status = ft_calloc(block->op_count + 1,
				sizeof(*(block->worker_exit_status)));
		ft_memset(block->worker_exit_status, -1, (block->op_count + 1)
			* sizeof(*(block->worker_exit_status)));
		if (!block->op_id || !block->worker_tasks || !block->worker_pids)
		{
			perror_msg("malloc");
			return (free_task_distributor(block));
		}
		i = 0;
		while (i < block->op_count + 1)
		{
			block->worker_exit_status[i] = -1;
			block->worker_tasks[i] = token_list_new();
			if (!block->worker_tasks[i])
				return (free_task_distributor(block));
			i++;
		}
	}
	return (1);
}

int	free_task_distributor(t_block *manager)
{
	manager_destroy_worker_tasks(manager);
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

int	check_if_worker_and_count_operators(t_block *block)
{
	int				open_par;
	int				count;
	t_token_node	*cur;

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
