/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_tree.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:26:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/20 10:33:28 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



int	block_exit_status_destroy(t_block *block);

int	execution_tree_exec_all(t_block *block)
{
	int	i;

	if (!prepare_redirections(block))
		return (0);
	if (block->is_worker)
		worker_execution(block);
	else if (!block->has_arithmatic_parenthesis)
	{
		i = 0;
		while (block->worker_list[i])
		{
			if (i > 0 && block->op_id[i - 1] == T_OP_PIPE)
				close(block->pipefd[1]);
			if (block->op_id && pipes_forks_and_conditionals(block, i) \
			&& !(block->worker_list[i]->i_am_forked && block->worker_pids[i] != 0))
				execution_tree_exec_all(block->worker_list[i]);
			if (i > 0 && block->op_id[i - 1] == T_OP_PIPE)
				close(block->prev_pipe[0]);
			i++;
		}
		waiting_for_my_workers(block, block->op_count + 1);
		close_in_fds(block);
		close_out_fds(block);
	}
	else
		block->my_status = 1;
	block_exit_status_destroy(block);
	return (1);
}

int	block_exit_status_destroy(t_block *block)
{
	int	status;

	if (block->i_am_forked)
	{
		status = block->my_status;
		ms_destroy(block->ms);
		exit(status);
	}
	else if (block->my_manager && !block->is_worker)
	{
		block->my_manager->worker_exit_status[block->my_id] = block->my_status;
	}
	else if (!block->my_manager && !block->is_worker)
		block->ms->exit_status = block->my_status;
	block_destroy(block);
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

int	get_all_here_docs(t_block *block)
{
	int	i;

	if (!block->is_worker)
	{
		i = 0;
		while (block->worker_list[i] \
		&& !block->has_arithmatic_parenthesis \
		&& save_signal(NULL) != EXIT_SIGINT)
		{
			get_all_here_docs(block->worker_list[i]);
			i++;
		}
	}
	open_here_docs_at_block(block);
	return (1);
}
