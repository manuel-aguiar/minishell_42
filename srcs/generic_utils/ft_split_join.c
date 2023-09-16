/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_join.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:22:00 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 16:23:02 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char    *ft_split_join(char **split, char *sep)
{
    int     len;
    int     i;
    int     j;
    int     k;
    char    *join;

    i = 0;
    len = 0;
    while (split[i])
        len += ft_strlen(split[i++]);
    len += (ft_strlen(sep) * (i - 1));
    join = malloc(sizeof(*join) * (len + 1));
    if (!join)
		return (perror_msg_ptr("malloc", NULL));
    k = 0;
    i = 0;
    while (split[i])
    {
        j = 0;
        while (split[i][j])
            join[k++] = split[i][j++];
        j = 0;
        if (split[i + 1])
            while (sep[j])
                join[k++] = sep[j++];
        i++;
    }
    join[k] = '\0';
    return (join);
}
