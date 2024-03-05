#ifndef	_LINUX_RBTREE_H
#define	_LINUX_RBTREE_H

#include "stdint.h"
#define NULL 0
// 红黑树是一种自平衡的二叉查找树
/*
1. 节点是红色或黑色。

2. 根节点是黑色。

3. 所有叶子节点都是黑色的空节点。(叶子节点是NIL节点或NULL节点)

4. 每个红色节点的两个子节点都是黑色节点。(从每个叶子节点到根的所有路径上不能有两个连续的红色节点)

5. 从任一节点到其每个叶子节点的所有路径都包含相同数目的黑色节点。
*/
// 定义红黑树的节点
struct rb_node
{
	// 这个成员变量存储了两个信息，一个是指向父节点的指针，另一个是节点的颜色信息
	// 通常情况下，高位存储父节点指针，低位存储节点颜色。这样设计可以节省空间
	unsigned long  rb_parent_color;	
#define	RB_RED		0
#define	RB_BLACK	1
	// 当前节点的左右子节点
	struct rb_node *rb_right;	
	struct rb_node *rb_left;
	// 某些嵌入式的处理器架构对内存对其有需要，一般用不到
} __attribute__((aligned(sizeof(long))));
    /* The alignment might seem pointless, but allegedly CRIS needs it */

// 定义红黑树的根节点
struct rb_root
{
	struct rb_node *rb_node;
};

// 宏函数
// rb_parent(r) 由于 rb_parent_color 低两位存储节点颜色，该函数作用是将低两位清零
// 32位 unsigned long 占四字节，而为了 rb_node 地址对齐，会导致 rb_parent 的指针二进制格式的低两位一定是0。
// 开发者将低两位用来设置红黑树节点颜色，因此是'& ~3' 而不是 `& ~1`
#define rb_parent(r)   ((struct rb_node *)((r)->rb_parent_color & ~3))
// rb_color(r) 返回当前节点颜色
#define rb_color(r)   ((r)->rb_parent_color & 1)
// 判断当前节点是否是红色
#define rb_is_red(r)   (!rb_color(r))
// 判断当前节点是否是黑色
#define rb_is_black(r) rb_color(r)
// 设置当前节点为红色
#define rb_set_red(r)  do { (r)->rb_parent_color &= ~1; } while (0)
// 设置当前节点为黑色
#define rb_set_black(r)  do { (r)->rb_parent_color |= 1; } while (0)
// 设置父节点地址
static inline void rb_set_parent(struct rb_node *rb, struct rb_node *p)
{
	rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
// 设置父节点颜色
static inline void rb_set_color(struct rb_node *rb, int color)
{
	rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

#define RB_ROOT	(struct rb_root) { NULL, }
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

// 检查给定红黑树根节点是否为空
#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
// 检查给定红黑树节点是否为空
#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)
// 将给定的红黑树节点标记为空节点，父节点指针指向自身表示该节点为空
#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))

// 向红黑树插入节点后，通过调整颜色和旋转操作来保持红黑树的平衡
extern void rb_insert_color(struct rb_node *, struct rb_root *);
// 在红黑树中删除节点后，通过调整颜色和节点的连接关系来保持红黑树的平衡 
extern void rb_erase(struct rb_node *, struct rb_root *);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(struct rb_node *);
extern struct rb_node *rb_prev(struct rb_node *);
extern struct rb_node *rb_first(struct rb_root *);
extern struct rb_node *rb_last(struct rb_root *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */

// 将 victim 节点替换为 new 节点
extern void rb_replace_node(struct rb_node *victim, struct rb_node *new, 
			    struct rb_root *root);
// 将节点 node 插入到 parent节点的自节点，并更新相应的指针
static inline void rb_link_node(struct rb_node * node, struct rb_node * parent,
				struct rb_node ** rb_link)
{
	node->rb_parent_color = (unsigned long )parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}
#endif