#include "libft.h"

int	ft_matrixlen(void *mat)
{
	t_uchar **move;
	int			i;

	if (!mat)
		return (-1);
	move = (t_uchar **)mat;
	i = 0;
	while (move[i])
		i++;
	return (i);
}

int	ft_charmatdup(char ***dest, char **src)
{
	char	**res;
	int		len;
	int		i;

	len = ft_matrixlen(src);
	res = malloc((len + 1) * sizeof(*res));
	if (!res)
		return (0);
	i = 0;
	while (i < len)
	{
		res[i] = ft_strdup(src[i]);
		if (!res[i])
		{
			ft_free_charmat_null(&res, &free);
			*dest = res;
			return (0);
		}
		i++;
	}
	res[i] = NULL;
	*dest = res;
	return (1);
}

static int	is_sep(char c, char *sepset)
{
	int	i;

	i = 0;
	while (sepset[i])
	{
		if (c == sepset[i])
			return (1);
		i++;
	}
	return (0);
}

static size_t	wordnum(t_cchar *str, char *sepset)
{
	size_t	count;
	int		i;

	i = 0;
	count = 0;
	if (str[i] && !is_sep(str[i], sepset))
		count++;
	while (str[i] && !is_sep(str[i], sepset))
		i++;
	while (str[i])
	{
		if (!is_sep(str[i], sepset) && is_sep(str[i - 1], sepset))
			count++;
		i++;
	}
	return (count);
}

static char	*lenword(t_cchar *str, char *sepset, size_t *wordlen)
{
	size_t	i;

	while (*str && is_sep(*str, sepset))
		str++;
	i = 0;
	while (str[i] && !is_sep(str[i], sepset))
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

char	**ft_split_count(t_cchar *s, char *sepset, int *place_count)
{
	size_t	wordlen;
	int		i;
	char	**split;

	if (!s)
		return (NULL);
	*place_count = wordnum(s, sepset);
	split = malloc(sizeof(*split) * (*place_count + 1));
	if (split)
	{
		i = 0;
		while (i < *place_count)
		{
			s = lenword(s, sepset, &wordlen);
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

static char	*copyword_replenish(char *dest, t_cchar *src, t_cchar *og, size_t wordlen)
{
	ft_memmove(dest, og, wordlen);
	dest[wordlen] = '\0';
	src += sizeof(*src) * wordlen;
	return ((char *)src);
}

char	**ft_split_count_replenish(t_cchar *s, t_cchar *og, char *sepset, int *place_count)
{
	size_t	wordlen;
	int		i;
	char	**split;
	char    *save;

	if (!s)
		return (NULL);
	save = (char *)s;
	*place_count = wordnum(s, sepset);
	split = malloc(sizeof(*split) * (*place_count + 1));
	if (split)
	{
		i = 0;
		while (i < *place_count)
		{
			s = lenword(s, sepset, &wordlen);
			split[i] = malloc(sizeof(*split[i]) * (wordlen + 1));
			if (split[i])
				s = copyword_replenish(split[i], s, &og[s - save], wordlen);
			else
				return (ft_free_charmat_null(&split, &free));
			i++;
		}
		split[i] = NULL;
	}
	return (split);
}