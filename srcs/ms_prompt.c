
#include "minishell.h"

/*
    get_prompt

    gets the prompt, validates syntax, removes unnecessary parenthesis,
    gives the prompt to the t_ms struct and saves the exit_status on t_ms
    in case a syntax error was found

*/

int get_prompt(t_ms *ms)
{
    t_prompt pmt;

    if (ms->prompt)
        ft_free_set_null(&ms->prompt);

    if(!setup_prompt_struct(&pmt, ms))
        return (0);

    validate_syntax(&pmt);
    validate_quote_close(&pmt);
    validate_parenthesis_close(&pmt);

    if (pmt.exit_status)
    {
        ms->exit_status = pmt.exit_status;
        return (syntax_error_msg(&pmt));
    }
    if (pmt.parenthesis)
        rm_unnecessary_parenthesis(&pmt);
    ft_free_set_null(&pmt.copy);
    ms->prompt = pmt.prompt;
    return (1);
}

/*
    syntax_error_msg is a helper function to help print the error messages found in
    the p+rompt, to STDERR

    NOT DONE YET, we must change to write to ms->errfd, as it could be changed...? maybe x'D

*/

int syntax_error_msg(t_prompt *pmt)
{
    int i;

    ft_putstr_fd("minishell: syntax error near unexpected token `", pmt->errfd);
    i = 0;
    if (pmt->err_token[i] != '<' \
    && pmt->err_token[i] != '>' \
    && pmt->err_token[i] != '(' \
    && pmt->err_token[i] != ')' \
    && pmt->err_token[i] != '&' \
    && pmt->err_token[i] != '|')
    {
    while (pmt->err_token[i] \
    && pmt->err_token[i] != '<' \
    && pmt->err_token[i] != '>' \
    && pmt->err_token[i] != '(' \
    && pmt->err_token[i] != ')' \
    && pmt->err_token[i] != '&' \
    && pmt->err_token[i] != '|')
        write(STDERR_FILENO, &pmt->err_token[i++], 1);
    }
    else
        ft_putstr_fd(pmt->err_token, pmt->errfd);
    ft_putstr_fd("'\n", pmt->errfd);
    ft_free_set_null(&pmt->err_token);
    ft_free_set_null(&pmt->prompt);
    ft_free_set_null(&pmt->copy);
    return (0);
}

/*
    syntax_error_manager
    when an error is found, it calls the error manager to print the corresponding message
    as well as saving the error code that matches the description
    this error code will later be given to the t_ms to save and report in case of $?

*/

int syntax_error_manager(t_prompt *pmt, int err_code, char *err, int position)
{
    if (pmt->first_error_index == -1)
    {
        pmt->first_error_index = position;
        pmt->exit_status = err_code;
        pmt->err_token = ft_strdup(err);
        if (!pmt->err_token)
            return (perror_msg("malloc"));
    }
    else if (position < pmt->first_error_index)
    {
        pmt->first_error_index = position;
        pmt->exit_status = err_code;
        ft_free_set_null(&pmt->err_token);
        pmt->err_token = ft_strdup(err);
        if (!pmt->err_token)
            return (perror_msg("malloc"));
    }
    return (0);
}


int validate_parenthesis_close(t_prompt *pmt)
{
    int         open;
    int         i;

    open = 0;
    i = 0;
    while (pmt->copy[i])
    {
        if (pmt->copy[i] == '(')
            open++;
        if (pmt->copy[i] == ')')
        {
            if (open == 0)
                return (syntax_error_manager(pmt, EXIT_SYNTAX, ")", i));
            open--;
        }
        i++;
    }
    if (open > 0)
        return (syntax_error_manager(pmt, EXIT_SYNTAX, "(", i));
    return(1);
}

int validate_quote_close(t_prompt *pmt)
{
    int i;
    int quote;

    i = 0;
    quote = 0;
    while (pmt->prompt[i])
    {
        while (pmt->prompt[i] && pmt->prompt[i] != '\'' && pmt->prompt[i] != '"')
            i++;
        if (pmt->prompt[i])
        {
            quote = pmt->prompt[i++];
            while (pmt->prompt[i] && pmt->prompt[i] != quote)
            {
                pmt->copy[i++] = ' ';
            }
            if (pmt->prompt[i] == quote)
            {
                quote = 0;
                i++;
            }
        }
    }
    if (quote)
        return (syntax_error_manager(pmt, EXIT_SYNTAX, "quotes", i));
    return (1);
}

/*
    syntax_operators_end/syntax_redir_end

    self explanatory, no prompts ending on a redirection symbol without a file names
    or ending on an operator with no further instructions.

*/


