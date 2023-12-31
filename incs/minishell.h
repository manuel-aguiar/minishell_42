/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 10:08:39 by marvin            #+#    #+#             */
/*   Updated: 2023/09/27 15:55:06 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H

# define MINISHELL_H

# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <limits.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <errno.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include <signal.h>
# include <termios.h>

# include "libft.h"
# include "token_list.h"

# define MYNAME "minishell"

/* error name macros */
# define ERR_CMD "command not found"
# define ERR_IS_DIR "Is a directory"
# define ERR_PERM "Permission denied"

/* error code macros */
# define CODE_OPEN 1
# define CODE_DIR 126
# define CODE_PERM 126
# define CODE_CMD 127
# define CODE_DUP 1
# define CODE_DUP2 1
# define CODE_EXECVE 1

# define EXIT_SIGINT 130
# define EXIT_SIGQUIT 131

# define CODE_SYNTAX_BINS 1
# define EXIT_SYNTAX 2
# define EXIT_AMBIG_REDIR 1
# define EXIT_SIGNALED 128
# define EXIT_TOO_MANY_ARGS 1
# define EXIT_NON_NUMERICAL 2

extern int				g_signal;
typedef struct s_ms		t_ms;
typedef struct s_block	t_block;

struct s_ms
{
	char				**env;
	t_token_list		*prompt;
	char				*name;
	char				*name_readline;
	int					infd;
	int					outfd;
	int					errfd;
	int					exit_status;
	t_block				*first;
	pid_t				my_kid;
	int					dup_stdin;
	int					kill_stdin;
	struct sigaction	sigact;
	struct termios		original;
	struct termios		modified;
};

struct s_block
{
	t_token_list	*prompt;				
	t_ms			*ms;					
	t_block			*my_manager;				
	t_block			**worker_list;
	t_token_list	**worker_tasks;
	pid_t			*worker_pids;
	int				*worker_exit_status;
	int				*op_id;
	int				op_count;
	int				is_worker;
	int				has_arithmatic_parenthesis;
	int				must_subshell;
	int				pipefd[2];
	int				prev_pipe[2];				
	int				my_status;				
	int				i_am_forked;
	char			*cmd;					
	char			**cmd_args;
	char			**env_path;
	t_token_list	*io_files;					
	char			*here_doc;
	int				here_doc_fd;
	int				here_doc_index;
	int				final_in;
	int				final_out;
	int				my_level;
	int				my_id;
};

typedef struct s_dollars
{
	t_ms		*ms;
	int			dol_len;
	int			quote;
	int			neg;
	char		*neg_copy;
	char		*new_final;
}				t_dollars;

typedef struct s_wildc
{
	char			*pattern;
	char			search_start[2];
	int				depth;
	int				sub_count;
	int				pat_len;
	t_vdmlist		*files;
	char			*test;
	char			*join;
	int				match_count;
	struct dirent	*entry;
	char			*filename;
	char			*copy_dir;
	int				candlen;
	int				i;
	int				j;
	int				startind;
	int				match;
}	t_wildc;

enum e_builtin
{
	BI_CD = 1,
	BI_ENV,
	BI_PWD,
	BI_ECHO,
	BI_EXIT,
	BI_UNSET,
	BI_EXPORT,
};
//////////////////////////////////////
//////////////////////////////////////
////////////// STRUCTS ///////////////
//////////////////////////////////////
//////////////////////////////////////

/*ms_setup.c*/
int		ms_init(t_ms *ms, char **env);
int		ms_set_path(t_ms *ms);
int		ms_setup_initial_env(t_ms *ms, char **env);
int		ms_increase_shell_level(t_ms *ms);
int		ms_destroy(t_ms *ms);

/*block_setup.c*/
t_block	*block_init(t_ms *ms, t_block *manager, \
		t_token_list *prompt, int my_id);
void	block_destroy(void *og_block);

/* ms_signals.c */
void	signal_handler(int signum);
int		ms_prepare_signal(t_ms *ms, void (*handler)(int));
int		ms_reset_signal(t_ms *ms);

//////////////////////////////////////
//////////////////////////////////////
//////////////// LEXER ///////////////
//////////////////////////////////////
//////////////////////////////////////

/*get_prompt.c*/
int		get_prompt(t_ms *ms);
char	*prompt_readline(t_ms *ms);
int		prompt_token_setup(t_ms *ms, char *line);

int		ft_isquote(int c);
char	*ft_strdup_len(char *s, int len);
int		prompt_is_valid(t_ms *ms);
int		valid_elem_order(t_ms *ms);
int		valid_redir_texts(t_ms *ms);
int		invalid_elem_msg(t_ms *ms, int type, char *has_text, char *text);
void	deal_with_text(t_token_list *list, \
		char **prompt, char **temp, int to_create);

//////////////////////////////////////
//////////////////////////////////////
////////////// PARSER ////////////////
//////////////////////////////////////
//////////////////////////////////////

/* task_distributor.c*/
int		distribute_tasks_between_managers_and_workers(t_block *block);
int		setup_task_distributor(t_block *block);
int		free_task_distributor(t_block *block);
int		check_if_worker_and_count_operators(t_block *block);

/*manager_tasks_1.c*/

int		manager_extract_redirections(t_block *manager);
int		manager_subshell_and_arithmatic(t_block *manager);
int		manager_check_arithmatic_parenthesis(t_block *manager);
void	manager_destroy_worker_tasks(t_block *manager);

/*manager_tasks_2.c*/
int		manager_gets_workers_and_operators(t_block *manager);

