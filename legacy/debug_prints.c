/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_prints.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/23 10:14:21 by codespace         #+#    #+#             */
/*   Updated: 2023/09/23 18:19:59 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
int	my_workers_addresses(t_block *block)
{
	if (block->is_worker)
		return (1);
	int i = 0;
	printf(" i am %p, my child list is %p, my children are:\n", 
	block, block->worker_list);
	while (block->worker_list[i])
	{
		printf("%p  ", block->worker_list[i++]);
	}
	printf("\n\n");
	return (1);
}

void	print_block(t_block *block)
{
	printf("cmd args are: ");
	int i = 0;
	while (block->cmd_args[i])
		printf("%s ", block->cmd_args[i++]);
	printf("\n");
	printf("\n redirections are: ");
	token_list_head_print(block->io_files, print_token_args);
	printf("\n");
}

int	print_worker_pids(t_block *block)
{
	int	i;

	i = 0;
	printf("i am [%s], starting status %d, mypid %d, my children are:\n    ", 
	block->prompt, block->my_status, getpid());
	while (i < block->op_count + 1)
	{
		printf("%d  ", block->worker_pids[i]);
		i++;
	}
	printf("\n");
	return (1);
}

void	print_execution_tree(t_block *block)
{
	int	i;

	i = 0;
	if (!block)
		return ;
	printf("lvl %d, id %d, prompt [%s]\n", block->my_level, block->my_id, 
	block->prompt);
	if (!block->is_worker)
	{
		i = 0;
		while (block->worker_list[i])
		{
			print_execution_tree(block->worker_list[i]);
			i++;
		}
	}
}
*/