int syntax_operators_end(t_prompt *pmt)
{
    int         len;
    int         j;
    char        error[3];

    len = ft_strlen(pmt->copy) - 1;
    while (len >= 0 && pmt->copy[len] == ' ')
        len--;
    if (!pmt->copy[len])
        return (1);
    if (pmt->copy[len] == '|' || pmt->copy[len] == '&')
    {
        j = 0;
        error[j++] = pmt->copy[len];
        if (pmt->copy[len - 1] && pmt->copy[len - 1] == pmt->copy[len])
            error[j++] = pmt->copy[len];
        error[j] = '\0';
        return (syntax_error_manager(pmt, EXIT_SYNTAX, error, len));
    }

    return (1);
}

int syntax_redir_end(t_prompt *pmt)
{
    int         len;

    len = ft_strlen(pmt->copy) - 1;
    while (len >= 0 && pmt->copy[len] == ' ')
        len--;
    if (!pmt->copy[len])
        return (1);
    if (pmt->copy[len] == '<' || pmt->copy[len] == '>')
        return (syntax_error_manager(pmt, EXIT_SYNTAX, "newline", len));
    return (1);
}

/*
    syntax_redirections checks that the prompt doesn't end in a Redirection
    or that no two redirection symbols appear in a row without
    an actual file to redirect to. here we only check syntax,
    if the files exist or not is a part of the execution of the program and if
    there are errors there, they will be reported later upon execution.

*/

int syntax_redirections(t_prompt *pmt)
{
    int         i;
    int         j;
    char        error[3];

    i = 0;
    while (pmt->copy[i])
    {
        if (pmt->copy[i] == '<' || pmt->copy[i] == '>')
        {
            i++;
            if (pmt->copy[i] == pmt->copy[i - 1])
                i++;
            while (pmt->copy[i] && pmt->copy[i] == ' ')
                i++;
            if (!pmt->copy[i])
                break ;
            if (pmt->copy[i] && (pmt->copy[i] == '|' || pmt->copy[i] == '&' \
                || pmt->copy[i] == '<' || pmt->copy[i] == '>'))
            {
                j = 0;
                error[j++] = pmt->copy[i];
                if (pmt->copy[i + 1] && pmt->copy[i] == pmt->copy[i + 1])
                    error[j++] = pmt->copy[i];
                error[j] = '\0';
                return (syntax_error_manager(pmt, EXIT_SYNTAX, error, i));
            }
        }
        i++;
    }
    return (1);
}

/*
    syntax_operators

    checks whether the operators are in the correct place.
    error[3] simply serves to report correctly to syntax_error_manager
    about which symbol is the one responsible for the error:
        could be &, &&, |, || ... etc, the size is not fixed but
        strlen is guaranteed to be max 2 (+1 for the null byte);

*/

int syntax_operators(t_prompt *pmt)
{
    int         i;
    int         j;
    char        error[3];

    i = 0;
    while (pmt->copy[i])
    {
        if (pmt->copy[i] == '|' || pmt->copy[i] == '&')
        {
            i++;
            if (pmt->copy[i - 1] == '&' && pmt->copy[i] != '&')
                return (syntax_error_manager(pmt, EXIT_SYNTAX, "&", i - 1));
            if (pmt->copy[i] && pmt->copy[i] == pmt->copy[i - 1])
                i++;
            while (pmt->copy[i] && pmt->copy[i] == ' ')
                i++;
            if (!pmt->copy[i])
                break ;
            if (pmt->copy[i] && (pmt->copy[i] == '|' || pmt->copy[i] == '&'))
            {
                j = 0;
                error[j++] = pmt->copy[i];
                if (pmt->copy[i + 1] && pmt->copy[i] == pmt->copy[i + 1])
                    error[j++] = pmt->copy[i];
                error[j] = '\0';
                return (syntax_error_manager(pmt, EXIT_SYNTAX, error, i));
            }
        }
        i++;
    }
    return (1);
}

/*
    open helper and close helper arenjust helper functions for syntax_parenthesis
    to get the corrrect index where the error was found.

*/

int open_prths_helper(char *copy, int index)
{
    --index;
    while (index >= 0 && copy[index] == ' ')
        index--;
    if (index >= 0 && copy[index] != '&' && copy[index] != '|'  \
        && copy[index] != '(')
        return (index);
    return (-1);
}

