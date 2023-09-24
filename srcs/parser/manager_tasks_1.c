/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   manager_tasks_1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:04:12 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 14:05:06 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	manager_extract_redirections(t_block *manager)
{
	int				i;
	t_token_node	*last;

	if (!manager->prompt || !manager->prompt->tail \
	|| !token_is_redirection(manager->prompt->tail))
		return (1);
	manager->io_files = token_list_new();
	if (!manager->io_files)
		return (0);
	last = manager->prompt->tail;
	manager->io_files->tail = last;
	i = 1;
	while (last && token_is_redirection(last->prev) && ++i)
		last = last->prev;
	manager->io_files->head = last;
	manager->prompt->tail = last->prev;
	if (last->prev)
		last->prev->next = NULL;
	else
		manager->prompt->head = NULL;
	last->prev = NULL;
	manager->io_files->len += i;
	manager->prompt->len -= i;
	return (1);
}

int	manager_subshell_and_arithmatic(t_block *manager)
{
	int				remove_corner;

	remove_corner = 0;
	if (manager->prompt->len >= 2)
	{
		remove_corner = (manager->prompt->head->type == T_OPEN_PAR
				&& manager->prompt->tail->type == T_CLOSE_PAR);
	}
	if (manager->prompt->len >= 4 && remove_corner
		&& manager->prompt->head->next->type == T_OPEN_PAR
		&& manager->prompt->tail->prev->type == T_CLOSE_PAR)
		manager_check_arithmatic_parenthesis(manager);
	if (remove_corner)
	{
		token_list_del_head(manager->prompt);
		token_list_del_tail(manager->prompt);
	}
	return (1);
}

int	manager_check_arithmatic_parenthesis(t_block *manager)
{
	t_token_node	*cur;
	int				open_par;

	open_par = 0;
	cur = manager->prompt->head->next;
	while (cur)
	{
		open_par += (cur->type == T_OPEN_PAR);
		open_par -= (cur->type == T_CLOSE_PAR);
		if (open_par == 0)
		{
			manager->has_arithmatic_parenthesis = (cur->next
					&& cur->next->type == T_CLOSE_PAR);
			return (1);
		}
		cur = cur->next;
	}
	return (0);
}

void	manager_destroy_worker_tasks(t_block *manager)
{
	int	i;

	if (!manager->worker_tasks)
		return ;
	i = 0;
	while (i < manager->op_count + 1)
	{
		if (manager->worker_tasks[i])
			token_list_destroy(&manager->worker_tasks[i]);
		i++;
	}
	ft_free_set_null(&manager->worker_tasks);
}
