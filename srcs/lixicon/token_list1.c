/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list1.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:32:56 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 17:33:07 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token_list.h"

t_token_node	*new_token_node(int type, char *text)
{
	t_token_node	*new;

	new = malloc(sizeof(*new));
	if (!new)
		return (perror_msg_ptr("malloc"));
	new->next = NULL;
	new->prev = NULL;
	new->type = type;
	new->text = text;
	return (new);
}

t_token_list	*token_list_new(void)
{
	t_token_list	*list;

	list = malloc(sizeof(*list));
	if (!list)
		return (perror_msg_ptr("malloc"));
	list->head = NULL;
	list->tail = NULL;
	list->len = 0;
	return (list);
}

int	token_list_in_head(t_token_list *list, int type, char *text)
{
	t_token_node	*new_node;

	if (!list)
		return (0);
	new_node = new_token_node(type, text);
	if (!new_node)
		return (0);
	if (list->head)
	{
		new_node->next = list->head;
		list->head->prev = new_node;
	}
	else
		list->tail = new_node;
	list->head = new_node;
	++(list->len);
	return (1);
}

int	token_list_in_tail(t_token_list *list, int type, char *text)
{
	t_token_node	*new_node;

	if (!list)
		return (0);
	new_node = new_token_node(type, text);
	if (!new_node)
		return (0);
	if (list->tail)
	{
		new_node->prev = list->tail;
		list->tail->next = new_node;
	}
	else
		list->head = new_node;
	list->tail = new_node;
	++(list->len);
	return (1);
}
