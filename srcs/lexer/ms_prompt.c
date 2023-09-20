/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_prompt.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 00:13:38 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/20 15:21:59 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_prompt(t_ms *ms)
{
	if (ms->prompt)
		token_list_destroy(&ms->prompt);
	if (!setup_prompt(ms))
		return (0);
	if (!prompt_is_valid(ms))
	{
		token_list_destroy(&ms->prompt);
		return (0);
	}
	// validate_syntax(&pmt);
	// validate_quote_close(&pmt);
	// validate_parenthesis_close(&pmt);
	// if (pmt.exit_status)
	//{
	//    ms->exit_status = pmt.exit_status;
	//    return (syntax_error_msg(&pmt));
	//}
	// if (pmt.parenthesis)
	//    rm_unnecessary_parenthesis(&pmt);
	// ft_free_set_null(&pmt.copy);
	// ms->prompt = pmt.prompt;
	return (1);
}

int	setup_prompt(t_ms *ms)
{
	char	*line;
	int		set_zero;

	//printf("active signal %d\n", save_signal(NULL));
	line = readline("minishell>$ ");
	if (save_signal(NULL))
	{
		set_zero = 0;
		ms->exit_status = save_signal(NULL);
		save_signal(&set_zero);
	}
	if (!line)
	{
		ms_destroy(ms);
		printf("exit\n");
		exit(0);
	}
	else if (!line[0] || save_signal(NULL) == EXIT_SIGINT)
	{
		ft_free_set_null(&line);
		return (0);
	}
	else
		add_history(line);
	//printf("command is [%s]\n", line);
	// if (!line[0] || is_only_spaces(line))
	//{
	//
	//    ft_free_set_null(&line);
	//   return (0);
	//}
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
