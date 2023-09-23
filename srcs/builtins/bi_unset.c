/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_unset.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 17:09:22 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 20:01:05 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_unset(t_block *block)
{
	int		i;
	int		j;

	if (!block->ms->env)
		return (1);
	j = 0;
	while (block->cmd_args[++j])
	{
		i = get_corr_env(block, block->cmd_args[j], 0);
		if (i == -2)
			continue ;
		else if (i != -1)
			env_remove(block, i);
	}
	return (1);
}
