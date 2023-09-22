/*
int	worker_args_expand_dollar_wildcard(t_block *worker)
{
	t_token_node	*cur;
	t_token_node	*next;

	cur = worker->prompt->head;
	while (cur)
	{
		if (!*(cur->text))
			cur = cur->next;
		else
		{
			expand_dollars(&cur->text, worker->ms);
			expand_wildcards(&cur->text, NULL);
			if (!*(cur->text))
			{
				next = cur->next;
				token_list_del_node(worker->prompt, cur);
				cur = next;
			}
			else
				cur = cur->next;
		}
	}
	return (1);
}
*/