/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_fit_candidates.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 13:50:47 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/27 12:56:20 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_char_fit(char *char_test, char char_pat)
{
	if ((char_pat < 0 && -(*char_test) == char_pat) || (char_pat >= 0
			&& *char_test == char_pat))
	{
		(*char_test) *= -1;
		return (1);
	}
	return (0);
}

static int	wildcard_loop_condition(t_wildc *wc, char *text)
{
	if (wc->j < wc->pat_len && (check_char_fit(&text[wc->i], \
	wc->pattern[wc->j])) && text[wc->i] != '*' && text[wc->i] != '*' * (-1))
	{
		wc->i++;
		wc->j++;
	}
	else if (wc->j < wc->pat_len && wc->pattern[wc->j] == '*')
	{
		wc->startind = wc->j;
		wc->match = wc->i;
		wc->j++;
	}
	else if (wc->startind != -1)
	{
		wc->j = wc->startind + 1;
		wc->match++;
		wc->i = wc->match;
	}
	else
		return (0);
	return (1);
}

int	wildcard_fit_check(t_wildc *wc, char *text)
{
	wc->candlen = ft_strlen(text);
	wc->startind = -1;
	wc->match = 0;
	wc->i = 0;
	wc->j = 0;
	while (wc->i < wc->candlen)
	{
		if (!wildcard_loop_condition(wc, text))
			return (0);
	}
	while (wc->j < wc->pat_len && wc->pattern[wc->j] == '*')
		wc->j++;
	return (wc->j == wc->pat_len);
}
