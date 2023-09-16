/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_temp.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:17:58 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 17:24:12 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	dfs(char *buf, int max, int cur, int *found)
{
	char	c;

	if (cur == max)
		return ;
	if (!*found && *buf && access(buf, F_OK) != 0)
	{
		*found = 1;
		return ;
	}
	if (*found)
		return ;
	else
	{
		c = 'a';
		while (c <= 'z')
		{
			buf[cur] = c;
			dfs(buf, max, cur + 1, found);
			if (*found)
				return ;
			buf[cur] = '\0';
			c++;
		}
	}
}

int	heredoc_temp_name(t_block *block)
{
	char	new[256];
	int		found;

	found = 0;
	ft_memset(new, '\0', sizeof(new));
	new[0] = '.';
	dfs(new, sizeof(new), 1, &found);
	block->here_doc = ft_strdup(new);
	if (!block->here_doc)
		return (perror_msg_int("malloc", 0));
	return (1);
}
