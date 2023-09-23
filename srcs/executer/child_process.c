/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/23 11:35:14 by codespace         #+#    #+#             */
/*   Updated: 2023/09/23 11:51:40 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	child_process(t_block *block)
{
	if (block->final_in == block->ms->infd && g_signal == SIGQUIT)
	{
		block->ms->kill_stdin = 1;
		if (dup2(block->ms->dup_stdin, block->ms->infd) == -1)
			perror_child_exit(block, CODE_DUP2, 1);
		close(block->ms->dup_stdin);
		block->ms->dup_stdin = dup(block->ms->infd);
		if (block->ms->dup_stdin == -1)
			perror_child_exit(block, CODE_DUP, 1);
	}
	if (tcsetattr(block->ms->infd, TCSANOW, &block->ms->original) == -1)
		perror_msg_ptr("tcsetattr", NULL);
	ms_reset_signal(block->ms);
	if (dup2(block->final_in, block->ms->infd) == -1)
		perror_child_exit(block, CODE_DUP2, 1);
	close_in_fds(block);
	if (dup2(block->final_out, block->ms->outfd) == -1)
		perror_child_exit(block, CODE_DUP2, 1);
	close_out_fds(block);
	if (block->ms->dup_stdin != -1)
		close(block->ms->dup_stdin);
	if (!exec_command(block))
		return (0);
	return (0);
}

void	error_child_exit(t_block *block, char *error_msg, int errcode,
		int with_ms)
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
	ft_putstr_fd(block->cmd, block->ms->errfd);
	ft_putstr_fd(": ", block->ms->errfd);
	ft_putstr_fd(error_msg, block->ms->errfd);
	ft_putstr_fd("\n", block->ms->errfd);
	close(block->ms->infd);
	close(block->ms->outfd);
	ms_destroy(block->ms);
	exit(errcode);
}

void	perror_child_exit(t_block *block, int errcode,
		int with_ms)
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
	perror(block->cmd);
	close(block->ms->infd);
	close(block->ms->outfd);
	ms_destroy(block->ms);
	exit(errcode);
}
