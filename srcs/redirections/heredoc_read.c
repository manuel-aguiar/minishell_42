/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_read.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:21:31 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/20 13:26:26 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_handler_heredoc(int signum)
{
	int	code;

	if (signum == SIGINT)
	{
		close(STDIN_FILENO);
		printf("\n");
		code = 130;
		save_signal(&code);
	}
	if (signum == SIGQUIT)
	{
		code = 131;
		save_signal(&code);
	}
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
			if (!ft_strncmp(eof, line, ft_strlen(eof)) \
			&& ft_strlen(eof) == ft_strlen(line))
			{
				ft_free_set_null(&line);
				break ;
			}
			else
			{
				if (!has_quote_guard && !here_doc_expand_dollars(&line, block->ms))
					return (0);
				ft_putstr_fd(line, block->here_doc_fd);
				ft_putstr_fd("\n", block->here_doc_fd);
			}
			ft_free_set_null(&line);
		}
		else if (save_signal(NULL) != EXIT_SIGINT)
		{
			ft_printf_fd(block->ms->errfd, "%s: warning: here-document at line %d delimited by end-of-file (wanted `%s')\n", block->ms->name, count, eof); 
			break ;
		}
		else
			break ;
	}
	return (1);
}

int	here_doc(t_block *block, char *eof, int has_quote_guard)
{
	int	pid;
	int	dup_stdin;

	dup_stdin = dup(block->ms->infd);
	if (!dup_stdin)
		return (perror_msg_int("dup", 0));
	if (!heredoc_temp_name(block))
		return (0);
	//printf("trying to open file [%s]\n", block->here_doc);
	block->here_doc_fd = open(block->here_doc, O_CREAT | O_TRUNC | O_RDWR, 0644);
	//printf("success? fd: %d\n", block->here_doc_fd);
	if (block->here_doc_fd == -1)
		return (perror_msg_func(block, block->here_doc, CODE_OPEN, 1));                 //open failed, perror(open) guarda exit status
	ms_prepare_signal(block->ms, signal_handler_heredoc);
	here_doc_fill(block, eof, has_quote_guard);
	
	ms_prepare_signal(block->ms, signal_handler);
	
	if (save_signal(NULL) == EXIT_SIGINT)
	{
		block->ms->kill_stdin = 1;
		dup2(dup_stdin, block->ms->infd);
		close(block->here_doc_fd);
		unlink(block->here_doc);
		close(dup_stdin);
		return (0);
	}
	close(dup_stdin);
	close(block->here_doc_fd);
	block->here_doc_fd = open(block->here_doc, O_RDONLY);
	if (block->here_doc_fd == -1)
		return (perror_msg_func(block, block->here_doc, CODE_OPEN, 1));                                        // perror("open");
	return (1);
}