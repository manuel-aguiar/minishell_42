/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/14 19:24:42 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_pwd(t_block *block)
{
	char	*pwd;
	int		i;

	i = 0;
	while (!ft_strnstr(block->ms->env[i], "PWD=", 4))
		i++;
	pwd = ft_strdup(&block->ms->env[i][4]);
	if (!pwd)
		return (perror_msg("malloc"));
	ft_putstr_fd(pwd, block->final_out);
	write(block->final_out, "\n", 1);
	free(pwd);
	return (1);
}

int	run_echo(t_block *block)
{
	int		i;
	int		jump;

	i = 0;
	jump = 0;
	if (block->cmd_args[1] && ft_strncmp(block->cmd_args[1], "-n", 2) == 0)
	{
		i++;
		while (block->cmd_args[1][i] && block->cmd_args[1][i] == 'n')
			i++;
		if (!block->cmd_args[1][i])
			jump = 1;
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
		i = get_corr_env(block, j, 'u');
		if (i == -2)
			continue ;
		else if (i != -1)
			env_remove(block, i);
	}
	return (1);
}

static int	run_exp_list(t_block *block)
{
	int	i;
	int	f;

	i = 0;
	if (!block->ms->env)
		return (1);
	while (block->ms->env[i])
	{
		f = -1;
		write(block->final_out, "declare -x ", 11);
		while (block->ms->env[i][++f] && block->ms->env[i][f] != '=')
			write(block->final_out, &block->ms->env[i][f], 1);
		write(block->final_out, &block->ms->env[i][f], 1);
		f++;
		if (ft_strchr(block->ms->env[i], '='))
			write(block->final_out, "\"", 1);
		ft_putstr_fd(&block->ms->env[i][f], block->final_out);
		if (ft_strchr(block->ms->env[i], '='))
			write(block->final_out, "\"", 1);
		write(block->final_out, "\n", 1);
		i++;
	}
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
			i = get_corr_env(block, j, 'e');
			if (i == -2)
				continue ;
			else if (i != -1)
				env_remove(block, i);
			env_add(block, block->cmd_args[j]);
		}
	}
	return (1);
}
