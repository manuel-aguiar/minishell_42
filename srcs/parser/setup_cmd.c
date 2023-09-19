/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_cmd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 21:27:03 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/17 14:53:12 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

int cmd_args_split_add_token(t_block *worker, t_token_node *arg, int *move)
{
    int     		i;
    char    		**split;
	t_token_node	*new;

	split = ft_split_count(arg->text, "\t\v\n\r ", move);
    //printf("printing [%s] and split\n", arg->text);
    //int j = 0;
    //while (split[j])
    //    printf("[%s] ", split[j++]);
    //printf ("\n");
    if (!split)
        return (perror_msg("malloc"));
	if (*move == 1)
	{
		ft_free_charmat_null(&split, free);
		return (1);
	}
	free(arg->text);
	arg->text = split[0];
	i = 1;
	while (i < *move)
	{
		new = new_token_node(T_ARG, split[i++]);
		if (!new)
		{
			ft_free_charmat_null(&split, free);
			return (0);
		}
		token_list_insert_after(worker->prompt, arg, new);
		arg = arg->next;
	}
	ft_free_set_null(&split);
    return (1);
}



/*
    cmd_args_rm_quotes_and_split
    This function removes the outter quotes of each argument.
    If there are no outter quotes but the argument can be
    split in spaces due to dollar/wildcard expansion, a split will be made
    into further individual arguments.
*/

int cmd_args_rm_quotes_and_split(t_block *worker)
{
    int				i;
    int				has_guards;
	int				move;
	t_token_node	*cur;

	cur = worker->prompt->head;
    has_guards = 0;
    i = 0;
    while (cur)
    {
        if (!remove_unguarded_quotes(&cur->text, &has_guards))
            return (0);
        if (!has_guards)
        {
            if (!cmd_args_split_add_token(worker, cur, &move))
                return (0);
			while (--move > 0)
				cur = cur->next;
        }
		cur = cur->next;
    }
    //printf("args list after expansion split: \n");
    //token_list_head_print(worker->prompt, print_token_args);
    return (1);
}

int cmd_args_expand_dollar_wildcard(t_block *worker)
{
    t_token_node	*cur;

    cur = worker->prompt->head;
    while (cur)
    {
        //printf("cur arg [%s] \n", cur->text);
        expand_dollars(&cur->text, worker->ms);					// protect
        //printf("cur arg after dolar [%s] \n", cur->text);
        expand_wildcards(&cur->text, NULL);						//protect
        //printf("cur arg after wildcard [%s] \n", cur->text);
        cur = cur->next;
    }
    return (1);
}

int	dump_list_to_cmd_args(t_block *worker)
{
	int				i;
	int				total_args;
	t_token_node	*cur;
	char			**cmd_args;

	total_args = worker->prompt->len;
	cmd_args = malloc(sizeof(*cmd_args) * (total_args + 1));
	if (!cmd_args)
		return (perror_msg_int("malloc", 0));
	i = 0;
	cur = worker->prompt->head;
    //token_list_head_print(worker->prompt, print_token_args);
	while (i < total_args)
	{
		cmd_args[i] = cur->text;
		cur->text = NULL;
        cur = cur->next;
		i++;
	}
	cmd_args[i] = NULL;
    i = 0;
    //printf("printing cmd args dump list\n");
    //while (cmd_args[i])
    //    printf("[%s] ", cmd_args[i++]);
    //printf("\n");
	worker->cmd_args = cmd_args;
	token_list_destroy(&worker->prompt);
	return (1);
}

int worker_task_expansions(t_block *worker)
{
    //printf("checking expansions\n");
    //printf("arg head [%s]\n", worker->prompt->head->text);
	if (!worker->prompt->head)
	{
		token_list_destroy(&worker->prompt);
		return (1);
	}
    if (!cmd_args_expand_dollar_wildcard(worker))
        return (0);
    if (!cmd_args_rm_quotes_and_split(worker))
        return (0);
	if (!dump_list_to_cmd_args(worker))
		return (0);
	worker->cmd = ft_strdup(worker->cmd_args[0]);
	if (!worker->cmd)
		return (0);
    
    return (1);
}
