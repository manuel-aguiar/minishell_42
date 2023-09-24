/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:09:22 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 12:29:46 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	worker_task_preparation(t_block *worker)
{
	if (!worker->prompt->head)
	{
		token_list_destroy(&worker->prompt);
		return (1);
	}
	if (!worker_args_expand_dollar_wildcard(worker))
		return (0);
	if (!worker_args_split_unguarded_quotes(worker))
		return (0);
	if (!worker_args_rm_unguarded_quotes(worker))
		return (0);
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
