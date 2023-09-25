/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_to_list2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 15:42:02 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/25 08:44:11 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	get_len_of_text_arg(char **prompt)
{
	int		len;
	int		dummy_token;
	int		dummy_res;
	int		count_squotes;
	int		count_dquotes;

	len = 0;
	dummy_token = 0;
	count_squotes = 0;
	count_dquotes = 0;
	dummy_res = is_token(prompt, &dummy_token, 0);
	while (**prompt && ((!dummy_res && !ft_isspace(**prompt)) || \
	(((ft_isspace(**prompt) || dummy_res) && (count_squotes % 2 != 0 \
	|| count_dquotes % 2 != 0)))))
	{
		update_quote_count(**prompt, &count_squotes, &count_dquotes);
		dummy_token = 0;
		len++;
		(*prompt)++;
		dummy_res = is_token(prompt, &dummy_token, 0);
	}
	return (len);
}

void	deal_with_text(t_token_list *list, \
char **prompt, char **temp, int to_create)
{
	int		len;
	int		rewind;

	len = 0;
	rewind = ft_strlen(*prompt);
	len = get_len_of_text_arg(prompt);
	while (!prompt)
		(*prompt)--;
	(*prompt) -= rewind - ft_strlen(*prompt);
	if (len > 0)
	{
		(*temp) = ft_strdup_len(*prompt, len);
		if (!(*temp))
			return ;
		(*prompt) += len;
		if (to_create)
			token_list_in_tail(list, T_ARG, (*temp));
	}
}
