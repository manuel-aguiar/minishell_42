/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_tree_loop.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:26:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 20:15:58 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	block_exit_status_destroy(t_block *block)
{
	int	status;

	if (block->i_am_forked)
	{
		status = block->my_status;
		if (block->ms->dup_stdin != -1)
			close(block->ms->dup_stdin);
		ms_destroy(block->ms);
		exit(status);
	}
	else if (block->my_manager && !block->is_worker)
	{
		block->my_manager->worker_exit_status[block->my_id] \
		= block->my_status;
	}
	else if (!block->my_manager && !block->is_worker)
		block->ms->exit_status = block->my_status;
	block_destroy(block);
	return (1);
}

static void	close_prev_write_pipe(t_block *block, int index)
{
	if (index > 0 && block->op_id[index - 1] == T_OP_PIPE)
		close(block->pipefd[1]);
}

static void	close_prev_read_pipe(t_block *block, int index)
{
	if (index > 0 && block->op_id[index - 1] == T_OP_PIPE)
		close(block->prev_pipe[0]);
}

static void	wait_for_workers_and_close_fds(t_block *block, int worker_count)
{
	waiting_for_my_workers(block, worker_count);
	close_in_fds(block);
	close_out_fds(block);
}

int	execution_tree_exec_all(t_block *block)
{
	int	i;

	if (g_signal != SIGINT && !prepare_redirections(block))
		return (0);
	if (block->is_worker && g_signal != SIGINT)
		worker_execution(block);
	else if (!block->has_arithmatic_parenthesis && g_signal != SIGINT)
	{
		i = 0;
		while (block->worker_list[i] && g_signal != SIGINT)
		{
			close_prev_write_pipe(block, i);
			if (block->op_id && pipes_forks_and_conditionals(block, i) \
			&& !(block->worker_list[i]->i_am_forked \
			&& block->worker_pids[i] != 0))
				execution_tree_exec_all(block->worker_list[i]);
			close_prev_read_pipe(block, i);
			i++;
		}
		wait_for_workers_and_close_fds(block, block->op_count + 1);
	}
	else
		block->my_status = 1;
	block_exit_status_destroy(block);
	return (1);
}
