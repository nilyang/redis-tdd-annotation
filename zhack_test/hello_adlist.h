#ifndef __HELLO_ADLIST_H
#define __HELLO_PRIME_H

#include"hello_zmalloc.h"

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

    // 结点值匹配函数
    int (*match)(void *ptr, void *key);

    // 链表所包含的结点数
    unsigned long len;
} list;

// 函数宏
#define listLength(l) ((l)->len)
#define listFirst(l) ((l)->head)
#define listLast(l) ((l)->tail)
#define listPrevNode(n) ((n)->prev)
#define listNextNode(n) ((n)->next)
#define listNodeValue(n) ((n)->value)

#define listSetDupMethod(l,m) ((l)->dup = (m))
#define listSetFreeMethod(l,m) ((l)->free = (m))
#define listSetMatchMethod(l,m) ((l)->match = (m))

#define listGetDupMethod(l) ((l)->dup)
#define listGetFree(l) ((l)->free)
#define listGetMatchMethod(l) ((l)->match)

// 原型
list *listCreate(void);
void listRelease(list *list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
void listDelNode(list *list, listNode *node);
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
void listReleaseIterator(listIter *iter);
list *listDup(list *orig);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);

/* Directions for iterators */
#define AL_START_HEAD 0
#define AL_START_TAIL 1

#endif