/*worker_tasks_1.c*/
int		worker_task_preparation(t_block *worker);
int		worker_extract_redirections(t_block *worker);
void	worker_turn_neg_chars_to_pos(t_token_list *list);
int		worker_expansion_add_tokens(t_block *worker, t_token_node **cur, \
			char ***split_place, int move);

/*worker_tasks_2.c*/
int		worker_args_rm_unguarded_quotes(t_block *worker);
int		worker_args_expand_dollar_split(t_block *worker);
int		worker_args_expand_wildcard_split(t_block *worker);
int		worker_dump_tasks_to_cmd_args(t_block *worker);

/*worker_tasks_3.c*/
int		worker_args_expand_wildcard_split(t_block *worker);

//////////////////////////////////////
//////////////////////////////////////
////////////// EXECUTER //////////////
//////////////////////////////////////
//////////////////////////////////////

/*execution_tree.c*/
int		execution_tree_exec_all(t_block *block);
int		setup_execution_tree(t_ms *ms, t_block *manager, \
							t_token_list *tasks, int my_id);
int		get_all_here_docs(t_block *block);

/*manager_execution.c*/
int		pipes_forks_and_conditionals(t_block *manager, int index);
int		waiting_for_my_workers(t_block *manager, int index);
int		wait_and_save_status(pid_t pid, int *status, int errfd);

/*worker_execution.c*/
int		worker_execution(t_block *worker);

/* error_message.c */

int		setup_execution_tree(t_ms *ms, t_block *manager, \
		t_token_list *prompt, int my_id);
int		exec_command(t_block *block);
int		process_execution(t_block *block);

/*child_process.c*/
int		child_process(t_block *block);
void	error_child_exit(t_block *block, char *error_msg, int errcode, \
		int with_ms);
void	perror_child_exit(t_block *block, int errcode, \
		int with_ms);

/*child_execve.c*/
int		exec_command(t_block *block);
int		perror_msg_func(t_block *block, char *function, \
		int errcode, int with_ms);

//////////////////////////////////////
//////////////////////////////////////
//////////// BUILTINS ////////////////
//////////////////////////////////////
//////////////////////////////////////

/*bi_check_and_exec.c*/
int		check_builtins(t_block *block);
int		exec_builtin(t_block *block, int builtin);

int		run_cd(t_block *block);
int		run_env(t_block *block);
int		run_pwd(t_block *block);
int		run_echo(t_block *block);
int		run_unset(t_block *block);
int		run_export(t_block *block);
int		run_exit(t_block *block);

int		set_beg_path(t_block *block, char **curpath, char *curr);
int		get_corr_env(t_block *block, char *arg, int is_exporting);
int		env_add(t_block *block, char *new);
int		env_remove(t_block *block, int index);
int		env_strcmp(void *s1, void *s2);

//////////////////////////////////////
//////////////////////////////////////
//////////// REDIRECTIONS ////////////
//////////////////////////////////////
//////////////////////////////////////

/*prepare_redirections.c*/
int		prepare_redirections(t_block *block);

/*expand_redir_args.c*/
int		manage_io_expansion(t_block *block);

/*close_in_out.c*/
void	close_in_fds(t_block *block);
void	close_out_fds(t_block *block);

/*heredoc_open*/
int		open_here_docs_at_block(t_block *block);

/*heredoc_temp.c*/
int		heredoc_temp_name(t_block *block);

/*heredoc_read.c*/
int		here_doc(t_block *block, char *eof, int has_quote_guard);

/*heredoc_open.c*/
int		open_here_docs_at_block(t_block *block);

//////////////////////////////////////
//////////////////////////////////////
//////////// EXPANSIONS //////////////
//////////////////////////////////////
//////////////////////////////////////

/*wildcard_expansion.c*/
int		expand_wildcards(char **to_expand, int *count, char ***place_split);

/*wildcard_return.c*/
char	**wildcard(char *pattern, int pat_len, int *match_count);
void	*destroy_wildcard(t_wildc *wildcard, int clean_exit);

/*wildcard_fit_candidates.c*/
int		wildcard_fit_check(t_wildc *wc, char *text);

/* wildcard_search_files.c */
void	void_putstr(void *str);
int		list_all_wildcard_matches(t_wildc *wildc, char *path, int cur_lvl);
char	**list_to_array(t_vdmlist *list);

/* dollar_expansion.c */
int		expand_dollars(char **to_expand, t_ms *ms, int turn_negative);
int		dollar_search_replace(t_dollars *dol, char **to_expand, int *index);

/* dollar_concat.c*/
int		dollar_search_env(t_dollars *dol, char **to_expand, int *index);

/* dollar_heredoc*/
int		here_doc_expand_dollars(char **to_expand, t_ms *ms);

/*rm_unguarded_quotes*/
int		remove_unguarded_quotes(char **str, int *has_guards);
int		remove_unguarded_quotes_wildcard(char **str, int *has_guards);
int		count_chars(char *str, char c);

/* expansion_char_protec.c*/
void	turn_positive(char *str);
int		empty_quotes(char *str);
int		count_split_after_dollar(char ***split_place, char *redir_copy, \
		int *count);

//////////////////////////////////////
//////////////////////////////////////
//////////// GENERIC UTILS ///////////
//////////////////////////////////////
//////////////////////////////////////

int		ft_matrixlen(void *mat);
int		ft_charmatdup(char ***dest, char **src);
void	*quicksort_pointers(void *arr, int size, int (*cmp)(void *, void *));
int		ft_matrixlen(void *mat);
char	**ft_split_count_replenish(t_cchar *s, t_cchar *og, \
		char *sepset, int *place_count);
char	*ft_split_join(char **split, char *sep);
char	*ft_triple_join(char *first, char *second, char *third);

#endif
