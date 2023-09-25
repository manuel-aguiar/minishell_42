/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:37:28 by codespace         #+#    #+#             */
/*   Updated: 2023/09/25 08:39:05 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_subshell_and_prepare_forks(t_block *manager, int index)
{
	if (manager->must_subshell \
	&& !manager->has_arithmatic_parenthesis && manager->op_count == 0)
		manager->worker_list[index]->i_am_forked = 1;
	if (manager->worker_list[index]->i_am_forked)
	{
		manager->worker_pids[index] = fork();
		if (manager->worker_pids[index] == -1)
			return (perror_msg_int("fork", 0));
		if (!manager->worker_pids[index])
		{
			if (index < manager->op_count \
			&& manager->op_id[index] == T_OP_PIPE)
			{
				if (close(manager->pipefd[0]) == -1)
					perror("close");
			}
		}
	}
	return (1);
}

int	pipes_forks_and_conditionals(t_block *manager, int index)
{
	if (index > 0 && manager->op_id[index - 1] == T_OP_PIPE)
	{
		manager->prev_pipe[0] = manager->pipefd[0];
		manager->prev_pipe[1] = manager->pipefd[1];
		manager->worker_list[index]->i_am_forked = 1;
	}
	if (index < manager->op_count && manager->op_id[index] == T_OP_PIPE)
	{
		if (pipe(manager->pipefd) == -1)
			return (perror_msg_int("pipe", 0));
		manager->worker_list[index]->i_am_forked = 1;
	}
	if (index > 0 && index <= manager->op_count \
	&& (manager->op_id[index - 1] == T_OP_AND \
	|| manager->op_id[index - 1] == T_OP_OR \
	|| manager->op_id[index - 1] == T_OP_SEMICOL))
	{
		waiting_for_my_workers(manager, index);
		if ((manager->op_id[index - 1] == T_OP_AND \
		&& manager->ms->exit_status != 0) \
		|| (manager->op_id[index - 1] == T_OP_OR \
		&& manager->ms->exit_status == 0))
			return (0);
	}
	return (check_subshell_and_prepare_forks(manager, index));
}

int	wait_and_save_status(pid_t pid, int *status, int errfd)
{
	if (waitpid(pid, status, 0) == -1)
		perror("waitpid");
	if (WIFEXITED(*status))
		*status = WEXITSTATUS(*status);
	else if (WIFSIGNALED(*status))
	{
		if (WTERMSIG(*status) == SIGINT)
			ft_putstr_fd("\n", errfd);
		*status = WTERMSIG(*status) + EXIT_SIGNALED;
	}
	return (1);
}

int	waiting_for_my_workers(t_block *manager, int index)
{
	int	i;

	i = 0;
	while (i < index)
	{
		if (manager->worker_pids[i] != 0)
		{
			wait_and_save_status(manager->worker_pids[i], \
								&manager->my_status, manager->ms->errfd);
			manager->worker_pids[i] = 0;
		}
		else if (manager->worker_exit_status[i] >= 0)
		{
			manager->my_status = manager->worker_exit_status[i];
			manager->worker_exit_status[i] = -1;
		}
		i++;
	}
	if (!manager->i_am_forked && manager->my_manager)
		manager->my_manager->my_status = manager->my_status;
	manager->ms->exit_status = manager->my_status;
	return (1);
}
