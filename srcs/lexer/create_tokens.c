/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   create_tokens.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:03:01 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/25 09:02:11 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token_list.h"

int	update_quote_count(char prompt, int *squotes, int *dquotes)
{
	if (prompt == '\'' && (*dquotes) % 2 == 0)
		(*squotes)++;
	else if (prompt == '\"' && (*squotes) % 2 == 0)
		(*dquotes)++;
	else
		return (0);
	return (1);
}

void	is_divisor_token(char **str, int *place_token)
{
	if (**str == '&')
	{
		(*str)++;
		if (**str == '&')
		{
			*place_token = T_OP_AND;
			(*str)++;
		}
		else
			*place_token = T_OP_AMPER;
	}
	else if (**str == '|')
	{
		(*str)++;
		if (**str == '|')
		{
			*place_token = T_OP_OR;
			(*str)++;
		}
		else
			*place_token = T_OP_PIPE;
	}
}

void	is_redirection_token(char **str, int *place_token)
{
	if (**str == '<')
	{
		(*str)++;
		if (**str == '<')
		{
			*place_token = T_INDIR_HD;
			(*str)++;
		}
		else
			*place_token = T_INDIR_OPEN;
	}
	else if (**str == '>')
	{
		(*str)++;
		if (**str == '>')
		{
			*place_token = T_OUTDIR_APPEND;
			(*str)++;
		}
		else
			*place_token = T_OUTDIR_TRUNC;
	}
}

void	save_tokens(char **str, int *place_token)
{
	if (**str == '&' || **str == '|')
		is_divisor_token(str, place_token);
	else if (**str == '<' || **str == '>')
		is_redirection_token(str, place_token);
	else if (**str == ';')
	{
		*place_token = T_OP_SEMICOL;
		(*str)++;
	}
}

int	is_token(char **str, int *place_token, int to_update)
{
	size_t	rewind;

	rewind = ft_strlen((*str));
	if (!**str)
		return (0);
	else if (**str == '(')
	{
		*place_token = T_OPEN_PAR;
		(*str)++;
	}
	else if (**str == ')')
	{
		*place_token = T_CLOSE_PAR;
		(*str)++;
	}
	else if (**str == '&' || **str == '|' || **str == '<' \
		|| **str == '>' || **str == ';')
		save_tokens(str, place_token);
	else
		return (0);
	if (!to_update)
		(*str) -= rewind - ft_strlen((*str));
	return (1);
}
