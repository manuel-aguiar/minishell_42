/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 15:07:24 by codespace        ###   ########.fr       */
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

static int	manage_io_expansion_helper(t_block *block, int *count, \
			char ***split_place, char **fail_return)
{
	if (*count > 1)
	{
		ft_free_charmat_null(split_place, free);
		return (ambiguous_redirection_err(block, fail_return));
	}
	ft_free_set_null(&block->io_files->head->text);
	block->io_files->head->text = (*split_place)[0];
	ft_free_set_null(split_place);
	if (!remove_unguarded_quotes(&block->io_files->head->text, NULL))
	{
		ft_free_set_null(fail_return);
		return (0);
	}
	turn_positive(block->io_files->head->text);
	ft_free_set_null(fail_return);
	if (!block->io_files->head->text || !*(block->io_files->head->text))
		return (perror_msg_func(block, block->io_files->head->text, \
		CODE_OPEN, 1));
	return (1);
}

int	manage_io_expansion(t_block *block)
{
	char	*fail_return;
	char	**split;
	int		count;

	fail_return = ft_triple_join("\'", block->io_files->head->text, "\'");
	if (!fail_return)
		return (perror_msg_int("malloc", 0));
	if (!expand_dollars(&block->io_files->head->text, block->ms, true))
	{
		free(fail_return);
		return (0);
	}
	count_split_after_dollar(NULL, block->io_files->head->text, &count);
	if (count != 1)
		return (ambiguous_redirection_err(block, &fail_return));
	if (!remove_unguarded_quotes_wildcard(&block->io_files->head->text, NULL) \
	|| !expand_wildcards(&block->io_files->head->text, &count, &split))
	{
		free(fail_return);
		return (0);
	}
	return (manage_io_expansion_helper(block, &count, &split, &fail_return));
}
