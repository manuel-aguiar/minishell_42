/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   close_in_out.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 13:44:40 by codespace         #+#    #+#             */
/*   Updated: 2023/09/21 12:13:08 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	close_in/close_out_fds
	These two functions check wether the current file descriptor was
	inherited or not. If not, they can close it. The only file descriptors
	that blocks are allowed to close that were not opened by them are
	pipe ends. All others that are inherited, are the responsibility of their
	block parent to check if it is theirs and close it.

*/

void	close_in_fds(t_block *block)
{
	if ((block->my_manager \
    && block->final_in != block->my_manager->final_in) \
	|| (!block->my_manager \
    && block->final_in != block->ms->infd))
		close(block->final_in);
}

void	close_out_fds(t_block *block)
{
	if ((block->my_manager \
    && block->final_out != block->my_manager->final_out) \
	|| (!block->my_manager \
    && block->final_out != block->ms->outfd))
		close(block->final_out);
}
