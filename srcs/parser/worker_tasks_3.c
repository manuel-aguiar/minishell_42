/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 12:41:01 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:35:47 by codespace        ###   ########.fr       */
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

int	worker_args_rm_unguarded_quotes(t_block *worker)
{
	t_token_node	*cur;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!remove_unguarded_quotes(&cur->text, NULL))
			return (0);
		cur = cur->next;
	}
	return (1);
}

int	worker_dump_tasks_to_cmd_args(t_block *worker)
{
	int				i;
	int				total_args;
	t_token_node	*cur;
	char			**cmd_args;

	total_args = worker->prompt->len;
	if (!total_args)
		return (0);
	cmd_args = malloc(sizeof(*cmd_args) * (total_args + 1));
	if (!cmd_args)
		return (perror_msg_int("malloc", 0));
	i = 0;
	cur = worker->prompt->head;
	while (i < total_args)
	{
		cmd_args[i] = cur->text;
		cur->text = NULL;
		cur = cur->next;
		i++;
	}
	cmd_args[i] = NULL;
	worker->cmd_args = cmd_args;
	token_list_destroy(&worker->prompt);
	return (1);
}
