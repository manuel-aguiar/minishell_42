/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/15 00:07:14 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	aux_cd_paths(t_block *block, char **curpath, char *curr);
static int	get_curpath(t_block *block, char **curpath);
static int	relative_paths(t_block *block, char **curpath, char *curr);

int	run_cd(t_block *block)
{
	int		i;
	char	*curpath;
	char	*temp;
	char	*curr;

	curr = getcwd(NULL, 0);
	temp = ft_strjoin("OLDPWD=", curr);
	free(curr);
	curpath = 0;
	if (cd_exists(block) == 0)
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
	free(curpath);
	free(temp);
	return (1);
}

static int	upd_pwd(t_block *block)
{
	char	*curr;
	char	*temp;
	int		i;

	i = 0;
	curr = getcwd(NULL, 0);
	while (ft_strnstr(block->ms->env[i], "PWD=", 4))
		i++;
	env_remove(block, i);
	temp = ft_strjoin("PWD=", curr);
	free(curr);
	if (!temp)
	{
		perror("malloc");
		return (0);
	}
	env_add(block, temp);
	free(temp);
	return (1);
}

static int	cd_error(t_block *block, int arg)
{
	ft_putstr_fd("minishell: cd: ", block->final_out);
	ft_putstr_fd(block->cmd_args[arg], block->final_out);
	ft_putstr_fd(": No such file or directory\n", block->final_out);
	return (0);
}

int	cd_exists(t_block *block)
{
	int		i;

	i = 0;
	while (ft_strncmp(block->ms->env[i], "HOME=", 5) != 0)
		i++;
	if (!block->cmd_args[1] && !block->ms->env[i])
		return (1);
	else if ((!block->cmd_args[1] || block->cmd_args[1] \
	&& block->cmd_args[1][0] == '~') && block->ms->env[i])
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
