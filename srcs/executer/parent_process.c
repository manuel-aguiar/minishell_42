/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parent_process.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/01 12:32:48 by marvin            #+#    #+#             */
/*   Updated: 2023/09/26 11:22:12 by codespace        ###   ########.fr       */
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

static int	parent_process(t_block *block, pid_t pid)
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

static int	external_program_execution(t_block *block)
{
	pid_t	pid;

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
	return (1);
}

int	process_execution(t_block *block)
{
	int		builtin;

	builtin = check_builtins(block);
	if (!builtin)
		external_program_execution(block);
	else
	{
		ms_prepare_signal(block->ms, SIG_IGN);
		exec_builtin(block, builtin);
		if (block->my_manager)
			block->my_manager->worker_exit_status[block->my_id] \
			= block->my_status;
		else
			block->ms->exit_status = block->my_status;
		ms_prepare_signal(block->ms, signal_handler);
		close_in_fds(block);
		close_out_fds(block);
	}
	return (1);
}
