/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hashtable.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/20 09:18:43 by codespace         #+#    #+#             */
/*   Updated: 2023/09/20 11:17:26 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HASHTABLE_H

# define HASHTABLE_H

# include "error_handling.h"
# include "ft_memfunc.h"
# include "ht_linkedlists.h"
# include <stdlib.h>

typedef struct s_hashtable
{
	t_htlist	**collision;
	int			size;
	int			(*hash_func)(void *);
	int			(*key_compare)(void *, void *);
}				t_hashtable;

t_hashtable		*hashtable_init(int size, int (*hash_func)(void *),
					int (*key_compare)(void *, void *));
int				hashtable_contains(t_hashtable *table, void *key);
int				hashtable_insert(t_hashtable *table, void *key, void *values);
int				hashtable_remove(t_hashtable *table, void *key,
					void (*del_key)(void *), void (*del_value)(void *));
void			hashtable_destroy(t_hashtable **table, void (*del_key)(void *),
					void (*del_value)(void *));

#endif