/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 10:08:39 by marvin            #+#    #+#             */
/*   Updated: 2023/09/14 23:28:00 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H

# define MINISHELL_H

# include <fcntl.h>
# include <stdlib.h>
# include <stdio.h>
# include <unistd.h>
# include <stdio.h>
# include <limits.h>
# include <sys/wait.h>
# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include <signal.h>
# include <sys/ioctl.h>


# include "libft.h"

# define TRUE 1
# define FALSE 0

# define MYNAME "minishell"

/* error name macros */
# define ERR_PIPE "pipe"
# define ERR_FORK "fork"
# define ERR_DUP "dup"
# define ERR_OPEN "open"
# define ERR_DUP2 "dup2"
# define ERR_MALLOC "malloc"
# define ERR_CMD "command not found"

/* error code macros */

# define CODE_OPEN 1
# define CODE_CMD 127
# define CODE_DUP2 123123     //i don't know...
# define CODE_EXECVE 123123     //i don't know...

# define EXIT_SIGINT 130

# define EXIT_SYNTAX 2
# define EXIT_AMBIG_REDIR 1

typedef struct s_ms		t_ms;
typedef struct s_block	t_block;
typedef struct s_redir	t_redir;

struct s_ms
{
	char		        **env;
	char		        **path;
	char		        *prompt;
	char		        *name;
	int			        infd;
	int			        outfd;
	int                 errfd;
	int			        exit_status;
	t_block		        *first;
	pid_t               my_kid;
	struct sigaction    sigact;
};



typedef struct s_prompt
{
    char    *prompt;
    char    *copy;
    int     parenthesis;
    int     first_error_index;
    int     exit_status;
    char    *err_token;
    int     errfd;
}   t_prompt;


struct s_block
{
    char		*prompt;	             //herdado do bloco pai;
	t_ms		*ms;	                	//to access and change env and path
	t_block		*father;                	// to comunicate exit status with the parent

    t_block     **child_list;
    char        **child_prompts;
    pid_t       *child_pids;
	int			*child_exit_status;
    int         *op_id;
    int         op_count;
    int         is_cmd;
    int         has_unnecessary_parenthesis;
    int         parenthesis_fork;

    int         pipefd[2];
    int         prev_pipe[2];                                    //posso precisar para unir os blocos filhos
    int         my_status;	                // recebido dos filhos para informar o avô;
    int         i_am_forked;

    char    	*cmd;	                //decomposição do prompt
    char    	**cmd_args;                 //decomposição do prompt
    t_vdmlist   *io_files;	            //decomposição do prompt, farão override ao infd do block, deixa de heredar do bloco anterior  void *list, com struct t_redir;
    char        *here_doc;              //here_doc, just in case, analisado um a um;
    int         here_doc_fd;
    int         here_doc_index;
    int         final_in;
    int         final_out;

    //char        **help_cmd;
    int         my_level;
    int         my_id;
};

struct s_redir
{
    char    *file;
    int     type;
};


typedef struct s_split_prompt
{
    char        *prompt_orig;
    char        *prompt_copy;
    char        **children;
    pid_t       *child_pids;
    int         *op_id;
    int         op_count;
    char        **get_redir;
    int         redir_split_len;
    t_vdmlist   *io_files;
    int         parenthesis_fork;
    int         has_unnecessary_parenthesis;
}   t_split_prompt;

typedef struct s_prep_cmd
{
    char    	*cmd;	                //decomposição do prompt
    char    	**cmd_args;                 //decomposição do prompt
    char        *prompt_copy;
    char        **cmd_args_copy;
    int         args_len;
    int         split_len;
    t_ms        *ms;
}   t_prep_cmd;

typedef struct s_wildc
{
    char        *pattern;
    char        **sub_pats;
    int         depth;
    int         sub_count;
    int         pat_len;
    t_vdmlist   *files;
    char        *test;
    char        **split;
    char        *join;
    int         match_count;
}   t_wildc;



enum e_op
{
	OP_AND = 1,
	OP_OR,
	OP_PIPE,
};

enum e_redir
{
    RE_HEREDOC = 1,
    RE_INFILE,
    RE_TRUNC,
    RE_APPEND,
};

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

/*
	if (len == 2 && ft_strncmp(cmd[0], "cd", 2) == 0)
		run_cd(cmd, env, final_out);
	else if (len == 3 && ft_strncmp(cmd[0], "env", 3) == 0)
		run_env(env, final_out);
	else if (len == 3 && ft_strncmp(cmd[0], "pwd", 3) == 0)
		run_pwd(final_out);
	else if (len == 4 && ft_strncmp(cmd[0], "echo", 4) == 0)
		run_echo(cmd, final_out);
	else if (len == 4 && ft_strncmp(cmd[0], "exit", 4) == 0)
		run_exit();
	else if (len == 5 && ft_strncmp(cmd[0], "unset", 5) == 0)
		run_unset(cmd, env);
	else if (len == 6 && ft_strncmp(cmd[0], "export", 6) == 0)

*/

/* init_destroy */
int     init_ms(t_ms *ms, char *avzero, char **env);
int     destroy_ms(t_ms *ms);

