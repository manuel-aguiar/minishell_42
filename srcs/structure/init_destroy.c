/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_destroy.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mnascime <mnascime@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 00:11:09 by mmaria-d          #+#    #+#             */
/*   Updated: 2023/09/17 15:49:55 by mnascime         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	if (block->child_exit_status)
		ft_free_set_null(&block->child_exit_status);
	if (block->op_id)
		ft_free_set_null(&block->op_id);
    if (block->father)
        block->father->child_list[block->my_id] = NULL;
    else
        block->ms->first = NULL;
    if (block->prompt)
	    token_list_destroy(&block->prompt);
	if (block->io_files)
	    token_list_destroy(&block->io_files);
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

t_block *init_block(t_ms *ms, t_block *father, t_token_list *prompt, int my_id)
{
    t_block *new;

    new = malloc(sizeof(*new));
    if (!new)
		return (perror_msg_ptr("malloc", NULL));
    new->ms = ms;
	new->father = father;
	new->i_am_forked = 0;
	new->prompt = prompt;
	if (father)
	{
		new->father->child_prompts[my_id] = NULL;
		new->father->child_list[my_id] = new;
		new->my_level = new->father->my_level + 1;
	}
	else
	{
		ms->prompt = NULL;
		ms->first = new;
		new->my_level = 0;
	}


	new->child_prompts = NULL;



	new->child_list = NULL;
	new->child_pids = NULL;
	new->child_exit_status = NULL;
    new->op_count = 0;
    new->op_id = NULL;
	new->is_cmd = 0;
	new->has_arithmatic_parenthesis = 0;
	new->must_subshell = 0;

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
	new->here_doc_index = -1;
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
	        return (perror_msg_int("malloc", 0));
	    return (1);
    }
    ms->env = malloc(sizeof(*ms->env) * 1);
    if (!ms->env)
        return (perror_msg_int("malloc", 0));
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
        return (perror_msg_int("malloc", 0));
    new_shlvl_env = ft_triple_join("SHLVL=", "", new_lvl_itoa);                        // substituir pelo ft_strjoin na boa;
    if (!new_shlvl_env)
    {
        free(new_lvl_itoa);
        return (perror_msg_int("malloc", 0));
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
	ms->exit_status = 0;
	sigint_heredoc_where_ms_is(ms);                             // potentially remove this, check heredoc sigint
	if (!ms_prepare_signal(ms, signal_handler))
	return (destroy_ms(ms));
	return (1);
}
