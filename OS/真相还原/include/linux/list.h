#ifndef  __LINUX_LIST_H__
#define __LINUX_LIST_H__

struct list_head {
        struct list_head *next,*prev;
};


#endif