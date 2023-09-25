/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_return.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 13:51:53 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/25 16:02:11 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	count_chars(char *str, char c)
{
	int	i;
	int	count;

	count = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == c)
			count++;
		i++;
	}
	return (count);
}

void	*destroy_wildcard(t_wildc *wildcard, int clean_exit)
{
	if (wildcard->files)
		vdmlist_destroy(&wildcard->files, free);
	if (wildcard->test)
		ft_free_set_null(&wildcard->test);
	if (wildcard->split)
		ft_free_charmat_null(&wildcard->split, free);
	if (!clean_exit && wildcard->join)
		ft_free_set_null(wildcard->join);
	if (wildcard->filename)
		ft_free_set_null(&wildcard->filename);
	if (wildcard->copy_dir)
		ft_free_set_null(&wildcard->copy_dir);
	return (NULL);
}

static int	init_wildcard_struct_on_stack(t_wildc *wildcard, char *pattern, \
			int pat_len)
{
	wildcard->pattern = pattern;
	wildcard->pat_len = pat_len;
	wildcard->match_count = 0;
	wildcard->depth = count_chars(pattern, '/');
	wildcard->files = vdmlist_new();
	if (!wildcard->files)
		return (0);
	wildcard->test = NULL;
	wildcard->split = NULL;
	wildcard->join = NULL;
	wildcard->entry = NULL;
	wildcard->filename = NULL;
	wildcard->copy_dir = NULL;
	return (1);
}

void		set_negative(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		str[i] *= -1;
		i++;
	}
}

void	wildcard_list_set_negative(t_vdmlist *files)
{
	t_vdmnode	*cur;

	cur = files->head;
	while (cur)
	{
		set_negative((char *)cur->data);
		cur = cur->next;
	}
}

void	void_putstr(void *str)
{
	char *my_str;
	my_str = (char *)str;
	printf("[%s]\n", my_str);
}

char	*wildcard(char *pattern, int pat_len, int *match_count)
{
	t_wildc	wildcard;

	if (!init_wildcard_struct_on_stack(&wildcard, pattern, pat_len))
		return (destroy_wildcard(&wildcard, 0));
	if (!list_all_wildcard_matches(&wildcard, ".", 0))
		return (destroy_wildcard(&wildcard, 0));
	wildcard.match_count = wildcard.files->len;
	if (match_count)
		*match_count = wildcard.match_count;
	if (wildcard.match_count == 0)
		wildcard.join = ft_triple_join("\'", pattern, "\'");
	else
	{
		wildcard.split = list_to_array(wildcard.files);
		if (!wildcard.split)
			return (destroy_wildcard(&wildcard, 0));
		wildcard.join = ft_split_join(wildcard.split, " ");
	}
	if (!wildcard.join)
		return (destroy_wildcard(&wildcard, 0));
	destroy_wildcard(&wildcard, 1);
	return (wildcard.join);
}
