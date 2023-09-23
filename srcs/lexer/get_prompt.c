/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_prompt.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 00:13:38 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/23 11:57:02 by codespace        ###   ########.fr       */
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

	line = readline(ms->name_readline);
	if (g_signal == SIGINT)
	{
		ms->exit_status = EXIT_SIGINT;
		ft_putstr_fd("\n", ms->errfd);
		return (NULL);
	}
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