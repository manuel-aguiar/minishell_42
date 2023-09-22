/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/18 15:53:50 by codespace         #+#    #+#             */
/*   Updated: 2023/09/22 13:15:44 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_handler(int signum)
{
	int	code;

	if (signum == SIGINT)
	{
		
		code = 130;
		save_signal(&code);
		g_signal = SIGINT;
		close(STDIN_FILENO);
	}
	if (signum == SIGQUIT)
	{
		code = 131;
		save_signal(&code);
		g_signal = SIGQUIT;
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
		g_signal = code;
	}
	return (1);
}

int	ms_reset_signal(t_ms *ms)
{
	ms->sigact.sa_flags = 0;
	ms->sigact.sa_handler = SIG_DFL;
	if (sigemptyset(&(ms->sigact.sa_mask)) == -1)
		return (perror_msg("sigemptyset"));
	if (sigaction(SIGINT, &ms->sigact, NULL) == -1)
		return (perror_msg("sigaction"));
	if (sigaction(SIGQUIT, &ms->sigact, NULL) == -1)
		return (perror_msg("sigaction"));
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
	return (1);
}
