/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_env.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/12 19:21:09 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 21:27:18 by codespace        ###   ########.fr       */
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


int	env_strcmp(void *s1, void *s2)
{
	int		i;
	char	*str1;
	char	*str2;

	str1 = (char *)s1;
	str2 = (char *)s2;
	i = 0;
	while (str1[i] == str2[i] && str1[i] != '\0' && str2[i] != '\0')
		i++;
	return (!(str2[i] >= str1[i]));
}
