/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_aux.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 18:27:58 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/14 17:01:58 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strchr(const char *s, int c);

char	*ft_strchr(const char *s, int c)
{
	while (*s != (char)c)
	{
		if (*s == '\0')
			return (NULL);
		s++;
	}
	return ((char *)s);
}

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
		return (1);
	else if (builtin == BI_UNSET)
		return (run_unset(block));
	else if (builtin == BI_EXPORT)
		return (run_export(block));
	return (1);
}

int	ft_isalpha(int c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		return (1);
	return (0);
}

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}