int close_prths_helper(char *copy, int index)
{
    int len;

    len = ft_strlen(copy);
    ++index;
    while (index < len && copy[index] == ' ')
        index++;
    if (index < len && copy[index] != '&' && copy[index] != '|'  \
        && copy[index] != ')' && copy[index] != '<' && copy[index] != '>')
        return (index);
    return (-1);
}

/*
    syntax_parenthesis

    checks wether parenthesis are correctly opened and closed

*/

int syntax_parenthesis(t_prompt *pmt)
{
    int         i;
    int         fail_index;

    i = 0;
    while (pmt->copy[i])
    {
        if (pmt->copy[i] == '(')
        {
            pmt->parenthesis = 1;
            fail_index = open_prths_helper(pmt->copy, i);
            if (fail_index != -1)
                return (syntax_error_manager(pmt, EXIT_SYNTAX, \
                        "(", i));
        }
        if (pmt->copy[i] == ')')
        {
            fail_index = close_prths_helper(pmt->copy, i);
            if (fail_index != -1)
                return (syntax_error_manager(pmt, EXIT_SYNTAX, \
                        &pmt->copy[fail_index], fail_index));
        }
        i++;
    }
    return (1);
}

/*
    syntax_begin

    just checks whether the first non-space character is an operators,
    in which case the syntax is not correct.

*/

int syntax_begin(t_prompt *pmt)
{
    int i;

    i = 0;
    while (pmt->prompt[i] == ' ')
        i++;
    if (pmt->prompt[i] == '|')
    {
        if (pmt->prompt[i + 1] == '|')
            return (syntax_error_manager(pmt, EXIT_SYNTAX, "||", i));
        return (syntax_error_manager(pmt, EXIT_SYNTAX, "|", i));
    }
    if (pmt->prompt[i] == '&')
    {
        if (pmt->prompt[i + 1] == '&')
            return (syntax_error_manager(pmt, EXIT_SYNTAX, "&&", i));
        return (syntax_error_manager(pmt, EXIT_SYNTAX, "&", i));
    }
    if (pmt->prompt[i] == ')')
        return (syntax_error_manager(pmt, EXIT_SYNTAX, ")", i));
    return (1);
}

/*

    Thsi functions aggregates the various syntax checks we will be performing on the
    prompt:
        syntax_begin: if there are operators in the beginning of the prompt
        parenthesis: if they are correctly opened and closed
        Operators: one operator cannot be immediatelly after another with no arguments, etc
        Redirections: no redirections without arguments, no redirections before parenthesis, etc
        no redirections or operators at the end

        Redirections can exist on their own withou not commands: fds are opened and closed by the shell
*/

int validate_syntax(t_prompt *pmt)
{
    if (!syntax_begin(pmt)              \
    || !syntax_parenthesis(pmt)           \
    || !syntax_operators(pmt)              \
    || !syntax_redirections(pmt)            \
    || !syntax_redir_end(pmt)               \
    || !syntax_operators_end(pmt))
        return (0);
    return (1);
}

/*
    copy_empty_quotes
    sets up our prompt copy for analysis
    it checks which of " or ' is open at any point in time and
    sets all characters within quotes to spaces.
    This copy will be used throughout the whole analysis since it removes
    characters in between quotes that could result in missfires.

*/

int copy_empty_quotes(t_prompt *pmt)
{
    int i;
    int quote;

    pmt->copy = ft_strdup(pmt->prompt);
    if (!pmt->copy)
        return (perror_msg("malloc"));
    i = 0;
    quote = 0;
    while (pmt->prompt[i])
    {
        while (pmt->prompt[i] && pmt->prompt[i] != '\'' && pmt->prompt[i] != '"')
            i++;
        if (pmt->prompt[i])
        {
            quote = pmt->prompt[i++];
            while (pmt->prompt[i] && pmt->prompt[i] != quote)
                pmt->copy[i++] = ' ';
            if (pmt->prompt[i] == quote)
            {
                quote = 0;
                i++;
            }
        }
    }
    pmt->copy[i] = '\0';
    return (1);
}


/*

    is_only_spaces

    just loops and checks if the prompt is just made of spaces
    If so, no further analysis is needed, give control back to the user.

*/

int is_only_spaces(t_prompt *pmt)
{
    int i;

    i = 0;
    while (pmt->prompt[i])
    {
        if (!ft_isspace(pmt->prompt[i]))
            return (0);
        i++;
    }
    return (1);
}


