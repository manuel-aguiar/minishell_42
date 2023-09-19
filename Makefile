
## BUILD DETAILS

##CHANGE COMPILER AND MAKE

NAME 		=		minishell

CC 			= 		cc -g
## -Wall Wextra Werror
FLAGS 		= 		-Wall -Werror -Wextra -fsanitize=address
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
FD_MAN			=	fd_management
EXPAND			=	expansions
GENERIC			=	generic_utils

## SOURCE FILES ##

FILES_STRUCTS	=	ms_setup.c								\
					block_setup.c							\
					signals.c

FILES_LEXER	=		ms_prompt.c								\
					create_tokens.c 						\
					prompt_to_list.c 						\
					token_list_print.c						\
					token_list1.c							\
					token_list2.c							\
					token_list3.c							\
					token_identification.c

FILES_BUILTINS	=	bi_exit.c 								\
					bi_cd.c 								\
					bi_check_and_exec.c						\
					bi_pwd_echo_unset_export.c				\
					bi_env.c

FILES_EXECUTER	=	execution_tree.c					\
					manager_execution.c					\
					worker_execution.c					\
					exec.c					\
					main.c

FILES_FD_MAN	=	manage_files.c			\
					heredoc_open.c			\
					heredoc_read.c			\
					heredoc_temp.c


FILES_PARSER	=	task_distributor.c		\
					manager_tasks.c			\
					worker_tasks_1.c		\
					worker_tasks_2.c
					
FILES_EXPAND	=	wildcard_return.c			\
					wildcard_search_files.c		\
					wildcard_fit_candidates.c	\
					wildcard_expansion.c		\
					dollar_expansion.c			\
					dolar_heredoc.c

FILES_GENERIC	=	ft_split_count_replenish.c			\
					ft_isquote.c						\
					ft_strdup_len.c						\
					ft_split_join.c			\
					ft_triple_join.c		\
					ft_charmatdup.c			\
					ft_matrixlen.c

## ALL SOURCE FOLDERS

SRC_DIRS		=	$(BUILTINS)		$(EXECUTER)	$(FD_MAN)	$(PARSER) 	\
					$(EXPAND)		$(GENERIC)  $(LEXER) 	$(STRUCTS)

SRCS		:= 		$(foreach src_dir,$(SRC_DIRS),$(wildcard $(SRC_PATH)/$(src_dir)/*.c))

OBJS 		=		$(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o,$(SRCS))

INCS		= 		-I$(INC_PATH) -I$(LIB_PATH)/$(LIB_INC_P)

all: $(NAME)

$(NAME): message $(LIBFT) $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) $(INCS) -o $(NAME) $(ADD_LIB) $(LIB_PATH)/$(LIBFT)
	@echo Program $(NAME) ready!!

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INCS) -c $< -o $@

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
