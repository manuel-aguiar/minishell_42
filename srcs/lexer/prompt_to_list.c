/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_to_list.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 15:42:02 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/18 15:54:13 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token_list.h"

void	deals_with_text(t_token_list *list, char **prompt, char **temp)
{
	int		len;
	int		rewind;
	int		dummy_token;

	len = 0;
	dummy_token = 0;
	rewind = ft_strlen(*prompt);
	while (**prompt && !ft_isspace(**prompt) \
	&& !is_token(prompt, &dummy_token, 0))
	{
		dummy_token = 0;
		len++;
		(*prompt)++;
	}
	while (!prompt)
		(*prompt)--;
	(*prompt) -= rewind - ft_strlen(*prompt);
	if (len > 0)
	{
		(*temp) = ft_strdup_len(*prompt, len);					//malloc
		(*prompt) += len;
		token_list_in_tail(list, T_ARG, (*temp));				//malloc
	}
}

void	deals_with_quotes(char **prompt, char **temp)
{
	int		len;
	char	*str;
	char	current;

	len = 0;
	current = **prompt;
	str = (*prompt);
	str++;
	while (str[len] && !ft_isspace(str[len]))
	{
		while (str[len] && str[len] != current)
			len++;
		if (str[len] && str[len] == current)
			len++;
	}
	(*temp) = ft_strdup_len((*prompt), len + 1);					//malloc
	(*prompt) += len;
	if (**prompt && **prompt + 1)
		(*prompt)++;
}

void	create_args_for_token(char **prompt, \
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
		update_quote_count(prompt, &count_squotes, &count_dquotes);
		(*dummy_token) = 0;
		len++;
		(*prompt)++;
	}
	(*prompt) -= rewind - ft_strlen(*prompt);
	if (len > 0)
		(*temp) = ft_strdup_len((*prompt), len);								//malloc
	(*prompt) += len;
	if (token_is_operator(token))
		(*temp) = NULL;
}

void	fill_in_token(char **prompt, int *token, char **temp)
{
	int		dummy_token;

	dummy_token = 0;
	while (*prompt && ft_isspace(**prompt))
		(*prompt)++;
	if (*prompt && !is_token(prompt, &dummy_token, 0))
	{
		if (*prompt && !token_is_operator(*token) && ft_isquote(**prompt))
			deals_with_quotes(prompt, temp);
		else
			create_args_for_token(prompt, *token, &dummy_token, temp);
	}
	else
		(*temp) = NULL;
}

//NO CHECKSSSSSSSSSS

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
			fill_in_token(&prompt, &token, &temp);
			token_list_in_tail(list, token, temp);
		}
		else if (*prompt && ft_isquote(*prompt))
		{
			deals_with_quotes(&prompt, &temp);
			token_list_in_tail(list, T_ARG, temp);
		}
		else if (*prompt)
			deals_with_text(list, &prompt, &temp);
	}
	return (1);
}
/*
int	main(void)
{
	char			*line;
	t_token_list	*list;

	while (1)
	{
		line = readline("line: ");
		if (!line)
			break ;
		list = token_list_new();
		prompt_to_list(list, line);
		free(line);
		token_list_head_print(list, print_token);
		token_list_destroy(&list);
	}
	return (0);
}
*/
