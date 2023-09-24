/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_fit_candidates.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 13:50:47 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/24 12:28:46 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	wc_mandatory_end(char *file, char *sub_pat, int last_index)
{
	int	i;
	int	j;

	j = ft_strlen(sub_pat) - 1;
	i = ft_strlen(file) - 1;
	while (i > last_index)
	{
		if (file[i] == sub_pat[j] && j >= 0)
			j--;
		if (j < 0)
			return (1);
		i--;
	}
	if (j >= 0)
		return (0);
	return (1);
}

static int	wc_mandatory_start(char *file, char *pattern)
{
	int	i;

	i = 0;
	while (file[i] && file[i] == pattern[i])
		i++;
	if (pattern[i])
		return (0);
	return (1);
}

int	wildcard_fit_check(t_wildc *wc, char *file)
{
	wc->k = 0;
	if (wc->pattern[0] != '*')
	{
		if (!wc_mandatory_start(file, wc->sub_pats[wc->k]))
			return (0);
		wc->k++;
	}
	wc->i = 0;
	while (wc->sub_pats[wc->k])
	{
		wc->j = 0;
		while (file[wc->i] && file[wc->i] != wc->sub_pats[wc->k][wc->j])
			wc->i++;
		while (file[wc->i + wc->j] && file[wc->i + wc->j] \
		== wc->sub_pats[wc->k][wc->j])
			wc->j++;
		if (wc->sub_pats[wc->k][wc->j])
			return (0);
		wc->k++;
	}
	if (wc->pattern[wc->pat_len - 1] != '*' && file[wc->i + wc->j])
		return (wc_mandatory_end(file, wc->sub_pats[wc->k - 1], wc->i));
	return (1);
}
