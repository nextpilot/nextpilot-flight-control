# 链表操作

## 单向链表

|函数|功能||
|---|---|---|
|sq_addfirst|将节点添加到链表头部||
|sq_addlast|||
|sq_addafter|将节点添加到指定节点后面||
|sq_count|统计链表中元素个数||
|sq_cat|连接两个链表|
|sq_rem|从链表删除节点|
|sq_remfirst|移除第一个节点|
|sq_remlast|移除最后一个节点|
|sq_remafter||

### 创建链表
```c
#include "queue.h"

// 先定义一个节点结构体
struct mynode{
    // 第一个元素必须是sq_entry_s
    struct sq_entry_s link;
    // 其他元素自定义哈
    double  lat;
    double lon;
    float alt;
}

// 创建一个链表
struct sq_queue_s mylist;
// 初始化链表，其实就是将head和tail赋NULL
sq_init(&mylist)

// 创建节点1
struct mynode node1;
// 将节点添加到链表
sq_addlast(&node1, &mylist);

// 创建节点2
struct mynode node2
sq_addfirst(&node2, &mylist);

// while遍历链表
// 取出第一个节点，相当于mylist.head
mynode* node = (mynode*)sq_peek(&mylist);
// 逐节点遍历
while (node != NULL){
    // do something

    // 下一个节点
    node = (mynode*)sq_next(&node->link);
}

// for遍历链表
for ( mynode* node = (mynode*)sq_peek(&mylist); node != NULL; node = (mynode*)sq_next(&node->link) ) {
    // do something
}


```