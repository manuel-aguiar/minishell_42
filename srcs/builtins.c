/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/12 22:42:47 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/*
int	run_builtins(char **cmd, char **env, int final_out)
{
	int	len;

	len = ft_strlen(cmd[0]);
	if (len == 2 && ft_strncmp(cmd[0], "cd", 2) == 0)
		return (1);																		//replace with run_cd
	else if (len == 3 && ft_strncmp(cmd[0], "env", 3) == 0)
		run_env(env, final_out);
	else if (len == 3 && ft_strncmp(cmd[0], "pwd", 3) == 0)
		run_pwd(final_out);
	else if (len == 4 && ft_strncmp(cmd[0], "echo", 4) == 0)
		run_echo(cmd, final_out);
	else if (len == 4 && ft_strncmp(cmd[0], "exit", 4) == 0)
		run_exit();
	else if (len == 5 && ft_strncmp(cmd[0], "unset", 5) == 0)
		run_unset(cmd, env);
	else if (len == 6 && ft_strncmp(cmd[0], "export", 6) == 0)
		run_export(cmd, env, final_out);
	else
		return (0);
	return (1);
}*/

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
		if (i > 1 + jump && ft_strlen(block->cmd_args[i - 1]) > 0)
			write(block->final_out, " ", 1);
		ft_putstr_fd(block->cmd_args[i], block->final_out);
	}
	if (jump == 0)
		write(block->final_out, "\n", 1);
	return (1);
}

void	run_exit(void)
{

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
		i = get_corr_env(block->cmd_args[j], block->ms->env);
		if (i != -1)
			env_remove(block->ms->env, i);
	}
	return (1);
}


/*
void	run_export(char **cmd, char **env, int final_out)
{
	int		i;
	int		j;

	j = 0;
	if (!cmd[1])
		run_env(env, final_out);
	else
	{
		while (cmd[++j])
		{
			i = get_corr_env(cmd[j], env);
			if (i == -1)
				env_add(cmd[j], env);
			else
				env_substitute(cmd[j], env, i);
		}
	}
	run_env(env, final_out);
}
*/
