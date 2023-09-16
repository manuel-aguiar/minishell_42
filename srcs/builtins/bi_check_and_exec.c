/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bi_check_and_exec.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:27:58 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/16 16:51:38 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_builtins(t_block *block)
{
	if (ft_strncmp(block->cmd, "cd", 3) == 0)
		return (BI_CD);
	else if (ft_strncmp(block->cmd, "env", 4) == 0)
		return (BI_ENV);
	else if (ft_strncmp(block->cmd, "pwd", 4) == 0)
		return (BI_PWD);
	else if (ft_strncmp(block->cmd, "echo", 5) == 0)
		return (BI_ECHO);
	else if (ft_strncmp(block->cmd, "exit", 5) == 0)
		return (BI_EXIT);
	else if (ft_strncmp(block->cmd, "unset", 6) == 0)
		return (BI_UNSET);
	else if (ft_strncmp(block->cmd, "export", 7) == 0)
		return (BI_EXPORT);
	return (0);
}

int	exec_builtin(t_block *block, int builtin)
{
	if (builtin == BI_CD)
		return (run_cd(block));
	else if (builtin == BI_ENV)
		return (run_env(block));
	else if (builtin == BI_PWD)
		return (run_pwd(block));
	else if (builtin == BI_ECHO)
		return (run_echo(block));
	else if (builtin == BI_EXIT)
		return (run_exit(block));
	else if (builtin == BI_UNSET)
		return (run_unset(block));
	else if (builtin == BI_EXPORT)
		return (run_export(block));
	return (1);
}
