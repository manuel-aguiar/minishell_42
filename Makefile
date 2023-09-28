
## BUILD DETAILS

##CHANGE COMPILER AND MAKE

NAME 		=		minishell

CC 			= 		cc -g #-fsanitize=address
## -Wall Wextra Werror
FLAGS 		= 		-Wall -Werror -Wextra
ADD_LIB 	= 		-L./$(LIB_PATH) -lft -lreadline
RM 			=		rm
MAKE		=		make

## FOLDER PATH ##
INC_PATH	=		incs
SRC_PATH	=		srcs
OBJ_PATH	=		objs

LIB_PATH	=		libft
LIB_INC_P	=		incs

## SOURCE LIBRARIES
LIBFT		=		libft.a

## SRC SUB-FOLDERS ##
STRUCTS			=	structs
LEXER			=	lexer
PARSER			=	parser
BUILTINS		=	builtins
EXECUTER		=	executer
REDIR			=	redirections
EXPAND			=	expansions
GENERIC			=	generic_utils

## SOURCE FILES ##

SRCS_STRUCTS	=	ms_setup.c								\
					block_setup.c							\
					signals.c

SRCS_LEXER	=		get_prompt.c							\
					create_tokens.c 						\
					prompt_to_list.c 						\
					prompt_to_list2.c						\
					token_list_print.c						\
					token_list1.c							\
					token_list2.c							\
					token_list3.c							\
					token_identification.c					\
					validate_prompt.c						\
					validate_2.c

SRCS_BUILTINS	=	bi_check_and_exec.c						\
					bi_exit.c 								\
					bi_cd.c 								\
					bi_unset.c								\
					bi_pwd.c								\
					bi_export.c								\
					bi_env.c								\
					bi_echo.c								\
					unset_export_aux.c
					

SRCS_EXECUTER	=	minishell_main.c					\
					execution_tree_loop.c				\
					execution_tree_setup.c				\
					manager_execution.c					\
					worker_execution.c					\
					parent_process.c					\
					child_process.c						\
					child_execve.c						

SRCS_REDIR		=	prepare_redirections.c				\
					expand_redir_args.c					\
					close_in_out.c						\
					heredoc_open.c						\
					heredoc_read.c						\
					heredoc_temp.c


SRCS_PARSER		=	task_distributor.c		\
					manager_tasks_1.c		\
					manager_tasks_2.c		\
					worker_tasks_1.c		\
					worker_tasks_2.c		\
					worker_tasks_3.c
					
SRCS_EXPAND		=	wildcard_return.c			\
					wildcard_search_files.c		\
					wildcard_fit_candidates.c	\
					wildcard_expansion.c		\
					dollar_expansion.c			\
					dollar_concat.c				\
					dollar_heredoc.c			\
					expansion_char_protect.c	\
					rm_unguarded_quotes.c		\
					rm_unguarded_wildcard.c

SRCS_GENERIC	=	ft_split_count_replenish.c			\
					ft_isquote.c						\
					ft_strdup_len.c						\
					ft_split_join.c						\
					ft_triple_join.c					\
					ft_charmatdup.c						\
					ft_matrixlen.c

## ALL SOURCE FOLDERS

SRC_DIRS		=	$(BUILTINS)		$(EXECUTER)	$(REDIR)	$(PARSER) 	\
					$(EXPAND)		$(GENERIC)  $(LEXER) 	$(STRUCTS)

SRCS			:=  $(addprefix $(SRC_PATH)/$(BUILTINS)/,$(SRCS_BUILTINS)) \
					$(addprefix $(SRC_PATH)/$(EXECUTER)/,$(SRCS_EXECUTER)) \
					$(addprefix $(SRC_PATH)/$(REDIR)/,$(SRCS_REDIR)) \
					$(addprefix $(SRC_PATH)/$(PARSER)/,$(SRCS_PARSER)) \
					$(addprefix $(SRC_PATH)/$(EXPAND)/,$(SRCS_EXPAND)) \
					$(addprefix $(SRC_PATH)/$(GENERIC)/,$(SRCS_GENERIC)) \
					$(addprefix $(SRC_PATH)/$(LEXER)/,$(SRCS_LEXER)) \
					$(addprefix $(SRC_PATH)/$(STRUCTS)/,$(SRCS_STRUCTS))

OBJS 		=		$(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o,$(SRCS))

INCS		= 		-I$(INC_PATH) -I$(LIB_PATH)/$(LIB_INC_P)

all: $(NAME)

$(NAME): message $(LIBFT) $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) $(INCS) -o $(NAME) $(ADD_LIB) $(LIB_PATH)/$(LIBFT)
	@echo Program $(NAME) ready!!

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) $(INCS) -c $< -o $@

$(LIBFT):
	@echo Preparing Libft....
	@$(MAKE) -C $(LIB_PATH)

message:
	@echo Building program....

clean:
	@echo Removing object files.....
	@if [ -d "$(OBJ_PATH)" ]; then \
        rm -rf $(OBJ_PATH); \
    fi
	@make clean -C $(LIB_PATH)
	@echo Objects successfully deleted!

fclean: clean
	@echo Deleting libft.a...
	@if [ -n "$(wildcard  $(NAME))" ]; then \
        $(RM) $(NAME); \
    fi
	@if [ -n "$(wildcard  $(LIB_PATH)/$(LIBFT))" ]; then \
        $(RM) $(LIB_PATH)/$(LIBFT); \
    fi
	@echo Done!!

re: fclean all

.PHONY: clean fclean re message bonus
