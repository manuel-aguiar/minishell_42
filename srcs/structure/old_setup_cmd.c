#include "minishell.h"

/*

    remove_unguarded_quotes
    fan favourite stupid function for those nasty cases like:
    echo "'"hello"'"bye'"'''

    checks whether a quote guard is guarded itself by another quote and should stay,
    then mallocs the whole damn thing and you have a total of 2x the memory of your
    starting string, once again, by the 100th time since this program started, nice.

    which by the way, if you are wondering, the above example should output this: 'hello'bye"

    happy? who dafuq tries this stuff

*/



/*
    cmd_args_has_space_split
    This function lacks creativity in its naming.
    sees if an argument has space in it (after confirming it is not between quotes)
    and splits and updates the full list....

    it sees what the new total is, gets the split "split", allocates "new" for the new total.
    copies all block->cmd_args in order, then inserts the split, copies the remaining cmd_args
    and thhen frees the holding array of the old block->cmd_args and split (all the pointers
    were passed to "new", we don't want to free those).

    (it also frees the cmd_arg[index] in the middle that we split, since we won't use that anymore)

    And we go back to the original loop.
    #ugliest_function_ever


int cmd_args_has_space_split(t_prep_cmd *cmd, int *index)
{
    int     i;
    int     count;
    char    **split;
    char    **new;

    i = 0;
    while (cmd->cmd_args[*index][i] && !ft_isspace(cmd->cmd_args[*index][i]))
        i++;
    if (!cmd->cmd_args[*index][i])
        return (1);

    split = ft_split_count(cmd->cmd_args[*index], " ", &count);
    if (!split)
        return (perror_msg("malloc"));
    ft_free_set_null(&cmd->cmd_args[*index]);
    cmd->args_len += count - 1;
    new = malloc(sizeof(*new) * (cmd->args_len + 1));
    if (!new)
    {
        ft_free_charmat_null(&split, free);
        return (perror_msg("malloc"));
    }
    i = 0;
    while (i < cmd->args_len)
    {
        /*new[i] = (char *)(t_ulong)(i < *index) * (t_ulong)cmd->cmd_args[i]  \
                + (t_ulong)(i == *index && i - *index < count) * (t_ulong)split[i - *index]  \
                + (t_ulong)(i < cmd->args_len && i > *index) * (t_ulong)cmd->cmd_args[i - count + 1]);
        i++;*/

        if (i < *index)
        {
            new[i] = cmd->cmd_args[i];
            i++;
        }
        else if (i == *index)
        {
            while (i - *index < count)
            {
                new[i] = split[i - *index];
                i++;
            }
        }
        else if (i < cmd->args_len)
        {
            new[i] = cmd->cmd_args[i - count + 1];
            i++;
        }
    }
    new[i] = NULL;
    ft_free_set_null(&cmd->cmd_args);
    ft_free_set_null(&split);
    cmd->cmd_args = new;
    *index += (count - 1);
    return (1);
}
*/
/*
    cmd_args_rm_quotes_and_split
    This function removes the outter quotes of each argument.
    If there are no outter quotes but the argument can be
    split in spaces due to dollar/wildcard expansion, a split will be made
    into further individual arguments.


int cmd_args_rm_quotes_and_split(t_prep_cmd *cmd)
{
    int i;
    int has_guards;

    has_guards = 0;
    i = 0;
    while (cmd->cmd_args[i])
    {
        if (!remove_unguarded_quotes(&cmd->cmd_args[i], &has_guards))
            return (0);
        if (!has_guards)
        {
            if (!cmd_args_has_space_split(cmd, &i))
                return (0);
        }
        i++;
    }
    return (1);
}
*/
/*
    cmd_args_expand_dollar_wildcard
    As per the name, goes through all the cmd_args and expands wildcards
    and envieronment variables within each.
    Expand dollars in every case except \'.
    Expand wildcards whenever there are no quotes surrounding the argument.
    We use block->prompt_copy to hold our argument to the other function calls,
    just to spare one variable....


int cmd_args_expand_dollar_wildcard(t_prep_cmd *cmd)
{
    int i;

    i = 0;
    while (cmd->cmd_args[i])
    {
        expand_dollars(&cmd->cmd_args[i], cmd->ms);
        expand_wildcards(&cmd->cmd_args[i], NULL);
        i++;
    }
    return (1);
}

*/

