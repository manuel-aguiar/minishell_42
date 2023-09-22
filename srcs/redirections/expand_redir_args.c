/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/22 17:34:21 by codespace        ###   ########.fr       */
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
	char	*redir_copy;
	char	*fail_return;
	char	*copy;
	char	**split;
	int		count;

	fail_return = NULL;
	redir_copy = block->io_files->head->text;
	copy = ft_strdup(redir_copy);												//unprotected
	
	if (!expand_dollars(&redir_copy, block->ms))
		return (0);
	split = ft_split_count(redir_copy, "\n\t\v ", &count);						//unprotected
	if (count != 1)
	{
		ambiguous_redirection_err(block, &copy);
		ft_free_charmat_null(&split, free);
		dprintf(2, "NO EXIT STATUS YET\n");
		block->io_files->head->text = redir_copy;
		return (0);
	}
	ft_free_set_null(&copy);
	ft_free_charmat_null(&split, free);
	if (!expand_wildcards(&redir_copy, &fail_return))
		return (0);
	if (!remove_unguarded_quotes(&redir_copy, NULL))
		return (0);
	if (fail_return)
		return (ambiguous_redirection_err(block, &fail_return));
	if (!*(redir_copy))
		dprintf(2, "no such file or directory REVER E ERROR MESSAGE\n");
	block->io_files->head->text = redir_copy;
	return (1);
}
