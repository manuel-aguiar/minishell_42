/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 12:32:48 by marvin            #+#    #+#             */
/*   Updated: 2023/09/22 15:24:04 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	perror_msg(char *text)
{
	perror(text);
	return (0);
}

int	error_msg(char *text)
{
	ft_putstr_fd(text, STDERR_FILENO);
	return (0);
}

int	perror_msg_func(t_block *block, char *function, int errcode, int with_ms)
{
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

void	error_child_exit(t_block *block, char *function, char *cmd, int errcode, int with_ms)
{
	if (block->my_manager)
		block->my_manager->my_status = errcode;
	else
		block->ms->exit_status = errcode;
	//printf("error code sent by child..? %d\n", block->ms->exit_status);
	if (with_ms)
	{
		ft_putstr_fd(block->ms->name, block->ms->errfd);
		ft_putstr_fd(": ", block->ms->errfd);
	}
	ft_putstr_fd(function, block->ms->errfd);
	ft_putstr_fd(": ", block->ms->errfd);
	ft_putstr_fd(cmd, block->ms->errfd);
	ft_putstr_fd("\n", block->ms->errfd);


	close(block->ms->infd);
	close(block->ms->outfd);

	ms_destroy(block->ms);
	exit(errcode);
}

void	perror_child_exit(t_block *block, \
char *function, int errcode, int with_ms)
{
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

	//close fds
	close(block->ms->infd);
	close(block->ms->outfd);

	ms_destroy(block->ms);
	exit(errcode);
}


/*

	join_bin_path
	self explanatory, joining the bin to a single path, returning, and exec_cmd_search_path determines
	whether this is a valid path for execution

*/


int	join_path_bin(char **full_path, char *path, char *bin)
{
	int		path_len;
	int		bin_len;
	char	*new;

	path_len = ft_strlen(path);
	if (path[path_len - 1] == '/')
		path_len--;
	bin_len = ft_strlen(bin);
	new = malloc(sizeof(*new) * (path_len + bin_len + 1 + 1));
	if (!new)
	{
		*full_path = new;
		return (perror_msg("malloc"));
	}
	ft_memcpy(new, path, path_len);
	new[path_len] = '/';
	ft_memcpy(&new[path_len + 1], bin, bin_len);
	new[path_len + bin_len + 1] = '\0';
	*full_path = new;
	return (1);
}



/*

	checks if the absolute/relative address that was given is valid, and if so, executes
	otherwise, it returns the corrresponding error message and exits

*/


int	exec_cmd_with_path(t_block *block)
{
	if (access(block->cmd, F_OK))
		return (perror_msg(block->cmd));											//double check this
	else if (execve(block->cmd, block->cmd_args, block->ms->env) == -1)
		return (perror_msg(block->cmd));											//double check this
	return (1);
}

/*

	searches path on the $PATH env variable and tries the find the command
	it needs to execute from there

*/

int	exec_cmd_search_path(t_block *block)
{
	int		i;
	char	*full_path;

	if (!block->ms->path)
	{
		error_child_exit(block, block->cmd_args[0], ERR_CMD, CODE_CMD, 1);		// substituir
		return (0);
	}
	i = 0;
	while (block->ms->path[i])
	{
		if (!join_path_bin(&full_path, block->ms->path[i++], block->cmd))
			return (0);
		if (access(full_path, F_OK) == 0)
		{
			if (execve(full_path, block->cmd_args, block->ms->env) == -1)
			{
				perror_child_exit(block, block->cmd_args[0], CODE_EXECVE, 1);
				ft_free_set_null(&full_path);
				return (0);
			}
		}
		else
			ft_free_set_null(&full_path);
	}
	error_child_exit(block, block->cmd_args[0], ERR_CMD, CODE_CMD, 1);    //error messageeeeeee
	return (0);
}

/*

	exec_command
	straight forward, checks whether the path to the command is absolute/relative
	or $PATH is required to find the correct executable and calls the apropriate function
	to finish the job.

*/


int	exec_command(t_block *block)
{
	if (ft_strrchr(block->cmd, '/'))
		exec_cmd_with_path(block);
	else
		exec_cmd_search_path(block);
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

int	child_process(t_block *block)
{
	if (block->final_in == block->ms->infd && g_signal == SIGQUIT)
	{
		block->ms->kill_stdin = 1;
		if (dup2(block->ms->dup_stdin, block->ms->infd) == -1)
			perror_msg_ptr("dup2", NULL);
		close(block->ms->dup_stdin);
		block->ms->dup_stdin = dup(block->ms->infd);
		if (block->ms->dup_stdin == -1)
			perror_msg_ptr("dup", NULL);
	}
	if (tcsetattr(block->ms->infd, TCSANOW, &block->ms->original) == -1)
		perror_msg_ptr("tcsetattr", NULL);
	ms_reset_signal(block->ms);
	if (dup2(block->final_in, block->ms->infd) == -1)
		perror_child_exit(block, block->cmd_args[0], CODE_DUP2, 1);
	close_in_fds(block);
	if (dup2(block->final_out, block->ms->outfd) == -1)
		perror_child_exit(block, block->cmd_args[0], CODE_DUP2, 1);
	close_out_fds(block);
	if (block->ms->dup_stdin != -1)
		close(block->ms->dup_stdin);
	if (!exec_command(block))
		return (0);
	return (0);
}

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
	{
		ms_prepare_signal(block->ms, SIG_IGN);
		if (waitpid(pid, &block->my_status, 0) == -1)
					perror("waitpid");
		//dprintf(2, "exec %d received pid %d\n", getpid(), pid);
		if (WIFEXITED(block->my_status))
			block->my_status = WEXITSTATUS(block->my_status);
		else if (WIFSIGNALED(block->my_status))
		{
			block->my_status = WTERMSIG(block->my_status) + EXIT_SIGNALED;
			ft_putstr_fd("\n", block->ms->errfd);
		}
		ms_prepare_signal(block->ms, signal_handler);
	}
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
			return (perror_msg("fork"));
		if (!pid)
			child_process(block);
		//dprintf(2, "exec %d created pid %d\n", getpid(), pid);
		parent_process(block, pid);
	}
	else
	{
		if (exec_builtin(block, builtin))
		{
			if (block->my_manager)
				block->my_manager->worker_exit_status[block->my_id] = 0;						// success
																						// manage for non success
		}
		close_in_fds(block);
		close_out_fds(block);
	}
	return (1);
}
