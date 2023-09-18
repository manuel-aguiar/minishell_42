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

#include "token_list.h"

static void	prepare_pars_error(t_token_list *list, int *parenthesis);
static int	valid_num_of_pars(t_token_list *list);
static int	valid_quote_num(t_token_list *list);
static int	no_amper(t_token_list *list);

int	prompt_is_valid(t_token_list *list)
{
	if (valid_elem_order(list) && valid_num_of_pars(list) \
	&& valid_redir_texts(list) && valid_quote_num(list) \
	&& no_amper(list))
		return (1);
	return (0);
}

static void	prepare_pars_error(t_token_list *list, int *parenthesis)
{
	t_token_node	*cur;
	int				type;

	cur = list->head;
	if ((*parenthesis) > 0)
		type = T_OPEN_PAR;
	else
		type = T_CLOSE_PAR;
	while (cur && cur->type != type)
		cur = cur->next;
	invalid_elem_msg(cur, "minishell: syntax error near unexpected token `");
}

static int	valid_num_of_pars(t_token_list *list)
{
	t_token_node	*cur;
	int				parenthesis;

	if (!list)
		return (1);
	cur = list->head;
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
	prepare_pars_error(list, &parenthesis);
	return (0);
}

static int	valid_quote_num(t_token_list *list)
{
	t_token_node	*cur;

	if (!list)
		return (1);
	cur = list->head;
	while (cur)
	{
		if (cur->type >= T_INDIR_HD && cur->text && \
		(cur->text[0] == '\'' || cur->text[0] == '\"'))
		{
			if (cur->text && cur->text[0] != \
			cur->text[ft_strlen(cur->text) - 1])
				return (invalid_elem_msg(cur, \
			"minishell: lack of matching quote for argument `"));
		}
		cur = cur->next;
	}
	return (1);
}

static int	no_amper(t_token_list *list)
{
	t_token_node	*cur;

	if (!list)
		return (1);
	cur = list->head;
	while (cur)
	{
		if (cur->type == T_OP_AMPER)
		{
			return (invalid_elem_msg(cur, \
			"minishell: unsupported functionality "));
		}
		cur = cur->next;
	}
	return (1);
}
