/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenlist2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:59:23 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/16 17:32:41 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token_list.h"

void	token_list_del_head(t_token_list *list)
{
	t_token_node	*cur;

	if (!list || !(list->head))
		return ;
	cur = list->head;
	if (cur->next)
		cur->next->prev = NULL;
	else
		list->tail = NULL;
	list->head = cur->next;
	if (cur->text)
		free(cur->text);
	free(cur);
	--(list->len);
}

void	token_list_del_tail(t_token_list *list)
{
	t_token_node	*cur;

	if (!list || !(list->tail))
		return ;
	cur = list->tail;
	if (cur->prev)
		cur->prev->next = NULL;
	else
		list->head = NULL;
	list->tail = cur->prev;
	if (cur->text)
		free(cur->text);
	free(cur);
	--(list->len);
}

void	token_list_destroy(t_token_list **list)
{
	t_token_node	*delete;
	t_token_node	*next;

	if (!list || !*list)
		return ;
	delete = (*list)->head;
	while (delete)
	{
		next = delete->next;
		if (delete->text)
			free(delete->text);
		free(delete);
		delete = next;
	}
	ft_free_set_null(list);
}

void	token_list_head_print(t_token_list *list, void (*pnt)(t_token_node *))
{
	t_token_node	*cur;

	if (!list)
		return ;
	cur = list->head;
	while (cur)
	{
		pnt(cur);
		cur = cur->next;
	}
}

void	token_list_tail_print(t_token_list *list, void (*pnt)(t_token_node *))
{
	t_token_node	*cur;

	if (!list)
		return ;
	cur = list->tail;
	while (cur)
	{
		pnt(cur);
		cur = cur->prev;
	}
}
