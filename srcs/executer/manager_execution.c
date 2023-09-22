/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager_execution.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:37:28 by codespace         #+#    #+#             */
/*   Updated: 2023/09/22 09:17:35 by codespace        ###   ########.fr       */
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
	{
		//printf("child: [%s] has parenthesis and must be forked\n", manager->worker_list[index]->prompt);
		manager->worker_list[index]->i_am_forked = 1;
	}
	if (manager->worker_list[index]->i_am_forked)
	{
		manager->worker_pids[index] = fork();
		if (manager->worker_pids[index] == -1)
			return (perror_msg("fork"));
		if (!manager->worker_pids[index])
		{
			if (index < manager->op_count && manager->op_id[index] == T_OP_PIPE)
				close(manager->pipefd[0]);
			//if (index > 0 && manager->op_id[index - 1] == T_OP_PIPE)
			//	close(manager->prev_pipe[1]);
		}
		else
			dprintf(2, "manager %d created pid %d\n", getpid(), manager->worker_pids[index]);
	}
	return (1);
}

int	waiting_for_my_workers(t_block *manager, int index)
{
	int	i;

	//check_for_signals(manager->ms);
	i = 0;
	//printf("manager [%s] waiting for %d children \n", manager->prompt, manager->op_count + 1);
	//printf("i am [%s], starting status %d, waiting, mypid %d\n", manager->prompt, manager->my_status, getpid());
	//print_worker_pids(manager);
	while (i < index)
	{
		//printf("child [%s], index, %d, has pid? %d\n", manager->worker_list[i]->prompt, i, manager->worker_pids[i]);
		if (manager->worker_pids[i] != 0)
		{
			//printf("my lvl id (%d, %d), waiting for pid %d, my status now is: %d  ", manager->my_level, manager->my_id, manager->worker_pids[i], manager->my_status);
			waitpid(manager->worker_pids[i], &manager->my_status, 0);
			dprintf(2, "manager %d received pid %d\n", getpid(), manager->worker_pids[i]);
			if (WIFEXITED(manager->my_status))
				manager->my_status = WEXITSTATUS(manager->my_status);
			//printf("  and changed to %d i received from child (%d, %d)\n", manager->my_status, manager->my_level +1, i);
			manager->worker_pids[i] = 0;
		}
		else if (manager->worker_exit_status[i] >= 0)
		{
			//printf("my lvl id (%d, %d) original status %d will change to %d from child wnumber (%d,%d)\n", manager->my_level, manager->my_id, manager->my_status, manager->worker_exit_status[i], manager->my_level +1, i);
			manager->my_status = manager->worker_exit_status[i];
			manager->worker_exit_status[i] = -1;
		}
		//else
		//	printf("no pid, no manual status\n");
		i++;
	}
	//printf("ending status %d, moving on, mypid %d\n", manager->my_status, getpid());
	if (!manager->i_am_forked && manager->my_manager)
		manager->my_manager->my_status = manager->my_status;
	manager->ms->exit_status = manager->my_status;
	return (1);
}