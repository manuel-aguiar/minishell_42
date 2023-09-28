
## BUILD DETAILS

NAME = libft.a
CC = cc
FLAGS = -Wall -Wextra -Werror
LIB = ar rcs
RM = rm


## FOLDER PATH ##
INC_PATH		=	incs
SRC_PATH		=	srcs
OBJ_PATH		=	objs

## SRC SUB-FOLDERS ##
ATOI			=	atoi
MEMFUNC 		=	ft_memfunc
OUTPUT			=	ft_output
PRINTF			=	ft_printf
STRING			=	ft_string
GNL				=	get_next_line
HASHINT			=	hashset_int
HASHTABLE		=	hashtable
LICIRC			=	int_circular_list
LIDBL			=	int_doubly_list
LISNGL			=	int_single_list
LIXOR			=	int_xor_linkedlist
LVDBL			=	void_dl_list
ISORT			=	int_sorting
MATH			=	math
MPOOL			=	memory_pool
ERROR			=	error_handling


## SOURCE FILES ##

SRCS_ATOI		=	ft_atoi_arr.c 				\
					ft_atoi_overf.c 			\
					ft_atoi.c					\
					ft_itoa.c

SRCS_MEMFUNC	=	ft_bzero.c					\
					ft_calloc.c					\
					ft_free_misc.c				\
					ft_memchr.c					\
					ft_memcmp.c					\
					ft_memcpy.c					\
					ft_memmove.c				\
					ft_memset.c

SRCS_OUTPUT	=	ft_print_arr.c				\
					ft_putchar_fd.c				\
					ft_putendl_fd.c				\
					ft_putnbr_fd.c				\
					ft_putstr_fd.c

SRCS_PRINTF	=	ft_printf_csdiu_perc.c		\
					ft_printf_pxX.c				\
					ft_printf.c

SRCS_STRING	=	ft_isalnum.c				\
					ft_isalpha.c				\
					ft_isascii.c				\
					ft_isdigit.c				\
					ft_isprint.c				\
					ft_isspace.c				\
					ft_split.c					\
					ft_strchr.c					\
					ft_strdup.c					\
					ft_striteri.c				\
					ft_strjoin.c				\
					ft_strlcat.c				\
					ft_strlcpy.c				\
					ft_strlen.c					\
					ft_strmapi.c				\
					ft_strncmp.c				\
					ft_strnstr.c				\
					ft_strrchr.c				\
					ft_strtrim.c				\
					ft_substr.c					\
					ft_tolower.c				\
					ft_toupper.c

SRCS_GNL		=	get_next_line.c				\
					get_next_line_utils.c

SRCS_HASHTABLE	=	hashtable.c						\
					hashtable_utils.c				\
					ht_linkedlist1.c				\
					ht_linkedlist2.c				\
					ht_linkedlist3.c

SRCS_HASHINT	=	hashset_int.c				\
					hashset_int_utils.c

SRCS_LICIRC	=	icd_list_mpool.c			\
					icd_list_mpool_utils.c

SRCS_LIDBL		=	idl_list_malloc.c			\
					idl_list_malloc_utils.c		\
					idl_list_malloc_print.c		\
					idl_list_mpool.c			\
					idl_list_mpool_utils.c

SRCS_LISNGL	=	isl_list_malloc.c			\
					isl_list_malloc_utils.c		\
					isl_list_malloc_print.c

SRCS_LIXOR		=	xor_list_malloc.c			\
					xor_list_malloc_utils.c		\
					xor_list_malloc_print.c		\
					xor_list_mpool.c			\
					xor_list_mpool_utils.c

SRCS_LVDBL		=	vdlist_malloc.c				\
					vdlist_malloc_utils.c		\
					vdlist_malloc_print.c		\
					vdlist_mpool.c				\
					vdlist_mpool_utils.c		\
					vdlist_mpool_print.c

