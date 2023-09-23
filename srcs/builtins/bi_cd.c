/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_cd.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 19:58:04 by codespace        ###   ########.fr       */
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
	!ft_strnstr(block->ms->env[i], "PWD=", 4))
		i++;
	if (block->ms->env[i])
	{
		temp = ft_strjoin("OLDPWD=", block->ms->env[i]);
		if (!temp)
			return (perror_msg_int("malloc", 0));
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
	if (!upd_pwd(block))
		return (0);
	return (1);
}

static int	cd_error(t_block *block, int arg)
{
	if (ft_putstr_fd(block->ms->name, block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (ft_putstr_fd(": cd: ", block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (arg == -1)
	{
		if (ft_putstr_fd("too many arguments\n", block->final_out) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
	}
	else
	{
		if (ft_putstr_fd(block->cmd_args[arg], block->final_out) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
		if (ft_putstr_fd(": No such file or directory\n", \
		block->final_out) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
	}
	if (block->my_status != SIGPIPE + EXIT_SIGNALED)
		block->my_status = 1;		//add macro
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

/*
  109 |  else if ((!block->cmd_args[1] || block->cmd_args[1] \
      |                                   ~~~~~~~~~~~~~~~~~~~~
  110 |  && block->cmd_args[1][0] == '~') && block->ms->env[i])
*/