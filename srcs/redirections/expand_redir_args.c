/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_redir_args.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 14:02:09 by codespace         #+#    #+#             */
/*   Updated: 2023/09/27 14:36:52 by codespace        ###   ########.fr       */
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



int	count_split_after_dollar(char ***split_place, char *redir_copy, int *count)
{
	char	*copy;
	char	**split;

	copy = ft_strdup(redir_copy);
	if (!copy)
		return (perror_msg_int("malloc", 0));
	empty_quotes(copy);
	split = ft_split_count_replenish(copy, redir_copy, "\t\v\n\r ", count);
	free(copy);
	if (split_place)
		*split_place = split;
	else
		ft_free_charmat_null(&split, free);
	return (1);
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
	if (!expand_dollars(&redir_copy, block->ms, true))
		return (0);
	count_split_after_dollar(NULL, redir_copy, &count);
	block->io_files->head->text = redir_copy;
	if (count != 1)
		return (ambiguous_redirection_err(block, &fail_return));
	redir_copy = block->io_files->head->text;
	if (!remove_unguarded_quotes_wildcard(&redir_copy, NULL))
		return (0);
	if (!expand_wildcards(&redir_copy, &count, &split))
		return (0);
	block->io_files->head->text = redir_copy;
	if (count > 1)
	{
		ft_free_charmat_null(&split, free);
		return (ambiguous_redirection_err(block, &fail_return));
	}
	ft_free_set_null(&redir_copy);
	block->io_files->head->text = split[0];
	ft_free_set_null(&split);
	if (!remove_unguarded_quotes(&block->io_files->head->text, NULL))
		return (0);
	turn_positive(block->io_files->head->text);
	ft_free_set_null(&fail_return);
	if (!block->io_files->head->text || !*(block->io_files->head->text))
		return (perror_msg_func(block, block->io_files->head->text, \
		CODE_OPEN, 1));
	return (1);
}
