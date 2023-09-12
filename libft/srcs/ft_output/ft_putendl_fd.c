/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putend_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: manuel <manuel@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/11 15:32:46 by manuel            #+#    #+#             */
/*   Updated: 2023/03/11 15:32:46 by manuel           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_output.h"

void	ft_putendl_fd(char *str, int fd)
{
	if (!str || fd < 0)
		return ;
	write(fd, str, ft_strlen(str));
	write(fd, "\n", 1);
}

void	ft_putendl(char *str)
{
	ft_putendl_fd(str, 1);
}
