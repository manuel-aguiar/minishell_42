
#include "libft.h"

int	ft_isspace(int c)
{
	if ((c >= '\t' && c <= '\r') || c == ' ')
		return (1);
	return (0);
}

static t_ulong	multiply_char(int c)
{
	t_ulong	word;

	word = (t_uchar)c;
	word = (word << 8) | word;
	word = (word << 16) | word;
	if (UL_SIZE > 4)
		word = ((word << 16) << 16) | word;
	return (word);
}

static t_uchar	*msloop_unroll(
t_ulong *b, t_ulong word, size_t len, size_t roll)
{
	if (roll == 4)
	{
		while (len-- > 0)
		{
			b[0] = word;
			b[1] = word;
			b[2] = word;
			b[3] = word;
			b += 4;
		}
	}
	else
	{
		while (len-- > 0)
		{
			b[0] = word;
			b[1] = word;
			b += 2;
		}
	}
	return ((t_uchar *)b);
}

static t_uchar	*charword_set(
t_uchar *str, t_ulong word, size_t *len)
{
	size_t	temp_len;
	size_t	roll;

	roll = *len / UL_SIZE;
	if (roll >= 4)
		roll = 4;
	else
		roll = 2;
	temp_len = *len / (UL_SIZE * roll);
	str = msloop_unroll((t_ulong *)str, word, temp_len, roll);
	*len %= UL_SIZE * roll;
	temp_len = *len / UL_SIZE;
	while (temp_len-- > 0)
	{
		*((t_ulong *)str) = word;
		str += UL_SIZE;
	}
	*len %= UL_SIZE;
	return (str);
}

void	*ft_memset(void *b, int c, size_t len)
{
	t_uchar	*str;
	t_ulong	word;

	str = (t_uchar *)b;
	if (len >= UL_SIZE * 4)
	{
		word = multiply_char(c);
		while (((t_ulong)str & (UL_SIZE - 1)) != 0)
		{
			*str++ = c;
			len--;
		}
		str = charword_set(str, word, &len);
	}
	while (len-- > 0)
		*str++ = c;
	return (b);
}



char	*ft_strdup(t_cchar *s)
{
	int		len;
	char	*dest;

	len = ft_strlen(s);
	dest = malloc(sizeof(*dest) * (len + 1));
	if (dest)
	{
		memcpy(dest, s, len);
		dest[len] = '\0';
	}
	return (dest);
}

int	ft_strncmp(t_cchar *s1, t_cchar *s2, size_t n)
{
	t_cchar	*str1;
	t_cchar	*str2;

	str1 = (t_cchar *)s1;
	str2 = (t_cchar *)s2;
	if (n == 0)
		return (0);
	while (--n && (*str1 || *str2) && *str1 == *str2)
	{
		str1++;
		str2++;
	}
	return (*str1 - *str2);
}

static size_t	wordnum(t_cchar *str, char sep)
{
	size_t	count;
	int		i;

	i = 0;
	count = 0;
	if (str[i] && str[i] != sep)
		count++;
	while (str[i] && str[i] != sep)
		i++;
	while (str[i])
	{
		if (str[i] != sep && str[i - 1] == sep)
			count++;
		i++;
	}
	return (count);
}

static char	*lenword(t_cchar *str, char sep, size_t *wordlen)
{
	size_t	i;

	while (*str && *str == sep)
		str++;
	i = 0;
	while (str[i] && str[i] != sep)
		i++;
	*wordlen = i;
	return ((char *)str);
}

static char	*copyword(char *dest, t_cchar *src, size_t wordlen)
{
	ft_memmove(dest, src, wordlen);
	dest[wordlen] = '\0';
	src += sizeof(*src) * wordlen;
	return ((char *)src);
}

char	**ft_split(t_cchar *s, char c)
{
	size_t	numwords;
	size_t	wordlen;
	size_t	i;
	char	**split;

	if (!s)
		return (NULL);
	numwords = wordnum(s, c);
	split = malloc(sizeof(*split) * (numwords + 1));
	if (split)
	{
		i = 0;
		while (i < numwords)
		{
			s = lenword(s, c, &wordlen);
			split[i] = malloc(sizeof(*split[i]) * (wordlen + 1));
			if (split[i])
				s = copyword(split[i], s, wordlen);
			else
				return (ft_free_charmat_null(&split, &free));
			i++;
		}
		split[i] = NULL;
	}
	return (split);
}

void	*ft_free_set_null(void *ptr)
{
	t_uchar	**to_free;

	to_free = (unsigned char **)ptr;
	if (*to_free)
		free(*to_free);
	*to_free = NULL;
	return (NULL);
}

