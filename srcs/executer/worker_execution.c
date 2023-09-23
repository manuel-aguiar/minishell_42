/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   worker_execution.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/19 11:37:25 by codespace         #+#    #+#             */
/*   Updated: 2023/09/23 10:19:33 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	worker_execution(t_block *worker)
{
	if (!worker_task_preparation(worker))
		return (0);
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