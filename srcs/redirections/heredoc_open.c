/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_open.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:33:20 by codespace         #+#    #+#             */
/*   Updated: 2023/09/22 13:14:51 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	open_here_docs_at_block(t_block *block)
{
	int				i;
	int				has_quotes;
	t_token_node	*cur;

	if (!block->io_files)
		return (1);
	cur = block->io_files->head;
	i = 0;
	//printf("checking here_docs at block [%s]\n", block->prompt);
	while (cur && g_signal != SIGINT)
	{
		//printf("checking redir [%s] of type %d\n", redir->file, redir->type);
		if (cur->type == T_INDIR_HD)
		{
			
			if (block->here_doc)
			{
				close(block->here_doc_fd);
				unlink(block->here_doc);
				ft_free_set_null(&block->here_doc);
			}
			if (!remove_unguarded_quotes(&cur->text, &has_quotes)
				|| !here_doc(block, cur->text, has_quotes))
				return (0);

			//block->here_doc = redir->file;
			//printf("here doc name [%s]\n", block->here_doc);
			//redir->file = NULL;
			block->here_doc_index = i;
			//printf("here doc: fd %d, i %d, limiter [%s], file name [%s]\n", block->here_doc_fd, i, redir->file, block->here_doc);
		}
		cur = cur->next;
		i++;
	}
	return (1);
}
