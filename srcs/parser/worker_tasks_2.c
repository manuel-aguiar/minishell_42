/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:18:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:19:51 by codespace        ###   ########.fr       */
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
				cur = cur->next;
		}
	}
	return (1);
}

int	empty_quotes(char *str)
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
				open = str[i];
			else if (open && str[i] == open)
				open = 0;
		}
		else if (open)
			str[i] = -1;
		i++;
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
