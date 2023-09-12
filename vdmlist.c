#include "libft.h"




t_vdmnode	*new_vdmnode(void *data)
{
	t_vdmnode	*new;

	new = malloc(sizeof(*new));
	if (!new)
		return (NULL);
	new->data = data;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}

t_vdmlist	*vdmlist_new(void)
{
	t_vdmlist	*list;

	list = malloc(sizeof(*list));
	if (!list)
		return (NULL);
	list->head = NULL;
	list->tail = NULL;
	list->len = 0;
	return (list);
}

int	vdmlist_in_head(t_vdmlist *list, void *data)
{
	t_vdmnode	*new_node;

	if (!list)
		return (0);
	new_node = new_vdmnode(data);
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

int	vdmlist_in_tail(t_vdmlist *list, void *data)
{
	t_vdmnode	*new_node;

	if (!list)
		return (0);
	new_node = new_vdmnode(data);
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

static void	vdmlist_removeif_help(t_vdmlist *list, t_vdmnode *cur, \
									void (*del)(void *))
{
	if (cur->prev && list->len > 1)
		cur->prev->next = cur->next;
	else
	{
		list->head = cur->next;
		cur->next->prev = NULL;
	}
	if (cur->next && list->len > 1)
		cur->next->prev = cur->prev;
	else
	{
		list->tail = cur->prev;
		cur->prev->next = NULL;
	}
	if (del && cur->data)
		del(cur->data);
	free(cur);
	--(list->len);
}

void	vdmlist_remove_if(t_vdmlist *list, void *data, \
					int (*cmp)(void *, void *), void (*del)(void *))
{
	t_vdmnode	*cur;
	t_vdmnode	*next;

	if (!list || !list->head)
		return ;
	cur = list->head;
	while (cur)
	{
		next = cur->next;
		if (!cmp(cur->data, data))
		{
		    if (list->len == 1)
		    {
		        if (del && cur->data)
		            del(cur->data);
		        free(cur);
		        list->head = NULL;
		        list->tail = NULL;
		        --(list->len);
		    }
		    else
			    vdmlist_removeif_help(list, cur, del);
		}
		cur = next;
	}
}

void	vdmlist_del_head(t_vdmlist *list, void (*del)(void *))
{
	t_vdmnode	*cur;

	if (!list || !(list->head))
		return ;
	cur = list->head;
	if (cur->next)
		cur->next->prev = NULL;
	else
		list->tail = NULL;
	list->head = cur->next;
	if (del)
		del(cur->data);
	free(cur);
	--(list->len);
}

void	vdmlist_del_tail(t_vdmlist *list, void (*del)(void *))
{
	t_vdmnode	*cur;

	if (!list || !(list->tail))
		return ;
	cur = list->tail;
	if (cur->prev)
		cur->prev->next = NULL;
	else
		list->head = NULL;
	list->tail = cur->prev;
	if (del)
		del(cur->data);
	free(cur);
	--(list->len);
}

void	vdmlist_destroy(t_vdmlist **list, void (*del)(void *))
{
	t_vdmnode	*delete;
	t_vdmnode	*next;

	if (!list || !*list)
		return ;
	delete = (*list)->head;
	while (delete)
	{
		next = delete->next;
		if (del)
			del(delete->data);
		free(delete);
		delete = next;
	}
	ft_free_set_null(list);
}

void	vdmlist_head_print(t_vdmlist *list, void (*pnt)(void *))
{
	t_vdmnode	*cur;

	if (!list)
		return ;
	cur = list->head;
	while (cur)
	{
		pnt(cur->data);
		cur = cur->next;
	}
}

void	vdmlist_tail_print(t_vdmlist *list, void (*pnt)(void *))
{
	t_vdmnode	*cur;

	if (!list)
		return ;
	cur = list->tail;
	while (cur)
	{
		pnt(cur->data);
		cur = cur->prev;
	}
}

int	vdmlist_find(t_vdmlist *list, void *data, int (*cmp)(void *, void *))
{
	t_vdmnode	*cur;

	if (!list || !(list->head) || !cmp)
		return (0);
	cur = list->head;
	while (cur)
	{
		if (!cmp(cur->data, data))
			return (1);
		cur = cur->next;
	}
	return (0);
}