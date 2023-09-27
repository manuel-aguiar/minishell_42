/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_exit.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 18:28:24 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/27 13:17:26 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	exit_destroy_and_exit(t_block *block)
{
	int	save_status;

	save_status = block->my_status;
	ms_destroy(block->ms);
	exit(save_status);
}

int	exit_execution(t_block *block, char *arg, int is_exiting, int is_error)
{
	if (!block->i_am_forked && ft_putstr_fd("exit\n", block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (is_error)
	{
		ft_putstr_fd(block->ms->name, block->ms->errfd);
		ft_putstr_fd(": exit: ", block->ms->errfd);
		if (arg)
		{
			ft_putstr_fd(arg, block->ms->errfd);
			ft_putstr_fd(": numeric argument required\n", block->ms->errfd);
		}
		else
			ft_putstr_fd("too many arguments\n", block->ms->errfd);
	}
	if (is_exiting)
		exit_destroy_and_exit(block);
	else if (block->my_manager)
		block->my_manager->my_status = block->my_status;
	else
		block->ms->exit_status = block->my_status;
	return (1);
}

int	exit_atoi(char *arg, int *place_res)
{
	int	res;
	int	sign;

	sign = 1;
	while (*arg && ft_isspace(*arg))
		arg++;
	if (*arg == '-')
		sign = -1;
	if (*arg == '-' || *arg == '+')
		arg++;
	if (!(*arg >= '0' && *arg <= '9'))
		return (0);
	res = 0;
	while (*arg >= '0' && *arg <= '9')
		res = res * 10 + (*arg++ - '0');
	while (*arg && ft_isspace(*arg))
		arg++;
	if (*arg)
		return (0);
	*place_res = res * sign % (UCHAR_MAX + 1);
	return (1);
}

int	run_exit(t_block *block)
{
	if (block->cmd_args[1])
	{
		if (!exit_atoi(block->cmd_args[1], &block->my_status))
		{
			block->my_status = EXIT_NON_NUMERICAL;
			exit_execution(block, block->cmd_args[1], 1, 1);
		}
		else if (block->cmd_args[2])
		{
			block->my_status = EXIT_TOO_MANY_ARGS;
			exit_execution(block, NULL, false, true);
		}
		else
			exit_execution(block, NULL, true, false);
	}
	else
		exit_execution(block, NULL, true, false);
	return (1);
}
