/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mmaria-d <mmaria-d@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/25 09:52:17 by marvin            #+#    #+#             */
/*   Updated: 2023/09/14 19:33:33 by mmaria-d         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*

    waiting_for_my_children
    blocks waits for all the live children and, after arriving, sets their
    child_pid to zero to signal that either it is a manager and has no pid
    or that the child has already arrived because another conditional needed it
    to arrive before.

    Called by a manager block to wait for all its children and to decide
    on conditionals whether to proceed or not.

    t_ms will call its only pid if needed, corresponding to a single command
    with no manager behind other than t_ms itself.

*/

int print_child_pids(t_block *block)
{
    printf("i am [%s], starting status %d, mypid %d, my children are:\n       ", block->prompt, block->my_status, getpid());
    int i = 0;
    while (i < block->op_count + 1)
    {
        printf("%d  ", block->child_pids[i]);
        i++;
    }
    printf("\n");
    return (1);
}


int waiting_for_my_children(t_block *block, int index)
{
    int i;

    check_for_signals(block->ms);
    i = 0;
    //printf("block [%s] waiting for %d children \n", block->prompt, block->op_count + 1);
    //printf("i am [%s], starting status %d, waiting, mypid %d\n", block->prompt, block->my_status, getpid());
    //print_child_pids(block);
    while (i < index)
    {
        //printf("child [%s], index, %d, has pid? %d\n", block->child_list[i]->prompt, i, block->child_pids[i]);
        if (block->child_pids[i] != 0)
        {
            waitpid(block->child_pids[i], &block->my_status, 0);
            if (WIFEXITED(block->my_status))
                block->my_status = WEXITSTATUS(block->my_status);
            block->child_pids[i] = 0;
        }
		else if (block->child_exit_status[i] >= 0)
		{
			block->my_status = block->child_exit_status[i];
			block->child_exit_status[i] = -1;
		}

        i++;
    }
    //printf("i am [%s], ending status %d, moving on, mypid %d\n", block->prompt, block->my_status, getpid());
    if (block->father)
        block->father->my_status = block->my_status;
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

int pipes_and_conditionals(t_block *block, int index, int *must_fork)
{
    *must_fork = 0;
    if (index > 0 && block->op_id[index - 1] == OP_PIPE)
    {
        block->prev_pipe[0] = block->pipefd[0];
        block->prev_pipe[1] = block->pipefd[1];
        *must_fork = 1;
    }
    if (index < block->op_count && block->op_id[index] == OP_PIPE)
    {
        if (pipe(block->pipefd) == -1)
            return (perror_msg("pipe"));
        *must_fork = 1;
    }
    if (index > 0 && index <= block->op_count \
    && (block->op_id[index - 1] == OP_AND || block->op_id[index - 1] == OP_OR))
    {
        waiting_for_my_children(block, index);
        if ((block->op_id[index - 1] == OP_AND && block->ms->exit_status != 0) \
        || (block->op_id[index - 1] == OP_OR && block->ms->exit_status  == 0))
            return (0);
    }
    if (block->child_list[index]->parenthesis_fork \
    && !block->child_list[index]->has_unnecessary_parenthesis)
    {
        //printf("child: [%s] has parenthesis and must be forked\n", block->child_list[index]->prompt);
        *must_fork = 1;
    }
    if (*must_fork)
    {
        block->child_pids[index] = fork();
        if (block->child_pids[index] == -1)
            return (perror_msg("fork"));
        if (!block->child_pids[index] && index < block->op_count && block->op_id[index] == OP_PIPE)
            close(block->pipefd[0]);
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

int execute(t_block *block)
{
    if (!manage_cmd_expansions(block))
        return (0);
    if (block->cmd)
        process_execution(block);
    else
    {
        close_in_fds(block);
        close_out_fds(block);
    }
    if (block->i_am_forked)
        block->father->my_status = block->my_status;
    return (1);
}

/*

    execution_tree
    function responsible for the whole thing, pretty much.
    all the t_blocks are set in the stack frame created for the caller: when
    calling destroy_block it just frees the malloc'ed pointers inside the structure
    but not the structure itself (it is destroyed when the function exists)
    When dealing with a father with multiple kids, the previous kid cleans the structure
    when exiting their function call such that the father can use it again from scratch
    for the next kid:
        All Quiet on the Western Front
    so for instance with: ls | cat | cat | cat | cat
    only 2 blocks exist at any given time as one child is done, its function call to execution_tree
    will destroy his own block upon exiting (the child must call destroy_block anyways)
    Upon execution, all the fds are analysed before the command itself, so if
    any fd fails to open, the command won't analyse it s own arguments.



*/

int open_here_docs_at_block(t_block *block)
{
    int         i;
    t_redir     *redir;
    t_vdmnode   *cur;

    if (!block->io_files)
        return (1);
    cur = block->io_files->head;
    i = 0;
    //printf("checking here_docs at block [%s]\n", block->prompt);
    while (cur && save_signal(NULL) != EXIT_SIGINT)
    {
        redir = (t_redir *)cur->data;
        //printf("checking redir [%s] of type %d\n", redir->file, redir->type);
        if (redir->type == RE_HEREDOC)
        {
            if (block->here_doc)
            {
                close(block->here_doc_fd);
                unlink(block->here_doc);
                ft_free_set_null(&block->here_doc);
            }
            if (!remove_unguarded_quotes(&redir->file, NULL)
            || !here_doc(block, redir->file))
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


int get_all_here_docs(t_block *block)
{
    int i;

    if (!block->is_cmd)
    {
        i = 0;
        while (block->child_list[i] \
        && !block->has_unnecessary_parenthesis \
        && save_signal(NULL) != EXIT_SIGINT)
        {
            get_all_here_docs(block->child_list[i]);
            i++;
        }
    }
    open_here_docs_at_block(block);
    return (1);
}

int execution_tree(t_block *block, int i_am_forked)
{
    int         i;
    int         status;
    int         must_fork;

    if (i_am_forked)
        block->i_am_forked = 1;
    if (!manage_io_files(block))
        return (0);
    if (block->is_cmd)
        execute(block);
    else if (!block->has_unnecessary_parenthesis)
    {
        i = 0;
        while (block->child_list[i])
        {
            if (i > 0 && block->op_id[i - 1] == OP_PIPE)
                close(block->pipefd[1]);
            if (block->op_id && pipes_and_conditionals(block, i, &must_fork))
            {
                if (!(must_fork && block->child_pids[i] != 0))
                    execution_tree(block->child_list[i], must_fork);
            }
            if (i > 0 && block->op_id[i - 1] == OP_PIPE)
                close(block->prev_pipe[0]);
            i++;
        }
        waiting_for_my_children(block, block->op_count + 1);
        close_in_fds(block);
        close_out_fds(block);
    }
    else
        block->my_status = 1;               //unnecessary parenthesis status

    //printf("block [%s], my status %d, my address %p\n", block->prompt, block->my_status, block);
    if(i_am_forked)
    {
        //printf("block [%s] is forked, my status %d\n", block->prompt, block->my_status);
        status = block->my_status;
        destroy_ms(block->ms);
        exit(status);
    }
	else if (block->father)
		block->father->child_exit_status[block->my_id] = block->my_status;
	else
		block->ms->exit_status = block->my_status;
    //printf("block [%s], my status %d, my address %p, not forked ready to destroy\n", block->prompt, block->my_status, block);
    destroy_block(block);
    return (1);
}


void    print_execution_tree(t_block *block)
{
    int i;

    i = 0;
    if (!block)
        return ;
    //printf("lvl %d, id %d, prompt [%s]\n", block->my_level, block->my_id, block->prompt);
    if (!block->is_cmd)
    {
        i = 0;
        while (block->child_list[i])
        {
            print_execution_tree(block->child_list[i]);
            i++;
        }
    }
}

int setup_execution_tree(t_ms *ms, t_block *father, char *pmt, int my_id)
{
    int     i;
    t_block *block;

    block = init_block(ms, father, pmt, my_id);
    if (!block)
        return (0);
    if (!split_prompt(block))
        return (0);
    if (block->is_cmd && !setup_cmd_pre_expansion(block))
        return (0);
    if (!block->is_cmd)
    {
        i = 0;
        while (block->child_prompts[i])
        {
            setup_execution_tree(ms, block, block->child_prompts[i], i);
            i++;
        }
        block->child_list[i] = NULL;
        ft_free_charmat_null(&block->child_prompts, free);
    }
    return (1);
}

int ms_prompt_loop(t_ms *ms)
{
    while (1)
	{
    	if (get_prompt(ms))
    	{
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



int main(int ac, char **av, char **env)
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
