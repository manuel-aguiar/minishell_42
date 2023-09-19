/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_execution.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:37:25 by codespace         #+#    #+#             */
/*   Updated: 2023/09/19 12:53:34 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	worker_execution(t_block *worker)
{
	//printf("executing [%s] i'm pid %d\n", block->prompt, block->my_manager->worker_pids[0]);
	//token_list_head_print(block->prompt, print_token_args);
	if (!worker_task_preparation(worker))
		return (0);
	//print_block(block);
	//printf("executing block [%s]\n", block->cmd);
	if (worker->cmd)
		process_execution(worker);
	else
	{
		close_in_fds(worker);
		close_out_fds(worker);
	}
	if (worker->i_am_forked)
		worker->my_manager->my_status = worker->my_status;
	return (1);
}