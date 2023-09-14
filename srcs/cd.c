/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/14 20:38:31 by mmaria-d         ###   ########.fr       */
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
	while (ft_strnstr(block->ms->env[i], \
	"OLDPWD=/", ft_strlen(block->ms->env[i])))
		i++;
	env_remove(block, i);
	env_add(block, temp);
	printf("%s\n", block->ms->env[i]);
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
	printf("%s\n", temp);
	free(temp);
	return (1);
}

static int	cd_error(t_block *block, int arg)
{
	write(1, "minishell: ", 11);
	ft_putstr_fd(block->cmd_args[arg], block->final_out);
	write(1, ": No such file or directory\n", 28);
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

/*
static int	get_curpath(t_block *block, char **curpath)
{
	char	*curr;

	curr = getcwd(NULL, 0);
	if (!curr)
	{
		perror("malloc");
		return (0);
	}
	if (!set_beg_path(block, curpath, curr) \
	|| !curpath || !block->cmd_args[1])
	{
		if (*curpath)
			free(*curpath);
		free(curr);
		return (0);
	}
	free(curr);
	if (cd_exists(block) == 0)
		return (0);
	return (1);
}

int	set_beg_path(t_block *block, char **curpath, char *curr)
{
	int		i;

	i = 0;
	while (ft_strncmp(block->ms->env[i], "HOME=", 5) != 0)
		i++;
	if (!block->cmd_args[1] && !block->ms->env[i])
		return (1);
	else if (!block->cmd_args[1] && block->ms->env[i])
		*curpath = ft_strdup(&block->ms->env[i][5]);
	else if (block->cmd_args[1] && block->cmd_args[1][0] == '-')
	{
		i = 0;
		while (!ft_strnstr(block->ms->env[i], "OLDPWD=", 7))
			i++;
		*curpath = ft_strdup(&block->ms->env[i][7]);
	}
	else
		aux_cd_paths(block, curpath, curr);
	if (!(*curpath))
	{
		perror("malloc");
		return (0);
	}
	return (1);
}

static void	aux_cd_paths(t_block *block, char **curpath, char *curr)
{
	int		i;
	char	*temp;

	i = 0;
	if (block->cmd_args[1] && block->cmd_args[1][0] == '/')
	{
		*curpath = ft_strdup(block->cmd_args[1]);
		if (!(*curpath))
		{
			perror("malloc");
			return ;
		}
		i = ft_strlen(*curpath);
		while (*curpath[--i] == '.' || (*curpath[i] == '/' \
		&& i - 1 >= 0 && *curpath[i - 1] == '.'))
			*curpath[i] = 0;
	}
	else
		relative_paths(block, curpath, curr);
}

static int	relative_paths(t_block *block, char **curpath, char *curr)
{
	char	*temp;

	if (curr[ft_strlen(curr)] != '/')
		temp = ft_strjoin(curr, "/");
	else
		temp = ft_strdup(curr);
	if (!temp)
	{
		perror("malloc");
		return (0);
	}
	*curpath = ft_strjoin(temp, block->cmd_args[1]);
	free(temp);
	if (!(*curpath))
	{
		perror("malloc");
		return (0);
	}
	return (1);
}
*/
