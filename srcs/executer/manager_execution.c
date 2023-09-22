/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:37:28 by codespace         #+#    #+#             */
/*   Updated: 2023/09/22 21:12:49 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



/*

	pipes_forks_and_conditionals
	Sets up the commands based on the big 3 operators: && || and |.
		-   Saves the read end of the previous pipe, if a pipe was open
			the next operator may be a pipe as well: pipe will overwrite
			the previous file descriptors
		-   Opens a new pipe if the current command will write too a pipe;
		-   In case of && or ||, waits for the previous commands to arrive
			in order to determine their exit status and decide wether
			the following command will execute or not;

*/

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
			return (perror_msg("pipe"));
		manager->worker_list[index]->i_am_forked = 1;
	}
	if (index > 0 && index <= manager->op_count \
	&& (manager->op_id[index - 1] == T_OP_AND || manager->op_id[index - 1] == T_OP_OR))
	{
		waiting_for_my_workers(manager, index);
		if ((manager->op_id[index - 1] == T_OP_AND && manager->ms->exit_status != 0) \
		|| (manager->op_id[index - 1] == T_OP_OR && manager->ms->exit_status == 0))
			return (0);
	}
	if (manager->must_subshell \
	&& !manager->has_arithmatic_parenthesis && manager->op_count == 0)
		manager->worker_list[index]->i_am_forked = 1;
	if (manager->worker_list[index]->i_am_forked)
	{
		manager->worker_pids[index] = fork();
		if (manager->worker_pids[index] == -1)
			return (perror_msg("fork"));
		if (!manager->worker_pids[index] && index < manager->op_count && manager->op_id[index] == T_OP_PIPE)
			close(manager->pipefd[0]);
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
			if (waitpid(manager->worker_pids[i], &manager->my_status, 0) == -1)
				perror("waitpid");
			if (WIFEXITED(manager->my_status))
				manager->my_status = WEXITSTATUS(manager->my_status);
			else if (WIFSIGNALED(manager->my_status))
				manager->my_status = WTERMSIG(manager->my_status) + EXIT_SIGNALED;
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