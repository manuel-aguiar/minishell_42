/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:42:58 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/22 12:44:01 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	dollar expansion functions


	all of these serve to insert on the cmd_arg/redirection the corresponding expansion of $
	according to the env variables and/or exit status,
	all the functions take the place where the original argument is (char **).
	This intent is to make the code more modular:
		-   both cmd and block can use these when dealing with their redirections.
		-   all that is required is that there is a pointer to t_ms available to search env
		in the case of expanding $


*/

int	dollar_exit_status(char **to_expand, t_ms *ms, int *index, int dol_len)
{
	char	*itoa;
	char	*new;

	//check_for_signals(ms);
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

int	dollar_search_env(char **to_expand, t_ms *ms, int *index, int dol_len)
{
	int		i;
	char	*new;

	i = 0;
	(*to_expand)[*index] = '\0';
	if (ms->env)
	{
		while (ms->env[i])
		{
			if (!ft_strncmp(&((*to_expand)[*index + 1]), ms->env[i], dol_len) \
			&& ms->env[i][dol_len] == '=')
			{
				new = ft_triple_join(*to_expand, &ms->env[i][dol_len + 1], &((*to_expand)[*index + 1 + dol_len]));
				if (!new)
					return (0);
				free(*to_expand);
				(*to_expand) = new;
				*index += ft_strlen(&ms->env[i][dol_len + 1]);
				return (1);
			}
			i++;
		}
	}
	new = ft_triple_join(*to_expand, "\0", &((*to_expand)[*index + 1 + dol_len]));
	if (!new)
		return (0);
	free(*to_expand);
	*to_expand = new;
	return (1);
}

int	dollar_search_replace(char **to_expand, t_ms *ms, int *index)
{
	int	len;

	if (!(*to_expand)[*index + 1] || (*to_expand)[*index + 1] == ' ')
		return (1);
	if ((*to_expand)[*index + 1] == '?')
		return (dollar_exit_status(to_expand, ms, index, 2));
	len = 0;
	while ((*to_expand)[*index + 1 + len] && (*to_expand)[*index + 1 + len] != ' ' \
	&& (*to_expand)[*index + 1 + len] != '\'' && (*to_expand)[*index + 1 + len] != '"' \
	&& (*to_expand)[*index + 1 + len] != '$')
		len++;
	return (dollar_search_env(to_expand, ms, index, len));
}

int	expand_dollars(char **to_expand, t_ms *ms)
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
		else if ((*to_expand)[i] == '$' && (*to_expand)[i + 1] \
				&& (!quote || quote == '"'))
		{
			if (!dollar_search_replace(to_expand, ms, &i))
				return (0);
		}
		else
			i++;
	}
	return (1);
}

int	here_doc_expand_dollars(char **to_expand, t_ms *ms)
{
	int	i;
	int	quote;

	quote = 0;
	i = 0;
	while ((*to_expand)[i])
	{
		if ((*to_expand)[i] == '$' && (!quote || quote == '"'))
		{
			if (!dollar_search_replace(to_expand, ms, &i))
				return (0);
		}
		else
			i++;
	}
	return (1);
}
