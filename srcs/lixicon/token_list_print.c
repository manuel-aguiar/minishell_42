/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list_print.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:47:59 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/16 17:44:02 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token_list.h"

char	*token_enum_to_text(int token)
{
	if (token == T_OPEN_PAR)
		return (PRINT_OPEN_PAR);
	if (token == T_CLOSE_PAR)
		return (PRINT_CLOSE_PAR);
	if (token == T_OP_PIPE)
		return (PRINT_OP_PIPE);
	if (token == T_OP_OR)
		return (PRINT_OP_OR);
	if (token == T_OP_AND)
		return (PRINT_OP_AND);
	if (token == T_OP_AMPER)
		return (PRINT_OP_AMPER);
	if (token == T_INDIR_HD)
		return (PRINT_INDIR_HD);
	if (token == T_INDIR_INF)
		return (PRINT_INDIR_INF);
	if (token == T_OUTDIR_AP)
		return (PRINT_OUTDIR_AP);
	if (token == T_OUTDIR_TRUN)
		return (PRINT_OUTDIR_TRUN);
	if (token == T_ARG)
		return (PRINT_ARG);
	if (token == T_OPEN_PAR)
		return (PRINT_OPEN_PAR);
	return (NULL);
}

void	token_list_head_print(t_token_list *list, void (*pnt)(t_token_node *))
{
	t_token_node	*cur;

	if (!list)
		return ;
	cur = list->head;
	while (cur)
	{
		pnt(cur);
		cur = cur->next;
	}
}

void	token_list_tail_print(t_token_list *list, void (*pnt)(t_token_node *))
{
	t_token_node	*cur;

	if (!list)
		return ;
	cur = list->tail;
	while (cur)
	{
		pnt(cur);
		cur = cur->prev;
	}
}

void	print_token(t_token_node *token)
{
	printf("type [%s]  ", token_enum_to_text(token->type));
	printf("text, [%s]", token->text);
	printf("\n");
}
