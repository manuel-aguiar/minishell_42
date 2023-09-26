/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rm_unguarded_wildcard.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 13:51:18 by codespace         #+#    #+#             */
/*   Updated: 2023/09/25 19:44:18 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	check_if_wildcard(char *str, int *index, int quote)
{
	int	i;
	int	found;

	i = 1;
	found = 0;
	while (str[i] && str[i] != quote)
	{
		if (str[i] == '*')
			found = 1;
		i++;
	}
	if (!found)
	{
		str[0] = -1;
		str[i] = -1;
	}
	*index += (i + 1);
}

static int	setup_guard_array(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
			check_if_wildcard(&str[i], &i, str[i]);
		else
			i++;
	}
	return (1);
}

static int	remove_quotes_copy_replace(char **str, int *has_guards, int oldlen,
		int newlen)
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

int	remove_unguarded_quotes_wildcard(char **str, int *has_guards)
{
	int	oldlen;
	int	newlen;
	int	i;

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
