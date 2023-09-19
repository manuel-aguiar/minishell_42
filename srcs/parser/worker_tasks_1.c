/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:09:22 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 10:20:24 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int worker_task_preparation(t_block *worker)
{
	if (!worker->prompt->head)
	{
		token_list_destroy(&worker->prompt);
		return (1);
	}
    if (!worker_args_expand_dollar_wildcard(worker))
        return (0);
    if (!worker_args_rm_quotes_and_split(worker))
        return (0);
	if (!worker_dump_tasks_to_cmd_args(worker))
		return (0);
	worker->cmd = ft_strdup(worker->cmd_args[0]);
	if (!worker->cmd)
		return (0);
    return (1);
}

int	worker_extract_redirections(t_block *worker)
{
	t_token_node *cur;

	cur = worker->prompt->head;
	while (cur)
	{
		if (token_is_redirection(cur))
		{
			if (!worker->io_files)
				worker->io_files = token_list_new();
			if (!worker->io_files)
				return (0);
			cur = move_node_to_list_and_retrive_next(worker->io_files, worker->prompt, cur);
		}
		else
			cur = cur->next;
	}
	return (1);
}

int setup_guard_array(char *str)
{
    int i;
    int open;

    open = 0;
    i = 0;
    while (str[i])
    {
        if (str[i] == '"' || str[i] == '\'')
        {
            if (!open)
            {
                open = str[i];
                str[i] = -1;
            }
            else if (open && str[i] == open)
            {
                str[i] = -1;
                open = 0;
            }
        }
        i++;
    }
    return (1);
}

int remove_unguarded_quotes(char **str, int *has_guards)
{
    int len;
    char *new;
    int i;
    int j;

    setup_guard_array(*str);
    i = 0;
    j = 0;
    while ((*str)[i])
        if ((*str)[i++] != -1)
            j++;
    len = i;
    if (j == len)
    {
        if (has_guards)
            *has_guards = 0;
        return (1);
    }
    new = malloc(sizeof(*new) * (j + 1));
    if (!new)
        return (0);
    i = 0;
    j = 0;
    while (i < len)
    {
        if ((*str)[i] != -1)
            new[j++] = (*str)[i];
        i++;
    }
    new[j] = '\0';
    free(*str);
    *str = new;
    if (has_guards)
        *has_guards = 1;
    return (1);
}


