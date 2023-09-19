/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 14:08:00 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ambiguous_redirection_err(t_block *block, char **fail_return)
{
	if (block->my_manager)
		block->my_manager->my_status = EXIT_AMBIG_REDIR;
	else
		block->ms->exit_status = EXIT_AMBIG_REDIR;
	ft_printf_fd(block->ms->errfd, "%s: %s ambiguous redirect\n", \
				block->ms->name, *fail_return);
	ft_free_set_null(fail_return);
	return (0);
}

int	manage_io_expansion(t_block *block)
{
	char	*temp;
	char	*redir_copy;
	char	*fail_return;

	fail_return = NULL;
	redir_copy = block->io_files->head->text;
	if (!expand_dollars(&redir_copy, block->ms) \
	|| !expand_wildcards(&redir_copy, &fail_return))
		return (0);
	if (fail_return)
		return (ambiguous_redirection_err(block, &fail_return));
	if (!remove_unguarded_quotes(&redir_copy, NULL))
		return (0);
	block->io_files->head->text = redir_copy;
	return (1);
}
