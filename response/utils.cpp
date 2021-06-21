/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddraco <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/21 01:05:08 by efumiko           #+#    #+#             */
/*   Updated: 2021/06/21 01:15:31 by ddraco           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

static const char	*skip_delimeters(const char *s, char c)
{
	while (*s == c)
		s++;
	return (s);
}

static int			count_words(const char *s, char c)
{
	int				result;

	result = 0;
	while (*s != '\0')
	{
		s = skip_delimeters(s, c);
		if (*s != '\0')
		{
			result++;
			while (*s != '\0' && *s != c)
				s++;
		}
	}
	return (result);
}

static char			**free_space(char **to_return)
{
	int				i;

	i = 0;
	while (to_return[i])
		free(to_return[i++]);
	free(to_return);
	return (NULL);
}

static int			get_len(char const *s, char c)
{
	int				i;

	i = 0;
	while (*s != '\0' && *s != c)
	{
		i++;
		s++;
	}
	return (i);
}

char				**ft_split(char const *s, char c)
{
	char	**to_return;
	int		words_amount;
	int		cur_length;
	int		index;

	index = 0;
	words_amount = count_words(s, c);
	if (!(to_return = (char**)malloc(sizeof(char*) * (words_amount + 1))))
		return (NULL);
	while (index < words_amount)
	{
		s = skip_delimeters(s, c);
		cur_length = get_len(s, c);
		if (!(to_return[index] = (char*)malloc(sizeof(char) *
						(cur_length + 1))))
			return (free_space(to_return));
		strlcpy(to_return[index], s, cur_length + 1);
		s += cur_length;
		index++;
	}
	to_return[index] = NULL;
	return (to_return);
}

void	ft_free_array(char ***ar)
{
	char	**array;
	int		i;

	array = *ar;
	i = 0;
	while (array && array[i])
	{
		free(array[i]);
		array[i] = NULL;
		i++;
	}
	free(array);
	array = NULL;
}