void	*ft_free_charmat_null(void *table, void (*del)(void *))
{
	int		i;
	char	**split;

	if (!table)
		return (NULL);
	split = *(char ***)table;
	if (!split)
		return (NULL);
	i = 0;
	while (split[i])
	{
		del(split[i]);
		split[i] = NULL;
		i++;
	}
	return (ft_free_set_null(table));
}

size_t	ft_strlen(t_cchar *str)
{
	size_t	i;

	i = 0;
	while (str[i])
		i++;
	return (i);
}

void	ft_putstr_fd(char *str, int fd)
{
	if (!*str || !str)
		return ;
	write(fd, str, ft_strlen(str));
}

static size_t	check_align(t_uchar **dest, t_uchar **src, size_t *len)
{
	t_ulong	check;

	while (((t_ulong)(*dest) & UL_ALIGN) != 0)
	{
		*(*dest)++ = *(*src)++;
		(*len)--;
	}
	check = ((t_ulong)(*src) & UL_ALIGN);
	if (check != 0)
	{
		*src += (UL_SIZE - check);
		return (check);
	}
	return (0);
}

static void	loop_unroll(t_ulong **dest, t_ulong **src, size_t chunk,
		size_t roll)
{
	if (roll == FOUR_UNROLL)
	{
		while (chunk--)
		{
			(*dest)[0] = (*src)[0];
			(*dest)[1] = (*src)[1];
			(*dest)[2] = (*src)[2];
			(*dest)[3] = (*src)[3];
			*dest += 4;
			*src += 4;
		}
	}
	else
	{
		while (chunk--)
		{
			(*dest)[0] = (*src)[0];
			(*dest)[1] = (*src)[1];
			*dest += 2;
			*src += 2;
		}
	}
}

static size_t	misalign_copy(t_ulong **dest, t_ulong **src, size_t len,
		size_t bytedif)
{
	size_t	chunk_len;
	t_uchar	lbytes;
	t_uchar	rbytes;

	(*src)--;
	lbytes = (bytedif * CHAR_BIT);
	rbytes = (UL_SIZE * CHAR_BIT) - lbytes;
	chunk_len = (len / (UL_SIZE * TWO_UNROLL));
	len %= (UL_SIZE * TWO_UNROLL);
	while (chunk_len--)
	{
		(*dest)[0] = (((*src)[0] >> lbytes) | ((*src)[1] << rbytes));
		(*dest)[1] = (((*src)[1] >> lbytes) | ((*src)[2] << rbytes));
		*src += TWO_UNROLL;
		*dest += TWO_UNROLL;
	}
	chunk_len = len / UL_SIZE;
	len %= UL_SIZE;
	while (chunk_len--)
	{
		(*dest)++[0] = (((*src)[0] >> lbytes) | ((*src)[1] << rbytes));
		(*src)++;
	}
	(*src)++;
	return (len);
}

static size_t	aligned_copy(t_ulong **dest, t_ulong **src, size_t len)
{
	size_t	chunk_len;

	chunk_len = len / (UL_SIZE * FOUR_UNROLL);
	if (chunk_len > 0)
	{
		loop_unroll(dest, src, chunk_len, FOUR_UNROLL);
		len %= (UL_SIZE * FOUR_UNROLL);
	}
	chunk_len = len / (UL_SIZE * TWO_UNROLL);
	if (chunk_len > 0)
	{
		loop_unroll(dest, src, chunk_len, TWO_UNROLL);
		len %= (UL_SIZE * TWO_UNROLL);
	}
	chunk_len = len / UL_SIZE;
	len %= UL_SIZE;
	while (chunk_len--)
		(*dest)++[0] = (*src)++[0];
	return (len);
}

void	*ft_memcpy(void *dest, const void *src, size_t len)
{
	t_uchar	*d;
	t_uchar	*s;
	size_t	misalign;

	if (!dest && !src)
		return (NULL);
	d = (t_uchar *)dest;
	s = (t_uchar *)src;
	if (len >= UL_SIZE * 4 && UL_SIZE != 1)
	{
		misalign = check_align(&d, &s, &len);
		if (misalign)
		{
			len = misalign_copy((t_ulong **)&d, (t_ulong **)&s, len, misalign);
			s -= (UL_SIZE - misalign);
		}
		else
			len = aligned_copy((t_ulong **)&d, (t_ulong **)&s, len);
	}
	while (len--)
		*d++ = *s++;
	return (dest);
}

char	*ft_strrchr(t_cchar *s, int c)
{
	t_cchar	*move;
	t_uchar	cc;

	cc = (t_uchar)c;
	move = s + ft_strlen(s);
	while (move > s && *move != cc)
		move--;
	if (*move == cc)
		return ((char *)move);
	return (NULL);
}

