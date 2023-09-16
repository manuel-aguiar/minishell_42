/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_matrixlen.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:29:34 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 16:32:09 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_matrixlen(void *mat)
{
	t_uchar **move;
	int			i;

	if (!mat)
		return (-1);
	move = (t_uchar **)mat;
	i = 0;
	while (move[i])
		i++;
	return (i);
}
