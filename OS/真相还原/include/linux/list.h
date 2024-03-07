#ifndef  __LINUX_LIST_H__
#define __LINUX_LIST_H__

struct list_head {
        struct list_head *next,*prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}


static inline void list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	list_add(new, head->prev, head);
}

static inline void list_add_head(struct list_head *new, struct list_head *head)
{
	list_add(new, head, head->next);
}

#endif