/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:42:58 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/26 10:20:32 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	dollar expansion functions


	all of these serve to insert on the cmd_arg/redirection the 
	corresponding expansion of $
	according to the env variables and/or exit status,
	all the functions take the place where the original argument
	 is (char **).
	This intent is to make the code more modular:
		-   both cmd and block can use these when dealing with 
		their redirections.
		-   all that is required is that there is a pointer to 
		t_ms available to search env
		in the case of expanding $


*/

int	dollar_exit_status(char **to_expand, t_ms *ms, int *index, int dol_len)
{
	char	*itoa;
	char	*new;

	itoa = ft_itoa(ms->exit_status);
	if (!itoa)
		return (0);
	(*to_expand)[*index] = '\0';
	new = ft_triple_join(*to_expand, itoa, &((*to_expand)[*index + dol_len]));
	if (!new)
		return (0);
	*index += ft_strlen(itoa);
	free(itoa);
	free(*to_expand);
	*to_expand = new;
	return (1);
}

static int	dollar_check_malloc_and_replace(char **to_expand, char *new)
{
	if (!new)
		return (0);
	free(*to_expand);
	*to_expand = new;
	return (1);
}

static int negative_copy(char *str, char **place_neg_copy)
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

int	dollar_search_env(char **to_expand, t_ms *ms, int *index, int dol_len, int turn_negative)
{
	int		i;
	char	*new;
	char	*neg_copy;

	i = 0;
	(*to_expand)[*index] = '\0';
	if (ms->env)
	{
		while (ms->env[i])
		{
			if (!ft_strncmp(&((*to_expand)[*index + 1]), ms->env[i], dol_len) \
			&& ms->env[i][dol_len] == '=')
			{
				if (turn_negative && !negative_copy(&ms->env[i][dol_len + 1], &neg_copy))
					return (0);
				if (turn_negative)
					new = ft_triple_join(*to_expand, neg_copy, \
					&((*to_expand)[*index + 1 + dol_len]));
				else
					new = ft_triple_join(*to_expand, &ms->env[i][dol_len + 1], \
					&((*to_expand)[*index + 1 + dol_len]));
				if (turn_negative)
					free(neg_copy);
				*index += ft_strlen(&ms->env[i][dol_len + 1]);
				break ;
			}
			i++;
		}
	}
	if (!ms->env || !ms->env[i])
		new = ft_strjoin(*to_expand, &((*to_expand)[*index + 1 + dol_len]));
	return (dollar_check_malloc_and_replace(to_expand, new));
}

int	dollar_search_replace(char **to_expand, t_ms *ms, int *index, int turn_negative)
{
	int	len;

	if (!(*to_expand)[*index + 1] || ft_isspace((*to_expand)[*index + 1]))
	{
		(*index)++;
		return (1);
	}
	if ((*to_expand)[*index + 1] == '?')
		return (dollar_exit_status(to_expand, ms, index, 2));
	len = 0;
	while ((*to_expand)[*index + 1 + len] && \
	ft_isalnum((*to_expand)[*index + 1 + len]))
		len++;
	return (dollar_search_env(to_expand, ms, index, len, turn_negative));
}

int	expand_dollars(char **to_expand, t_ms *ms, int turn_negative)
{
	int	i;
	int	quote;

	quote = 0;
	i = 0;
	while ((*to_expand)[i])
	{
		if ((*to_expand)[i] == '\'' || (*to_expand)[i] == '"')
		{
			if (!quote)
				quote = (*to_expand)[i];
			else if (quote == (*to_expand)[i])
				quote = 0;
			i++;
		}
		else if ((*to_expand)[i] == '$' && !ft_isspace((*to_expand)[i + 1])  \
		&& (*to_expand)[i + 1] && (*to_expand)[i + 1] != '$' \
		&& (!quote || quote == '"'))
		{
			if (!dollar_search_replace(to_expand, ms, &i, turn_negative))
				return (0);
		}
		else
			i++;
	}
	return (1);
}
