/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hashtable_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 10:59:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/20 11:28:36 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hashtable.h"

void	*hashtable_return_value(t_hashtable *table, void *key)
{
	int			index;
	t_htnode	*cur;

	if (!table)
		return (NULL);
	index = table->hash_func(key) % table->size;
	if (!table->collision[index])
		return (NULL);
	cur = table->collision[index]->head;
	while (cur)
	{
		if (!table->key_compare(key, cur->key))
			return (cur->value);
		cur = cur->next;
	}
	return (NULL);
}
