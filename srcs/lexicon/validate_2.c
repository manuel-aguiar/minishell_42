/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   validate_2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 18:32:01 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/17 04:03:46 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token_list.h"

char	*ft_strjoin(char *s1, char const *s2)
{
	char	*ptr;
	size_t	i;
	size_t	f;

	i = 0;
	f = 0;
	ptr = malloc((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	while (s1 && s1[i] != '\0')
	{
		ptr[i] = s1[i];
		i++;
	}
	while (s2[f] != '\0')
	{
		ptr[i++] = s2[f];
		if (s2[f] == '\n')
			break ;
		f++;
	}
	ptr[i] = '\0';
	return (ptr);
}

int	invalid_elem_msg(t_token_node *elem, char *text)
{
	char	*msg;

	if (!elem->text)
		msg = ft_strjoin(text, token_enum_to_text(elem->type));
	else
		msg = ft_strjoin(text, &elem->text[0]);
	if (!msg)
	{
		perror_msg_ptr("malloc");
		return (0);
	}
	printf("%s\n", msg);
	free(msg);
	return (0);
}

int	valid_redir_texts(t_token_list *list)
{
	t_token_node	*cur;

	if (!list)
		return (1);
	cur = list->head;
	while (cur)
	{
		if (cur->type >= T_INDIR_HD && cur->type <= T_OUTDIR_TRUN && !cur->text)
			return (invalid_elem_msg(cur, \
		"minishell: syntax error near unexpected token "));
		cur = cur->next;
	}
	return (1);
}

static int	valid_consecutive_elem(t_token_node *cur)
{
	if (cur->type == cur->prev->type && \
	cur->type >= T_OP_PIPE && cur->type <= T_OP_AND)
		return (invalid_elem_msg(cur, \
	"minishell: syntax error near unexpected token "));
	else if (cur->type != cur->prev->type && \
	((cur->prev->type >= T_OP_PIPE && cur->prev->type <= T_OP_AND \
	&& cur->type >= T_OP_PIPE && cur->type <= T_OP_AND) || \
	(cur->prev->type == T_OPEN_PAR && cur->type >= T_OP_PIPE \
	&& cur->type <= T_OP_AND) || \
	(cur->prev->type >= T_OP_PIPE && cur->prev->type <= T_OP_AND \
	&& cur->type == T_CLOSE_PAR) || \
	(cur->type == T_OPEN_PAR && cur->prev->type == T_CLOSE_PAR) || \
	(cur->type == T_CLOSE_PAR && cur->prev->type == T_OPEN_PAR) || \
	(cur->type >= T_OPEN_PAR && cur->type <= T_CLOSE_PAR && \
	cur->prev->type >= T_INDIR_HD && cur->prev->type <= T_OUTDIR_TRUN) || \
	(cur->type == T_OPEN_PAR && cur->prev->type >= T_ARG) || \
	(cur->type == T_ARG && cur->prev->type >= T_CLOSE_PAR)))
		return (invalid_elem_msg(cur, \
	"minishell: syntax error near unexpected token "));
	return (1);
}

int	valid_elem_order(t_token_list *list)
{
	t_token_node	*cur;

	if (!list)
		return (1);
	cur = list->head;
	if (cur->type >= T_CLOSE_PAR && cur->type <= T_OP_AND)
		return (invalid_elem_msg(cur, \
	"minishell: syntax error near unexpected token "));
	while (cur && cur->next)
	{
		cur = cur->next;
		if (!valid_consecutive_elem(cur))
			return (0);
	}
	cur = list->tail;
	if (cur->type == T_OPEN_PAR || \
	(cur->type >= T_OP_PIPE && cur->type <= T_OP_AND))
		return (invalid_elem_msg(cur, \
	"minishell: syntax error near unexpected token "));
	return (1);
}
