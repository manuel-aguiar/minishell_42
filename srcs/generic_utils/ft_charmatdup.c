/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_charmatdup.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:30:23 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/16 16:30:30 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_charmatdup(char ***dest, char **src)
{
	char	**res;
	int		len;
	int		i;

	len = ft_matrixlen(src);
	res = malloc((len + 1) * sizeof(*res));
	if (!res)
		return (0);
	i = 0;
	while (i < len)
	{
		res[i] = ft_strdup(src[i]);
		if (!res[i])
		{
			ft_free_charmat_null(&res, &free);
			*dest = res;
			return (0);
		}
		i++;
	}
	res[i] = NULL;
	*dest = res;
	return (1);
}
