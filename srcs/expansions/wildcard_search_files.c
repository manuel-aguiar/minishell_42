/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_search_files.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 13:52:13 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/25 16:01:53 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	close_dir_return(DIR *dir)
{
	closedir(dir);
	return (0);
}

static int	check_fit_and_copy(t_wildc *wildc, char *path, char *filename)
{
	if (!ft_strncmp(path, ".", 1))
		wildc->filename = ft_strdup(filename);
	else
		wildc->filename = ft_triple_join(path, "/", filename);
	if (!wildc->filename)
		return (0);
	if (wildcard_fit_check(wildc->pattern, wildc->filename))
	{
		//printf("candidate [%s] approoved\n", wildc->filename);
		if (!vdmlist_in_tail(wildc->files, wildc->filename))
			return (0);
		wildc->filename = NULL;
	}
	else
		ft_free_set_null(&wildc->filename);
	return (1);
}

int	list_all_wildcard_matches(t_wildc *wildc, char *path, int cur_lvl)
{
	DIR	*dir;

	dir = opendir(path);
	if (!dir)
		return (0);
	while (1)
	{
		wildc->entry = readdir(dir);
		if (!wildc->entry)
			break ;
		if (ft_strncmp(wildc->entry->d_name, ".", 1) \
		&& ft_strncmp(wildc->entry->d_name, "..", 2))
		{
			if (!check_fit_and_copy(wildc, path, wildc->entry->d_name))
				return (close_dir_return(dir));
			if (wildc->entry->d_type == DT_DIR && cur_lvl + 1 <= wildc->depth \
			&& !list_all_wildcard_matches(wildc, wildc->entry->d_name, \
				cur_lvl + 1))
				return (close_dir_return(dir));
			free(wildc->filename);
		}
	}
	closedir(dir);
	return (1);
}

char	**list_to_array(t_vdmlist *list)
{
	char		**split;
	int			len;
	int			i;
	t_vdmnode	*cur;

	if (!list)
		return (NULL);
	len = list->len;
	split = malloc(sizeof(*split) * (len + 1));
	if (!split)
		return (perror_msg_ptr("malloc", NULL));
	i = 0;
	cur = list->head;
	while (i < len)
	{
		split[i++] = (char *)cur->data;
		cur->data = NULL;
		cur = cur->next;
	}
	split[i] = NULL;
	return (split);
}
