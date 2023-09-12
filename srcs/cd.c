/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/12 22:29:12 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void		run_cd(char **cmd, char **env, int final_out);
int			cd_exists(char *cmd, char *curpath, int final_out);
char		*set_beg_path(char *cmd, char **env, char *curpath);
static char	*aux_cd_paths(char *cmd, char *curpath, char *curr);

int	run_pwd(t_block *block)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (perror_msg("malloc"));
	ft_putstr_fd(pwd, block->final_out);
	write(block->final_out, "\n", 1);
	free(pwd);
	return (1);
}

/*



int	run_cd(t_block *block, char **cmd, char **env, int final_out)
{
	int		i;
	char	*curr;
	char	*curpath;

	curr = getcwd(NULL, 0);								//malloc can fail
	curpath = set_beg_path(cmd, env, 0, curr);
	if (!curpath || !cmd[1])
	{
		if (curpath)
			free(curpath);
		free(curr);
		return ;
	}
	if (cd_exists(cmd[1], curpath, final_out) == 0)
		return (free(curr));
	i = 0;
	while (ft_strnstr(env[i], "OLDPWD=/", ft_strlen(env[i])))
		i++;
	if (env[i] && chdir(curpath) == 0)
		env[i] = ft_strjoin("OLDPWD=", curr);				// malloc can fail
	printf("%s\n", env[i]);
	free(env[i]);
	free(curpath);
	free(curr);
	return (1);
}

char	*set_beg_path(char **cmd, char **env, char *curpath, char *curr)
{
	int		i;

	i = 0;
	while (ft_strncmp(env[i], "HOME=", 5) != 0)
		i++;
	if (!cmd[1] && !env[i])
	{
		free(curr);
		return (NULL);
	}
	else if (!cmd[1] && env[i])
		curpath = ft_strdup(&env[i][5]);
	else if (cmd[1] && cmd[1] == '-' && !cmd[2])
	{
		i = 0;
		while (ft_strnstr(env[i], "OLDPWD=/", ft_strlen(env[i])))
			i++;
		curpath = ft_strdup(&env[i][5]);
	}
	else
		curpath = aux_cd_paths(cmd[1], curpath, curr);
	free(curr);
	return (curpath);
}

static char	*aux_cd_paths(char *cmd, char *curpath, char *curr)
{
	int		i;
	char	*temp;

	i = 0;
	if (cmd && cmd[0] == '/')
	{
		curpath = ft_strdup(cmd);						//malloc, perror_msg
		i = ft_strlen(curpath);
		while (curpath[--i] == '.' || (curpath[i] == '/' \
		&& i - 1 >= 0 && curpath[i - 1] == '.'))
			curpath[i] = 0;
	}
	else
	{
		if (curr[ft_strlen(curr)] != '/')
			temp = ft_strjoin(curr, "/");
		else
			temp = ft_strdup(curr);
		// test teemp and perror_msg;
		curpath = ft_strjoin(temp, cmd);				//malloc, perror_msg
		free(temp);
	}
	return (curpath);
}

int	cd_exists(char *cmd, char *curpath, int final_out)
{
	char	*curr;
	char	*temp;
	// int		i;

	// i = 0;
	if (curpath)
	{
		if (chdir(curpath) != 0)
		{
			write(1, "minishell: ", 11);
			ft_putstr_fd(cmd, final_out);
			write(1, ": No such file or directory\n", 28);
			free(curpath);
			return (0);
		}
		temp = getcwd(NULL, 0);								// malloc can fail
		curr = ft_strjoin("PWD=", temp);					// malloc can fail
		free(temp);
		// while (ft_strnstr(env[i], "PWD=/", ft_strlen(env[i])))
		// 	i++;
		// env[i] = 0;
		// env[i] = ft_strdup(curr);
		printf("%s\n", curr);
		free(curr);
	}
	return (1);
}
*/