/*
    get_cmd_args

    after having succeessfully removed redirections to the linkedlist,
    we agreggate the remaining split since they are all the command args.
    after that, we will need to expand wildcards, environment and
    remove quotes

    expansion of enviroment variables and wildcards must occur seperatelly
    in cmd_args and io_files because on the latter, multiple io_files
    per redirection will result in ambiguous redirection and execution must
    therefore stop there. So, we can manage io_files before continuing expanding
    the command, since once we find a problem in redirections, the rest is not
    really needed because execution will stop anyways.

    at the end, we can do ft_free_set_null to block->cmd_args_copy (despite being char **)
    because we have successefully moved all addresses we wanted to block->cmd_args
    therefore, no malloc'ed pointers are being held exclusively by block->cmd_args_copy and we can
    free the main char** directly.

*/

/*

int get_cmd_args(t_prep_cmd *cmd)
{
    int i;
    int len;

    cmd->cmd_args = malloc(sizeof(*cmd->cmd_args) * (cmd->args_len + 1));
    if (!cmd->cmd_args)
        return (perror_msg("malloc"));
    i = 0;
    len = 0;
    while (i < cmd->split_len)
    {
        if (cmd->cmd_args_copy[i])
            cmd->cmd_args[len++] = cmd->cmd_args_copy[i];
        i++;
    }
    cmd->cmd_args[len] = NULL;
    ft_free_set_null(&cmd->cmd_args_copy);
    return (1);
}


    split_cmd_by_quotes_spaces
    make a copy of the original, set characters between quotes to an
    arbitrary non-space value.
    split by spaces (spaces between quotes will be set to something else)
    loop through the whole thing to replenish the empty quotes with the original values


int split_cmd_by_quotes_spaces(t_prep_cmd *cmd, t_block *block)
{
    cmd->prompt_copy = ft_strdup(block->prompt);
    if (!cmd->prompt_copy)
        return (perror_msg("malloc"));
    set_in_between_to(cmd->prompt_copy, '"', '"', -1);
    set_in_between_to(cmd->prompt_copy, '\'', '\'', -1);
    cmd->cmd_args_copy = ft_split_count_replenish(cmd->prompt_copy, block->prompt, " ", &cmd->split_len);
    if (!cmd->cmd_args_copy)
        return (perror_msg("malloc"));
    ft_free_set_null(&cmd->prompt_copy);
    return (1);
}
*/
/*
    setup_cmd
    preparing the command for execution
    here we take the prompt, which we have identified already as a final command and:
        - separate redirections
        - expand environment variables
        - expand wildcards
        - remove quotes

*/

/*
    free_cmd_struct
    the struct is not malloc'ed, just the pointers may be. we free those and the stack call
    by "setup_cmd" will destroy the struct itself

    If everything goes to plan, this function will not be called
    In the end, all that was done by t_prep_cmd will be given to t_block.



int free_cmd_struct(t_prep_cmd *cmd)
{
    if (cmd->cmd)
        ft_free_set_null(&cmd->cmd);
    if (cmd->cmd_args)
        ft_free_charmat_null(&cmd->cmd_args, free);
    if (cmd->prompt_copy)
        ft_free_set_null(&cmd->prompt_copy);
    if (cmd->cmd_args_copy)
        ft_free_sizemat_null(&cmd->cmd_args_copy, cmd->split_len, free);
    return (0);
}

int setup_prep_cmd_struct(t_prep_cmd *cmd, t_block *block)
{
    cmd->cmd = NULL;
    cmd->cmd_args = NULL;
    cmd->prompt_copy = NULL;
    cmd->cmd_args_copy = NULL;
	cmd->args_len = 0;
	cmd->split_len = 0;
    cmd->ms = block->ms;
    if (!split_cmd_by_quotes_spaces(cmd, block))
        return (0);
    return (1);
}
*/

