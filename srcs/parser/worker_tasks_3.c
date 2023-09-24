/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_tasks_3.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/24 12:41:01 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 13:02:25 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	empty_quotes(char *str)
{
	int	i;
	int	open;

	open = 0;
	i = 0;
	while (str[i])
	{
		if (str[i] == '"' || str[i] == '\'')
		{
			if (!open)
				open = str[i];
			else if (open && str[i] == open)
				open = 0;
		}
		else if (open)
			str[i] = -1;
		i++;
	}
	return (1);
}

int	worker_args_split_helper(char ***split_place, t_token_node *arg, int *move)
{
	char    *copy;

	copy = ft_strdup(arg->text);
	if (!copy)
		return (perror_msg_int("malloc", 0));
	empty_quotes(copy);
	*split_place = ft_split_count_replenish(copy, arg->text, "\t\v\n\r ", move);
	free(copy);
	if (!*split_place)
		return (perror_msg_int("malloc", 0));
	if (*move == 1)
	{
		ft_free_charmat_null(split_place, free);
		return (1);
	}
	return (1);
}

int	worker_args_split_add_token(t_block *worker, t_token_node *arg, int *move)
{
	int				i;
	char			**split;
	t_token_node	*new;

	if (!worker_args_split_helper(&split, arg, move))
		return (0);
	if (*move == 1)
		return (1);
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
