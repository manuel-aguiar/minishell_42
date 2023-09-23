/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_pwd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:26:37 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/23 19:59:00 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_pwd(t_block *block)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (perror_msg_int("malloc", 0));
	if (ft_putstr_fd(pwd, block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	if (ft_putstr_fd("\n", block->final_out) == -1)
		block->my_status = SIGPIPE + EXIT_SIGNALED;
	free(pwd);
	return (1);
}
