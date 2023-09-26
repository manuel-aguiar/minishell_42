/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_fit_candidates.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 13:50:47 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/25 19:39:39 by codespace        ###   ########.fr       */
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

int	wildcard_fit_check(char *pattern, char *text)
{
    int n = strlen(text);
    int m = strlen(pattern);
    int i = 0, j = 0, startIndex = -1, match = 0;

	//printf("pattern [%s] candidate: [%s]\n", pattern, text);
    while (i < n)
	{
        if (j < m && (check_char_fit(&text[i], pattern[j])) && text[i] != '*' && text[i] != -'*') {
            // Characters match or '?' in pattern matches any character.
            i++;
            j++;
        }
        else if (j < m && pattern[j] == '*')
		{
            // Wildcard character '*', mark the current position in the pattern and the text as a proper match.
            startIndex = j;
            match = i;
            j++;
        }
        else if (startIndex != -1)
		{
            // No match, but a previous wildcard was found. Backtrack to the last '*' character position and try for a different match.
            j = startIndex + 1;
            match++;
            i = match;
        }
        else
		{
            // If none of the above cases comply, the pattern does not match.
            return false;
        }
    }

    // Consume any remaining '*' characters in the given pattern.
    while (j < m && pattern[j] == '*')
	{
        j++;
    }

    // If we have reached the end of both the pattern and the text, the pattern matches the text.
    return (j == m);
}