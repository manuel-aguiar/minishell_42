
#include "minishell.h"

t_redir *init_redir(char *file, int type)
{
    t_redir *redir;

    redir = malloc(sizeof(*redir));
    if (!redir)
        return (NULL);
    redir->file = file;
    redir->type = type;
    return (redir);
}

void	destroy_redir(void *og_redir)
{
	t_redir *rdir;

	rdir = (t_redir *)og_redir;
	ft_free_set_null(&rdir->file);
	free(og_redir);
}

void	destroy_block(void *og_block)
{
    t_block *block;


    block = (t_block *)og_block;

    if (!block)
    return ;
	if (block->child_list)
	    ft_free_sizemat_null(&block->child_list, block->op_count + 1, destroy_block);
	if (block->child_prompts)
	    ft_free_charmat_null(&block->child_prompts, free);
	if (block->child_pids)
		ft_free_set_null(&block->child_pids);
	if (block->op_id)
		ft_free_set_null(&block->op_id);
    if (block->father)
        block->father->child_list[block->my_id] = NULL;
    else
        block->ms->first = NULL;
    if (block->prompt)
	    ft_free_set_null(&block->prompt);
	if (block->io_files)
	    vdmlist_destroy(&block->io_files, destroy_redir);
	if (block->here_doc)
	{
	    close(block->here_doc_fd);
	    unlink(block->here_doc);
	    ft_free_set_null(&block->here_doc);
	}
	if (block->cmd)
		ft_free_set_null(&block->cmd);
	if (block->cmd_args)
		ft_free_charmat_null(&block->cmd_args, free);
	free(block);
}

t_block *init_block(t_ms *ms, t_block *father, char *pmt, int my_id)
{
    t_block *new;

    new = malloc(sizeof(*new));
    if (!new)
    {
        perror("malloc");
        return (0);
    }
    new->ms = ms;
	new->father = father;
	new->i_am_forked = 0;
	if (father)
	{
		new->prompt = ft_strdup(pmt);
		new->father->child_list[my_id] = new;
		new->my_level = new->father->my_level + 1;
	}
	else
	{
		new->prompt = ft_strdup(ms->prompt);
		ms->first = new;
		new->my_level = 0;
	}

	new->child_prompts = NULL;
	new->child_list = NULL;
	new->child_pids = NULL;
    new->op_count = 0;
    new->op_id = NULL;
	new->is_cmd = 0;
	new->has_unnecessary_parenthesis = 0;
	new->parenthesis_fork = 0;

	new->pipefd[0] = -1;
	new->pipefd[1] = -1;
	new->prev_pipe[0] = -1;
	new->prev_pipe[1] = -1;
	new->my_status = 0;

	new->cmd = NULL;
	new->cmd_args = NULL;
	new->io_files = NULL;
	new->final_in = -1;
	new->final_out = -1;
	new->here_doc = NULL;
	new->here_doc_fd = -1;

	new->my_id = my_id;
	return (new);
}

int	destroy_ms(t_ms *ms)
{
	if (ms->env)
	    ft_free_charmat_null(&ms->env, free);
	if (ms->path)
	    ft_free_charmat_null(&ms->path, free);
	if (ms->prompt)
	    ft_free_set_null(&ms->prompt);
	if (ms->first)
	    destroy_block(ms->first);
	return (1);
}

int ms_set_path(t_ms *ms)
{
    int     i;

    if (!ms->env)
        return (1);
    i = 0;
    while (ms->env[i] && ft_strncmp("PATH=", ms->env[i], 5))
        i++;
    if (!ms->env[i])
        ms->path = NULL;
    else
    {
        ms->path = ft_split(&ms->env[i][5], ':');
        if (!ms->path)
            return (0);
    }
    return (1);
}

int ms_setup_initial_env(t_ms *ms, char **env)
{
    if (env)
    {
        if (!ft_charmatdup(&ms->env, env))
	        return (perror_msg("malloc"));
	    return (1);
    }
    ms->env = malloc(sizeof(*ms->env) * 1);
    if (!ms->env)
        return (perror_msg("malloc"));
    ms->env[0] = NULL;
    return (1);
}

int ms_increase_shell_level(t_ms *ms)
{
    int i;
    char *new_lvl_itoa;
    char *new_shlvl_env;

    if (!ms->env)
        return (1);
    i = 0;
    while (ms->env[i] && ft_strncmp("SHLVL=", ms->env[i], 6))
        i++;
    if (!ms->env[i])
        return (1);
    new_lvl_itoa = ft_itoa(ft_atoi(&ms->env[i][6]) + 1);
    if (!new_lvl_itoa)
        return (perror_msg("malloc"));
    new_shlvl_env = triple_join("SHLVL=", "", new_lvl_itoa);                        // substituir pelo ft_strjoin na boa;
    if (!new_shlvl_env)
    {
        free(new_lvl_itoa);
        return (perror_msg("malloc"));
    }
    free(new_lvl_itoa);
    free(ms->env[i]);
    ms->env[i] = new_shlvl_env;
    return (1);
}

int init_ms(t_ms *ms, char *avzero, char **env)
{
    ms->env = NULL;
    ms->path = NULL;
    if (!ms_setup_initial_env(ms, env))
	    return (destroy_ms(ms));
	if (!ms_increase_shell_level(ms))
	return (destroy_ms(ms));
    if (!ms_set_path(ms))
    return (destroy_ms(ms));
    ms->prompt = NULL;
    ms->name = avzero;
	ms->infd = STDIN_FILENO;
	ms->outfd = STDOUT_FILENO;
	ms->errfd = STDERR_FILENO;
	ms->first = NULL;
	ms->my_kid = -1;
	sigint_heredoc_where_ms_is(ms);                             // potentially remove this, check heredoc sigint
	if (!ms_prepare_signal(ms, signal_handler))
	return (destroy_ms(ms));
	return (1);
}
