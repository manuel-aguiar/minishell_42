/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 12:32:48 by marvin            #+#    #+#             */
/*   Updated: 2023/09/23 15:25:57 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	perror_msg_func(t_block *block, char *function, int errcode, int with_ms)
{
	block->my_status = errcode;
	if (block->my_manager)
		block->my_manager->my_status = errcode;
	else
		block->ms->exit_status = errcode;
	if (with_ms)
	{
		ft_putstr_fd(block->ms->name, block->ms->errfd);
		ft_putstr_fd(": ", block->ms->errfd);
	}
	perror(function);
	return (0);
}






/*
	child_process
	redirects block->final file descriptors in order for the command called from execve
	to use them for input and output.
	After dup2, it closes the original file descriptors (not inherited) as these are not needed anymore
	If exec_command fails, it will exit. exec_command itself reports on the error found:
		-   command not found
		-   permissions
		-   etc
	Given that pipes are communication between children (even though they are set by the manager)
	it is up to them to close them.
	If the current operation by the child is to wirte of the pipe, it is its responsibility
	to close the read-end since it will not use it. WIthout that, there will be always a reference to
	the read-end of the pipe, it is never closed, and another process that is reading will assume the piep is open.
	This child has to close the read-end and force a SIGPIPE on the following process to guarantee that
	the next one isn't kept waiting by a pipe that is no longer under use.
	Upon returning to the execution_tree function, the function will call destroy block to make sure that
	it is over.
	In fact, exec command itself will exit and report on the exit status of failure and avoiding the child process
	to continue past this function.

*/



/*
	parent_process
	Closes the oustanding file descriptors that are not inherited or are pipes
	frees block->here_doc after unlinking, in order to destroy the here_doc used
	as infput, if such was the case.
*/

int	parent_process(t_block *block, pid_t pid)
{
	close_in_fds(block);
	close_out_fds(block);
	if (block->here_doc)
	{
		unlink(block->here_doc);
		ft_free_set_null(&block->here_doc);
	}
	if (block->i_am_forked)
		wait_and_save_status(pid, &block->my_status, block->ms->errfd);
	return (1);
}

/*
	process_execution
	This is called inside the "execute" function, close to main function, after all cmd_args and
	fds are correctly setup.
	It forks the process, saves the pid in the manager block (as the manager will need to know for which
	children to wait for) and, based on the return of pid, does both the child process and the parent process.

*/

int	process_execution(t_block *block)
{
	pid_t	pid;
	int		builtin;

	builtin = check_builtins(block);
	if (!builtin)
	{
		pid = fork();
		if (!block->i_am_forked)
		{
			if (block->my_manager)
				block->my_manager->worker_pids[block->my_id] = pid;
			else
				block->ms->my_kid = pid;
		}
		if (pid == -1)
			return (perror_msg_int("fork", 0));
		if (!pid)
			child_process(block);
		parent_process(block, pid);
	}
	else
	{
		ms_prepare_signal(block->ms, SIG_IGN);
		if (exec_builtin(block, builtin))
		{
			if (block->my_manager)
				block->my_manager->worker_exit_status[block->my_id] = block->my_status;						
			else
				block->ms->exit_status = block->my_status;
		}
		ms_prepare_signal(block->ms, signal_handler);
		close_in_fds(block);
		close_out_fds(block);
	}
	return (1);
}
