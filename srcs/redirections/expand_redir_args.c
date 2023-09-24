/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/24 13:02:13 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ambiguous_redirection_err(t_block *block, char **fail_return)
{
	if (block->my_manager)
		block->my_manager->my_status = EXIT_AMBIG_REDIR;
	else
		block->ms->exit_status = EXIT_AMBIG_REDIR;
	ft_printf_fd(block->ms->errfd, "%s: %s: ambiguous redirect\n", \
				block->ms->name, *fail_return);
	ft_free_set_null(fail_return);
	return (0);
}

int	manage_io_expansion(t_block *block)
{
	char	*redir_copy;
	char	*fail_return;
	char	**split;
	int		count;

	redir_copy = block->io_files->head->text;
	fail_return = ft_triple_join("\'", redir_copy, "\'");
	if (!fail_return)
		return (perror_msg_int("malloc", 0));
	if (!expand_dollars(&redir_copy, block->ms) \
	|| !expand_wildcards(&redir_copy))
		return (0);
	split = ft_split_count(redir_copy, "\t\v\n\r ", &count);
	ft_free_charmat_null(&split, free);
	if (!remove_unguarded_quotes(&redir_copy, NULL))
		return (0);
	block->io_files->head->text = redir_copy;
	if (count != 1)
		return (ambiguous_redirection_err(block, &fail_return));
	ft_free_set_null(&fail_return);
	if (!block->io_files->head->text || !*(block->io_files->head->text))
		return (perror_msg_func(block, block->io_files->head->text, \
		CODE_OPEN, 1));
	return (1);
}
