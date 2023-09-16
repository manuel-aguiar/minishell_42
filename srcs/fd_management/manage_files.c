#include "minishell.h"


/*
    close_in/close_out_fds
    These two functions check wether the current file descriptor was
    inherited or not. If not, they can close it. The only file descriptors
    that blocks are allowed to close that were not opened by them are
    pipe ends. All others that are inherited, are the responsibility of their
    block parent to check if it is theirs and close it.

*/

void    close_in_fds(t_block *block)
{
    if ((block->father && block->final_in != block->father->final_in) \
    || (!block->father && block->final_in != block->ms->infd))
        close(block->final_in);
}

void    close_out_fds(t_block *block)
{
    if ((block->father && block->final_out != block->father->final_out) \
    || (!block->father && block->final_out != block->ms->outfd))
        close(block->final_out);
}

int redir_has_quotes(char *arg)
{
    int i;

    i = 0;
    while (arg[i])
    {
        if (arg[i] == '\'' || arg[i] == '"')
            return (1);
        i++;
    }
    return (0);
}
/*
int outfiles_from_args_to_list(t_vdmlist **io_files, char **cmd_args, int *i)
{
    int     type;
    int     cur;
    int     c;

    cur = *i;
    c = 1;
    if (!*io_files)
        *io_files = vdmlist_new();
    if (!*io_files)
        return (perror_msg("malloc"));
    if (cmd_args[cur][c] == '>')
    {
        type = T_OUTDIR_APPEND;
        c++;
    }
    else
        type = T_OUTDIR_TRUNC;
    if (cmd_args[cur][c])
    {
        if (!vdmlist_in_tail(*io_files, init_redir(ft_strdup(&cmd_args[cur][c]), type, redir_has_quotes(&cmd_args[cur][c]))))
            return (perror_msg("malloc"));
    }
    else
    {
        if (!vdmlist_in_tail(*io_files, init_redir(cmd_args[cur + 1], type, redir_has_quotes(cmd_args[cur + 1]))))
            return (perror_msg("malloc"));
        cmd_args[cur + 1] = NULL;
        *i += 1;
    }
    ft_free_set_null(&cmd_args[cur]);
    *i += 1;
    return (1);
}

int infiles_from_args_to_list(t_vdmlist **io_files, char **cmd_args, int *i)
{
    int     type;
    int     cur;
    int     c;

    cur = *i;
    c = 1;
    if (!*io_files)
        *io_files = vdmlist_new();
    if (!*io_files)
        return (perror_msg("malloc"));
    if (cmd_args[cur][c] == '<')
    {
        type = T_INDIR_HD;
        c++;
    }
    else
        type = T_INDIR_OPEN;
    if (cmd_args[cur][c])
    {
        if (!vdmlist_in_tail(*io_files, init_redir(ft_strdup(&cmd_args[cur][c]), type, redir_has_quotes(&cmd_args[cur][c]))))
            return (perror_msg("malloc"));
    }
    else
    {
        if (!vdmlist_in_tail(*io_files, init_redir(cmd_args[cur + 1], type, redir_has_quotes(cmd_args[cur + 1]))))
            return (perror_msg("malloc"));
        cmd_args[cur + 1] = NULL;
        *i += 1;
    }
    ft_free_set_null(&cmd_args[cur]);
    *i += 1;
    return (1);
}

*/

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

int ambiguous_redirection_err(t_block *block, char **fail_return)
{
    if (block->father)
        block->father->my_status = EXIT_AMBIG_REDIR;
    else
        block->ms->exit_status = EXIT_AMBIG_REDIR;
    ft_putstr_fd(block->ms->name, block->ms->errfd);
    ft_putstr_fd(": ", block->ms->errfd);
    ft_putstr_fd(*fail_return, block->ms->errfd);
    ft_putstr_fd(" ambiguous redirect\n", block->ms->errfd);
    free(*fail_return);
    return (0);
}

int manage_io_expansion(t_block *block)
{
    char    *temp;
    char    *redir_copy;
    char    *fail_return;

    fail_return = NULL;
    redir_copy = block->io_files->head->text;
    if (redir_copy[0] != '\'')
        if (!expand_dollars(&redir_copy, block->ms))
            return (0);
    if (redir_copy[0] != '\'' \
    && redir_copy[0] != '"')
        if (!expand_wildcards(&redir_copy, &fail_return))
            return (0);
    if (fail_return)
        return (ambiguous_redirection_err(block, &fail_return));
    if (redir_copy[0] == '\'' \
    || redir_copy[0] == '"')
    {
        redir_copy[ft_strlen(redir_copy) - 1] = '\0';
        temp = ft_strdup(&redir_copy[1]);
        if (!temp)
            return (perror_msg("malloc"));
        free(redir_copy);
        redir_copy = temp;
    }
    block->io_files->head->text = redir_copy;
    return (1);
}


