/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/16 16:51:38 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_echo(t_block *block)
{
	int		i;
	int		j;
	int		jump;

	i = 0;
	j = 0;
	jump = 0;
	while (block->cmd_args[++j] && ft_strncmp(block->cmd_args[j], "-n", 2) == 0)
	{
		i = 1;
		while (block->cmd_args[j][i] && block->cmd_args[j][i] == 'n')
			i++;
		if (!block->cmd_args[j][i])
			jump++;
		i = jump;
	}
	while (block->cmd_args[++i])
	{
		if (i > 1 + jump)
			write(block->final_out, " ", 1);
		ft_putstr_fd(block->cmd_args[i], block->final_out);
	}
	if (jump == 0)
		write(block->final_out, "\n", 1);
	return (1);
}

int	run_unset(t_block *block)
{
	int		i;
	int		j;

	if (!block->ms->env)
		return (1);
	j = 0;
	while (block->cmd_args[++j])
	{
		i = get_corr_env(block, block->cmd_args[j], 0);
		if (i == -2)
			continue ;
		else if (i != -1)
			env_remove(block, i);
	}
	return (1);
}

static void	write_export_list(char *exp_item)
{
	int		f;

	f = -1;
	ft_printf_fd(block->final_out, "declare -x ");
	while (exp_item[++f] && exp_item[f] != '=')
		ft_printf_fd(block->final_out, "%c", exp_item[f]);
	if (exp_item[f] && exp_item[f] == '=')
		ft_printf_fd(block->final_out, "%c%c", exp_item[f++], '\"');
	if (exp_item[f])
		ft_printf_fd(block->final_out, "%s", &exp_item[f]);
	if ((exp_item[f] && exp_item[f] == '=') \
	|| (exp_item[f - 1] && exp_item[f - 1] == '='))
		ft_printf_fd(block->final_out, "\"");
	ft_printf_fd(block->final_out, "\n");
}

static int	run_exp_list(t_block *block)
{
	int		i;
	int		len;
	char	**exp_list;

	i = -1;
	if (!block->ms->env)
		return (1);
	ft_charmatdup(&exp_list, block->ms->env);
	if (!exp_list)
		return (0);
	quicksort_pointers(exp_list, ft_matrixlen(exp_list), &env_strcmp);
	while (exp_list[++i])
		write_export_list(char exp_list[i]);
	i = -1;
	while (exp_list[++i])
		free((exp_list[i]));
	free(exp_list);
	return (1);
}

int	run_export(t_block *block)
{
	int		i;
	int		j;

	j = 0;
	if (!block->cmd_args[1])
		run_exp_list(block);
	else
	{
		while (block->cmd_args[++j])
		{
			i = get_corr_env(block, block->cmd_args[j], 1);
			if (i == -2 || (i > -1 && !ft_strchr(block->cmd_args[j], '=') \
			&& block->ms->env[i] && ft_strchr(block->ms->env[i], '=')))
				continue ;
			else if (i != -1)
				env_remove(block, i);
			env_add(block, block->cmd_args[j]);
		}
	}
	return (1);
}
