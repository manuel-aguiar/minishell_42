/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup_len.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 17:45:19 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 17:45:26 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*ft_strdup_len(char *s, int len)
{
	char	*dest;

	dest = malloc(sizeof(*dest) * (len + 1));
	if (!dest)
		return (perror_msg_ptr("malloc"));
	if (dest)
	{
		memcpy(dest, s, len);
		dest[len] = '\0';
	}
	return (dest);
}
