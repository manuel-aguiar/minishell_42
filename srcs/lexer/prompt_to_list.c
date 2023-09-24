/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_to_list.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 15:42:02 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/24 18:36:23 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	upd_temp_and_prompt(char **prompt, int len, char **temp)
{
	if (len > 0)
	{
		(*temp) = ft_strdup_len((*prompt), len);
		if (!(*temp))
			return ;
	}
	(*prompt) += len;
}

static void	create_args_for_token(char **prompt, \
int token, int *dummy_token, char **temp)
{
	int		len;
	int		rewind;
	int		count_squotes;
	int		count_dquotes;

	len = 0;
	count_squotes = 0;
	count_dquotes = 0;
	rewind = ft_strlen(*prompt);
	while (**prompt && (((!token_is_operator(token) && \
	!is_token(prompt, dummy_token, 0)) && !ft_isspace(**prompt)) \
	|| ((ft_isspace(**prompt) && (count_squotes % 2 != 0 \
	|| count_dquotes % 2 != 0)))))
	{
		update_quote_count(**prompt, &count_squotes, &count_dquotes);
		(*dummy_token) = 0;
		len++;
		(*prompt)++;
	}
	(*prompt) -= rewind - ft_strlen(*prompt);
	upd_temp_and_prompt(prompt, len, temp);
	if (token_is_operator(token))
		(*temp) = NULL;
}

static void	fill_in_token(t_token_list *list, \
char **prompt, int *token, char **temp)
{
	int		dummy_token;

	dummy_token = 0;
	while (*prompt && ft_isspace(**prompt))
		(*prompt)++;
	if (*prompt && !is_token(prompt, &dummy_token, 0))
	{
		if (*prompt && !token_is_operator(*token) && ft_isquote(**prompt))
			deal_with_text(list, prompt, temp, 0);
		else
			create_args_for_token(prompt, *token, &dummy_token, temp);
	}
	else
		(*temp) = NULL;
}

int	prompt_to_list(t_token_list *list, char *prompt)
{
	int		token;
	char	*temp;

	while (*prompt)
	{
		temp = NULL;
		token = 0;
		while (*prompt && ft_isspace(*prompt))
			prompt++;
		if (*prompt && is_token(&prompt, &token, 1))
		{
			fill_in_token(list, &prompt, &token, &temp);
			token_list_in_tail(list, token, temp);
		}
		else if (*prompt)
			deal_with_text(list, &prompt, &temp, 1);
	}
	return (1);
}
