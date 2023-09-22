/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:52:17 by marvin            #+#    #+#             */
/*   Updated: 2023/09/22 21:54:13 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	my_workers_addresses(t_block *block)
{
	if (block->is_worker)
		return (1);
	int i = 0;
	printf(" i am %p, my child list is %p, my children are:\n", block, block->worker_list);
	while (block->worker_list[i])
	{
		printf("%p  ", block->worker_list[i++]);
	}
	printf("\n\n");
	return (1);
}

void	print_block(t_block *block)
{
	printf("cmd args are: ");
	int i = 0;
	while (block->cmd_args[i])
		printf("%s ", block->cmd_args[i++]);
	printf("\n");
	printf("\n redirections are: ");
	token_list_head_print(block->io_files, print_token_args);
	printf("\n");
}

/*

	waiting_for_my_workers
	blocks waits for all the live children and, after arriving, sets their
	child_pid to zero to signal that either it is a manager and has no pid
	or that the child has already arrived because another conditional needed it
	to arrive before.

	Called by a manager block to wait for all its children and to decide
	on conditionals whether to proceed or not.

	t_ms will call its only pid if needed, corresponding to a single command
	with no manager behind other than t_ms itself.

*/

int	print_worker_pids(t_block *block)
{
	int	i;

	i = 0;
	//printf("i am [%s], starting status %d, mypid %d, my children are:\n       ", block->prompt, block->my_status, getpid());
	while (i < block->op_count + 1)
	{
		printf("%d  ", block->worker_pids[i]);
		i++;
	}
	printf("\n");
	return (1);
}






/*
	execute
	This function prepares the command for execution.
	If redirections have a problem (ambiguous/non existent), close the remaining file descriptors
	and exit.

	If there is no command (the prompt is made of only redirections, totally legal)
	just close the file descriptors and exit.

	Otherwise, call process_execution: responsible for fork and execve.

*/



/*

	execution_tree
	function responsible for the whole thing, pretty much.
	all the t_blocks are set in the stack frame created for the caller: when
	calling destroy_block it just frees the malloc'ed pointers inside the structure
	but not the structure itself (it is destroyed when the function exists)
	When dealing with a manager with multiple kids, the previous kid cleans the structure
	when exiting their function call such that the manager can use it again from scratch
	for the next kid:
		All Quiet on the Western Front
	so for instance with: ls | cat | cat | cat | cat
	only 2 blocks exist at any given time as one child is done, its function call to execution_tree
	will destroy his own block upon exiting (the child must call destroy_block anyways)
	Upon execution, all the fds are analysed before the command itself, so if
	any fd fails to open, the command won't analyse it s own arguments.



*/








void	print_execution_tree(t_block *block)
{
	int	i;

	i = 0;
	if (!block)
		return ;
	//printf("lvl %d, id %d, prompt [%s]\n", block->my_level, block->my_id, block->prompt);
	if (!block->is_worker)
	{
		i = 0;
		while (block->worker_list[i])
		{
			print_execution_tree(block->worker_list[i]);
			i++;
		}
	}
}





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
				//ms_prepare_signal(ms, SIG_IGN);
				if (waitpid(ms->my_kid, &ms->exit_status, 0) == -1)
					perror("waitpid");
				if (WIFEXITED(ms->exit_status))
				{
					//dprintf(2, "process was exited\n");
					ms->exit_status = WEXITSTATUS(ms->exit_status);
					//dprintf(2, "exit status %d\n", exit_status);
				}
				else if (WIFSIGNALED(ms->exit_status))
				{
					//dprintf(2, "process was signaled by %d\n", WTERMSIG(exit_status));
					ms->exit_status = WTERMSIG(ms->exit_status) + EXIT_SIGNALED;
					
					//dprintf(2, "signal status %d\n", ms->exit_status);
				}
				ms->my_kid = -1;
				//ms_prepare_signal(ms, signal_handler);
				//dprintf(2, "final exit status %d\n", ms->exit_status);
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
			
			if (g_signal == SIGINT)
				printf("\n");
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
