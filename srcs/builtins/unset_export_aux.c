/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset_export_aux.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 20:06:46 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	env_error(t_block *block, int is_exporting, char *arg)
{
	if (ft_putstr_fd(block->ms->name, block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (ft_putstr_fd(": ", block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (is_exporting)
	{
		if (ft_putstr_fd("export: `", block->final_out) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
	}
	else
	{
		if (ft_putstr_fd("unset: `", block->final_out) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
	}
	if (ft_putstr_fd(arg, block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (ft_putstr_fd("': not a valid identifier\n", block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (block->my_status != SIGPIPE + EXIT_SIGNALED)
		block->my_status = 1;		//add macro
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

int	get_corr_env(t_block *block, char *arg, int is_export)
{
	int		i;
	size_t	f;
	size_t	j;

	i = -1;
	j = 0;
	while (arg[j] && (ft_isalpha(arg[j]) \
	|| ft_isdigit(arg[j]) \
	|| (arg[j] == '=' && is_export)))
		j++;
	if ((j == 0 && ft_strlen(arg) > 0 && !ft_strrchr(arg, '_')) || \
	ft_strlen(arg) == 0 || (arg[0] == '=' && is_export) || \
	(ft_strrchr(arg, '=') && !is_export))
		return (env_error(block, is_export, arg));
	while (block->ms->env[++i])
	{
		f = 0;
		while (block->ms->env[i][f] && block->ms->env[i][f] != '=')
			f++;
		if (f < j)
			f = j;
		if (f > 0 && ft_strncmp(block->ms->env[i], arg, f) == 0)
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
		return (perror_msg_int("malloc", 0));
	i = -1;
	while (++i < len + 1)
	{
		if (i == 0)
		{
			res[i] = ft_strdup(new);
			if (!res[i])
				return (perror_msg_int("malloc", 0));
		}
		else
			res[i] = block->ms->env[i - 1];
	}
	res[i] = NULL;
	free(block->ms->env);
	block->ms->env = res;
	return (0);
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
		return (perror_msg_int("malloc", 0));
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
