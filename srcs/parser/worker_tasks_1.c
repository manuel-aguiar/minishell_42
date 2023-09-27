/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:09:22 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:33:46 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	worker_turn_neg_chars_to_pos(t_token_list *list)
{
	t_token_node	*cur;
	int				i;

	cur = list->head;
	while (cur)
	{
		i = 0;
		while (cur->text[i])
		{
			if (cur->text[i] < 0)
				cur->text[i] *= -1;
			i++;
		}
		cur = cur->next;
	}
}

int	worker_task_preparation(t_block *worker)
{
	if (!worker->prompt->head)
	{
		token_list_destroy(&worker->prompt);
		return (0);
	}
	if (!worker_args_expand_dollar_split(worker))
		return (0);
	if (!worker_args_expand_wildcard_split(worker))
		return (0);
	if (!worker_args_rm_unguarded_quotes(worker))
		return (0);
	worker_turn_neg_chars_to_pos(worker->prompt);
	if (!worker_dump_tasks_to_cmd_args(worker))
		return (0);
	worker->cmd = ft_strdup(worker->cmd_args[0]);
	if (!worker->cmd)
		return (0);
	return (1);
}

int	worker_extract_redirections(t_block *worker)
{
	t_token_node	*cur;

	cur = worker->prompt->head;
	while (cur)
	{
		if (token_is_redirection(cur))
		{
			if (!worker->io_files)
				worker->io_files = token_list_new();
			if (!worker->io_files)
				return (0);
			cur = move_node_to_list_and_retrive_next(worker->io_files,
					worker->prompt, cur);
		}
		else
			cur = cur->next;
	}
	return (1);
}

int	worker_expansion_add_tokens(t_block *worker, t_token_node **cur, \
			char ***split_place, int move)
{
	int				i;
	t_token_node	*new;

	i = 1;
	while (i < move)
	{
		new = new_token_node(T_ARG, (*split_place)[i++]);
		if (!new)
		{
			ft_free_charmat_null(split_place, free);
			return (0);
		}
		token_list_insert_after(worker->prompt, *cur, new);
		*cur = (*cur)->next;
	}
	ft_free_set_null(split_place);
	return (1);
}
