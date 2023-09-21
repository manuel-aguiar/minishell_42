/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 19:21:09 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/21 12:12:07 by codespace        ###   ########.fr       */
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

int	env_remove(t_block *block, int index)
{
	char	**res;
	int		len;
	int		i;
	int		found;

	len = ft_matrixlen(block->ms->env);
	res = malloc(len * sizeof(*res));
	if (!res)
		return (0);
	i = 0;
	found = 0;
	while (i < len - 1)
	{
		if (i == index)
		{
			found = 1;
			free(block->ms->env[i]);
		}
		res[i] = block->ms->env[i + found];
		i++;
	}
	res[i] = NULL;
	free(block->ms->env);
	block->ms->env = res;
	return (1);
}

static int	env_error(t_block *block, int is_exporting, char *arg)
{
	ft_putstr_fd("minishell: ", block->final_out);
	if (is_exporting)
		ft_putstr_fd("export: `", block->final_out);
	else
		ft_putstr_fd("unset: `", block->final_out);
	ft_putstr_fd(arg, block->final_out);
	ft_putstr_fd("': not a valid identifier\n", block->final_out);
	return (-2);
}

/*
	while (arg[j] && (ft_isalpha(arg[j])) \
	|| ft_isdigit(arg[j]) \
	|| (arg[j] == '=' && is_exporting))
*/

/*
	if (j > 0 && (arg[0] == '=' && is_exporting) \
	|| (ft_strrchr(arg, '=') && !is_exporting))

*/

int	get_corr_env(t_block *block, char *arg, int is_exporting)
{
	int		i;
	size_t	f;
	size_t	j;

	i = -1;
	j = 0;
	while (arg[j] && (ft_isalpha(arg[j]) \
	|| ft_isdigit(arg[j]) \
	|| (arg[j] == '=' && is_exporting)))
		j++;
	if (j > 0 && ((arg[0] == '=' && is_exporting) \
	|| (ft_strrchr(arg, '=') && !is_exporting)))
		return (env_error(block, is_exporting, arg));
	while (block->ms->env[++i])
	{
		f = 0;
		while (block->ms->env[i][f] && block->ms->env[i][f] != '=')
			f++;
		if (f < j)
			f = j;
		if (f > 0 && ft_strncmp(block->ms->env[i], arg, --f) == 0)
			return (i);
	}
	return (-1);
}

int	env_add(t_block *block, char *new)
{
	char	**res;
	int		len;
	int		i;

	len = ft_matrixlen(block->ms->env);
	res = malloc((len + 2) * sizeof(*res));
	if (!res)
		return (0);
	i = -1;
	while (++i < len + 1)
	{
		if (i == 0)
		{
			res[i] = ft_strdup(new);
			if (!res[i])
				return (perror_msg("malloc"));
		}
		else
			res[i] = block->ms->env[i - 1];
	}
	res[i] = NULL;
	free(block->ms->env);
	block->ms->env = res;
	return (0);
}
