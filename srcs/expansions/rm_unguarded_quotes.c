/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rm_unguarded_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 13:51:18 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 13:50:39 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	setup_guard_array(char *str)
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
			{
				open = str[i];
				str[i] = -1;
			}
			else if (open && str[i] == open)
			{
				str[i] = -1;
				open = 0;
			}
		}
		i++;
	}
	return (1);
}

static int	remove_quotes_copy_replace(char **str, int *has_guards, \
			int oldlen, int newlen)
{
	char	*new;
	int		i;
	int		j;

	new = malloc(sizeof(*new) * (newlen + 1));
	if (!new)
		return (perror_msg_int("malloc", 0));
	i = 0;
	j = 0;
	while (i < oldlen)
	{
		if ((*str)[i] != -1)
			new[j++] = (*str)[i];
		i++;
	}
	new[j] = '\0';
	free(*str);
	*str = new;
	if (has_guards)
		*has_guards = 1;
	return (1);
}

int	remove_unguarded_quotes(char **str, int *has_guards)
{
	int		oldlen;
	int		newlen;
	int		i;

	setup_guard_array(*str);
	i = 0;
	newlen = 0;
	while ((*str)[i])
		if ((*str)[i++] != -1)
			newlen++;
	oldlen = i;
	if (newlen == oldlen)
	{
		if (has_guards)
			*has_guards = 0;
		return (1);
	}
	return (remove_quotes_copy_replace(str, has_guards, oldlen, newlen));
}
