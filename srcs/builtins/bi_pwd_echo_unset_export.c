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

int	run_pwd(t_block *block)
{
	char	*pwd;
	int		i;

	i = 0;
	while (block->ms->env[i] && \
	!ft_strnstr(block->ms->env[i], "PWD=", 4))
		i++;
	if (!block->ms->env[i])
		return (1);
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
		i = get_corr_env(block, block->cmd_args[j], 0);
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

	i = -1;
	if (!block->ms->env)
		return (1);
	while (block->ms->env[++i])
	{
		f = -1;
		ft_putstr_fd("declare -x ", block->final_out);
		while (block->ms->env[i][++f] && block->ms->env[i][f] != '=')
			write(block->final_out, &block->ms->env[i][f], 1);
		if (block->ms->env[i][f] && block->ms->env[i][f] == '=')
		{
			write(block->final_out, &block->ms->env[i][f++], 1);
			ft_putstr_fd("\"", block->final_out);
		}
		if (block->ms->env[i][f])
			ft_putstr_fd(&block->ms->env[i][f], block->final_out);
		if ((block->ms->env[i][f] && block->ms->env[i][f] == '=') \
		|| (block->ms->env[i][f - 1] && block->ms->env[i][f - 1] == '='))
			ft_putstr_fd("\"", block->final_out);
		ft_putstr_fd("\n", block->final_out);
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