SRCS_ISORT		=	charmat_quicksort.c			\
					cocktailsort.c				\
					mergesort.c					\
					quicksort.c					\
					tim_sort.c					\
					tim_sort_utils.c			\

SRCS_MATH		=	bin_srch.c					\
					ft_min_max.c

SRCS_MPOOL		=	memory_pool.c				\
					memory_pool_utils.c

SRCS_ERROR		=	error_msg.c					\
					perror_msg.c

## ALL SOURCE FOLDERS

SRC_DIRS		=	$(ATOI)		$(MEMFUNC)	$(OUTPUT)	$(PRINTF) 	\
					$(STRING)	$(GNL)		$(HASHINT)	$(LICIRC)	\
					$(LIDBL)	$(LISNGL)	$(LIXOR)	$(LVDBL)	\
					$(ISORT)	$(MATH)		$(MPOOL)	$(ERROR)	\
					$(HASHTABLE)

SRCS			:=  $(addprefix $(SRC_PATH)/$(ATOI)/,$(SRCS_ATOI)) 			\
					$(addprefix $(SRC_PATH)/$(MEMFUNC)/,$(SRCS_MEMFUNC)) 	\
					$(addprefix $(SRC_PATH)/$(OUTPUT)/,$(SRCS_OUTPUT)) 		\
					$(addprefix $(SRC_PATH)/$(PRINTF)/,$(SRCS_PRINTF)) 		\
					$(addprefix $(SRC_PATH)/$(STRING)/,$(SRCS_STRING)) 		\
					$(addprefix $(SRC_PATH)/$(GNL)/,$(SRCS_GNL)) 			\
					$(addprefix $(SRC_PATH)/$(HASHINT)/,$(SRCS_HASHINT)) 	\
					$(addprefix $(SRC_PATH)/$(LICIRC)/,$(SRCS_LICIRC))		\
					$(addprefix $(SRC_PATH)/$(LIDBL)/,$(SRCS_LIDBL)) 		\
					$(addprefix $(SRC_PATH)/$(LISNGL)/,$(SRCS_LISNGL)) 		\
					$(addprefix $(SRC_PATH)/$(LIXOR)/,$(SRCS_LIXOR)) 		\
					$(addprefix $(SRC_PATH)/$(LVDBL)/,$(SRCS_LVDBL)) 		\
					$(addprefix $(SRC_PATH)/$(ISORT)/,$(SRCS_ISORT)) 		\
					$(addprefix $(SRC_PATH)/$(MATH)/,$(SRCS_MATH)) 			\
					$(addprefix $(SRC_PATH)/$(MPOOL)/,$(SRCS_MPOOL)) 		\
					$(addprefix $(SRC_PATH)/$(ERROR)/,$(SRCS_ERROR))		\
					$(addprefix $(SRC_PATH)/$(HASHTABLE)/,$(SRCS_HASHTABLE))


OBJS 		=		$(patsubst $(SRC_PATH)/%.c,$(OBJ_PATH)/%.o,$(SRCS))

all: $(NAME)

$(NAME):  message $(OBJS)
	@echo Compiling Libft, wait a moment....
	@$(LIB) $@ $(OBJS)
	@echo Libft is ready!!

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@$(CC) -g $(FLAGS) -I$(INC_PATH) -c $< -o $@

message:
	@echo Creating object files for Libft...

clean:
	@echo Removing object files.....
	@if [ -d "$(OBJ_PATH)" ]; then \
        rm -rf $(OBJ_PATH); \
    fi
	@echo Objects successfully deleted!

fclean: clean
	@echo Deleting libft.a...
	@if [ -e "$(NAME)" ]; then \
        $(RM) $(NAME); \
    fi
	@echo Done!!

re: fclean all

.PHONY: clean fclean re message

##COLOUR

CLR_RMV		:= \033[0m
RED		    := \033[1;31m
GREEN		:= \033[1;32m
YELLOW		:= \033[1;33m
BLUE		:= \033[1;34m
CYAN 		:= \033[1;36m
