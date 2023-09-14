#include "minishell.h"

t_ms    *sigint_heredoc_where_ms_is(t_ms *ms)
{
    static t_ms *save;

    if (ms)
        save = ms;
    return (save);
}

int    save_signal(int *num)
{
    static int g_my_signal;

    if (num)
        g_my_signal = *num;
    return (g_my_signal);
}

int     check_for_signals(t_ms *ms)
{
    int code;

    if (save_signal(NULL))
	{
	    code = 0;
	    ms->exit_status = save_signal(NULL);
	    save_signal(&code);
	}
	return (1);
}

void    signal_handler(int signum)
{
    int code;

    if (signum == SIGINT)
    {
        printf("\n");
    	rl_on_new_line();
    	rl_replace_line("", 0);
    	rl_redisplay();
    	code = 130;
    	save_signal(&code);
    }
    if (signum == SIGQUIT)
    {
        code = 131;
		save_signal(&code);
    	//rl_on_new_line();
    	//rl_redisplay();
    }
}

int ms_prepare_signal(t_ms *ms, void (*handler)(int))
{
    ms->sigact.sa_flags = 0;
    ms->sigact.sa_handler = handler;
	if (sigemptyset(&(ms->sigact.sa_mask)) == -1)
		return (perror_msg("sigemptyset"));
    if (sigaction(SIGINT, &ms->sigact, NULL) == -1)
        return (perror_msg("sigaction"));
    if (sigaction(SIGQUIT, &ms->sigact, NULL) == -1)
        return (perror_msg("sigaction"));
    if (sigaction(SIGPIPE, &ms->sigact, NULL) == -1)
        return (perror_msg("sigaction"));
    return (1);
}
