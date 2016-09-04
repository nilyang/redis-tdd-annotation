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
    listNode *node = (listNode*)zmalloc(sizeof(listNode));
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

/**
 * 从给定链表元素后面或前面插新结点
 */
list *listInsertNode(list *list, listNode *old_node, void *value, int after)
{
    listNode *node;
    if((node=(listNode*)zmalloc(sizeof(listNode))) == NULL)
        return NULL;
    //0. value赋值
    node->value = value;

    //1. 首先将结点插入到链表中
    if (after) {
        //在结点old_node后面插入
        node->prev = old_node;
        node->next = old_node->next;
        if (list->tail == old_node) {
            list->tail = node;
        }
    } else {
        //在结点old_node前面插入
        node->next = old_node;
        node->prev = old_node->prev;
        if (list->head == old_node) {
            list->head = node;
        }
    }

    //2. 然后将结点前后的双向链接打通
    if (node->prev != NULL) {
        node->prev->next = node;//让前置结点的next指向当前结点
    }
    if (node->next != NULL) {
        node->next->prev = node;//让后置结点的prev指向当前结点
    }

    list->len++;

    return list;
}


/* 
 * 删除指定节点
 *
 * Remove the specified node from the specified list.
 * It's up to the caller to free the private value of the node.
 *
 * This function can't fail. 
 */
void listDelNode(list *list, listNode *node)
{
    // 如何报错？
    if (list==NULL || list->head == NULL || node == NULL)
        return;

    // 剪掉node在链中和其他节点的关联

    // 处理前置节点的next字段
    if (node->prev)
        node->prev->next = node->next;
    else
        list->head = node->next;
    // 处理后置节点的prev字段
    if (node->next)
        node->next->prev = node->prev;
    else
        list->tail = node->prev;
    
    // 释放资源
    if (list->free) list->free(node->value);
    zfree(node);
    // 节点数减一
    list->len--;
}

/**
 * 链表遍历迭代器初始化工具，返回迭代器iter， direction表示遍历方向，0：正向遍历，1：反向遍历
 * 注：被 listNext() 调用时，用于返回下一个元素，不可失败！
 * Returns a list iterator 'iter'. After the initialization every
 * call to listNext() will return the next element of the list.
 *
 * This function can't fail
 */
listIter *listGetIterator(list *list, int direction)
{
    listIter *iter = (listIter*)zmalloc(sizeof(listIter));
    if (iter == NULL) return NULL;

    iter->next = direction == AL_START_HEAD ? list->head:list->tail;
    iter->direction = direction;
    return iter;
}

/**
 * 通过迭代器，迭代返回下一个元素
 *
 *
 * Return the next element of an iterator.
 * It's valid to remove the currently returned element using
 * listDelNode(), but not to remove other elements.
 *
 * The function returns a pointer to the next element of the list,
 * or NULL if there are no more elements, so the classical usage patter
 * is:
 *
 * iter = listGetIterator(list,<direction>);
 * while ((node = listNext(iter)) != NULL) {
 *     doSomethingWith(listNodeValue(node));
 * }
 *
 */
listNode *listNext(listIter *iter)
{
    if (iter == NULL) return NULL;

    listNode *node;

    node = iter->next;

    if (node == NULL) return NULL;

    if (iter->direction == AL_START_HEAD) {
        iter->next = node->next;
    } else {
        iter->next = node->prev;
    }
    return node;
}


/**
 * 释放迭代器指针
 */
void listReleaseIterator(listIter *iter)
{
    if (iter == NULL) return;
    zfree(iter);
}

/* 
 * 复制整个链表, 内存不足时返回 NULL，成功饭返回副本。
 * 
 * 如果链表结构的 `dup` （list->dup）函数设置了拷贝函数(. listSetDupMethod(m) ，则使用拷贝函数复制结点值。
 * 否则，新节点和原结点共同使用同一指针。
 * 
 * 无论结果成功或失败，都不修改原来的链表。
 *
 * Duplicate the whole list. On out of memory NULL is returned.
 * On success a copy of the original list is returned.
 *
 * The 'Dup' method set with listSetDupMethod() function is used
 * to copy the node value. Otherwise the same pointer value of
 * the original node is used as value of the copied node.
 *
 * The original list both on success or error is never modified. */
list *listDup(list *orig)
{
    list *copy;
    listIter *iter;
    listNode *node;
    copy = listCreate();
    if (copy == NULL) return NULL;

    copy->dup = orig->dup;
    copy->free = orig->free;
    copy->match = orig->match;
    
    iter = listGetIterator(orig, AL_START_HEAD);
    while((node = listNext(iter)) != NULL) {
        void *value;
        if (copy->dup) {
            value = copy->dup(node->value);
            // 拷贝失败，返回 NULL
            if (value == NULL) {
                listRelease(copy);
                listReleaseIterator(iter);
                return NULL;
            }
        } else {
            //没有 指定拷贝函数，则共享数据指针
            value = node->value;
        }

        //添加结点到链表尾部
        if (listAddNodeTail(copy, value) == NULL){
            listRelease(copy);
            listReleaseIterator(iter);
            return NULL;
        }
    }

    listReleaseIterator(iter);

    return copy;
}

