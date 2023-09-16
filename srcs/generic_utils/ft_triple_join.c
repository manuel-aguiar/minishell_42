/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_triple_join.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:24:17 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 16:24:32 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char    *ft_triple_join(char *first, char *second, char *third)
{
    int     i;
    char    *new;

    new = malloc((ft_strlen(first) + ft_strlen(second) \
		+ ft_strlen(third) + 1) * sizeof(*first));
    if (!new)
		return (perror_msg_ptr("malloc", NULL));
    i = 0;
    while (*first)
        new[i++] = *first++;
    while (*second)
        new[i++] = *second++;
    while (*third)
        new[i++] = *third++;
    new[i] = '\0';
    return (new);
}
