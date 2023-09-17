/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list_print.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:47:59 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/17 15:42:23 by mnascime         ###   ########.fr       */
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
	if (token == T_INDIR_OPEN)
		return (PRINT_INDIR_OPEN);
	if (token == T_OUTDIR_APPEND)
		return (PRINT_OUTDIR_APPEND);
	if (token == T_OUTDIR_TRUNC)
		return (PRINT_OUTDIR_TRUNC);
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
	printf("\n");
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
	printf("\n");
}

void	print_token(t_token_node *token)
{
	printf("type [%s]  ", token_enum_to_text(token->type));
	printf("text, [%s]", token->text);
	printf("\n");
}

void	print_token_args(t_token_node *token)
{
	printf("[%s]   ", token->text);
}
