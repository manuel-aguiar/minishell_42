/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   new_setup_cmd.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 21:27:03 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 22:42:57 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int cmd_args_has_space_split(t_prep_cmd *cmd, int *index)
{
    int     i;
    int     count;
    char    **split;
    char    **new;

    i = 0;
    while (cmd->cmd_args[*index][i] && !ft_isspace(cmd->cmd_args[*index][i]))
        i++;
    if (!cmd->cmd_args[*index][i])
        return (1);

    split = ft_split_count(cmd->cmd_args[*index], " ", &count);
    if (!split)
        return (perror_msg("malloc"));
    ft_free_set_null(&cmd->cmd_args[*index]);
    cmd->args_len += count - 1;
    new = malloc(sizeof(*new) * (cmd->args_len + 1));
    if (!new)
    {
        ft_free_charmat_null(&split, free);
        return (perror_msg("malloc"));
    }
    i = 0;
    while (i < cmd->args_len)
    {
        /*new[i] = (char *)(t_ulong)(i < *index) * (t_ulong)cmd->cmd_args[i]  \
                + (t_ulong)(i == *index && i - *index < count) * (t_ulong)split[i - *index]  \
                + (t_ulong)(i < cmd->args_len && i > *index) * (t_ulong)cmd->cmd_args[i - count + 1]);
        i++;*/

        if (i < *index)
        {
            new[i] = cmd->cmd_args[i];
            i++;
        }
        else if (i == *index)
        {
            while (i - *index < count)
            {
                new[i] = split[i - *index];
                i++;
            }
        }
        else if (i < cmd->args_len)
        {
            new[i] = cmd->cmd_args[i - count + 1];
            i++;
        }
    }
    new[i] = NULL;
    ft_free_set_null(&cmd->cmd_args);
    ft_free_set_null(&split);
    cmd->cmd_args = new;
    *index += (count - 1);
    return (1);
}

/*
    cmd_args_rm_quotes_and_split
    This function removes the outter quotes of each argument.
    If there are no outter quotes but the argument can be
    split in spaces due to dollar/wildcard expansion, a split will be made
    into further individual arguments.
*/

int cmd_args_rm_quotes_and_split(t_block *block)
{
    int i;
    int has_guards;

    has_guards = 0;
    i = 0;
    while (cmd->cmd_args[i])
    {
        if (!remove_unguarded_quotes(&cmd->cmd_args[i], &has_guards))
            return (0);
        if (!has_guards)
        {
            if (!cmd_args_has_space_split(cmd, &i))
                return (0);
        }
        i++;
    }
    return (1);
}

int new_cmd_args_expand_dollar_wildcard(t_block *block)
{
    t_token_node	*cur;

    cur = block->prompt->head;
    while (cur)
    {
        expand_dollars(&cur->text, block->ms);
        expand_wildcards(&cur->text, NULL);
        cur = cur->next;
    }
    return (1);
}

int new_manage_cmd_expansions(t_block *block)
{
    if (!new_cmd_args_expand_dollar_wildcard(block))
        return (0);
    if (!cmd_args_rm_quotes_and_split(block))
        return (0);
	/*
    cmd.cmd = ft_strdup(cmd.cmd_args[0]);
    if (!cmd.cmd)
        return (free_cmd_struct(block));
    block->cmd = cmd.cmd;
    block->cmd_args = cmd.cmd_args;*/
    return (1);
}
