/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_fit_candidates.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 13:50:47 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/25 16:00:10 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_char_fit(char *char_test, char char_pat)
{
	if ((char_pat < 0 && -(*char_test) == char_pat) \
	|| (char_pat >= 0 && *char_test == char_pat))
	{
		(*char_test) *= -1;
		return (1);
	}
	return (0);
}

/*
static int	wc_mandatory_end(char *file, char *sub_pat, int last_index)
{
	int	i;
	int	j;

	j = ft_strlen(sub_pat) - 1;
	i = ft_strlen(file) - 1;
	while (i > last_index)
	{
		if (check_char_fit(&file[i], sub_pat[j]) && j >= 0)
			j--;
		if (j < 0)
			return (1);
		i--;
	}
	if (j >= 0)
	{
		printf(" didnt pass mandatory end\n");
		return (0);
	}
	return (1);
}



static int	wc_mandatory_start(char *file, char *pattern)
{
	int	i;

	i = 0;
	while (file[i])
	{
		if (!check_char_fit(&file[i], pattern[i]))
			break ;
		i++;
	}
		
	if (pattern[i])
		return (0);
	return (1);
}

int	wildcard_fit_check(t_wildc *wc, char *file)
{
	printf("candidate: [%s]", file);
	wc->k = 0;
	if (wc->pattern[0] != '*')
	{
		if (!wc_mandatory_start(file, wc->sub_pats[wc->k]))
		{
			printf(" didnt pass mandatory start\n");
			return (0);
		}
		wc->k++;
	}
	wc->i = 0;
	while (wc->sub_pats[wc->k])
	{
		wc->j = 0;
		while (file[wc->i] && file[wc->i] != wc->sub_pats[wc->k][wc->j])
			wc->i++;
		while (file[wc->i + wc->j] && check_char_fit(&file[wc->i + wc->j], \
													wc->sub_pats[wc->k][wc->j]))
			wc->j++;
		if (wc->sub_pats[wc->k][wc->j])
		{
			printf("didn't pass middle [%s]\n", &file[wc->i + wc->j]);
			return (0);
		}
		wc->k++;
	}
	if (wc->pattern[wc->pat_len - 1] != '*' && file[wc->i + wc->j])
		return (wc_mandatory_end(file, wc->sub_pats[wc->k - 1], wc->i));
	return (1);
}
*/

int	wildcard_fit_check(char *pattern, char *text) {
    int n = strlen(text);
    int m = strlen(pattern);
    int i = 0, j = 0, startIndex = -1, match = 0;

	//printf("pattern [%s] candidate: [%s]\n", pattern, text);
    while (i < n) {
        if (j < m && (check_char_fit(&text[i], pattern[j])) && text[i] != '*' && text[i] != -'*') {
            // Characters match or '?' in pattern matches any character.
            i++;
            j++;
        }
        else if (j < m && pattern[j] == '*') {
            // Wildcard character '*', mark the current position in the pattern and the text as a proper match.
            startIndex = j;
            match = i;
            j++;
        }
        else if (startIndex != -1) {
            // No match, but a previous wildcard was found. Backtrack to the last '*' character position and try for a different match.
            j = startIndex + 1;
            match++;
            i = match;
        }
        else {
            // If none of the above cases comply, the pattern does not match.
            return false;
        }
    }

    // Consume any remaining '*' characters in the given pattern.
    while (j < m && pattern[j] == '*') {
        j++;
    }

    // If we have reached the end of both the pattern and the text, the pattern matches the text.
    return j == m;
}