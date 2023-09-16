/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_perror_msg.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 12:36:13 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 12:52:28 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_handling.h"
#include "ft_printf.h"

int	error_msg_int(char *text, int print_to_this_fd, int number_to_return)
{
	ft_printf_fd(print_to_this_fd, "%s", text);
	return (number_to_return);
}

void	*error_msg_ptr(char *text, int print_to_this_fd, void *ptr_to_return)
{
	ft_printf_fd(print_to_this_fd, "%s", text);
	return (ptr_to_return);
}

int	perror_msg_int(char *text, int number_to_return)
{
	perror(text);
	return (number_to_return);
}

void	*perror_msg_ptr(char *text, void *ptr_to_return)
{
	perror(text);
	return (ptr_to_return);
}
