/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dollar_heredoc.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:17:21 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/24 13:36:09 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	here_doc_expand_dollars(char **to_expand, t_ms *ms)
{
	int	i;
	int	quote;

	quote = 0;
	i = 0;
	while ((*to_expand)[i])
	{
		if ((*to_expand)[i] == '$' && (*to_expand)[i + 1] \
		&& (*to_expand)[i + 1] != '$' && (!quote || quote == '"'))
		{
			if (!dollar_search_replace(to_expand, ms, &i))
				return (0);
		}
		else
			i++;
	}
	return (1);
}
