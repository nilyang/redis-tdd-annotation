#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>

#include"hello_adlist.h"

/*
 * 创建一个新的空列表
 *
 * Create a new list. The created list can be freed with
 * AlFreeList(), but private value of every node need to be freed
 * by the user before to call AlFreeList().
 *
 * On error, NULL is returned. Otherwise the pointer to the new list.
 */
list *listCreate(void)
{
    list *newList  = (list *)malloc(sizeof(list));
    if(newList == NULL) return NULL;

    newList->head  = NULL;
    newList->tail = NULL;
    newList->len = 0; 

    newList->dup = NULL;
    newList->free = NULL;
    newList->match = NULL;
    return newList;
}

/*
 * 释放list空间，不能失败
 *
 * Free the whole list.
 *
 * This function can't fail. 
 */
void listRelease(list *list)
{
    unsigned long len = list->len;
    listNode *last = NULL, *tail=NULL;
    //从后往前删
    tail = list->tail;
    while(tail && len--){
        last = tail->prev;
        if (last != NULL) last->next = NULL;// 第1个结点
        if (list->free) list->free(tail->value);
        zfree(tail);
        tail = last;
    }

    zfree(list);
}

/**
 * 新建节点插入到list的head处（并传入结点value地址），成功返回链表指针,如果失败返回NULL
 *
 * Add a new node to the list, to head, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
list *listAddNodeHead(list *list, void *value)
{
    listNode *node = (listNode *)malloc(sizeof(listNode));
    if (node == NULL) return NULL;
    
    if (list->len == 0){
        list->head = node; 
        list->tail = node;
        node->prev = NULL;
        node->next = NULL;
    }else{
        //从head处插入一个节点
        node->prev = NULL;
        node->next = list->head;
        list->head->prev = node;
        // head指针往前移动
        list->head = node;
    }
    node->value = value;
    list->len++;

    return list;
}

/**
 * 新建结点插入到链表尾部，成功返回链表指针，失败返回 NULL
 * 
 * Add a new node to the list, to tail, containing the specified 'value'
 * pointer as value.
 *
 * On error, NULL is returned and no operation is performed (i.e. the
 * list remains unaltered).
 * On success the 'list' pointer you pass to the function is returned. */
list *listAddNodeTail(list *list, void *value)
{
    listNode *node = (listNode*)malloc(sizeof(listNode));
    if(node==NULL) return NULL;

    node->value = value;
    if(list->len == 0){
        list->tail = list->head = node;
        node->prev = node->next = NULL;
    } else {
        //从链表尾部处插入一个节点
        node->next = NULL;
        node->prev = list->tail;
        list->tail->next = node;
        // tail指针往后移动
        list->tail = node;
    }
    
    list->len++;

    return list;
}
// list *listInsertNode(list *list, listNode *old_node, void *value, int after);
// void listDelNode(list *list, listNode *node);
// listIter *listGetIterator(list *list, int direction);
// listNode *listNext(listIter *iter);
// void listReleaseIterator(listIter *iter);
// list *listDup(list *orig);
// listNode *listSearchKey(list *list, void *key);
// listNode *listIndex(list *list, long index);
// void listRewind(list *list, listIter *li);
// void listRewindTail(list *list, listIter *li);
// void listRotate(list *list);

// gcc -D ADLIST_TEST_MAIN  -Wall  -ggdb hello_adlist.c -o adlist &&
// valgrind --vgdb=yes --leak-check=full --track-origins=yes ./adlist
#ifdef ADLIST_TEST_MAIN
#include<stdio.h>
#include<limits.h>
#include"testhelp_ext.h"
#include"hello_sds.h"
#include"hello_sds.c"

int sdsfree_wrapper(void *value)
{
    sdsfree((sds)value);
    return 1;
}

int main()
{
    //1. listCreate test
    list *pList = listCreate();
    test_cond_ext("listCreate() pList",
        pList!=NULL 
        && pList->len == 0 
        && pList->head == NULL
        && pList->head == pList->tail
        && pList->dup == NULL 
        && pList->free == NULL
        && pList->match == NULL);
    //2. listRelease test
    // pList->len = 100;
    listRelease(pList);
    //printf("%ld\n", pList->len); //为什么这里还能打印100 ?
    
    //3 .listAddNodeHead test
    pList = listCreate();
    pList->free = sdsfree_wrapper; // 释放sds字符串
    pList = listAddNodeHead(pList, sdsnew("hello world"));
    // printf("%s\n",(char *) pList->head->value);
    test_cond_ext("listAddNodeHead() test",
        memcmp( pList->head->value,"hello world\0", 12) == 0
        && pList->tail == pList->head 
        && pList->len == 1)
    listRelease(pList);

    
    //4. listAddNodeTail test
    pList = listCreate();
    pList->free = sdsfree_wrapper;
    pList = listAddNodeTail(pList, sdsnew("Hello world"));
    test_cond_ext("listAddNodeTail() test",
        memcmp( pList->tail->value,"Hello world\0", 12) == 0 
        && pList->tail == pList->head 
        && pList->len == 1)
    listRelease(pList);
    return 0;
}
#endif