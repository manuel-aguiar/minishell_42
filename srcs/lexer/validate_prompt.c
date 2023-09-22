/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_prompt.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 18:32:01 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/17 04:03:04 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "token_list.h"

static void	prepare_pars_error(t_ms *ms, int *parenthesis);
static int	valid_num_of_pars(t_ms *ms);
static int	valid_quote_num(t_ms *ms);
static int	no_amper(t_ms *ms);

int	prompt_is_valid(t_ms *ms)
{
	if (valid_elem_order(ms) && valid_num_of_pars(ms) \
	&& valid_redir_texts(ms) && valid_quote_num(ms) \
	&& no_amper(ms))
		return (1);
	return (0);
}

static void	prepare_pars_error(t_ms *ms, int *parenthesis)
{
	t_token_node	*cur;
	int				type;

	cur = ms->prompt->head;
	if ((*parenthesis) > 0)
		type = T_OPEN_PAR;
	else
		type = T_CLOSE_PAR;
	while (cur && cur->type != type)
		cur = cur->next;
	invalid_elem_msg(ms, cur->type, cur->text, \
	": syntax error near unexpected token `");
}

static int	valid_num_of_pars(t_ms *ms)
{
	t_token_node	*cur;
	int				parenthesis;

	if (!ms->prompt)
		return (1);
	cur = ms->prompt->head;
	parenthesis = 0;
	while (cur && parenthesis >= 0)
	{
		if (cur->type == T_OPEN_PAR)
			parenthesis++;
		if (cur->type == T_CLOSE_PAR)
			parenthesis--;
		cur = cur->next;
	}
	if (parenthesis == 0)
		return (1);
	prepare_pars_error(ms, &parenthesis);
	return (0);
}

static int	valid_quote_num(t_ms *ms)
{
	int				i;
	int				squote;
	int				dquote;
	t_token_node	*cur;

	if (!ms->prompt)
		return (1);
	cur = ms->prompt->head;
	while (cur)
	{
		if (cur->type >= T_INDIR_HD && cur->text)
		{
			i = -1;
			squote = 0;
			dquote = 0;
			while (cur->text[++i])
				update_quote_count(cur->text[i], &squote, &dquote);
			if (squote % 2 != 0 || dquote % 2 != 0)
				return (invalid_elem_msg(ms, cur->type, \
				cur->text, ": lack of matching quote for argument `"));
		}
		cur = cur->next;
	}
	return (1);
}

static int	no_amper(t_ms *ms)
{
	t_token_node	*cur;

	if (!ms->prompt)
		return (1);
	cur = ms->prompt->head;
	while (cur)
	{
		if (cur->type == T_OP_AMPER)
		{
			return (invalid_elem_msg(ms, cur->type, \
			cur->text, ": unsupported functionality `"));
		}
		cur = cur->next;
	}
	return (1);
}
