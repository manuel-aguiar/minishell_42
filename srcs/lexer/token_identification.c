/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_identification.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:00:55 by codespace         #+#    #+#             */
/*   Updated: 2023/09/25 08:54:29 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	token_is_big_operator(t_token_node *token)
{
	if (!token)
		return (0);
	if (token->type == T_OP_PIPE \
	|| token->type == T_OP_SEMICOL \
	|| token->type == T_OP_OR \
	|| token->type == T_OP_AND)
		return (1);
	return (0);
}

int	token_is_redirection(t_token_node *token)
{
	if (!token)
		return (0);
	if (token->type == T_INDIR_HD \
	|| token->type == T_INDIR_OPEN \
	|| token->type == T_OUTDIR_APPEND \
	|| token->type == T_OUTDIR_TRUNC)
		return (1);
	return (0);
}

int	token_is_operator(int token)
{
	if (token == T_OP_PIPE \
	|| token == T_OP_SEMICOL \
	|| token == T_OP_OR \
	|| token == T_OP_AND \
	|| token == T_OP_AMPER \
	|| token == T_OPEN_PAR \
	|| token == T_CLOSE_PAR)
		return (1);
	return (0);
}
