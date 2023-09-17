/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_matrixlen.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:29:34 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/17 13:47:30 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_matrixlen(void *mat)
{
	t_uchar	**move;
	int		i;

	if (!mat)
		return (-1);
	move = (t_uchar **)mat;
	i = 0;
	while (move[i])
		i++;
	return (i);
}
