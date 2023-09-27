/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:42:58 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/27 16:20:07 by codespace        ###   ########.fr       */
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

int	dollar_exit_status(t_dollars *dol, char **to_expand, \
	int *index, int dol_len)
{
	char	*itoa;
	char	*new;

	itoa = ft_itoa(dol->ms->exit_status);
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

int	dollar_delete_jump(char **to_expand, int *index, int jump)
{
	char	*new;

	(*to_expand)[*index] = '\0';
	new = ft_strjoin(*to_expand, &((*to_expand)[*index + 2 - jump]));
	if (!new)
		return (0);
	free(*to_expand);
	*to_expand = new;
	return (1);
}

int	dollar_search_replace(t_dollars *dol, char **to_expand, int *index)
{
	if ((*to_expand)[*index + 1] == '?')
		return (dollar_exit_status(dol, to_expand, index, 2));
	if (!(*to_expand)[*index + 1] || ft_isspace((*to_expand)[*index + 1]) \
	|| (dol->quote && (*to_expand)[*index + 1] == dol->quote))
	{
		(*index)++;
		return (1);
	}
	if ((*to_expand)[*index + 1] && !ft_isalpha((*to_expand)[*index + 1]) \
	&& dol->neg)
		return (dollar_delete_jump(to_expand, index, \
		((*to_expand)[*index + 1] == '\'' \
		|| (*to_expand)[*index + 1] == '\"')));
	dol->dol_len = 0;
	while ((*to_expand)[*index + 1 + dol->dol_len] && \
	ft_isalnum((*to_expand)[*index + 1 + dol->dol_len]))
		dol->dol_len++;
	if (!dol->dol_len)
	{
		(*index)++;
		return (1);
	}
	return (dollar_search_env(dol, to_expand, index));
}

static void	dollar_save_quote_move(t_dollars *dol, char **to_expand, \
			int *index)
{
	if (!dol->quote)
		dol->quote = (*to_expand)[*index];
	else if (dol->quote == (*to_expand)[*index])
		dol->quote = 0;
	(*index)++;
}

int	expand_dollars(char **to_expand, t_ms *ms, int turn_negative)
{
	int			i;
	t_dollars	dol;

	dol.ms = ms;
	dol.neg = turn_negative;
	dol.dol_len = 0;
	dol.quote = 0;
	i = 0;
	while ((*to_expand)[i])
	{
		if ((*to_expand)[i] == '\'' || (*to_expand)[i] == '"')
			dollar_save_quote_move(&dol, to_expand, &i);
		else if ((*to_expand)[i] == '$' && (*to_expand)[i + 1] \
		&& !ft_isspace((*to_expand)[i + 1]) \
		&& (*to_expand)[i + 1] != '$' \
		&& (!dol.quote || dol.quote == '"'))
		{
			if (!dollar_search_replace(&dol, to_expand, &i))
				return (0);
		}
		else
			i++;
	}
	return (1);
}
