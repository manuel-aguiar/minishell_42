/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rm_unguarded_quotes.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 13:51:18 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 13:53:50 by codespace        ###   ########.fr       */
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

int	remove_unguarded_quotes(char **str, int *has_guards)
{
	int		len;
	char	*new;
	int		i;
	int		j;

	setup_guard_array(*str);
	i = 0;
	j = 0;
	while ((*str)[i])
		if ((*str)[i++] != -1)
			j++;
	len = i;
	if (j == len)
	{
		if (has_guards)
			*has_guards = 0;
		return (1);
	}
	new = malloc(sizeof(*new) * (j + 1));
	if (!new)
		return (perror_msg_int("malloc", 0));
	i = 0;
	j = 0;
	while (i < len)
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