/*
    here_doc functions

    These are used to manage here_doc.
    dfs (depth first search) will create a file name that is not in use in the current directory
    on which the user will write to from the terminal.

    here_doc_fill will write to the file using get_next_line and ft_putstr_fd to
    that file  descriptor. The file will then be closed and open in order to reset its offset to 0
    and allow the command that uses it as input to be able to read it from the beginning.

    here_doc will simply print error messages in case there is a problem while opening and closing
    the file and leading execution to fail (here_doc -> manage_infiles -> manage_io_files -> execute return 0)

*/



/*

    here_doc_file
    fills the here_doc temp file with readline.

    it uses here_doc_expand_dollars for env expansion because in here_docs, quotes
    dont' guard the expansion of env variables: they are expanded both with \' and ' " '

*/



/*

    manage_infile/manage_outfile

    These functions are called from manage_io_files
    manage_io_files checks all redirections from the t_vdmlist *files,
    checks if they are in or out, and of which type, then forwarding to manage_infile/manage_outfile
    to manage their particular file descroptors.
    Both of these functions start by closing the previously open file descriptor for their respective
    category (in or out) since these previous fds will not be needed anymore and can be safely closed.
    After analysing their particular file descriptor, the functions will delete the head of t_vdmlist *files
    (which is they one they are always analysing, until the list is empty).

(cat && cat << eof ) < main.c
*/

int manage_infile(t_block *block, int index)
{
    t_token_node	*redir;

    redir = block->io_files->head;
    close_in_fds(block);
    //printf("redirection [%s] at [%s]\n", redir->file, block->prompt);
    if (redir->type == T_INDIR_HD)
    {
        //printf("index   %d, here doc index %d\n", index, block->here_doc_index);
        if (index == block->here_doc_index)
            block->final_in = block->here_doc_fd;
        //printf("here doc, final in is %d\n", block->final_in);
    }
    else
    {
        //printf("at [%s] block->here doc? [%s]\n", redir->file, block->here_doc);
        if (block->here_doc && index > block->here_doc_index)
        {
            //printf("no way...?\n");
            close(block->here_doc_fd);
            unlink(block->here_doc);
            ft_free_set_null(&block->here_doc);
        }
        block->final_in = open(redir->text, O_RDWR);
        //printf("prompt [%s] final in is %d\n", block->prompt, block->final_in);
        if (block->final_in == -1)
             return (perror_msg_func(block, redir->text, CODE_OPEN, 1));

    }
    token_list_del_head(block->io_files);
    return (1);
}

int manage_outfile(t_block *block)
{
    t_token_node *redir;

    redir = block->io_files->head->text;
    close_out_fds(block);
    if (redir->type == T_OUTDIR_TRUNC)
        block->final_out = open(redir->text, O_CREAT | O_RDWR | O_TRUNC, 0644);
    else
        block->final_out = open(redir->text, O_CREAT | O_RDWR | O_APPEND, 0644);
    if (block->final_out == -1)
        return (perror_msg_func(block, redir->text, CODE_OPEN, 1));
    token_list_del_head(block->io_files);
    return (1);
}

/*
    manage_io_files
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

int manage_inherited_fds(t_block *block)
{
    if (!block->father)
    {
        block->final_out = block->ms->outfd;
        block->final_in = block->ms->infd;
        return (1);
    }
    block->final_out = block->father->final_out;
    block->final_in = block->father->final_in;
    if (block->my_id < block->father->op_count \
    && block->father->op_id \
    && block->father->op_id[block->my_id] == OP_PIPE)
        block->final_out = block->father->pipefd[1];
    if (block->my_id > 0 \
    && block->father->op_id \
    && block->father->op_id[block->my_id - 1] == OP_PIPE)
        block->final_in = block->father->prev_pipe[0];
    return (1);
}

int manage_io_files(t_block *block)
{
    int         i;
    int         type;
    int         success;

    manage_inherited_fds(block);
    if (!block->io_files)
    {
        //printf("block [%s], final in %d, final out %d\n", block->prompt, block->final_in, block->final_out);
        return (1);
    }
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
    //printf("block [%s], final in %d, final out %d\n", block->prompt, block->final_in, block->final_out);
    token_list_destroy(&block->io_files);
    return (success);
}





