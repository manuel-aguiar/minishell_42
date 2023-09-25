/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:36:31 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/25 19:38:11 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	expand_wildcards(char **to_expand, int *count, char ***place_split)
{
	char	**split;

	split = wildcard(*to_expand, ft_strlen(*to_expand), count);
	if (!split)
		return (0);
	if (split)
		*place_split = split;
	else
		ft_free_charmat_null(&split, free);
	return (1);
}
