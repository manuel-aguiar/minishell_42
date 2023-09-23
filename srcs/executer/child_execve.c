/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_execve.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/23 11:35:39 by codespace         #+#    #+#             */
/*   Updated: 2023/09/23 14:17:28 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	join_path_bin(char **full_path, char *path, char *bin)
{
	int		path_len;
	int		bin_len;
	char	*new;

	path_len = ft_strlen(path);
	if (path[path_len - 1] == '/')
		path_len--;
	bin_len = ft_strlen(bin);
	new = malloc(sizeof(*new) * (path_len + bin_len + 1 + 1));
	if (!new)
	{
		*full_path = new;
		return (perror_msg_int("malloc", 0));
	}
	ft_memcpy(new, path, path_len);
	new[path_len] = '/';
	ft_memcpy(&new[path_len + 1], bin, bin_len);
	new[path_len + bin_len + 1] = '\0';
	*full_path = new;
	return (1);
}

static int	exec_cmd_with_path(t_block *block)
{
	if (access(block->cmd, F_OK))
		return (perror_msg_int(block->cmd, 0));											//double check this
	else if (execve(block->cmd, block->cmd_args, block->ms->env) == -1)
		return (perror_msg_int(block->cmd, 0));											//double check this
	return (1);
}

int	get_path_from_env(t_block *block)
{
	int		i;

	if (!block->ms->env)
		return (0);
	i = 0;
	while (block->ms->env[i] && ft_strncmp("PATH=", block->ms->env[i], 5))
		i++;
	if (!block->ms->env[i])
		return (0);
	else
	{
		block->env_path = ft_split(&block->ms->env[i][5], ':');
		if (!block->env_path)
			return (0);
	}
	return (1);
}

static void	exec_cmd_search_path(t_block *block)
{
	int		i;
	char	*full_path;

	if (!get_path_from_env(block))
		return (error_child_exit(block, ERR_CMD, CODE_CMD, 1));
	i = 0;
	while (block->env_path[i])
	{
		if (!join_path_bin(&full_path, block->env_path[i++], block->cmd))
			return ;
		if (access(full_path, F_OK) == 0)
		{
			if (execve(full_path, block->cmd_args, block->ms->env) == -1)
			{
				ft_free_set_null(&full_path);
				perror_child_exit(block, CODE_EXECVE, 1);
				return ;
			}
		}
		else
			ft_free_set_null(&full_path);
	}
	error_child_exit(block, ERR_CMD, CODE_CMD, 1);
	return ;
}

int	exec_command(t_block *block)
{
	if (ft_strrchr(block->cmd, '/'))
		exec_cmd_with_path(block);
	else
		exec_cmd_search_path(block);
	return (0);
}