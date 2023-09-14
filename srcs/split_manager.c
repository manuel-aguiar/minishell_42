
#include "minishell.h"

/*

    split_redirections
    function used to extract the redirections from a split that is not final and as son-blocks below
    The file descriptors present at this level must be extracted in order to be passed to its sons.

    The function calls ft_split_count_replenish
        this function, splits help_pmt that has been cleaned up of quotes and parenthesis in order
        to correctly seperate the arguments. Then, it uses to original to copy the base values
        found on the original prompt but split according to the required criteria (replenishes the former string)

    A split with redirections can only have at most 1 set of open parent parenthesis. Since redirections affecting
    parenthesis can only come afterwards, we will be sure that the parenthesis correspond to children[0];
    After that, we safely remove the outside parenthesis and move on.


*/



/*
    free_split_prompt
    will free the pointers within the t_split_prompt structure
    it doesn't free the structure itself since it is allocated on the stack by the "split_prompt"
    function call;

*/

void    set_in_between_to(char *pmt, char start, char end, char newchar)
{
    int i;
    int open;

    open = 0;
    i = 0;
    while (pmt[i])
    {
        while (pmt[i] && pmt[i] != start)
            i++;
        if (pmt[i])
        {
            open = 1;
            i++;
            while (pmt[i] && open)
            {
                if (pmt[i] == start)
                    open++;
                if (pmt[i] == end)
                {
                    open--;
                    if (end == start)
                        open--;
                }
                if (open)
                    pmt[i] = newchar;
                i++;
            }
        }
    }
}

char *split_copy_empty_quotes_and_parenthesis(char *prompt)
{
    char *copy;

    copy = ft_strdup(prompt);
    if (!copy)
    {
        perror("malloc");
        return (0);
    }
    set_in_between_to(copy, '"', '"', -1);
    set_in_between_to(copy, '\'', '\'', -1);
    set_in_between_to(copy, '(', ')', -1);
    return (copy);
}


int split_extract_redirections(t_split_prompt *split)
{
    int     i;

    split->get_redir = ft_split_count_replenish(split->prompt_copy, split->prompt_orig, " ", &split->redir_split_len);
    if (!split->get_redir)
        return (perror_msg("malloc"));
    i = 0;
    while (i < split->redir_split_len)
    {
        if (split->get_redir[i][0] == '<')
            infiles_from_args_to_list(&split->io_files, split->get_redir, &i);
        else if (split->get_redir[i][0] == '>')
            outfiles_from_args_to_list(&split->io_files, split->get_redir, &i);
        else
            i++;
    }
    free(split->children[0]);
    split->children[0] = split->get_redir[0];
    ft_free_set_null(&split->get_redir);
    return (1);
}




int split_count_operators(t_split_prompt *split)
{
    int i;
    int count;

    count = 0;
    i = 0;
    while (split->prompt_copy[i])
    {
        if (split->prompt_copy[i] == '&' || split->prompt_copy[i] == '|')
        {
            count++;
            i++;
            if (split->prompt_copy[i] && split->prompt_copy[i] == split->prompt_copy[i - 1])
                i++;
        }
        i++;
    }
    return (count);
}

int split_get_operator(t_split_prompt *split, int index, int count)
{
    if (split->prompt_copy[index] == split->prompt_copy[index + 1])
    {
        if (split->prompt_copy[index] == '|')
            split->op_id[count] = OP_OR;
        if (split->prompt_copy[index] == '&')
            split->op_id[count] = OP_AND;
        index++;
    }
    else
        split->op_id[count] = OP_PIPE;
    index++;
    return (index);
}

int split_children_and_operators(t_split_prompt *split)
{
    int len;
    int all;
    int i;

    all = 0;
    i = 0;
    while (all < split->op_count)
    {
        len = 0;
        while (split->prompt_copy[i + len] != '&' && split->prompt_copy[i + len] != '|')
            len++;
        split->children[all] = malloc(sizeof(*(split->children[all])) * (len + 1));
        if (!split->children[all])
            return (perror_msg("malloc"));
        ft_memcpy(split->children[all], &split->prompt_orig[i], len);
        split->children[all][len] = '\0';
        i = split_get_operator(split, i + len, all);
        all++;
    }
    split->children[all++] = ft_strdup(&split->prompt_orig[i]);
    if (!split->children[all - 1])
        return (0);
    split->children[all] = NULL;
    return (1);
}

int free_split_prompt(t_split_prompt *split)
{
    if (split->prompt_copy)
        ft_free_set_null(&split->prompt_copy);
    if (split->children)
        ft_free_charmat_null(&split->children, free);
    if (split->op_id)
        ft_free_set_null(&split->op_id);
    if (split->child_pids)
        ft_free_set_null(&split->child_pids);
    if (split->get_redir)
        ft_free_charmat_null(&split->children, free);
    if (split->io_files)
        vdmlist_destroy(&split->io_files, destroy_redir);
    return (0);
}

