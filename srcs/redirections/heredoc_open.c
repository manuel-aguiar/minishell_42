/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_open.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:33:20 by codespace         #+#    #+#             */
/*   Updated: 2023/09/23 18:41:04 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_if_prev_heredoc_and_get_new(t_block *block, \
			t_token_node *cur, int index)
{
	int	has_quotes;

	if (block->here_doc)
	{
		close(block->here_doc_fd);
		unlink(block->here_doc);
		ft_free_set_null(&block->here_doc);
	}
	if (!remove_unguarded_quotes(&cur->text, &has_quotes)
		|| !here_doc(block, cur->text, has_quotes))
		return (0);
	block->here_doc_index = index;
	return (1);
}

int	open_here_docs_at_block(t_block *block)
{
	int				i;
	t_token_node	*cur;

	if (!block->io_files)
		return (1);
	cur = block->io_files->head;
	i = 0;
	while (cur && g_signal != SIGINT)
	{
		if (cur->type == T_INDIR_HD)
		{
			if (!check_if_prev_heredoc_and_get_new(block, cur, i))
				return (0);
		}
		cur = cur->next;
		i++;
	}
	return (1);
}
