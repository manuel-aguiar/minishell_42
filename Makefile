
## BUILD DETAILS

##CHANGE COMPILER AND MAKE

NAME 		=		minishell

CC 			= 		cc -g
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

## SOURCE FILES ##
FILES		=		main.c							\
					init_destroy.c					\
					ms_prompt.c						\
					split_manager.c					\
					setup_cmd.c						\
					manage_files.c					\
					exec.c							\
					signals.c						\
					wildcard.c						\
					expansion.c						\
					cd.c							\
					env.c							\
					builtins.c						\
					builtins_aux.c					\
					bi_exit.c						\
					future_libft.c

SRCS		:= 		$(addprefix $(SRC_PATH)/, $(FILES))

OBJS		=		$(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o,$(SRCS))

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
