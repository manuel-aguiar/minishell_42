/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 18:32:01 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/25 09:43:23 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "token_list.h"

int	invalid_elem_msg(t_ms *ms, int type, char *has_text, char *text)
{
	char	*msg;

	if (!has_text)
		msg = ft_strjoin(text, token_enum_to_text(type));
	else
		msg = ft_strjoin(text, has_text);
	if (!msg)
		return (perror_msg_int("malloc", 0));
	ft_printf_fd(ms->errfd, "%s%s", ms->name, msg);
	ft_printf_fd(ms->errfd, "'\n");
	free(msg);
	return (0);
}

int	valid_redir_texts(t_ms *ms)
{
	t_token_node	*cur;

	if (!ms->prompt)
		return (1);
	cur = ms->prompt->head;
	while (cur)
	{
		if (cur->type >= T_INDIR_HD && \
		cur->type <= T_OUTDIR_TRUNC && !cur->text)
			return (invalid_elem_msg(ms, cur->type, cur->text, \
		": syntax error near unexpected token `"));
		cur = cur->next;
	}
	return (1);
}

static int	valid_consecutive_elem(t_ms *ms, t_token_node *cur)
{
	if (cur->type == cur->prev->type && \
	cur->type >= T_OP_PIPE && cur->type <= T_OP_AND)
		return (invalid_elem_msg(ms, cur->type, \
	cur->text, ": syntax error near unexpected token `"));
	else if (cur->type != cur->prev->type && \
	((cur->prev->type >= T_OP_PIPE && cur->prev->type <= T_OP_AND \
	&& cur->type >= T_OP_PIPE && cur->type <= T_OP_AND) || \
	(cur->prev->type == T_OPEN_PAR && cur->type >= T_OP_PIPE \
	&& cur->type <= T_OP_AND) || \
	(cur->prev->type >= T_OP_PIPE && cur->prev->type <= T_OP_AND \
	&& cur->type == T_CLOSE_PAR) || \
	(cur->type == T_OPEN_PAR && cur->prev->type == T_CLOSE_PAR) || \
	(cur->type == T_CLOSE_PAR && cur->prev->type == T_OPEN_PAR) || \
	(cur->type == T_OPEN_PAR && cur->prev->type >= T_INDIR_HD)))
		return (invalid_elem_msg(ms, cur->type, cur->text, \
	": syntax error near unexpected token `"));
	else if (cur->type == T_ARG && cur->prev->type == T_CLOSE_PAR)
		return (invalid_elem_msg(ms, cur->prev->type, \
	cur->prev->text, ": syntax error near unexpected token `"));
	return (1);
}

int	valid_elem_order(t_ms *ms)
{
	t_token_node	*cur;

	if (!ms->prompt)
		return (1);
	cur = ms->prompt->head;
	if (cur->type >= T_CLOSE_PAR && cur->type <= T_OP_AND)
		return (invalid_elem_msg(ms, cur->type, cur->text, \
	": syntax error near unexpected token `"));
	while (cur && cur->next)
	{
		cur = cur->next;
		if (!valid_consecutive_elem(ms, cur))
			return (0);
	}
	cur = ms->prompt->tail;
	if (cur->type == T_OPEN_PAR || (cur->type != T_OP_SEMICOL \
	&& cur->type >= T_OP_PIPE && cur->type <= T_OP_AND))
		return (invalid_elem_msg(ms, cur->type, \
	cur->text, ": syntax error near unexpected token `"));
	return (1);
}
