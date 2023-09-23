/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_echo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 21:40:32 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	print_echo(t_block *block, int i, int jump);

int	run_echo(t_block *block)
{
	int		i;
	int		j;
	int		jump;

	i = 0;
	j = 0;
	jump = 0;
	while (block->cmd_args[++j] && ft_strncmp(block->cmd_args[j], "-n", 2) == 0)
	{
		i = 1;
		while (block->cmd_args[j][i] && block->cmd_args[j][i] == 'n')
			i++;
		if (!block->cmd_args[j][i])
			jump++;
		i = jump;
	}
	return (print_echo(block, i, jump));
}

static int	print_echo(t_block *block, int i, int jump)
{
	while (block->cmd_args[++i])
	{
		if ((i > 1 + jump && write(block->final_out, " ", 1) == -1) \
		|| ft_putstr_fd(block->cmd_args[i], block->final_out) == -1)
		{
			block->my_status = SIGPIPE + EXIT_SIGNALED;
			return (0);
		}
	}
	if (jump == 0 && write(block->final_out, "\n", 1) == -1)
	{
		block->my_status = SIGPIPE + EXIT_SIGNALED;
		return (0);
	}
	return (1);
}
