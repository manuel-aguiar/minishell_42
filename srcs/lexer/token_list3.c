/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 18:29:11 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/23 10:43:42 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	token_list_del_node(t_token_list *list, t_token_node *node)
{
	if (!list || !(list->head) || !node)
		return ;
	if (node->prev)
		node->prev->next = node->next;
	else
		list->head = node->next;
	if (node->next)
		node->next->prev = node->prev;
	else
		list->tail = node->prev;
	if (node->text)
		free(node->text);
	free(node);
	--(list->len);
}

void	token_list_insert_after(t_token_list *list, \
		t_token_node *after, t_token_node *node)
{
	if (!list || !(list->head) || !after || !node)
		return ;
	node->next = after->next;
	node->prev = after;
	if (after->next)
		after->next->prev = node;
	after->next = node;
	if (list->tail == after)
		list->tail = node;
	++(list->len);
}

void	token_list_move_top_to_new(t_token_list *to, t_token_list *from, \
		t_token_node *until, int count)
{
	if (!to || !from || !until || !(from->head))
		return ;
	to->head = from->head;
	to->tail = until;
	from->head = until->next;
	if (from->head)
		from->head->prev = NULL;
	else
	{
		from->tail = NULL;
		from->head = NULL;
	}
	until->next = NULL;
	to->len += count;
	from->len -= count;
	return ;
}

static void		disconnect_node(t_token_list *to, \
				t_token_list *from, t_token_node *target)
{
	if (target->prev)
		target->prev->next = target->next;
	else
		from->head = target->next;
	if (target->next)
		target->next->prev = target->prev;
	else
		from->tail = target->prev;
	if (!to->tail)
	{
		to->head = target;
		to->tail = target;
		target->prev = NULL;
	}
	else
	{
		to->tail->next = target;
		target->prev = to->tail;
		to->tail = target;
	}
	target->next = NULL;
}

t_token_node	*move_node_to_list_and_retrive_next(t_token_list *to, \
				t_token_list *from, t_token_node *target)
{
	t_token_node	*retrieve;

	retrieve = target->next;
	disconnect_node(to, from, target);
	++(to->len);
	--(from->len);
	if (from->len == 0)
	{
		from->head = NULL;
		from->tail = NULL;
	}
	return (retrieve);
}
