/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:18:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/25 16:09:56 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	worker_args_split_unguarded_quotes(t_block *worker)
{
	int				move;
	t_token_node	*cur;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!*(cur->text))
			cur = cur->next;
		else
		{
			if (!worker_args_split_add_token(worker, cur, &move))
				return (0);
			while (move-- > 0)
				cur = cur->next;
		}
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

int	worker_args_expand_dollar_wildcard(t_block *worker)
{
	t_token_node	*cur;
	t_token_node	*next;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!*(cur->text))
			cur = cur->next;
		else
		{
			expand_dollars(&cur->text, worker->ms, true);
			remove_unguarded_quotes_wildcard(&cur->text, NULL);
			expand_wildcards(&cur->text, NULL);
			remove_unguarded_quotes(&cur->text, NULL);
			if (!*(cur->text))
			{
				next = cur->next;
				token_list_del_node(worker->prompt, cur);
				cur = next;
			}
			else
				cur = cur->next;
		}
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
