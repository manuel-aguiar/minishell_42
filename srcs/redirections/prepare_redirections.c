/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prepare_redirections.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/17 13:36:27 by mnascime          #+#    #+#             */
/*   Updated: 2023/09/19 14:08:01 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	manage_io_expansion
	This is called for every file saved on the t_vdmlist *files.
	It checks all redirections by expanding dollars and wildcards.
	Function expand_wildcards is given a pointer where the error message
	"ambiguous redirection" for the current wildcard was violated
	and will print it to STDERR and exit the command without further execution.
	Commands are only allowed to proceed if all their file descriptors are correct
	and opened properly without ambiguity.
	The function will change the name of the file in the t_vdmlist *files directly
	and the final name will be the one that will be used to open.

	Contrary to expansion on command arguments, if there are spaces these are not split
	and will be considered as a single name of a file. Eg: echo hello "hey there"
	will open/create a file called 'hey there' (without quotes) and will write "hello"
	in that file and close it.
*/

///////////////////////////////////////////////////////////////////////////////////////////////  FT_PRINTF_FD



/*
	here_doc functions

	These are used to manage here_doc.
	dfs (depth first search) will create a file name that is not in use in the current directory
	on which the user will write to from the terminal.

	here_doc_fill will write to the file using get_next_line and ft_putstr_fd to
	that file  descriptor. The file will then be closed and open in order to reset its offset to 0
	and allow the command that uses it as input to be able to read it from the beginning.

	here_doc will simply print error messages in case there is a problem while opening and closing
	the file and leading execution to fail (here_doc -> manage_infiles -> prepare_redirections -> execute return 0)

*/



/*

	here_doc_file
	fills the here_doc temp file with readline.

	it uses here_doc_expand_dollars for env expansion because in here_docs, quotes
	dont' guard the expansion of env variables: they are expanded both with \' and ' " '

*/



/*

	manage_infile/manage_outfile

	These functions are called from prepare_redirections
	prepare_redirections checks all redirections from the t_vdmlist *files,
	checks if they are in or out, and of which type, then forwarding to manage_infile/manage_outfile
	to manage their particular file descroptors.
	Both of these functions start by closing the previously open file descriptor for their respective
	category (in or out) since these previous fds will not be needed anymore and can be safely closed.
	After analysing their particular file descriptor, the functions will delete the head of t_vdmlist *files
	(which is they one they are always analysing, until the list is empty).

(cat && cat << eof ) < main.c
*/

static int	manage_infile(t_block *block, int index)
{
	t_token_node	*redir;

	redir = block->io_files->head;
	close_in_fds(block);
	if (redir->type == T_INDIR_HD)
	{
		if (index == block->here_doc_index)
			block->final_in = block->here_doc_fd;
	}
	else
	{
		if (block->here_doc && index > block->here_doc_index)
		{
			close(block->here_doc_fd);
			unlink(block->here_doc);
			ft_free_set_null(&block->here_doc);
		}
		block->final_in = open(redir->text, O_RDWR);
		if (block->final_in == -1)
			return (perror_msg_func(block, redir->text, CODE_OPEN, 1));
	}
	token_list_del_head(block->io_files);
	return (1);
}

static int	manage_outfile(t_block *block)
{
	t_token_node	*redir;

	redir = block->io_files->head;
	close_out_fds(block);
	if (redir->type == T_OUTDIR_TRUNC)
		block->final_out = open(redir->text, O_CREAT \
						| O_RDWR | O_TRUNC, 0644);
	else
		block->final_out = open(redir->text, O_CREAT \
						| O_RDWR | O_APPEND, 0644);
	if (block->final_out == -1)
		return (perror_msg_func(block, redir->text, CODE_OPEN, 1));
	token_list_del_head(block->io_files);
	return (1);
}

/*
	prepare_redirections
	manages all the file descriptors that were obtained from "split_redirections" and "setup_cmd"
	first it gives the inherited fds from the parent to its own command
	next, it checks wether there are pipes open, as these will override the previously inherited fds
	and, if so, it overrides these with the pipe WITHOUT CLOSING THE INHERITED ONES:
		child blocks are only allowed to close the fds they themselves opened + pipes
		parent blocks are the ones responsible to check if their own fds are inherited and, if not,
		to close them.
	After this step, it loops through t_vdmlist *files (filled by the functions "get_infiles" and "get_outfiles"
	in setup_redirections and setup_cmd) and analyses each redirection in order of appearence wiithin their current block level
	First, expansion of dollars and wildcards is conducted and errors are checked for ambiguous redirections,
	If not, they are handled by their corresponding manager function, manage_infile and manage_outfile.
	At the end, t_vdmlist *files is finally destroyed as it is no longer neede. Function "destroy_redir" is
	passed to it to destroy the t_redir struct that is populating the "data" field of this linkedlist.

*/

static int	take_inherited_fds(t_block *block)
{
	if (!block->my_manager)
	{
		block->final_out = block->ms->outfd;
		block->final_in = block->ms->infd;
		return (1);
	}
	block->final_out = block->my_manager->final_out;
	block->final_in = block->my_manager->final_in;
	if (block->my_id < block->my_manager->op_count \
	&& block->my_manager->op_id \
	&& block->my_manager->op_id[block->my_id] == T_OP_PIPE)
		block->final_out = block->my_manager->pipefd[1];
	if (block->my_id > 0 \
	&& block->my_manager->op_id \
	&& block->my_manager->op_id[block->my_id - 1] == T_OP_PIPE)
		block->final_in = block->my_manager->prev_pipe[0];
	return (1);
}

int	prepare_redirections(t_block *block)
{
	int	i;
	int	type;
	int	success;

	take_inherited_fds(block);
	if (!block->io_files || block->has_arithmatic_parenthesis)
		return (1);
	success = 1;
	i = 0;
	while (block->io_files->head && success)
	{
		type = block->io_files->head->type;
		success = manage_io_expansion(block);
		if (!success)
			break ;
		if (type == T_OUTDIR_TRUNC || type == T_OUTDIR_APPEND)
			success = manage_outfile(block);
		else
			success = manage_infile(block, i);
		i++;
	}
	token_list_destroy(&block->io_files);
	return (success);
}
