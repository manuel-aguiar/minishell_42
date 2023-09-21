/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/18 15:53:50 by codespace         #+#    #+#             */
/*   Updated: 2023/09/21 12:05:50 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_ms	*sigint_heredoc_where_ms_is(t_ms *ms)
{
	static t_ms	*save;

	if (ms)
		save = ms;
	return (save);
}

void	signal_handler(int signum)
{
	int	code;

	if (signum == SIGINT)
	{
		close(STDIN_FILENO);
		code = 130;
		save_signal(&code);
	}
	//if (signum == SIGQUIT)
	//{
	//	code = 131;
	//	save_signal(&code);
	//}
	if (signum == SIGPIPE)
	{
		ms_destroy(sigint_heredoc_where_ms_is(NULL));
		exit(13);
	}
}

int	save_signal(int *num)
{
	static int	g_my_signal;

	if (num)
		g_my_signal = *num;
	return (g_my_signal);
}

int	check_for_signals(t_ms *ms)
{
	int	code;

	if (save_signal(NULL))
	{
		code = 0;
		ms->exit_status = save_signal(NULL);
		save_signal(&code);
	}
	return (1);
}

int	ms_prepare_signal(t_ms *ms, void (*handler)(int))
{
	ms->sigact.sa_flags = 0;
	ms->sigact.sa_handler = handler;
	if (sigemptyset(&(ms->sigact.sa_mask)) == -1)
		return (perror_msg("sigemptyset"));
	if (sigaction(SIGINT, &ms->sigact, NULL) == -1)
		return (perror_msg("sigaction"));
	if (sigaction(SIGQUIT, &ms->sigact, NULL) == -1)
		return (perror_msg("sigaction"));
	if (sigaction(SIGPIPE, &ms->sigact, NULL) == -1)
		return (perror_msg("sigaction"));
	return (1);
}
