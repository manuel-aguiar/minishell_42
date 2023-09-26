/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:36:31 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/26 12:54:26 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	expand_wildcards(char **to_expand, int *count, char ***place_split)
{
	char	**split;

	if (**to_expand == '/' || !count_chars(*to_expand, '*'))
	{
		*count = 0;
		split = ft_calloc(2, sizeof(*split));
		if (!split)
			return (0);
		split[0] = ft_strdup(*to_expand);
		if (!split[0])
		{
			ft_free_charmat_null(&split, free);
			return (0);
		}
	}
	else
		split = wildcard(*to_expand, ft_strlen(*to_expand), count);
	if (!split)
		return (0);
	if (place_split)
		*place_split = split;
	else
		ft_free_charmat_null(&split, free);
	return (1);
}
