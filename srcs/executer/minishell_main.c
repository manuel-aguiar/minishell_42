/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_main.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:52:17 by marvin            #+#    #+#             */
/*   Updated: 2023/09/26 09:55:07 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	reopen_stdin(t_ms *ms)
{
	ms->kill_stdin = 1;
	if (dup2(ms->dup_stdin, ms->infd) == -1)
		perror_msg_int("dup2", 0);
	close(ms->dup_stdin);
	ms->dup_stdin = dup(ms->infd);
	if (ms->dup_stdin == -1)
		perror_msg_int("dup", 0);
	return (1);
}

int	minishell_main_loop(t_ms *ms)
{
	while (1)
	{
		if (get_prompt(ms))
		{
			setup_execution_tree(ms, NULL, ms->prompt, 0);
			if (g_signal != SIGINT)
				get_all_here_docs(ms->first);
			if (g_signal != SIGINT)
				execution_tree_exec_all(ms->first);
			block_destroy(ms->first);
			if (ms->my_kid != -1)
			{
				wait_and_save_status(ms->my_kid, &ms->exit_status, ms->errfd);
				ms->my_kid = -1;
			}
		}
		if (g_signal)
			reopen_stdin(ms);
		g_signal = 0;
		if (isatty(ms->infd) \
		&& tcsetattr(ms->infd, TCSANOW, &ms->modified) == -1)
			perror_msg_int("tcsetattr", 0);
	}
	return (1);
}

/*
valgrind --track-fds=yes --trace-children=yes 
--leak-check=full --show-leak-kinds=all ./minishell
*/

int	main(int ac, char **av, char **env)
{
	t_ms	ms;

	(void)ac;
	(void)av;
	if (!ms_init(&ms, env))
		return (0);
	minishell_main_loop(&ms);
	ms_destroy(&ms);
	return (0);
}