int	buffer_to_line(char *buf, char **line, int *line_len);
int	buffer_refill(int fd, char *buf, char **line);

char	*get_next_line(int fd)
{
	static char	buf[BUFFER_SIZE + 1];
	char		*line;
	int			line_len;

   
	if (BUFFER_SIZE <= 0 || fd < 0 || fd > FOPEN_MAX)
		return (NULL);
	
	
	
	line = NULL;
	line_len = 0;
	while (1)
	{
		if (buffer_to_line(buf, &line, &line_len))
			break ;
		if (!buffer_refill(fd, buf, &line))
			break ;
	}
	return (line);
}

char	*get_next_line_multi(int fd)
{
	static char	buf[FOPEN_MAX][BUFFER_SIZE + 1];
	char		*line;
	int			line_len;

	if (BUFFER_SIZE <= 0 || fd < 0 || fd > FOPEN_MAX)
		return (NULL);
	line = NULL;
	line_len = 0;
	while (1)
	{
		if (buffer_to_line(buf[fd], &line, &line_len))
			break ;
		if (!buffer_refill(fd, buf[fd], &line))
			break ;
	}
	return (line);
}

static int	increase_line(char **new, char **old, int *old_size, int inc_size)
{
	*new = malloc(sizeof(*new) * (*old_size + inc_size + 1));
	if (!*new)
	{
		if (*old)
			ft_free_set_null(old);
		return (0);
	}
	return (1);
}

static int	gnl_strncat(char *buf, char **line, int *old_size, int inc_size)
{
	char	*new_line;

	if (!increase_line(&new_line, line, old_size, inc_size))
		return (0);
	if (*line)
	{
		ft_memcpy(new_line, *line, *old_size);
		free(*line);
	}
	ft_memcpy(&new_line[*old_size], buf, inc_size);
	*old_size += inc_size;
	new_line[*old_size] = '\0';
	*line = new_line;
	return (1);
}

static void	buffer_clean(char *buf, int pivot)
{
	int	i;

	i = 0;
	while (buf[pivot] && pivot < BUFFER_SIZE)
		buf[i++] = buf[pivot++];
	buf[i] = '\0';
}

int	buffer_to_line(char *buf, char **line, int *line_len)
{
	int	i;
	int	stop;

	if (!*buf)
		return (0);
	stop = 0;
	i = 0;
	while (buf[i] && buf[i] != '\n')
		i++;
	if (buf[i] == '\n')
		stop = 1;
	if (!gnl_strncat(buf, line, line_len, i + stop))
		stop = 1;
	buffer_clean(buf, i + 1);
	return (stop);
}

int	buffer_refill(int fd, char *buf, char **line)
{
	int	bytes;

	bytes = read(fd, buf, BUFFER_SIZE);
	if (bytes < 0)
	{
		buf[0] = '\0';
		if (*line)
			ft_free_set_null(line);
		return (0);
	}
	buf[bytes] = '\0';
	if (!bytes)
		return (0);
	return (1);
}

static size_t	mmrev_check_align(t_uchar **dest, t_uchar **src, size_t *len)
{
	t_ulong	check;

	while (((t_ulong)(*dest) & (UL_ALIGN)) != 0)
	{
		*(*dest)-- = *(*src)--;
		(*len)--;
	}
	**dest = **src;
	(*len)--;
	check = ((t_ulong)(*src) & (UL_ALIGN));
	if (check != 0)
	{
		*src -= (check);
		return ((check));
	}
	return (0);
}

static void	mmrev_loop_unroll(t_ulong **dest, t_ulong **src, size_t chunk,
		size_t roll)
{
	if (roll == FOUR_UNROLL)
	{
		while (chunk--)
		{
			*dest -= FOUR_UNROLL;
			*src -= FOUR_UNROLL;
			(*dest)[3] = (*src)[3];
			(*dest)[2] = (*src)[2];
			(*dest)[1] = (*src)[1];
			(*dest)[0] = (*src)[0];
		}
	}
	else
	{
		while (chunk--)
		{
			*dest -= TWO_UNROLL;
			*src -= TWO_UNROLL;
			(*dest)[1] = (*src)[1];
			(*dest)[0] = (*src)[0];
		}
	}
}

static size_t	mmrev_misalign_copy(t_ulong **dest, t_ulong **src, size_t len,
		size_t bytedif)
{
	size_t	chunk_len;
	t_uchar	lbytes;
	t_uchar	rbytes;

	lbytes = (bytedif * CHAR_BIT);
	rbytes = (UL_SIZE * CHAR_BIT) - lbytes;
	chunk_len = len / UL_SIZE;
	len %= UL_SIZE;
	(*src)--;
	(*dest)--;
	while (chunk_len--)
	{
		(*dest)--[0] = (((*src)[0] >> lbytes) | ((*src)[1] << rbytes));
		(*src)--;
	}
	*(t_uchar **)dest += UL_ALIGN;
	*(t_uchar **)src += UL_ALIGN;
	return (len);
}

