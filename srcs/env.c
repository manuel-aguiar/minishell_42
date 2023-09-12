/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 19:21:09 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/12 22:47:00 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	env_remove(char **env, int i);
int		get_corr_env(char *cmd, char **env);
void	env_substitute(char *cmd, char **env, int i);
void	env_add(char *cmd, char **env);

int	run_env(t_block *block)
{
	int	i;

	i = 0;
	if (!block->ms->env)
		return (1);
	while (block->ms->env[i])
	{
		ft_putstr_fd(block->ms->env[i], block->final_out);
		write(block->final_out, "\n", 1);
		i++;
	}
	return (1);
}

void	env_remove(char **env, int i)
{
	while (env[i] && env[i + 1])
	{
		env[i] = env[i + 1];
		i++;
	}
	env[i] = NULL;
}

int	get_corr_env(char *cmd, char **env)
{
	int		i;
	size_t	f;
	size_t	j;

	i = 0;
	j = 0;
	while (cmd[j] && cmd[j] != '=')
		j++;
	while (env[i])
	{
		f = 0;
		while (env[i][f] && env[i][f] != '=')
			f++;
		if (f < j)
			f = j;
		if (ft_strncmp(env[i], cmd, f) == 0)
			return (i);
		i++;
	}
	return (-1);
}

void	env_substitute(char *cmd, char **env, int i)
{
	size_t	f;

	f = 0;
	while (env[i][f] && env[i][f] != '=')
		f++;
	if (!env[i][f])
		return ;
	f--;
	env[i] = 0;
	env[i] = malloc(ft_strlen(cmd) + f + 1);
	if (!env[i])
		return ;								//perror_msg
	env[i + ft_strlen(cmd) + 1] = cmd;
}

void	env_add(char *cmd, char **env)
{
	size_t	f;

	f = 0;
	while (env[f])
		f++;
	env[f] = malloc(ft_strlen(cmd) + f);
	if (!env[f])
		return ;								//perror_msg
	env[f] = cmd;
}
