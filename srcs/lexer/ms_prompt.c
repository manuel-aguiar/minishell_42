/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_prompt.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 00:13:38 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/21 13:15:46 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_prompt(t_ms *ms)
{
	char	*line;

	if (ms->prompt)
		token_list_destroy(&ms->prompt);
	line = prompt_readline(ms);
	if (!line)
		return (0);
	if (!prompt_token_setup(ms, line))
		return (0);
	if (!prompt_is_valid(ms))
	{
		token_list_destroy(&ms->prompt);
		return (0);
	}
	return (1);
}

char	*prompt_readline(t_ms *ms)
{
	char	*line;

	ms->dup_stdin = dup(ms->infd);
	if (!ms->dup_stdin)
		return (perror_msg_ptr("dup", NULL));
	if (tcsetattr(ms->dup_stdin, TCSANOW, &ms->modified) == -1)
		return (perror_msg_ptr("tcsetattr", NULL));
	line = readline("minishell>$ ");
	if (tcsetattr(ms->dup_stdin, TCSANOW, &ms->original) == -1)
		return (perror_msg_ptr("tcsetattr", NULL));
	if (save_signal(NULL) == EXIT_SIGINT)	
	{
		ms->kill_stdin = 1;
		if (dup2(ms->dup_stdin, ms->infd) == -1)
			perror_msg_ptr("dup2", NULL);
		close(ms->dup_stdin);
		printf("\n");
		return (NULL);
	}
	close(ms->dup_stdin);
	if (!line)
	{
		ms_destroy(ms);
		printf("exit\n");
		exit(0);
	}
	else if (!line[0])
		ft_free_set_null(&line);
	else
		add_history(line);
	return (line);
}

int	prompt_token_setup(t_ms *ms, char *line)
{
	ms->prompt = token_list_new();
	if (!ms->prompt)
	{
		free(line);
		return (0);
	}
	if (!prompt_to_list(ms->prompt, line))
	{
		free(line);
		token_list_destroy(&ms->prompt);
		return (0);
	}
	ft_free_set_null(&line);
	return (1);
}
