/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:17:21 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 16:23:45 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
    dollar expansion functions


    all of these serve to insert on the cmd_arg/redirection the corresponding expansion of $
    according to the env variables and/or exit status,
    all the functions take the place where the original argument is (char **).
    This intent is to make the code more modular:
        -   both cmd and block can use these when dealing with their redirections.
        -   all that is required is that there is a pointer to t_ms available to search env
        in the case of expanding $


*/

int dollar_exit_status(char **to_expand, t_ms *ms, int *index, int dol_len)
{
    char    *itoa;
    char    *new;

    check_for_signals(ms);
    itoa = ft_itoa(ms->exit_status);
    if (!itoa)
        return (0);
    (*to_expand)[*index] = '\0';
    new = ft_triple_join(*to_expand, itoa, &((*to_expand)[*index + dol_len]));
    if (!new)
        return (0);
    *index += ft_strlen(itoa);
    free(itoa);
    free(*to_expand);
    *to_expand = new;
    return (1);
}

int dollar_search_env(char **to_expand, t_ms *ms, int *index, int dol_len)
{
    int     i;
    char    *new;

    i = 0;
    (*to_expand)[*index] = '\0';
    if (ms->env)
    {
        while (ms->env[i])
        {
            if (!ft_strncmp(&((*to_expand)[*index + 1]), ms->env[i], dol_len) \
            && ms->env[i][dol_len] == '=')
            {
                new = ft_triple_join(*to_expand, &ms->env[i][dol_len + 1], &((*to_expand)[*index + 1 + dol_len]));
                if (!new)
                    return (0);
                free(*to_expand);
                (*to_expand) = new;
                *index += ft_strlen(&ms->env[i][dol_len + 1]);
                return (1);
            }
            i++;
        }
    }
    new = ft_triple_join(*to_expand, "\0", &((*to_expand)[*index + 1 + dol_len]));
    if (!new)
        return (0);
    free(*to_expand);
    *to_expand = new;
    return (1);
}

int dollar_search_replace(char **to_expand, t_ms *ms, int *index)
{
    int len;

    if (!(*to_expand)[*index + 1] || (*to_expand)[*index + 1] == ' ')
        return (1);
    if ((*to_expand)[*index + 1] == '?')
        return (dollar_exit_status(to_expand, ms, index, 2));
    len = 0;
    while ((*to_expand)[*index + 1 + len] && (*to_expand)[*index + 1 + len] != ' ' \
    && (*to_expand)[*index + 1 + len] != '\'' && (*to_expand)[*index + 1 + len] != '"' \
    && (*to_expand)[*index + 1 + len] != '$')
        len++;
    return (dollar_search_env(to_expand, ms, index, len));
}

int expand_dollars(char **to_expand, t_ms *ms)
{
    int i;
    int quote;

    quote = 0;
    i = 0;
    while ((*to_expand)[i])
    {
        if ((*to_expand)[i] == '\'' || (*to_expand)[i] == '"')
        {
            if (!quote)
                quote = (*to_expand)[i];
            else if (quote == (*to_expand)[i])
                quote = 0;
            i++;
        }
        else if ((*to_expand)[i] == '$' && (!quote || quote == '"'))
        {
            if (!dollar_search_replace(to_expand, ms, &i))
                return (0);
        }
        else
            i++;
    }
    return (1);
}

int here_doc_expand_dollars(char **to_expand, t_ms *ms)
{
    int i;
    int quote;

    quote = 0;
    i = 0;
    while ((*to_expand)[i])
    {
        if ((*to_expand)[i] == '$' && (!quote || quote == '"'))
        {
            if (!dollar_search_replace(to_expand, ms, &i))
                return (0);
        }
        else
            i++;
    }
    return (1);
}

/*
    wildcard_search_replace
    function to find and insert on the cmd_arg/redirection the expansion of the corresponding
    wildcard.
    "wildcard" function is only called from within this function, all the functionally of wildcard.c
    ends up on this single function with no further entry points.
    "wildcard" reports on the number of matches via "&count" and so this caller can determine whether the
    result will be ambiguous or not and free all the intermediate helpers on the way and exit.
    fail_return will be passed to "expand_wildcards" in case of non-NULL, and will give the caller of "expand_wildcards"
    the wildcard that failed in producing a single valid file descriptor.

*/

int wildcard_search_replace(char **to_expand, int *index, char **fail_return)
{
    char    *join;
    char    *new;
    int     count;
    int     start;
    int     end;

    start = *index;
    while (start > 0 && (*to_expand)[start - 1] != ' ' \
    && (*to_expand)[start - 1] != '<' && (*to_expand)[start - 1] != '>')
        start--;
    end = *index;
    while ((*to_expand)[end] && (*to_expand)[end] != ' ')
        end++;
    join = wildcard(&(*to_expand)[start], end - start, &count);
    if (count > 1 && fail_return)
    {
        (*to_expand)[end] = '\0';
        *fail_return = ft_triple_join("\'", &((*to_expand)[start]), "\'");
        free(join);
        return (1);
    }
    *to_expand[start] = '\0';
    new = ft_triple_join(*to_expand, join, &((*to_expand)[end]));
    if (!new)
        return (0);
    *index += (ft_strlen(join) - *index + start);
    free(*to_expand);
    free(join);
    *to_expand = new;

    return (1);

}


/*
    expand_wildcards

    function used to expand wildcards in cmd_args and redirections. It is only used after the command has been into individua
    portions meaning that redirections and cmd args are processed seperatelly:
        -   failure in the file descriptors due to ambiguous redirection means that the rest of the cmd_args
            dont need to be analysed and one can stop here.
    fail_return is used only when checking within redirections as the name of the fuull wildcard is needed to print
    to console in terms of ambiguous redirection. When called to check cmd_args, the caller calls the function with NULL here.
    This function takes a char ** (the place where the pointer to the string to expand is place) in an effort to
    make the code more modular by alllowing the use for both cmd_arg expansion and redirection expansion.

*/

int expand_wildcards(char **to_expand, char **fail_return)
{
    int i;
    int quote;

    quote = 0;
    i = 0;
    while ((*to_expand)[i])
    {
        if ((*to_expand)[i] == '\'' || (*to_expand)[i] == '"')
        {
            if (!quote)
                quote = (*to_expand)[i];
            else if (quote == (*to_expand)[i])
                quote = 0;
            i++;
        }
        else if ((*to_expand)[i] == '*' && !quote)
        {
            if (!wildcard_search_replace(to_expand, &i, fail_return))
                return (0);
            if (fail_return && *fail_return)
                break ;
        }
        else
            i++;
    }
    return (1);
}
