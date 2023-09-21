/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ms_setup.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 10:30:56 by codespace         #+#    #+#             */
/*   Updated: 2023/09/21 17:14:51 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ms_disable_sigquit(t_ms *ms)
{
	if (tcgetattr(ms->infd, &ms->original) == -1)
		return (perror_msg_int("tcgetattr", 0));
	ms->modified = ms->original;
	//ms->modified.c_lflag &= ~(ISIG);
	ms->modified.c_cc[VQUIT] = _POSIX_VDISABLE;
	if (tcsetattr(ms->infd, TCSANOW, &ms->modified) == -1)
		return (perror_msg_int("tcsetattr", 0));
	return (1);
}

int	ms_init(t_ms *ms, char *avzero, char **env)
{
	ms->env = NULL;
	ms->path = NULL;
	if (!ms_setup_initial_env(ms, env))
		return (ms_destroy(ms));
	if (!ms_increase_shell_level(ms))
		return (ms_destroy(ms));
	if (!ms_set_path(ms))
		return (ms_destroy(ms));
	ms->prompt = NULL;
	ms->name = avzero;
	ms->infd = STDIN_FILENO;
	ms->outfd = STDOUT_FILENO;
	ms->errfd = STDERR_FILENO;
	ms->first = NULL;
	ms->my_kid = -1;
	ms->exit_status = 0;
	ms->dup_stdin = dup(ms->infd);
	if (ms->dup_stdin == -1)
		return (ms_destroy(ms));
	ms->kill_stdin = 0;
	if (!ms_disable_sigquit(ms))
		return (ms_destroy(ms));
	sigint_heredoc_where_ms_is(ms);
	if (!ms_prepare_signal(ms, signal_handler))
		return (ms_destroy(ms));
	return (1);
}

int	ms_set_path(t_ms *ms)
{
	int	i;

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

int	ms_setup_initial_env(t_ms *ms, char **env)
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

int	ms_increase_shell_level(t_ms *ms)
{
	int		i;
	char	*new_lvl_itoa;
	char	*new_shlvl_env;

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
	new_shlvl_env = ft_triple_join("SHLVL=", "", new_lvl_itoa);
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

int	ms_destroy(t_ms *ms)
{
	if (ms->env)
		ft_free_charmat_null(&ms->env, free);
	if (ms->path)
		ft_free_charmat_null(&ms->path, free);
	if (ms->prompt)
		ft_free_set_null(&ms->prompt);
	if (ms->first)
		block_destroy(ms->first);
	if (tcsetattr(ms->infd, TCSANOW, &ms->original) == -1)
		return (0);
	if (ms->kill_stdin)
		close(ms->infd);
	close(ms->dup_stdin);
	return (1);
}
