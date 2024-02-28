#include "rbtree.h"

// 红黑树中执行左旋
/*
左旋：以某个节点作为支点(旋转节点)，其右子节点变为旋转节点的父节点，右子节点的左子节点变为旋转节点的右子节点，旋转节点的左子节点保持不变。右子节点的左子节点相当于从右子节点上“断开”，重新连接到旋转节点上。
*/
static void __rb_rotate_left(struct rb_node* node, struct rb_node * root)
{
        struct rb_node * right = node->rb_right;
        struct rb_node * parent = rb_parent(node);

        if ((node->rb_right = right->rb_left))
                rb_set_parent(right->rb_left, node);
        right->rb_left = node;
        //...
}

/*
右旋：以某个节点作为支点(旋转节点)，其左子节点变为旋转节点的父节点，左子节点的右子节点变为旋转节点的左子节点，旋转节点的右子节点保持不变。左子节点的右子节点相当于从左子节点上“断开”，重新连接到旋转节点上。
*/