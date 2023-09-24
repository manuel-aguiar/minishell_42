/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wildcard_expansion.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:36:31 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/24 12:23:25 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	wildcard_search_replace
	function to find and insert on the cmd_arg/redirection the expansion 
	of the corresponding
	wildcard.
	"wildcard" function is only called from within this function, all the 
	functionally of wildcard.c
	ends up on this single function with no further entry points.
	"wildcard" reports on the number of matches via "&count" and so this 
	caller can determine whether the
	result will be ambiguous or not and free all the intermediate helpers
	 on the way and exit.
	fail_return will be passed to "expand_wildcards" in case of non-NULL, 
	and will give the caller of "expand_wildcards"
	the wildcard that failed in producing a single valid file descriptor.

*/

static int	wildcard_strjoin_result(char **to_expand, char **join, \
			int start, int end)
{
	char	*new;

	*to_expand[start] = '\0';
	new = ft_triple_join(*to_expand, *join, &((*to_expand)[end]));
	if (!new)
	{
		ft_free_set_null(to_expand);
		ft_free_set_null(join);
		return (0);
	}
	ft_free_set_null(to_expand);
	ft_free_set_null(join);
	*to_expand = new;
	return (1);
}

int	wildcard_search_replace(char **to_expand, int *index, char **fail_return)
{
	char	*join;
	int		count;
	int		start;
	int		end;

	start = *index;
	while (start > 0 && (*to_expand)[start - 1] != ' ' \
	&& (*to_expand)[start - 1] != '<' && (*to_expand)[start - 1] != '>')
		start--;
	end = *index;
	while ((*to_expand)[end] && (*to_expand)[end] != ' ')
		end++;
	join = wildcard(&(*to_expand)[start], end - start, &count);
	if (!join)
		return (0);
	if (count > 1 && fail_return)
	{
		(*to_expand)[end] = '\0';
		*fail_return = ft_triple_join("\'", &((*to_expand)[start]), "\'");
		free(join);
		return (1);
	}
	*index += (ft_strlen(join) - *index + start);
	return (wildcard_strjoin_result(to_expand, &join, start, end));
}

/*
	expand_wildcards

	function used to expand wildcards in cmd_args and redirections. 
	It is only used after the command has been into individua
	portions meaning that redirections and cmd args are processed seperatelly:
		-   failure in the file descriptors due to ambiguous redirection
		 means that the rest of the cmd_args
			dont need to be analysed and one can stop here.
	fail_return is used only when checking within redirections as the name 
	of the fuull wildcard is needed to print
	to console in terms of ambiguous redirection. When called to check cmd_args,
	 the caller calls the function with NULL here.
	This function takes a char ** (the place where the pointer to the string to 
	expand is place) in an effort to
	make the code more modular by alllowing the use for both cmd_arg expansion
	 and redirection expansion.

*/

static void	manage_quotes(char checkquote, int *quote, int *index)
{
	if (!*quote)
		*quote = checkquote;
	else if (*quote == checkquote)
		*quote = 0;
	*index += 1;
}

int	expand_wildcards(char **to_expand, char **fail_return)
{
	int	i;
	int	quote;

	quote = 0;
	i = 0;
	while ((*to_expand)[i])
	{
		if ((*to_expand)[i] == '\'' || (*to_expand)[i] == '"')
			manage_quotes((*to_expand)[i], &quote, &i);
		else if ((*to_expand)[i] == '*' && !quote)
		{
			if (!wildcard_search_replace(to_expand, &i, fail_return))
				return (0);
			if (fail_return && *fail_return)
				break ;
		}
		else
			i++;
	}
	return (1);
}
