/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:18:35 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 14:57:17 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    set_in_between_to(char *pmt, char start, char end, char newchar)
{
    int i;
    int open;

    open = 0;
    i = 0;
    while (pmt[i])
    {
        while (pmt[i] && pmt[i] != start)
            i++;
        if (pmt[i])
        {
            open = 1;
            i++;
            while (pmt[i] && open)
            {
                if (pmt[i] == start)
                    open++;
                if (pmt[i] == end)
                {
                    open--;
                    if (end == start)
                        open--;
                }
                if (open)
                    pmt[i] = newchar;
                i++;
            }
        }
    }
}

int	worker_args_split_add_token(t_block *worker, t_token_node *arg, int *move)
{
	int				i;
	char			**split;
	t_token_node	*new;
	char			*copy;

	copy = ft_strdup(arg->text);
	if (!copy)
		return (perror_msg("malloc"));
	set_in_between_to(copy, '"', '"', -1);
	set_in_between_to(copy, '\'', '\'', -1);
	split = ft_split_count_replenish(copy, arg->text, "\t\v\n\r ", move);
	free(copy);
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

int	worker_args_split_unguarded_quotes(t_block *worker)
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
		if (!worker_args_split_add_token(worker, cur, &move))
			return (0);
		while (move-- > 0)
			cur = cur->next;
	}
	return (1);
}

int	worker_args_rm_unguarded_quotes(t_block *worker)
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
		if (!remove_unguarded_quotes(&cur->text, NULL))
			return (0);
		cur = cur->next;
	}
	return (1);
}

/*expansion is unprotected */

int	worker_args_expand_dollar_wildcard(t_block *worker)
{
	t_token_node	*cur;

	cur = worker->prompt->head;
	while (cur)
	{
		expand_dollars(&cur->text, worker->ms);
		expand_wildcards(&cur->text, NULL);
		cur = cur->next;
	}
	return (1);
}

int	worker_dump_tasks_to_cmd_args(t_block *worker)
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
	while (i < total_args)
	{
		cmd_args[i] = cur->text;
		cur->text = NULL;
		cur = cur->next;
		i++;
	}
	cmd_args[i] = NULL;
	worker->cmd_args = cmd_args;
	token_list_destroy(&worker->prompt);
	return (1);
}