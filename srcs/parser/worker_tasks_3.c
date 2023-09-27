/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 12:41:01 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:20:10 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	wildcard_replace_no_match(t_token_node **cur, \
			char ***split_place, char **dup_place)
{
	ft_free_charmat_null(split_place, free);
	free((*cur)->text);
	(*cur)->text = *dup_place;
	(*cur) = (*cur)->next;
}

static int	wildcard_expand_setup_dup(t_token_node *cur, \
			int *move, char ***split_place, char **dup_place)
{
	*dup_place = ft_strdup(cur->text);
	if (!*dup_place)
		return (0);
	if (!remove_unguarded_quotes_wildcard(&cur->text, NULL) \
	|| !expand_wildcards(&cur->text, move, split_place))
	{
		ft_free_set_null(dup_place);
		return (0);
	}
	return (1);
}

int	worker_args_expand_wildcard_split(t_block *worker)
{
	int				move;
	char			*dup;
	char			**split;
	t_token_node	*cur;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!wildcard_expand_setup_dup(cur, &move, &split, &dup))
			return (0);
		if (move > 0)
		{
			ft_free_set_null(&dup);
			free(cur->text);
			cur->text = split[0];
			if (!worker_expansion_add_tokens(worker, &cur, &split, move))
				return (0);
		}
		else
			wildcard_replace_no_match(&cur, &split, &dup);
	}
	return (1);
}
