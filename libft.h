#ifndef LIBFT_H

# define LIBFT_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <stddef.h>
# include <limits.h>
# include <string.h>

# if defined(__LP64__)
#  define UL_SIZE 8
#  define UL_ALIGN 7
# elif defined(_WIN64)
#  define UL_SIZE 8
#  define UL_ALIGN 7
# elif defined(__x86_64__)
#  define UL_SIZE 8
#  define UL_ALIGN 7
# elif defined(__aarch64__)
#  define UL_SIZE 8
#  define UL_ALIGN 7
# elif defined(__LP32__)
#  define UL_SIZE 4
#  define UL_ALIGN 3
# elif defined(_WIN32)
#  define UL_SIZE 4
#  define UL_ALIGN 3
# elif defined(__i386__)
#  define UL_SIZE 4
#  define UL_ALIGN 3
# elif defined(__arm__)
#  define UL_SIZE 4
#  define UL_ALIGN 3
# else
#  define UL_SIZE 1
#  define UL_ALIGN 0
# endif

# define FOUR_UNROLL 4
# define TWO_UNROLL 2

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 100
# endif


typedef unsigned long	t_ulong;
typedef unsigned char	t_uchar;
typedef const char		t_cchar;


void	*ft_memset(void *b, int c, size_t len);
char	*ft_strdup(t_cchar *s);
int	    ft_strncmp(t_cchar *s1, t_cchar *s2, size_t n);
char	**ft_split(t_cchar *s, char c);
void	*ft_free_set_null(void *ptr);
void	*ft_free_charmat_null(void *table, void (*del)(void *));
size_t	ft_strlen(t_cchar *str);
void	*ft_memcpy(void *dest, const void *src, size_t len);
char	*ft_strrchr(t_cchar *s, int c);
char	*get_next_line(int fd);
void	ft_putstr_fd(char *str, int fd);
void	*ft_memmove(void *dest, const void *src, size_t len);
size_t	ft_strlcpy(char *dst, t_cchar *src, size_t size);
void	*ft_calloc(size_t nmemb, size_t bytes);
char	**ft_split_count(t_cchar *s, char *sepset, int *place_count);
char	**ft_split_count_replenish(t_cchar *s, t_cchar *og, char *sepset, int *place_count);
void	*ft_free_sizemat_null(void *table, size_t size, void (*del)(void *));
char	*ft_strnstr(t_cchar *big, t_cchar *little, size_t len);
int     ft_isspace(int c);
char	*ft_itoa(int nb);


/* fuiture_libft.c*/
int	ft_matrixlen(void *mat);
int	ft_charmatdup(char ***dest, char **src);


typedef struct s_vdmnode
{
	void				*data;
	struct s_vdmnode	*next;
	struct s_vdmnode	*prev;
}	t_vdmnode;

typedef struct s_vdmlist
{
	t_vdmnode	*head;
	t_vdmnode	*tail;
	size_t		len;
}	t_vdmlist;

t_vdmlist	*vdmlist_new(void);
int			vdmlist_in_head(t_vdmlist *list, void *data);
int			vdmlist_in_tail(t_vdmlist *list, void *data);
int			vdmlist_find(t_vdmlist *list, void *data, \
int (*cmp)(void *, void *));
void		vdmlist_remove_if(t_vdmlist *list, void *data, \
int (*cmp)(void *, void *), void (*del)(void *));
void		vdmlist_del_head(t_vdmlist *list, void (*del)(void *));
void		vdmlist_del_tail(t_vdmlist *list, void (*del)(void *));
void		vdmlist_head_print(t_vdmlist *list, void (*pnt)(void *));
void		vdmlist_tail_print(t_vdmlist *list, void (*pnt)(void *));
void		vdmlist_destroy(t_vdmlist **list, void (*del)(void *));


#endif