/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:52:17 by marvin            #+#    #+#             */
/*   Updated: 2023/09/23 10:17:41 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	minishell_main_loop(t_ms *ms)
{
	while (1)
	{
		if (get_prompt(ms))
		{
			setup_execution_tree(ms, NULL, ms->prompt, 0);
			if (g_signal != SIGINT)
				get_all_here_docs(ms->first);
			if (g_signal != SIGINT)
				execution_tree_exec_all(ms->first);
			block_destroy(ms->first);
			if (ms->my_kid != -1)
			{
				wait_and_save_status(ms->my_kid, &ms->exit_status, ms->errfd);
				ms->my_kid = -1;
			}
		}
		if (g_signal)
		{
			ms->kill_stdin = 1;
			if (dup2(ms->dup_stdin, ms->infd) == -1)
				perror_msg_ptr("dup2", NULL);
			close(ms->dup_stdin);
			ms->dup_stdin = dup(ms->infd);
			if (ms->dup_stdin == -1)
				perror_msg_ptr("dup", NULL);
		}
		if (tcsetattr(ms->infd, TCSANOW, &ms->modified) == -1)
			perror_msg_ptr("tcsetattr", NULL);
		//check_for_signals(ms);
		g_signal = 0;
		//dprintf(2, "loop, pid %d\n", getpid());
		//save_signal((int *)0);
	}
	return (1);
}

/*
valgrind --track-fds=yes --trace-children=yes --leak-check=full --show-leak-kinds=all ./minishell
*/




int	main(int ac, char **av, char **env)
{
	t_ms	ms;

	(void)ac;
	if (!ms_init(&ms, &av[0][2], env))
		return (0);
	minishell_main_loop(&ms);
	ms_destroy(&ms);
	return (0);
}


/*

$ valgrind --track-fds=yes --trace-children=yes --leak-check=full --show-leak-kinds=all ./minishell 

int	main(void)
{
	char			*line;
	t_token_list	*list;

	while (1)
	{
		line = readline("line: ");
		if (!line)
			break ;
		list = token_list_new();
		printf("is there list %p\n", list);
		prompt_to_list(list, line);
		free(line);
		token_list_head_print(list, print_token);
		token_list_destroy(&list);
	}
	return (0);
}
*/
/*

ALTERAÇÕES:

main.c          // destroy_ms if block->i_am_forked, retira passagem de status, ja ta no wait
ms_prompt.c     // setup env, shlvl, atenção a NULL env
signals.c       // adiciona sigint heredoc para exit child process do heredoc.

manage_files: heredoc... preciso do ms para destruir tudo no child process
	- verificar se com heredoc da memory leak ja que nao tamos a mudar nada da infraestructura
exec: apenas altera child exit para destruir o underlying ms

Atenção para parentesis...........

	(exit), apesar de os parentesis serem irrelevantes, faz um child process na mesma
	significa que talvez nao possamos retirar unnecessary parenthesis
	e tenhamos de aceitar muuuuitos forks
	apenas podemos retirar os parenthesis exteriores....?

(((exit))) && ls
	wtf, does nothing, exit code 1.....


((ls))
	same, exit code 1,



[root@localhost ~]# ((ls)) || echo hello
hello
[root@localhost ~]# echo $?
0
[root@localhost ~]#

		ignores ls because it has excessive parenthesis, exit status 1 immediatelly,
		ges for echo hello

EXPANSION OF WILDCARDS AND $ TAKES PLACE DURING THE EXECUTION TREE AND NOT BEFORE

*/
