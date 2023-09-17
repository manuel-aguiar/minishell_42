/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_read.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:21:31 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/17 13:44:16 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_handler_heredoc(int signum)
{
	int	code;

	if (signum == SIGINT)
	{
		//printf("\n\n successfully changed the signal handler\n\n\n");
		rl_replace_line("", 0);
		destroy_ms(sigint_heredoc_where_ms_is(NULL));
		//printf("\n");
		exit(130);
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
		else
		{
			printf("minishell: warning: here-document at line %d delimited by end-of-file (wanted `%s')\n", count, eof);            //dprintf ft_putstr_fd(block->ms->errfd);
			break ;
		}
	}
	return (1);
}

int	here_doc(t_block *block, char *eof, int has_quote_guard)
{
	int	pid;
	int	hc_status;

	if (!heredoc_temp_name(block))
		return (0);
	//printf("trying to open file [%s]\n", block->here_doc);
	block->here_doc_fd = open(block->here_doc, O_CREAT | O_TRUNC | O_RDWR, 0644);
	//printf("success? fd: %d\n", block->here_doc_fd);
	if (block->here_doc_fd == -1)
		return (perror_msg_func(block, block->here_doc, CODE_OPEN, 1));                 //open failed, perror(open) guarda exit status
	ms_prepare_signal(block->ms, signal_handler_heredoc);
	pid = fork();
	if (pid == -1)
		return (perror_msg("fork"));
	if (!pid)
	{
		here_doc_fill(block, eof, has_quote_guard);
		exit(0);
	}
	ms_prepare_signal(block->ms, signal_handler);
	waitpid(pid, &hc_status, 0);
	//printf("exit status of child wifexited %d, status %d\n", WIFSIGNALED(hc_status), WTERMSIG(hc_status));
	if (WIFEXITED(hc_status) && WEXITSTATUS(hc_status))
	{
		//printf("here doc failed\n");
		unlink(block->here_doc);
		close(block->here_doc_fd);
		hc_status = 130;
		save_signal(&hc_status);
		rl_redisplay();
		return (0);
	}
	close(block->here_doc_fd);
	block->here_doc_fd = open(block->here_doc, O_RDONLY);
	if (block->here_doc_fd == -1)
		return (perror_msg_func(block, block->here_doc, CODE_OPEN, 1));                                        // perror("open");
	return (1);
}
