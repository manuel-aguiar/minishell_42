/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_export.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/24 18:56:21 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	run_exp_list(t_block *block);
static void	print_export_list(t_block *block, char *exp_item, int fd);

int	run_export(t_block *block)
{
	int		i;
	int		j;

	j = 0;
	if (!block->cmd_args[1])
		run_exp_list(block);
	else
	{
		while (block->cmd_args[++j])
		{
			i = get_corr_env(block, block->cmd_args[j], 1);
			if (i == -2 || (i > -1 && !ft_strchr(block->cmd_args[j], '=') \
			&& block->ms->env[i] && ft_strchr(block->ms->env[i], '=')))
				continue ;
			else if (i != -1)
				env_remove(block, i);
			env_add(block, block->cmd_args[j]);
		}
	}
	return (1);
}

static int	run_exp_list(t_block *block)
{
	int		i;
	char	**exp_list;

	i = -1;
	if (!block->ms->env)
		return (1);
	ft_charmatdup(&exp_list, block->ms->env);
	if (!exp_list)
		return (perror_msg_int("malloc", 0));
	quicksort_pointers(exp_list, ft_matrixlen(exp_list), &env_strcmp);
	while (exp_list[++i])
		print_export_list(block, exp_list[i], block->final_out);
	i = -1;
	while (exp_list[++i])
		free((exp_list[i]));
	free(exp_list);
	return (1);
}

static void	print_export_list(t_block *block, char *exp_item, int fd)
{
	int		f;

	f = -1;
	if (ft_putstr_fd("declare -x ", fd) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	while (exp_item[++f] && exp_item[f] != '=')
	{
		if (ft_putchar_fd(exp_item[f], fd) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
	}
	if (exp_item[f] && exp_item[f] == '=')
	{
		if (ft_putchar_fd(exp_item[f++], fd) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
		if (ft_putstr_fd("\"", fd) == -1)
			block->my_status = SIGPIPE + EXIT_SIGNALED;
	}
	if (exp_item[f] && ft_putstr_fd(&exp_item[f], fd) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (((exp_item[f] && exp_item[f] == '=') || (exp_item[f - 1] && \
	exp_item[f - 1] == '=')) && ft_putstr_fd("\"", fd) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (ft_putstr_fd("\n", fd) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
}
