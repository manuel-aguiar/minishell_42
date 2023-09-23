/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 19:21:09 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 20:04:36 by codespace        ###   ########.fr       */
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
			if (ft_putstr_fd(block->ms->env[i], block->final_out) == -1)
				block->my_status = SIGPIPE + EXIT_SIGNALED;
			if (write(block->final_out, "\n", 1) == -1)
				block->my_status = SIGPIPE + EXIT_SIGNALED;
		}
		i++;
	}
	return (1);
}