t_block	*init_block(t_ms *ms, t_block *father, char *pmt, int my_id);
void	destroy_block(void *og_block);

t_redir *init_redir(char *file, int type);
void	destroy_redir(void *og_redir);

/* ms_signals.c */

int     save_signal(int *num);
int     check_for_signals(t_ms *ms);
void    signal_handler(int signum);
int     ms_prepare_signal(t_ms *ms, void (*handler)(int));
t_ms    *sigint_heredoc_where_ms_is(t_ms *ms);									// not in use atm
void    signal_handler_heredoc(int signum);

/* ms_prompt.c */
int     get_prompt(t_ms *ms);
int     setup_prompt_struct(t_prompt *pmt, t_ms *ms);
int     syntax_error_msg(t_prompt *pmt);
int     syntax_error_manager(t_prompt *pmt, int err_code, char *err, int position);
int     validate_parenthesis_close(t_prompt *pmt);
int     validate_quote_close(t_prompt *pmt);
int     syntax_operators_end(t_prompt *pmt);
int     syntax_redir_end(t_prompt *pmt);
int     syntax_redirections(t_prompt *pmt);
int     syntax_operators(t_prompt *pmt);
int     open_prths_helper(char *copy, int index);
int     close_prths_helper(char *copy, int index);
int     syntax_parenthesis(t_prompt *pmt);
int     syntax_begin(t_prompt *pmt);
int     validate_syntax(t_prompt *pmt);
int     copy_empty_quotes(t_prompt *pmt);
void    rm_unnecessary_parenthesis(t_prompt *pmt);
void    rm_corner_parenthesis(char *copy, char *original);



//functions to split prompt into children

int     split_prompt(t_block *block);
int     split_children(t_split_prompt *split);
int     new_split_get_operator(t_split_prompt *split, int index, int count);

int     split_count_operators(t_split_prompt *split);
char    *split_copy_empty_quotes_and_parenthesis(char *prompt);
void    set_in_between_to(char *pmt, char start, char end, char newchar);
void    print_split(t_split_prompt *split);
int     free_split_prompt(t_split_prompt *split);


//functions to prepare commands

int     setup_cmd_pre_expansion(t_block *block);
int     manage_cmd_expansions(t_block *block);
void    print_cmd(t_block *block);

//int dump_cmd_to_block(t_block *block, t_block *block);

void    close_in_fds(t_block *block);
void    close_out_fds(t_block *block);
int     infiles_from_args_to_list(t_vdmlist **io_files, char **cmd_args, int *i);
int     outfiles_from_args_to_list(t_vdmlist **io_files, char **cmd_args, int *i);
int     manage_io_files(t_block *block);


int here_doc(t_block *block, char *eof);
int remove_unguarded_quotes(char **str, int *has_guards);

/* exec.c */
/*
int	join_path_bin(char **full_path, char *path, char *bin);
int	exec_cmd_with_path(t_pipex *pipex, char ***args);
int	exec_cmd_search_path(t_pipex *pipex, char ***args);
int	exec_command(t_pipex *pipex, char *cmd);
*/


/* error_message.c */
int	    perror_msg(char *text);
int	    error_msg(char *text);


int     exec_command(t_block *block);
int     process_execution(t_block *block);



void	error_child_exit(t_block *block, char *function, char *cmd, int errcode, int with_ms);
int     perror_msg_func(t_block *block, char *function, int errcode, int with_ms);
void	perror_child_exit(t_block *block, char *function, int errcode, int with_ms);


/* wildcard.c */
char    *wildcard(char *pattern, int pat_len, int *match_count);
void    void_putstr(void *str);
char    *triple_join(char *first, char *second, char *third);


/* expansion.c */
int     expand_dollars(char **to_expand, t_ms *ms);
int     expand_wildcards(char **to_expand, char **fail_return);
int     here_doc_expand_dollars(char **to_expand, t_ms *ms);


int     ms_prompt_loop(t_ms *ms);

/* future_libft.c*/
int		ft_matrixlen(void *mat);
int		ft_charmatdup(char ***dest, char **src);
void	*quicksort_pointers(void *arr, int size, int (*cmp)(void *, void *));
int		env_strcmp(void *s1, void *s2);
int     ft_matrixlen(void *mat);
int     ft_charmatdup(char ***dest, char **src);
char	**ft_split_count_replenish(t_cchar *s, t_cchar *og, char *sepset, int *place_count);

/*BUILTINS*/

int		check_builtins(t_block *block);
int		exec_builtin(t_block *block, int builtin);

int     run_cd(t_block *block);
int		run_env(t_block *block);
int		run_pwd(t_block *block);
int		run_echo(t_block *block);
int		run_unset(t_block *block);
int		run_export(t_block *block);
int		run_exit(t_block *block);

int		set_beg_path(t_block *block, char **curpath, char *curr);
int		cd_exists(t_block *block);

char	*ft_strchr(const char *s, int c);

void	rm_beg_end(char *curpath);

int	    get_corr_env(t_block *block, char *arg, int is_exporting);
void	env_add(t_block *block, char *new);

int	    ft_isdigit(int c);
int	    ft_isalpha(int c);

int     env_remove(t_block *block, int index);






#endif