/**
 * 链表的查找函数，在链表的节点中查找含有给定key值的结点。
 * 匹配算法使用'list->match'函数（如果设置了match函数的话）。
 * 如果没有指定'match'函数，则所有结点的比较都使用`value`指针来和key指针比较
 * 
 * 如果成功，返回第一个匹配节点的指针，否则返回NULL。
 *
 * Search the list for a node matching a given key.
 * The match is performed using the 'match' method
 * set with listSetMatchMethod(). If no 'match' method
 * is set, the 'value' pointer of every node is directly
 * compared with the 'key' pointer.
 *
 * On success the first matching node pointer is returned
 * (search starts from head). If no matching node exists
 * NULL is returned.
 */
listNode *listSearchKey(list *list, void *key)
{
    listNode *node;
    listIter *iter = listGetIterator(list,AL_START_HEAD);
    if (iter == NULL) return NULL;

    while((node = listNext(iter)) != NULL) {
        // 使用 match 自定义比较函数来判断
        if (list->match && list->match(node->value, key) == 0) {
            // printf("Find-1->%s\n", (char *)node->value);
            break;
        } else if(node->value == key) {
            // printf("Find-2->%s\n", (char *)node->value);
            break;
        } 
        // else{
        //     printf("not found\n");
        // }
    }

    listReleaseIterator(iter);

    return node;
}
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

void* sdsdup_wapper(void *ptr)
{
    return sdsdup((const sds)ptr);
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

    //5. listInsertNode test
    pList = listCreate();
    pList->free = sdsfree_wrapper;
    pList = listAddNodeHead(pList,sdsnew("Hello"));
    pList = listAddNodeTail(pList,sdsnew("world"));
    //在节点后面插入"goodbyte"
    pList = listInsertNode(pList,pList->head,sdsnew("goodbyte"), 1);
    test_cond_ext("listInsertNode() test insert node after head",
        pList->len==3 
        && pList->head->prev == NULL
        && pList->tail->next == NULL
        && memcmp(pList->head->next->value,"goodbyte\0", 9) == 0
        && memcmp(pList->tail->prev->value,"goodbyte\0", 9) == 0)
    //在head前面插入结点
    pList = listInsertNode(pList,pList->head,sdsnew("begin"), 0);
    test_cond_ext("listInsertNode() test insert node before head",
        pList->len == 4
        && pList->head->prev == NULL
        && pList->tail->next == NULL
        && memcmp(pList->head->value,"begin\0", 6) ==0
        && memcmp(pList->head->next->value,"Hello\0", 6) ==0 )

    //6. listDelNode test
    //删除节点测试
    listDelNode(pList,pList->head);
    test_cond_ext("listDelNode() test del head",
        pList->len == 3
        && pList->head->prev == NULL
        && memcmp(pList->head->value,"Hello\0", 6) ==0 )

    //三次删除
    listDelNode(pList,pList->head);
    listDelNode(pList,pList->head);
    listDelNode(pList,pList->tail);

    test_cond_ext("listDelNode() test del all heads one by one",
        pList->len == 0
        && pList->head == pList->tail 
        && pList->head == NULL)
    listRelease(pList);

    //7. listDup() test
    pList = listCreate();
    pList->free = sdsfree_wrapper;
    pList->dup = sdsdup_wapper;
    pList = listAddNodeHead(pList,sdsnew("Hello"));
    pList = listAddNodeTail(pList,sdsnew("world"));
    pList = listInsertNode(pList,pList->head,sdsnew("goodbyte"), 1);
    list *pList2 = listDup(pList);
    test_cond_ext("listDup() test",
        pList->len == pList2->len 
        && pList != pList2
        && pList->match == pList2->match
        && pList->dup == pList2->dup
        && pList->free == pList2->free
        && pList->match == pList2->match
        && memcmp(pList2->head->next->value, "goodbyte\0", 9) == 0
        && memcmp(pList2->head->value, "Hello\0", 6) == 0
        && memcmp(pList2->tail->value, "world\0", 6) == 0
        )
    // printf("%s\n", (char*)pList2->head->next->value);
    // 8. listSearchKey(list, key) test
    {
        printf("%s,%s,%s\n",
             (char*) pList->head->value
            ,(char*) pList->head->next->value
            ,(char*) pList->head->next->next->value
            );
        
        listNode* key = pList->head->next;

        listNode* node = listSearchKey(pList, key->value);
        if(node)
            printf("%s,%s\n",(char*) key->value, (char*)node->value);
    }
    listRelease(pList);
    listRelease(pList2);
    return 0;
}
#endif