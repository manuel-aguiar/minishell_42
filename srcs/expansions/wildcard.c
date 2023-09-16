/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:15:04 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 16:16:49 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "minishell.h"

/*
    wildcard struct used just for the wildcards
    will have to be put in minishell.h

*/




/*

    void putstr just to print the vdmlist, helper, wont be used in final.

*/


void    void_putstr(void *str)
{
    char *mine;

    mine = (char *)str;
    write(1, mine, ft_strlen(mine));
    printf("\n");
}

char    *triple_join(char *first, char *second, char *third)
{
    int     i;
    char    *new;

    new = malloc((ft_strlen(first) + ft_strlen(second) \
		+ ft_strlen(third) + 1) * sizeof(*first));
    if (!new)
		return (perror_msg_ptr("malloc", NULL));
    i = 0;
    while (*first)
        new[i++] = *first++;
    while (*second)
        new[i++] = *second++;
    while (*third)
        new[i++] = *third++;
    new[i] = '\0';
    return (new);
}

static int wc_mandatory_end(char *file, char *sub_pat, int last_index)
{
    int	i;
	int	j;

	j = ft_strlen(sub_pat) - 1;
    i = ft_strlen(file) - 1;
    while (i > last_index)
    {
		if (file[i] == sub_pat[j] && j >= 0)
			j--;
		if (j < 0)
			return (1);
		i--;
	}
	if (j >= 0)
		return (0);
    return (1);
}

static int wc_mandatory_start(char *file, char *pattern)
{
    int i;

    i = 0;
    while (file[i] && file[i] == pattern[i])
        i++;
    if (pattern[i])
        return (0);
    return (1);
}


int wildcard_fit_check(t_wildc *wc, char *file)
{
    int         i;
    int         j;
    int         k;

    k = 0;
    if (wc->pattern[0] != '*')
    {
        if (!wc_mandatory_start(file, wc->sub_pats[k]))
            return (0);
        k++;
    }
    i = 0;
    while (wc->sub_pats[k])
    {
        j = 0;
        while (file[i] && file[i] != wc->sub_pats[k][j])
            i++;
        while (file[i + j] && file[i + j] == wc->sub_pats[k][j])
            j++;
        if (wc->sub_pats[k][j])
            return (0);
        k++;
    }
    if (wc->pattern[wc->pat_len - 1] != '*' && file[i + j])
        return (wc_mandatory_end(file, wc->sub_pats[k - 1], i));
    return (1);
}

int	close_dir_return(DIR	*dir)
{
	closedir(dir);
	return (0);
}

int	check_fit_and_copy(t_wildc *wildc, char *path, char *filename)
{
	if (!ft_strncmp(path, ".", 1))
		wildc->filename = ft_strdup(filename);
	else
		wildc->filename = triple_join(path, "/", filename);
	if (!wildc->filename)
		return (0);
	if (wildcard_fit_check(wildc, wildc->filename))
	{
		if (!vdmlist_in_tail(wildc->files, wildc->filename))
			return (0);
		wildc->filename = NULL;
	}
	else
		ft_free_set_null(&wildc->filename);
	return (1);
}

int list_all_wildcard_matches(t_wildc *wildc, char *path, int cur_lvl)
{
	DIR             *dir;

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


char    **list_to_array(t_vdmlist *list)
{
    char        **split;
    int         len;
    int         i;
    t_vdmnode   *cur;

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

char    *split_join(char **split, char *sep)
{
    int     len;
    int     i;
    int     j;
    int     k;
    char    *join;

    i = 0;
    len = 0;
    while (split[i])
        len += ft_strlen(split[i++]);
    len += (ft_strlen(sep) * (i - 1));
    join = malloc(sizeof(*join) * (len + 1));
    if (!join)
		return (perror_msg_ptr("malloc", NULL));
    k = 0;
    i = 0;
    while (split[i])
    {
        j = 0;
        while (split[i][j])
            join[k++] = split[i][j++];
        j = 0;
        if (split[i + 1])
            while (sep[j])
                join[k++] = sep[j++];
        i++;
    }
    join[k] = '\0';
    return (join);
}



int count_chars(char *str, char c)
{
    int i;
    int count;

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
	if (wildcard->sub_pats)
		ft_free_charmat_null(&wildcard->sub_pats, free);
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

int	init_wildcard_struct_on_stack(t_wildc *wildcard, char *pattern, int pat_len)
{
    wildcard->pattern = pattern;
	wildcard->pat_len = pat_len;
	wildcard->sub_count = 0;
	wildcard->match_count = 0;
    wildcard->depth = count_chars(pattern, '/');
    wildcard->sub_pats = ft_split_count(wildcard->pattern, " *", &wildcard->sub_count);
    if (!wildcard->sub_pats)
        return (0);
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

char	*wildcard(char *pattern, int pat_len, int *match_count)
{
    t_wildc     wildcard;

	if (!init_wildcard_struct_on_stack(&wildcard, pattern, pat_len))
		return (destroy_wildcard(&wildcard, 0));
    if (!list_all_wildcard_matches(&wildcard, ".", 0))
		return (destroy_wildcard(&wildcard, 0));
    wildcard.match_count = wildcard.files->len;
    if (wildcard.match_count == 0)
        wildcard.join = triple_join("\'", pattern, "\'");
    else
    {
        wildcard.split = list_to_array(wildcard.files);
		if (!wildcard.split)
			return (destroy_wildcard(&wildcard, 0));
        wildcard.join = split_join(wildcard.split, " ");
    }
	if (!wildcard.join)
		return (destroy_wildcard(&wildcard, 0));
    *match_count = wildcard.match_count;
	destroy_wildcard(&wildcard, 1);
    return (wildcard.join);
}
