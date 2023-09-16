/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/16 16:50:30 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/16 17:29:30 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKEN_LIST_H
# define TOKEN_LIST_H

# include <stdlib.h>

typedef struct s_token_node	t_token_node;
typedef struct s_token_list	t_token_list;

typedef struct s_quote
{
	int	numtokens;
	int	dquote;
	int	squote;
	int	tstart;
}	t_quote;

struct s_token_node
{
	t_token_node	*next;
	t_token_node	*prev;
	int				type;
	char			*text;
};

struct s_token_list
{
	t_token_node	*head;
	t_token_node	*tail;
	size_t			len;
};

enum e_token
{
	T_OPEN_PAR = 1,
	T_CLOSE_PAR,
	T_OP_PIPE,
	T_OP_OR,
	T_OP_AND,
	T_OP_AMPER,
	T_INDIR_HD,
	T_INDIR_INF,
	T_OUTDIR_AP,
	T_OUTDIR_TRUN,
	T_ARG,
};

# define PRINT_OPEN_PAR "("
# define PRINT_CLOSE_PAR ")"
# define PRINT_OP_PIPE "|"
# define PRINT_OP_OR "||"
# define PRINT_OP_AND "&&"
# define PRINT_OP_AMPER "&"
# define PRINT_INDIR_HD "<<"
# define PRINT_INDIR_INF "<"
# define PRINT_OUTDIR_AP ">>"
# define PRINT_OUTDIR_TRUN ">"
# define PRINT_ARG "arg"

size_t			ft_strlen(const char *s);
void			print_token(t_token_node *token);
int				ft_isspace(int c);
int				ft_isquote(int c);
void			*ft_free_set_null(void *ptr);
void			*ft_free_charmat_null(void *table, void (*del)(void *));
char			*ft_strdup_len(char *s, int len);
void			*perror_msg_ptr(char *text);
t_token_node	*new_token_node(int type, char *text);
t_token_list	*token_list_new(void);
int				token_list_in_head(t_token_list *list, int type, char *text);
int				token_list_in_tail(t_token_list *list, int type, char *text);
void			token_list_del_head(t_token_list *list);
void			token_list_del_tail(t_token_list *list);
void			token_list_destroy(t_token_list **list);
void			token_list_head_print(t_token_list *list, \
void (*pnt)(t_token_node *));
void			token_list_tail_print(t_token_list *list, \
void (*pnt)(t_token_node *));
void			token_list_head_print(t_token_list *list, \
void (*pnt)(t_token_node *));
void			token_list_tail_print(t_token_list *list, \
void (*pnt)(t_token_node *));
void			update_quote_count(char **prompt, int *squotes, int *dquotes);
int				is_token(char **str, int *place_token, int to_update);
int				token_is_operator(int token);
char			*token_enum_to_text(int token);

#endif