/*

typedef struct s_prep_cmd
{
    char    	*cmd;	                //decomposição do prompt
    char    	**cmd_args;                 //decomposição do prompt
    char        *prompt_copy;
    char        **cmd_args_copy;
    int         args_len;
    int         split_len;
}   t_prep_cmd;



int dump_cmd_into_block(t_prep_cmd *cmd, t_block *block)
{
    block->cmd = cmd->cmd;
    block->cmd_args = cmd->cmd_args;
    if (cmd->prompt_copy)
        ft_free_set_null(cmd->prompt_copy);
    if (cmd->cmd_args_copy)
        ft_free_sizemat_null(&cmd->cmd_args_copy, cmd->split_len, free);
    return (1);
}
*/
/*
int setup_cmd_pre_expansion(t_block *block)
{
    int         i;
    t_prep_cmd  cmd;

    if (!setup_prep_cmd_struct(&cmd, block))
        return (0);
    i = 0;
    while (i < cmd.split_len)
    {
        if (cmd.cmd_args_copy[i][0] == '<')
            infiles_from_args_to_list(&block->io_files, cmd.cmd_args_copy, &i);
        else if (cmd.cmd_args_copy[i][0] == '>')
            outfiles_from_args_to_list(&block->io_files, cmd.cmd_args_copy, &i);
        else
        {
            i++;
            cmd.args_len++;
        }
    }
    if (cmd.args_len && !get_cmd_args(&cmd))
        return (free_cmd_struct(&cmd));
    dump_cmd_into_block(&cmd, block);
    return (1);
}




int manage_cmd_expansions(t_block *block)
{
    t_prep_cmd  cmd;

    cmd.cmd = NULL;
    cmd.cmd_args = block->cmd_args;
    cmd.prompt_copy = NULL;
    cmd.cmd_args_copy = NULL;
    cmd.ms = block->ms;
    if (!cmd.cmd_args)
        return (1);
    cmd.args_len = ft_matrixlen(cmd.cmd_args);
    if (!cmd_args_expand_dollar_wildcard(&cmd))
        return (0);
    if (!cmd_args_rm_quotes_and_split(&cmd))
        return (0);
    cmd.cmd = ft_strdup(cmd.cmd_args[0]);
    if (!cmd.cmd)
        return (free_cmd_struct(&cmd));
    block->cmd = cmd.cmd;
    block->cmd_args = cmd.cmd_args;
    return (1);
}
*/
/*

    print_cmd
    helper function to print commands, used only on the dev phase, not on the final cut


*/


void    print_cmd(t_block *block)
{
    printf("\ncommand args: \n");
    int i;
    if (block->cmd_args)
    {
        i = 0;
        while (block->cmd_args[i])
            printf("[%s]\n", block->cmd_args[i++]);
    }
    printf("\ninfiles: \n");
    if (block->io_files)
    {
        t_vdmnode *cur;
        t_redir *redir;
        cur = block->io_files->head;
        while (cur)
        {
            redir = (t_redir *)cur->data;
            printf("type %d, file [%s]\n", redir->type, redir->file);
            cur = cur->next;
        }
    }
    printf("\n\n");
}

/* TODO LIST

Refazer o split dos comandos, tem de ter em conta as aspas e está apenas a separar por
espaços
Depois do split de comandos, é preciso retirar as aspas exteriores de cada um dos comandos

Espansão de environment variables que inclui split normal por espaços

Expansão de wildcards


Expansao de env e wildcards tem de ser feita em separado na gestao de ficheiros
e em cmd args

em cmd args, vai tudo junto
em manage_io_files pode resultar em ambiguous redirection


*/
