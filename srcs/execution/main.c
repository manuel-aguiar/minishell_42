/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:52:17 by marvin            #+#    #+#             */
/*   Updated: 2023/09/19 10:08:36 by codespace        ###   ########.fr       */
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


int	waiting_for_my_workers(t_block *block, int index)
{
	int	i;

	check_for_signals(block->ms);
	i = 0;
	//printf("block [%s] waiting for %d children \n", block->prompt, block->op_count + 1);
	//printf("i am [%s], starting status %d, waiting, mypid %d\n", block->prompt, block->my_status, getpid());
	//print_worker_pids(block);
	while (i < index)
	{
		//printf("child [%s], index, %d, has pid? %d\n", block->worker_list[i]->prompt, i, block->worker_pids[i]);
		if (block->worker_pids[i] != 0)
		{
			//printf("my lvl id (%d, %d), waiting for pid %d, my status now is: %d  ", block->my_level, block->my_id, block->worker_pids[i], block->my_status);
			waitpid(block->worker_pids[i], &block->my_status, 0);
			if (WIFEXITED(block->my_status))
				block->my_status = WEXITSTATUS(block->my_status);
			//printf("  and changed to %d i received from child (%d, %d)\n", block->my_status, block->my_level +1, i);
			block->worker_pids[i] = 0;
		}
		else if (block->worker_exit_status[i] >= 0)
		{
			//printf("my lvl id (%d, %d) original status %d will change to %d from child wnumber (%d,%d)\n", block->my_level, block->my_id, block->my_status, block->worker_exit_status[i], block->my_level +1, i);
			block->my_status = block->worker_exit_status[i];
			block->worker_exit_status[i] = -1;
		}
		//else
		//	printf("no pid, no manual status\n");
		i++;
	}
	//printf("i am [%s], ending status %d, moving on, mypid %d\n", block->prompt, block->my_status, getpid());
	if (!block->i_am_forked && block->manager)
		block->manager->my_status = block->my_status;
	block->ms->exit_status = block->my_status;
	return (1);
}

/*

	pipes_and_conditionals
	Sets up the commands based on the big 3 operators: && || and |.
		-   Saves the read end of the previous pipe, if a pipe was open
			the next operator may be a pipe as well: pipe will overwrite
			the previous file descriptors
		-   Opens a new pipe if the current command will write too a pipe;
		-   In case of && or ||, waits for the previous commands to arrive
			in order to determine their exit status and decide wether
			the following command will execute or not;

*/

