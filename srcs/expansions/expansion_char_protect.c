/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion_char_protect.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 14:36:16 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:54:05 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	empty_quotes(char *str)
{
	int	i;
	int	open;

	open = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
		{
			if (!open)
				open = str[i];
			else if (open && str[i] == open)
				open = 0;
		}
		else if (open)
			str[i] = -1;
		i++;
	}
	return (1);
}

void	turn_positive(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < 0)
			str[i] *= -1;
		i++;
	}
}

int	count_split_after_dollar(char ***split_place, char *redir_copy, int *count)
{
	char	*copy;
	char	**split;

	copy = ft_strdup(redir_copy);
	if (!copy)
		return (perror_msg_int("malloc", 0));
	empty_quotes(copy);
	split = ft_split_count_replenish(copy, redir_copy, "\t\v\n\r ", count);
	free(copy);
	if (split_place)
		*split_place = split;
	else
		ft_free_charmat_null(&split, free);
	return (1);
}
