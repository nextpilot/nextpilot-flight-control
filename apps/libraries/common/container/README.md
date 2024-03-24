
# C++ 容器模板类

|类|接口|作用|
|---|---|---|
|LockGuard|无|保护锁，构造函数 lock mutex，析构函数unlock mutex，LockGuard作用范围之内的代码同一时间只能被一个线程访问|
||||
|Array|push_back、remove、at|定长数组|
|BlockingQueue|push、pop|带读写保护、定长度的**环形数组**，不要被名字欺骗了|
||||
|List|add、remove|侵入式单向链表，**add时不会判断node是否已经在list中**|
|IntrusiveSortedList|add、remove|侵入式排序单向链表，**add时不会判断node是否已经在list中**|
|BlockingList|add、remove|带读写保护的侵入式排序单向链表，**add时不会判断node是否已经在list中**|
||||
|IntrusiveQueue|push、pop、remove|侵入式双向链表，表有head和tail指针，普通节点只有next没有perv，**没有add函数，push时会判断node是否已在list中**，注意该链表只用于work_queue中item的管理|

## Array.hpp

定长度数组类，提供push_back、remove、at等简单操作函数。

```c
template <class T, size_t N>
class Array
{
public:
    bool push_back(const T &x);
    void remove(unsigned idx);
    void erase(T *item);
    T &operator[](size_t n);
    const T &operator[](size_t n) const;
    T &at(size_t n);
    const T &at(size_t n) const;
    size_t size() const;
    size_t max_size() const;
    size_t capacity() const;
    bool empty() const;
    bool is_overflowed();
    T *begin();
    T *end();
}
```

## List.hpp

单向链表，其中ListNode是节点，List是表

```c++
template<class T>
class ListNode
{
public:

    void setSibling(T sibling) { _list_node_sibling = sibling; }
    const T getSibling() const { return _list_node_sibling; }

protected:

    T _list_node_sibling{nullptr};

};

template<class T>
class List
{
public:

    void add(T newNode);  // 向链
    bool remove(T removeNode);
}
```

## IntrusiveSortedList.hpp

“侵入式”这个术语的来源可以追溯到数据结构和算法中的一个概念，即“对象的内部状态由外部对象管理”。在侵入式链表中，每个节点都包含指向下一个节点的指针，并且链接关系由节点本身维护。因此，在访问链表时，客户端代码必须直接侵入到节点结构体中来获取指向下一个节点的指针。

相比之下，在非侵入式链表中，每个节点只是一个独立的对象，并且链接关系由链表本身维护。因此，客户端代码不需要了解节点的内部结构，而只需调用链表提供的方法来插入、删除或遍历节点。这就是为什么它被称为“非侵入式”链表。

因此，“侵入式”一词指的是客户端代码需要直接侵入到节点结构体中才能访问链表的特定部分。虽然这种做法可能会使代码更复杂和难以维护，但它通常比非侵入式链表提供更高的性能和灵活性。
