/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:18:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/25 19:42:14 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	worker_args_expand_wildcard_split(t_block *worker)
{
	int				i;
	int				move;
	char			*dup;
	char			**split;
	t_token_node	*cur;
	t_token_node	*new;

	cur = worker->prompt->head;
	while (cur)
	{
		dup = ft_strdup(cur->text);
		if (!dup)
			return (0);
		if (!expand_wildcards(&cur->text, &move, &split))
			return (0);
		if (move > 1)
		{
			//printf("move is %d\n", move);
			free(dup);
			free(cur->text);
			cur->text = split[0];
			//printf("arg %d is [%s]\n", 0, split[0]);
			i = 1;
			while (i < move)
			{
				//printf("arg %d is [%s]\n", i, split[i]);
				//remove_unguarded_quotes(&split[i], NULL);
				new = new_token_node(T_ARG, split[i++]);
				if (!new)
				{
					ft_free_charmat_null(&split, free);
					return (0);
				}
				token_list_insert_after(worker->prompt, cur, new);
				cur = cur->next;
			}
			ft_free_set_null(&split);
		}
		else
		{
			ft_free_charmat_null(&split, free);
			free(cur->text);
			cur->text = dup;
			cur = cur->next;
		}
	}
	return (1);
}

int	worker_args_expand_dollar_split(t_block *worker)
{
	int				i;
	int				move;
	char			**split;
	t_token_node	*cur;
	t_token_node	*new;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!expand_dollars(&cur->text, worker->ms, true))
			return (0);
		count_split_after_dollar(&split, cur->text, &move);
		if (!move)
		{
			new = cur->next;
			ft_free_charmat_null(&split, free);
			token_list_del_node(worker->prompt, cur);
			cur = new;
		}
		else
		{
			if (!remove_unguarded_quotes_wildcard(&cur->text, NULL))
				return (0);
			free(cur->text);
			cur->text = split[0];
			if (move > 1)
			{
				i = 1;
				while (i < move)
				{
					new = new_token_node(T_ARG, split[i++]);
					if (!new)
					{
						ft_free_charmat_null(&split, free);
						return (0);
					}
					token_list_insert_after(worker->prompt, cur, new);
					cur = cur->next;
				}
			}
			else
				cur = cur->next;
			ft_free_set_null(&split);
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
