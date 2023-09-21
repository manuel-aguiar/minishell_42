/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_read.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:21:31 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/21 15:32:21 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	here_doc_wrong_delimiter(t_block *block, char *eof, int *count)
{
	ft_putstr_fd(block->ms->name, block->ms->errfd);
	ft_putstr_fd(": warning: here-document at line ", block->ms->errfd);
	ft_putnbr_fd(*count, block->ms->errfd);
	ft_putstr_fd(" delimited by end-of-file (wanted `", block->ms->errfd);
	ft_putstr_fd(eof, block->ms->errfd);
	ft_putstr_fd("')\n", block->ms->errfd);
	return (0);
}

static int	here_doc_write_until_delimiter(t_block *block, char **line, \
			char *eof, int has_quote_guard)
{
	if (!ft_strncmp(eof, *line, ft_strlen(eof)) \
	&& ft_strlen(eof) == ft_strlen(*line))
	{
		ft_free_set_null(line);
		return (0);
	}
	if (!has_quote_guard && !here_doc_expand_dollars(line, block->ms))
	{
		ft_free_set_null(line);
		return (0);
	}
	ft_printf_fd(block->here_doc_fd, "%s\n", *line);
	ft_free_set_null(line);
	return (1);
}

static int	here_doc_fill(t_block *block, char *eof, int has_quote_guard)
{
	char	*line;
	int		count;

	count = 0;
	while (1)
	{
		count++;
		line = readline("> ");
		if (line)
		{
			if (!here_doc_write_until_delimiter(block, &line, eof, \
				has_quote_guard))
				return (0);
		}
		else if (save_signal(NULL) != EXIT_SIGINT)
			return (here_doc_wrong_delimiter(block, eof, &count));
		else
			return (0);
	}
	return (1);
}

int	here_doc(t_block *block, char *eof, int has_quote_guard)
{
	if (!heredoc_temp_name(block))
		return (0);
	block->here_doc_fd = open(block->here_doc, O_CREAT \
						| O_TRUNC | O_RDWR, 0644);
	if (block->here_doc_fd == -1)
		return (perror_msg_func(block, block->here_doc, CODE_OPEN, 1));
	here_doc_fill(block, eof, has_quote_guard);
	if (save_signal(NULL) == EXIT_SIGINT)
	{
		close(block->here_doc_fd);
		unlink(block->here_doc);
		//printf("\n");
		return (0);
	}
	close(block->here_doc_fd);
	block->here_doc_fd = open(block->here_doc, O_RDONLY);
	if (block->here_doc_fd == -1)
		return (perror_msg_func(block, block->here_doc, CODE_OPEN, 1));
	return (1);
}
