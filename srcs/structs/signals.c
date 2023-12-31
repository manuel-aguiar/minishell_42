/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/18 15:53:50 by codespace         #+#    #+#             */
/*   Updated: 2023/09/23 14:24:24 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		g_signal = SIGINT;
		close(STDIN_FILENO);
	}
	if (signum == SIGQUIT)
		g_signal = SIGQUIT;
}

int	ms_reset_signal(t_ms *ms)
{
	ms->sigact.sa_flags = 0;
	ms->sigact.sa_handler = SIG_DFL;
	if (sigemptyset(&(ms->sigact.sa_mask)) == -1)
		return (perror_msg_int("sigemptyset", 0));
	if (sigaction(SIGINT, &ms->sigact, NULL) == -1)
		return (perror_msg_int("sigaction", 0));
	if (sigaction(SIGQUIT, &ms->sigact, NULL) == -1)
		return (perror_msg_int("sigaction", 0));
	if (sigaction(SIGPIPE, &ms->sigact, NULL) == -1)
		return (perror_msg_int("sigaction", 0));
	return (1);
}

int	ms_prepare_signal(t_ms *ms, void (*handler)(int))
{
	ms->sigact.sa_flags = SA_RESTART;
	ms->sigact.sa_handler = handler;
	if (sigemptyset(&(ms->sigact.sa_mask)) == -1)
		return (perror_msg_int("sigemptyset", 0));
	if (sigaction(SIGINT, &ms->sigact, NULL) == -1)
		return (perror_msg_int("sigaction", 0));
	if (sigaction(SIGQUIT, &ms->sigact, NULL) == -1)
		return (perror_msg_int("sigaction", 0));
	if (sigaction(SIGPIPE, &ms->sigact, NULL) == -1)
		return (perror_msg_int("sigaction", 0));
	return (1);
}