static size_t	mmrev_aligned_copy(t_ulong **dest, t_ulong **src, size_t len)
{
	size_t	chunk_len;

	chunk_len = len / (UL_SIZE * FOUR_UNROLL);
	if (chunk_len > 0)
	{
		mmrev_loop_unroll(dest, src, chunk_len, FOUR_UNROLL);
		len %= (UL_SIZE * FOUR_UNROLL);
	}
	chunk_len = len / (UL_SIZE * TWO_UNROLL);
	if (chunk_len > 0)
	{
		mmrev_loop_unroll(dest, src, chunk_len, TWO_UNROLL);
		len %= (UL_SIZE * TWO_UNROLL);
	}
	chunk_len = len / UL_SIZE;
	len %= UL_SIZE;
	(*src)--;
	(*dest)--;
	while (chunk_len--)
		(*dest)--[0] = (*src)--[0];
	*(t_uchar **)dest += UL_ALIGN;
	*(t_uchar **)src += UL_ALIGN;
	return (len);
}

void	*ft_memmove(void *dest, const void *src, size_t len)
{
	t_uchar	*d;
	t_uchar	*s;
	size_t	misalign;

	if (!dest && !src)
		return (NULL);
	if ((t_uchar *)src > (t_uchar *)dest)
		return (ft_memcpy(dest, src, len));
	d = (t_uchar *)dest + len - 1;
	s = (t_uchar *)src + len - 1;
	if (len >= UL_SIZE * 4 && UL_SIZE != 1)
	{
		misalign = mmrev_check_align(&d, &s, &len);
		if (misalign)
		{
			len = mmrev_misalign_copy((t_ulong **)&d, (t_ulong **)&s,
					len, misalign);
			s += misalign;
		}
		else
			len = mmrev_aligned_copy((t_ulong **)&d, (t_ulong **)&s, len);
	}
	while (len--)
		*d-- = *s--;
	return (dest);
}

size_t	ft_strlcpy(char *dst, t_cchar *src, size_t size)
{
	t_cchar	*st;

	st = src;
	if (size > 0)
	{
		while (--size > 0 && *src)
			*dst++ = *src++;
		*dst = '\0';
	}
	while (*src)
		src++;
	return ((size_t)(src - st));
}

void	*ft_calloc(size_t nmemb, size_t bytes)
{
	void	*new;

	new = malloc(nmemb * bytes);
	if (new)
		ft_memset(new, 0, nmemb * bytes);
	return (new);
}


void	*ft_free_sizemat_null(void *table, size_t size, void (*del)(void *))
{
	size_t		i;
	char		**split;

	if (!table)
		return (NULL);
	split = *(char ***)table;
	if (!split)
		return (NULL);
	i = 0;
	while (i < size)
	{
		del(split[i]);
		split[i] = NULL;
		i++;
	}
	return (ft_free_set_null(table));
}

char	*ft_strnstr(t_cchar *big, t_cchar *little, size_t len)
{
	size_t	i;
	size_t	j;
	size_t	lillen;

	if (!*little)
		return ((char *)big);
	lillen = ft_strlen((char *)little);
	i = 0;
	while (big[i] && i < len - lillen + 1)
	{
		if (big[i] == *little)
		{
			j = 0;
			while (big[i + j] == little[j] && little[j + 1] && i + j < len)
				j++;
			if (big[i + j] == little[j] && little[j + 1] == '\0')
				return ((char *)&big[i]);
		}
		i++;
	}
	return (NULL);
}


static int	numdigits(int nb)
{
	int	i;

	if (nb == 0)
		return (1);
	i = 0;
	while (nb != 0)
	{
		i++;
		nb /= 10;
	}
	return (i);
}

static void	stringit(char *str, int nb, int len)
{
	int	mod;

	str[len--] = '\0';
	if (nb == 0)
		str[0] = '0';
	else
	{
		while (nb != 0)
		{
			mod = nb % 10;
			if (mod < 0)
				mod *= -1;
			str[len] = mod + '0';
			len--;
			nb /= 10;
		}
	}
}

char	*ft_itoa(int nb)
{
	int		len;
	char	*str;

	len = 0;
	if (nb < 0)
		len++;
	len += numdigits(nb);
	str = malloc(sizeof(*str) * (len + 1));
	if (str)
	{
		if (nb < 0)
			str[0] = '-';
		stringit(str, nb, len);
	}
	return (str);
}
