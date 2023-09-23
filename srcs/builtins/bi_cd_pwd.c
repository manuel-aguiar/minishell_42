/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_cd_pwd.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 10:33:39 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_pwd(t_block *block)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (perror_msg_int("malloc", 0));
	ft_printf_fd(block->final_out, "%s\n", pwd);
	free(pwd);
	return (1);
}

int	run_cd(t_block *block)
{
	int		i;
	char	*temp;

	i = 0;
	while (block->ms->env[i] && \
	!ft_strnstr(block->ms->env[i], "PWD=", 4))
		i++;
	if (block->ms->env[i])
	{
		temp = ft_strjoin("OLDPWD=", block->ms->env[i]);
		if (!temp)
		{
			free(temp);
			return (0);
		}
		i = 0;
		while (block->ms->env[i] && !ft_strnstr(block->ms->env[i], \
		"OLDPWD=/", ft_strlen(block->ms->env[i])))
			i++;
		env_remove(block, i);
		env_add(block, temp);
		free(temp);
	}
	cd_exists(block);
	return (1);
}

static int	upd_pwd(t_block *block)
{
	char	*curr;
	char	*temp;
	int		i;

	i = 0;
	curr = getcwd(NULL, 0);
	while (block->ms->env[i] && \
	!ft_strnstr(block->ms->env[i], "PWD=", 4))
		i++;
	if (block->ms->env[i])
		env_remove(block, i);
	temp = ft_strjoin("PWD=", curr);
	free(curr);
	if (!temp)
		return (perror_msg_int("malloc", 0));
	env_add(block, temp);
	free(temp);
	return (1);
}

static int	cd_error(t_block *block, int arg)
{
	ft_printf_fd(block->final_out, "%s: cd: ", block->ms->name);
	if (arg == -1)
		ft_printf_fd(block->final_out, "too many arguments\n");
	else
	{
		ft_printf_fd(block->final_out, block->cmd_args[arg]);
		ft_printf_fd(block->final_out, ": No such file or directory\n");
	}
	return (0);
}

/*
  109 |  else if ((!block->cmd_args[1] || block->cmd_args[1] \
      |                                   ~~~~~~~~~~~~~~~~~~~~
  110 |  && block->cmd_args[1][0] == '~') && block->ms->env[i])
*/

int	cd_exists(t_block *block)
{
	int		i;

	i = 0;
	while (block->ms->env[i] && \
	ft_strncmp(block->ms->env[i], "HOME=", 5) != 0)
		i++;
	if (!block->cmd_args[1] && !block->ms->env[i])
		return (1);
	else if (block->cmd_args[1] && block->cmd_args[2])
		return (cd_error(block, -1));
	else if ((!block->cmd_args[1] || (block->cmd_args[1] \
	&& block->cmd_args[1][0] == '~')) && block->ms->env[i])
	{
		if (chdir(&block->ms->env[i][5]) != 0)
			return (cd_error(block, 0));
	}
	else if (block->cmd_args[1])
	{
		if (chdir(block->cmd_args[1]) != 0)
			return (cd_error(block, 1));
	}
	if (!upd_pwd(block))
		return (0);
	return (1);
}
