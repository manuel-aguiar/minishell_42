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
    
    new = malloc((ft_strlen(first) + ft_strlen(second) + ft_strlen(third) + 1) * sizeof(*first));
    if (!new)
    {
        perror("malloc");
        return (0);
    }
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

void file_list(t_vdmlist *files, char *path, int cur_lvl, int max_lvl)
{
    struct dirent   *entry;
    DIR             *dir;
    char            *filename;
    
    dir = opendir(path);
    if (!dir)
    {
        perror("opendir");
        return ;                               
    }
    while (1)
    {
        entry = readdir(dir);
        if (!entry)
            break ;
        if (ft_strncmp(entry->d_name, ".", 1) \
        && ft_strncmp(entry->d_name, "..", 2))
        {
            if (!ft_strncmp(path, ".", 1))
                filename = ft_strdup(entry->d_name);                                        //perror         
            else
                filename = triple_join(path, "/", entry->d_name);                           //perror
            if (entry->d_type == DT_REG)
                vdmlist_in_tail(files, filename);
            if (entry->d_type == DT_DIR)
            {
                vdmlist_in_tail(files, ft_strdup(filename));
                if (cur_lvl + 1 <= max_lvl)
                    file_list(files, filename, cur_lvl + 1, max_lvl);
                free(filename);
            }
        }
    }
    closedir(dir);
}

int wc_mandatory_end(char *file, char *sub_pat, int last_index)
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

int wc_mandatory_start(char *file, char *pattern)
{
    int i;

    i = 0;
    while (file[i] && file[i] == pattern[i])
        i++;
    if (pattern[i])
        return (0);
    return (1);
}


int wildcard_fit(void *file, void *wildcard)
{
    t_wildc     *wc;
    int         i;
    int         j;
    int         k;

    wc = (t_wildc *)wildcard;
    wc->test = (char *)file;
    k = 0;
    if (wc->pattern[0] != '*')
    {
        if (!wc_mandatory_start(wc->test, wc->sub_pats[k]))
            return (0);
        k++;
    }
    i = 0;
    while (wc->sub_pats[k])
    {
        j = 0;
        while (wc->test[i] && wc->test[i] != wc->sub_pats[k][j])
            i++;
        while (wc->test[i + j] && wc->test[i + j] == wc->sub_pats[k][j])
            j++;
        if (wc->sub_pats[k][j])
            return (0);
        k++;
    }
    if (wc->pattern[wc->pat_len - 1] != '*' && wc->test[i + j])
        return (wc_mandatory_end(wc->test, wc->sub_pats[k - 1], i));
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
    {
        perror("malloc");
        return (0);
    }
    i = 0;
    cur = list->head;
    while (i < len)
    {
        split[i++] = (char *)cur->data;
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
    {
        perror("malloc");
        return (0);
    }
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



char     *wildcard(char *pattern, int pat_len, int *match_count)
{
    t_wildc     wildcard;
    
    wildcard.pattern = pattern;
    wildcard.depth = count_chars(pattern, '/');
    wildcard.sub_pats = ft_split_count(wildcard.pattern, " *", &wildcard.sub_count);
    if (!wildcard.sub_pats)
    {
        perror("malloc");
        return (0);
    }
    wildcard.pat_len = pat_len;
    wildcard.files = vdmlist_new();
    wildcard.test = NULL;
    if (!wildcard.files)
    {
        perror("malloc");
        return (0);
    }
    wildcard.test = NULL;
    wildcard.split = NULL;
    wildcard.join = NULL;

    file_list(wildcard.files, ".", 0, wildcard.depth);
    
   /*
    t_vdmnode *cur;
    
    cur = wildcard.files->head;
    while (cur)
    {
        printf(" file [%s], fits? %d\n", (char *)cur->data, wildcard_fit(cur->data, &wildcard));
        cur = cur->next;
    }
    
    vdmlist_head_print(wildcard.files, void_putstr);
    */
    
    vdmlist_remove_if(wildcard.files, &wildcard, wildcard_fit, free);
    
    ft_free_charmat_null(&wildcard.sub_pats, free); 
    
    wildcard.match_count = wildcard.files->len;
    if (wildcard.match_count == 0)
        wildcard.join = triple_join("\'", pattern, "\'");
    else
    {
        wildcard.split = list_to_array(wildcard.files);
        wildcard.join = split_join(wildcard.split, " ");            //totally unprotected
        ft_free_charmat_null(&wildcard.split, free);
    }
    
    //vdmlist_head_print(wildcard.files, void_putstr);
    
    //totally unprotected
    vdmlist_destroy(&wildcard.files, NULL);             //totally unprotected
    
                //totally unprotected
    //printf("the winners are: %s\n", wildcard.join);
    *match_count = wildcard.match_count;
    return (wildcard.join);
}