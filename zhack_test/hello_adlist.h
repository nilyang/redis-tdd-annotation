#ifndef __HELLO_ADLIST_H
#define __HELLO_PRIME_H

//Node, List and Iterator are the only data structures used currently.
//节点/列表/迭代器是当前用到的所有的数据结构

//双端链表结点
typedef struct listNode {
    // 前置结点
    struct listNode *prev;
    // 后置结点
    struct listNode *next;
    // 结点值
    void *value;
} listNode;

// 双端链表迭代器
typedef struct listIter {
    // 当前迭代到的结点
    listNode *next;

    // 迭代方向
    int direction;
} listIter;

// 双端链表结构
typedef struct list {
    // 表头
    listNode *head;
    
    // 表尾
    listNode *tail;

    // 结点值复制函数
    void *(*dup)(void *ptr);

    // 节点值释放函数
    int (*free)(void *ptr);

    // 结点值对比函数
    int (*match)(void *ptr, void *key);

    // 链表所包含的结点数
    unsigned long len;
}
#endif