/*
    setup_prompt_struct

    The function prepares the t_prompt for processing.
    First, readline to get the prompt and check for success
    Second, add prompt to hiistory, bash saves all past prompts regardless of
    success or syntax, so one can save it as soon as it is received and
    memalloc by readline didn't fail.

    If prompt is empty or composed of just spaces (is_only_spaces), free it return:
    get_prompt returns (0), no further processing, and control is given back
    to the user.

    copy_empty_quotes is used to empty all characters in between "" or ' '
    keeping a copy will be usefull to check for syntax and order such that
    it avoids missfires where a token (operator, parenthesis, redirection)
    is inside quotes and should therefore be ignored.
    This copy will be freed after all processing of the prompt is done.

    the struct has a couple of control variables to ease management:
        -   parenthesis (to check later if the order is correct and allow
        for removal of unecessary parenthesis)
        -   first_error_index: the first syntax error that is found is
        the one that is reported.
        -   exit status to provide the ms-structure with the correct
        status upon a syntax failure
        -   err_token: to print to terminal what was the first character
        that resulted in a syntax error. The character is at index
        first_error_index, in the prompt.
        -   errfd: inheried from ms to print to the stderr file descriptor
    For now, these are set to default values as if no errors were found and
    will be change accordingly upon processing.

    If readline fails, it could be from memory failure or because readline received
    an end-of-file, meaning it should exit.
*/


int setup_prompt_struct(t_prompt *pmt, t_ms *ms)
{
    pmt->prompt = readline("minishell>$ ");
    if (!pmt->prompt)
    {
        destroy_ms(ms);
        printf("%s>$ exit\n", ms->name);
        exit(0);
    }
    add_history(pmt->prompt);
    if (!pmt->prompt[0] || is_only_spaces(pmt))
    {

        ft_free_set_null(&pmt->prompt);
        return (0);
    }
    if (!copy_empty_quotes(pmt))
    {
        free(pmt->prompt);
        return (0);
    }
    pmt->parenthesis = 0;
    pmt->first_error_index = -1;
    pmt->exit_status = 0;
    pmt->err_token = NULL;
    pmt->errfd = ms->errfd;
    return (1);
}

/*
    rm_doubled_parenthesis

    The point is to remove unnecessary parenthesis within the prompt
    Given that our implementation is based on blocks and subblocks, we don't
    want to create blocks that add just depth to the recursion but with no
    commands to execute, for instance: (((((ls)))))
        in this case, none of the parenthesis has any effect
    The criteria for removal is:
        no redirections, or operators within parenthesis
    We use the copy from setup_prompt_struct with empty quotes to avoid missfires

    As soon as we find a parenthesis that is not needed. we remove it both from
    the original as from the copy (since we are using the copy for evaluation)

    The algorithm resembles a stack: we increase "open" as parenthesis are opened
    and decrease it as they are closed

    If we find any symbol (| & < >) within a parenthesis, we keep it, otherwise we
    remove both ends

*/

void rm_doubled_parenthesis(t_prompt *pmt)
{
    int i;
    int j;
    int open;
    int keep;

    i = 0;
    while (pmt->copy[i])
    {
        while (pmt->copy[i] && pmt->copy[i] != '(')
            i++;
        if (pmt->copy[i] == '(')
        {
            open = 1;
            keep = 0;
            j = i + 1;
            while (open)
            {
                if (pmt->copy[j] && open == 1 &&\
                    (pmt->copy[j] == '|' || \
                    (pmt->copy[j] == '&' || \
                    pmt->copy[j] == '>'  ||
                    pmt->copy[j] == '<')))
                {
                    keep = 1;
                    break ;
                }
                if (pmt->copy[j] == '(')
                    open++;
                if (pmt->copy[j] == ')')
                    open--;
                if (open != 0)
                    j++;
            }
            if (!keep)
            {
                pmt->copy[i] = ' ';
                pmt->copy[j] = ' ';
                pmt->prompt[i] = ' ';
                pmt->prompt[j] = ' ';
            }
            i++;
        }
    }
}

/*

    remove_corned_parenthesis

    It is somewhat similar to remove double parenthesis.
    In this case, we don't what is within the parenthesis
    just that they are not corners since corner parenthesis don't
    serve any purpose.

    To be a corner, it must be the first non-space chrracter in the prompt

    when we find the character that closes it ')' we check if the remaining
    characters in the prompt are just spaces.
    if so, they are corner parenthesis and can be removed.

*/


void rm_corner_parenthesis(char *copy, char *original)
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
        return ;
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
                original[start] = ' ';
                original[end] = ' ';
            }
            return ;
        }
        i++;
    }
}

void rm_unnecessary_parenthesis(t_prompt *pmt)
{
    rm_doubled_parenthesis(pmt);
    rm_corner_parenthesis(pmt->copy, pmt->prompt);
}
