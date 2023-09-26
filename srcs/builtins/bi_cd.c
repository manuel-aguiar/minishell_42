/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/26 14:26:30 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	cd_exists(t_block *block);
static int	cd_error(t_block *block, int arg);
static int	upd_pwd(t_block *block);

int	run_cd(t_block *block)
{
	int		i;
	char	*temp;

	i = 0;
	while (block->ms->env[i] && \
	ft_strncmp(block->ms->env[i], "PWD=", 4) != 0)
		i++;
	if (block->ms->env[i])
	{
		temp = ft_strjoin("OLDPWD=", &block->ms->env[i][4]);
		if (!temp)
			return (perror_msg_int("malloc", 0));
		i = 0;
		while (block->ms->env[i] && ft_strncmp(block->ms->env[i], \
		"OLDPWD=", 7) != 0)
			i++;
		if (block->ms->env[i])
			env_remove(block, i);
		env_add(block, temp);
		free(temp);
	}
	return (cd_exists(block));
}

static int	cd_exists(t_block *block)
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
	return (upd_pwd(block));
}

static int	cd_error(t_block *block, int arg)
{
	block->my_status = CODE_SYNTAX_BINS;
	ft_putstr_fd(block->ms->name, block->ms->errfd);
	ft_putstr_fd(": cd: ", block->ms->errfd);
	if (arg == -1)
		ft_putstr_fd("too many arguments\n", block->ms->errfd);
	else
	{
		ft_putstr_fd(block->cmd_args[arg], block->ms->errfd);
		ft_putstr_fd(": No such file or directory\n", \
		block->ms->errfd);
	}
	return (0);
}

static int	upd_pwd(t_block *block)
{
	char	*curr;
	char	*temp;
	int		i;

	i = 0;
	curr = getcwd(NULL, 0);
	if (!curr)
		return (perror_msg_int("malloc", 0));
	while (block->ms->env[i] && \
	ft_strncmp(block->ms->env[i], "PWD=", 4) != 0)
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