int setup_split_prompt_struct(t_split_prompt *split, t_block *block)
{
    split->prompt_orig = block->prompt;
    split->prompt_copy = NULL;
    split->op_id = NULL;
    split->children = NULL;
    split->child_pids = NULL;
    split->get_redir = NULL;
    split->io_files = NULL;
    split->parenthesis_fork = 0;
    split->has_unnecessary_parenthesis = 0;
    split->prompt_copy = split_copy_empty_quotes_and_parenthesis(split->prompt_orig);
    if (!split->prompt_copy)
        return (free_split_prompt(split));
    split->op_count = split_count_operators(split);
    split->op_id = malloc(sizeof(*(split->op_id)) * split->op_count);
    split->children = ft_calloc((split->op_count + 2), sizeof(*(split->children)));
    split->child_pids = ft_calloc(split->op_count + 1, sizeof(*(split->child_pids)));
    if (!split->op_id || !split->children || !split->child_pids)
    {
        perror_msg("malloc");
        return (free_split_prompt(split));
    }
    return (1);
}


int dump_split_to_block(t_block *block, t_split_prompt *split)
{
    block->child_prompts = split->children;
    block->child_pids = split->child_pids;
    block->op_id = split->op_id;
    block->op_count = split->op_count;
    block->io_files = split->io_files;
    block->parenthesis_fork = split->parenthesis_fork;
    block->has_unnecessary_parenthesis = split->has_unnecessary_parenthesis;
    free(split->prompt_copy);
    if (!block->is_cmd)
    {
        block->child_list = malloc(sizeof(*block->child_list) * (block->op_count + 2));
		block->child_exit_status = malloc(sizeof(*block->child_list) * (block->op_count + 1));
        if (!block->child_list || !block->child_exit_status)
        {
            destroy_block(block);
            return (perror_msg("malloc"));
        }
		ft_memset(block->child_exit_status, -1, sizeof(int) * (block->op_count + 1));
    }
    return (1);
}

int has_corner_parenthesis(char *copy, char *original)
{
    int i;
    int open;
    int start;
    int end;

    set_in_between_to(copy, '(', ')', ' ');
    i = 0;
    while (copy[i] && ft_isspace(copy[i]))
        i++;
    if (!copy[i] || copy[i] != '(')
        return (0);
    start = i++;
    open = 1;
    while (copy[i])
    {
        if (copy[i] == '(')
            open++;
        if (copy[i] == ')')
            open--;
        if (open == 0)
        {
            end = i;
            while (ft_isspace(copy[++i]))
                ;
            if (!copy[i])
            {
                copy[start] = ' ';
                copy[end] = ' ';
                i = start + 1;
                while (i < end)
                {
                    copy[i] = original[i];
                    i++;
                }
                //printf("copy [%s] after removing corner\n", copy);
                return (1);
            }
            return (0);
        }
        i++;
    }
    return (0);
}

int split_unnecessary_parenthesis(t_split_prompt *split)
{
    char    *copy;

    copy = ft_strdup(split->children[0]);
    if (!copy)
        return (perror_msg("malloc"));
    if (has_corner_parenthesis(copy, split->children[0]) \
    && has_corner_parenthesis(copy, split->children[0]))
        split->has_unnecessary_parenthesis = 1;
    free(copy);
    copy = ft_strdup(split->children[0]);
    if (!copy)
        return (perror_msg("malloc"));
    //printf("split [%s] before removing\n", split->children[0]);
    rm_corner_parenthesis(copy, split->children[0]);
    //printf("split [%s] after removing\n", split->children[0]);
    free(copy);
    return (1);
}

int split_prompt(t_block *block)
{
    int             i;
    t_split_prompt  split;

    if (!setup_split_prompt_struct(&split, block))
        return (0);
    if (!split_children_and_operators(&split))
        return (free_split_prompt(&split));
    if (split.op_count == 0)
    {
        i = 0;
        while (split.prompt_copy[i] && split.prompt_copy[i] != '(')
            i++;
        if (!split.prompt_copy[i])
            block->is_cmd = 1;
        else
        {
            //printf("split [%s] must be forked cause it has parenthesis\n", split.prompt_orig);
            split.parenthesis_fork = 1;
            if (!split_extract_redirections(&split) \
            || !split_unnecessary_parenthesis(&split))
                return (free_split_prompt(&split));
            //printf("split [%s] has unnecessary? %d\n", split.prompt_orig, split.has_unnecessary_parenthesis);
        }
    }
    if (!dump_split_to_block(block, &split))
        return (0);
    return (1);
}

/*

    print_split
    This function serves only to print the split during dev phase, will not be a part of the final binary


*/



void    print_split(t_split_prompt *split)
{
    printf("split operators: %d, they are:\n", split->op_count);
    int i = 0;
    if (split->op_id)
    {
        while (i < split->op_count)
            printf("%d ", split->op_id[i++]);
    }
    printf("\n children are: \n");
    i = 0;
    if (split->children)
    {
        while (i < split->op_count + 1)
            printf("%s\n", split->children[i++]);
    }
    printf("total %d args\n", i);
}