int	pipes_and_conditionals(t_block *block, int index, int *must_fork)
{
	*must_fork = 0;
	if (index > 0 && block->op_id[index - 1] == T_OP_PIPE)
	{
		block->prev_pipe[0] = block->pipefd[0];
		block->prev_pipe[1] = block->pipefd[1];
		*must_fork = 1;
	}
	if (index < block->op_count && block->op_id[index] == T_OP_PIPE)
	{
		if (pipe(block->pipefd) == -1)
			return (perror_msg("pipe"));
		*must_fork = 1;
	}
	if (index > 0 && index <= block->op_count \
	&& (block->op_id[index - 1] == T_OP_AND || block->op_id[index - 1] == T_OP_OR))
	{
		waiting_for_my_workers(block, index);
		if ((block->op_id[index - 1] == T_OP_AND && block->ms->exit_status != 0) \
		|| (block->op_id[index - 1] == T_OP_OR && block->ms->exit_status == 0))
			return (0);
	}
	if (block->must_subshell \
	&& !block->has_arithmatic_parenthesis && block->op_count == 0)
	{
		//printf("child: [%s] has parenthesis and must be forked\n", block->worker_list[index]->prompt);
		*must_fork = 1;
	}
	if (*must_fork)
	{
		block->worker_pids[index] = fork();
		if (block->worker_pids[index] == -1)
			return (perror_msg("fork"));
		if (!block->worker_pids[index])
		{
			if (index < block->op_count && block->op_id[index] == T_OP_PIPE)
				close(block->pipefd[0]);
			//if (index > 0 && block->op_id[index - 1] == T_OP_PIPE)
			//	close(block->prev_pipe[1]);
		}
	}
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

int	execute(t_block *block)
{
	//printf("executing [%s] i'm pid %d\n", block->prompt, block->manager->worker_pids[0]);
	//token_list_head_print(block->prompt, print_token_args);
	if (!worker_task_expansions(block))
		return (0);
	//print_block(block);
	//printf("executing block [%s]\n", block->cmd);
	if (block->cmd)
		process_execution(block);
	else
	{
		close_in_fds(block);
		close_out_fds(block);
	}
	if (block->i_am_forked)
		block->manager->my_status = block->my_status;
	return (1);
}

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

int	open_here_docs_at_block(t_block *block)
{
	int				i;
	int				has_quotes;
	t_token_node	*cur;

	if (!block->io_files)
		return (1);
	cur = block->io_files->head;
	i = 0;
	//printf("checking here_docs at block [%s]\n", block->prompt);
	while (cur && save_signal(NULL) != EXIT_SIGINT)
	{
		//printf("checking redir [%s] of type %d\n", redir->file, redir->type);
		if (cur->type == T_INDIR_HD)
		{
			if (block->here_doc)
			{
				close(block->here_doc_fd);
				unlink(block->here_doc);
				ft_free_set_null(&block->here_doc);
			}
			if (!remove_unguarded_quotes(&cur->text, &has_quotes)
				|| !here_doc(block, cur->text, has_quotes))
				return (0);

			//block->here_doc = redir->file;
			//printf("here doc name [%s]\n", block->here_doc);
			//redir->file = NULL;
			block->here_doc_index = i;
			//printf("here doc: fd %d, i %d, limiter [%s], file name [%s]\n", block->here_doc_fd, i, redir->file, block->here_doc);
		}
		cur = cur->next;
		i++;
	}
	return (1);
}


int	get_all_here_docs(t_block *block)
{
	int	i;

	if (!block->is_worker)
	{
		i = 0;
		//printf("i am %p, my child list is %p\n", block, block->worker_list);
		//my_children_addresses(block);
		while (block->worker_list[i] \
		&& !block->has_arithmatic_parenthesis \
		&& save_signal(NULL) != EXIT_SIGINT)
		{
			get_all_here_docs(block->worker_list[i]);
			i++;
		}
	}
	open_here_docs_at_block(block);
	return (1);
}

int	execution_tree(t_block *block, int i_am_forked)
{
	int	i;
	int	status;
	int	must_fork;

	
	if (i_am_forked)
		block->i_am_forked = 1;
	if (!manage_io_files(block))
		return (0);
	if (block->is_worker)
		execute(block);
	else if (!block->has_arithmatic_parenthesis)
	{
		//printf("anything not cmd?\n");
		i = 0;
		while (block->worker_list[i])
		{
			//printf("got in loop\n");
			if (i > 0 && block->op_id[i - 1] == T_OP_PIPE)
				close(block->pipefd[1]);
			if (block->op_id && pipes_and_conditionals(block, i, &must_fork))
			{
				if (!(must_fork && block->worker_pids[i] != 0))
					execution_tree(block->worker_list[i], must_fork);
			}
			if (i > 0 && block->op_id[i - 1] == T_OP_PIPE)
				close(block->prev_pipe[0]);
			i++;
		}
		waiting_for_my_workers(block, block->op_count + 1);
		close_in_fds(block);
		close_out_fds(block);
	}
	else
		block->my_status = 1;				//unnecessary parenthesis status

	//printf("block [%s], my status %d, my address %p\n", block->prompt, block->my_status, block);
	if (i_am_forked)
	{
		//printf("my lvl id (%d, %d) i am forked, pid %d, my status %d, will exit and my manager picks me up\n", block->my_level, block->my_id, getpid(), block->my_status);
		status = block->my_status;
		destroy_ms(block->ms);
		exit(status);
	}
	else if (block->manager && !block->is_worker)
	{
		//printf("my lvl id (%d, %d) passing status %d to my manager\n", block->my_level, block->my_id, block->my_status);
		block->manager->worker_exit_status[block->my_id] = block->my_status;
	}
	else if (!block->manager && !block->is_worker)
		block->ms->exit_status = block->my_status;
	//printf("block [%s], my status %d, my address %p, not forked ready to destroy\n", block->prompt, block->my_status, block);
	destroy_block(block);
	return (1);
}

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



int	setup_execution_tree(t_ms *ms, t_block *manager, t_token_list *tasks, int my_id)
{
	int		i;
	t_block	*block;

	block = init_block(ms, manager, tasks, my_id);
	if (!block)
		return (0);
	//printf("printing prompt i have received: \n");
	//token_list_head_print(prompt, print_token);
	//printf("finished printing prompt i have received\n");
	if (!distribute_tasks_between_managers_and_workers(block))
		return (0);
	if (block->is_worker)
		return (0);
	if (!block->is_worker)
	{
		i = 0;
		while (block->worker_tasks[i])
		{
			setup_execution_tree(ms, block, block->worker_tasks[i], i);
			i++;
		}
		//my_children_addresses(block);
		manager_destroy_worker_tasks(block);
	}
	return (1);
}

int	ms_prompt_loop(t_ms *ms)
{
	while (1)
	{
		if (get_prompt(ms))
		{
			//token_list_head_print(ms->prompt, print_token_args);
			//setup the whole fuckin tree;
			setup_execution_tree(ms, NULL, ms->prompt, 0);
			if (save_signal(NULL) != EXIT_SIGINT)
				get_all_here_docs(ms->first);
			//print_execution_tree(ms->first);
			//printf("starting execution\n");
			if (save_signal(NULL) != EXIT_SIGINT)
				execution_tree(ms->first, 0);
			destroy_block(ms->first);
			//execution_tree(ms, NULL, ms->prompt, 0);


			if (ms->my_kid != -1)
			{
				waitpid(ms->my_kid, &ms->exit_status, 0);
				if (WIFEXITED(ms->exit_status))
					ms->exit_status = WEXITSTATUS(ms->exit_status);
				ms->my_kid = -1;
			}
		}
		//printf("signal is %d\n", save_signal(NULL));
		check_for_signals(ms);
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
	if (!init_ms(&ms, &av[0][2], env))
		return (0);
	ms_prompt_loop(&ms);
	destroy_ms(&ms);
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
