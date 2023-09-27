/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_concat.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 15:53:21 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 16:00:34 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	dollar_check_malloc_and_replace(t_dollars *dol, char **to_expand)
{
	if (!dol->new_final)
		return (0);
	free(*to_expand);
	*to_expand = dol->new_final;
	dol->new_final = NULL;
	return (1);
}

static int	negative_copy(char *str, char **place_neg_copy)
{
	int		i;
	char	*new;

	new = malloc(ft_strlen(str) + 1);
	if (!new)
		return (perror_msg_int("malloc", 0));
	i = 0;
	while (str[i])
	{
		if (ft_isspace(str[i]))
			new[i] = str[i];
		else
			new[i] = -str[i];
		i++;
	}
	new[i] = '\0';
	*place_neg_copy = new;
	return (1);
}

static int	dol_neg_copy(t_dollars *dol, char **to_expand, \
			int *index, int env_index)
{
	if (!negative_copy(&dol->ms->env[env_index][dol->dol_len + 1], \
	&dol->neg_copy))
		return (0);
	dol->new_final = ft_triple_join(*to_expand, dol->neg_copy, \
	&((*to_expand)[*index + 1 + dol->dol_len]));
	ft_free_set_null(&dol->neg_copy);
	if (!dol->new_final)
		return (0);
	return (1);
}

static int	dol_pos_copy(t_dollars *dol, char **to_expand, \
			int *index, int env_index)
{
	dol->new_final = ft_triple_join(*to_expand, \
	&dol->ms->env[env_index][dol->dol_len + 1], \
	&((*to_expand)[*index + 1 + dol->dol_len]));
	if (!dol->new_final)
		return (0);
	return (1);
}

int	dollar_search_env(t_dollars *dol, char **to_expand, int *index)
{
	int		i;

	i = 0;
	(*to_expand)[*index] = '\0';
	if (dol->ms->env)
	{
		while (dol->ms->env[i])
		{
			if (!ft_strncmp(&((*to_expand)[*index + 1]), dol->ms->env[i], \
			dol->dol_len) && dol->ms->env[i][dol->dol_len] == '=')
			{
				if ((dol->neg && !dol_neg_copy(dol, to_expand, index, i)) \
				|| (!dol->neg && !dol_pos_copy(dol, to_expand, index, i)))
					return (0);
				*index += ft_strlen(&dol->ms->env[i][dol->dol_len + 1]);
				break ;
			}
			i++;
		}
	}
	if (!dol->ms->env || !dol->ms->env[i])
		dol->new_final = ft_strjoin(*to_expand, \
		&((*to_expand)[*index + 1 + dol->dol_len]));
	return (dollar_check_malloc_and_replace(dol, to_expand));
}
