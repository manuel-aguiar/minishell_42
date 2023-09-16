/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list3.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 18:29:11 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 19:56:06 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	token_list_del_node(t_token_list *list, t_token_node *node)
{
	t_token_node	*cur;

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

void	token_list_insert_after(t_token_list *list, t_token_node *after, t_token_node *node)
{
	t_token_node	*cur;

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

void	token_list_move_top_to_new(t_token_list *to, t_token_list *from, t_token_node *until, int count)
{
	t_token_node *save_source;

	if (!to || !from || !until || !(from->head))
		return ;
	to->head = from->head;
	to->tail = until;
	from->head = until->next;
	if (from->head)
		from->head->prev = NULL;
	else
		from->tail = NULL;
	until->next = NULL;
	to->len += count;
	from->len -= count;
	return ;
}
