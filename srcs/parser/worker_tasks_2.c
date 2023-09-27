/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:18:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:39:03 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	expansion_remove_empty_arg(t_block *worker, \
		t_token_node **cur, char ***split_place)
{
	t_token_node	*new;

	new = (*cur)->next;
	ft_free_charmat_null(split_place, free);
	token_list_del_node(worker->prompt, *cur);
	*cur = new;
}

static void	expand_dollar_move_next(t_token_node **cur, \
		char ***split_place)
{
	ft_free_set_null(split_place);
	(*cur) = (*cur)->next;
}

int	worker_args_expand_dollar_split(t_block *worker)
{
	int				move;
	char			**split;
	t_token_node	*cur;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!expand_dollars(&cur->text, worker->ms, true))
			return (0);
		count_split_after_dollar(&split, cur->text, &move);
		if (!move)
			expansion_remove_empty_arg(worker, &cur, &split);
		else
		{
			free(cur->text);
			cur->text = split[0];
			if (move > 1 \
			&& !worker_expansion_add_tokens(worker, &cur, &split, move))
				return (0);
			if (move <= 1)
				expand_dollar_move_next(&cur, &split);
		}
	}
	return (1);
}
