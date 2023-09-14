/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 19:21:09 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/14 19:10:24 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_env(t_block *block)
{
	int	i;

	i = 0;
	if (!block->ms->env)
		return (1);
	while (block->ms->env[i])
	{
		if (ft_strchr(block->ms->env[i], '='))
		{
			ft_putstr_fd(block->ms->env[i], block->final_out);
			write(block->final_out, "\n", 1);
		}
		i++;
	}
	return (1);
}

void	env_remove(t_block *block, int i)
{
	char	**dupenv;
	int		f;

	while (block->ms->env[i] && block->ms->env[i + 1])
	{
		block->ms->env[i] = block->ms->env[i + 1];
		i++;
	}
	block->ms->env[i] = NULL;
	free(block->ms->env[i]);
	if (!ft_charmatdup(&dupenv, block->ms->env))
		return ;
	f = -1;
	while (block->ms->env[++f])
		free(block->ms->env[f]);
	free(block->ms->env);
	block->ms->env = dupenv;
}

static int	env_error(t_block *block, char t, int arg)
{
	write(1, "minishell: ", 11);
	if (t == 'e')
		ft_putstr_fd("export: `", block->final_out);
	else if (t == 'u')
		ft_putstr_fd("unset: `", block->final_out);
	ft_putstr_fd(block->cmd_args[arg], block->final_out);
	write(1, "': not a valid identifier\n", 26);
	return (-2);
}

int	get_corr_env(t_block *block, int b, char t)
{
	int		i;
	size_t	f;
	size_t	j;

	i = -1;
	j = 0;
	while (block->cmd_args[b][j] && (ft_isalpha(block->cmd_args[b][j])) \
	|| ft_isdigit(block->cmd_args[b][j]) \
	|| (block->cmd_args[b][j] == '=' && t == 'e'))
		j++;
	if ((block->cmd_args[b][0] == '=' && t == 'e') \
	|| (ft_strchr(block->cmd_args[b], '=') && t == 'u'))
		return (env_error(block, t, b));
	while (block->ms->env[++i])
	{
		f = 0;
		while (block->ms->env[i][f] && block->ms->env[i][f] != '=')
			f++;
		if (--f < --j)
			f = j;
		if (ft_strncmp(block->ms->env[i], block->cmd_args[b], f) == 0)
			return (i);
	}
	return (-1);
}

void	env_add(t_block *block, char *new)
{
	char	**res;
	int		len;
	int		i;

	len = ft_matrixlen(block->ms->env);
	res = malloc((len + 2) * sizeof(*res));
	if (!res)
		return ;
	i = -1;
	while (++i < len + 1)
	{
		if (i == 0)
		{
			res[i] = ft_strdup(new);
			if (!res[i])
				return (perror("malloc"));
		}
		else
			res[i] = block->ms->env[i - 1];
	}
	res[i] = NULL;
	free(block->ms->env);
	block->ms->env = res;
}
