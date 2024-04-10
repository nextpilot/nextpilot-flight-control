/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/
#pragma once

#include <stdlib.h>

// namespace nextpilot::container {

template <class T>
class IntrusiveQueue {
public:
    bool empty() const {
        return _head == nullptr;
    }

    T front() const {
        return _head;
    }

    T back() const {
        return _tail;
    }

    size_t size() const {
        size_t sz = 0;

        for (auto node = front(); node != nullptr; node = node->next_intrusive_queue_node()) {
            sz++;
        }

        return sz;
    }

    bool push(T newNode) {
        // error, node already queued or already inserted
        if ((newNode->next_intrusive_queue_node() != nullptr) || (newNode == _tail)) {
            return false;
        }

        if (_head == nullptr) {
            _head = newNode;
        }

        if (_tail != nullptr) {
            _tail->set_next_intrusive_queue_node(newNode);
        }

        _tail = newNode;

        return true;
    }

    T pop() {
        T ret = _head;

        if (!empty()) {
            if (_head != _tail) {
                _head = _head->next_intrusive_queue_node();

            } else {
                // only one item left
                _head = nullptr;
                _tail = nullptr;
            }

            // clear next in popped (in might be re-inserted later)
            ret->set_next_intrusive_queue_node(nullptr);
        }

        return ret;
    }

    bool remove(T removeNode) {
        // base case
        if (removeNode == _head) {
            if (_head->next_intrusive_queue_node() != nullptr) {
                _head = _head->next_intrusive_queue_node();
                removeNode->set_next_intrusive_queue_node(nullptr);

            } else {
                _head = nullptr;
                _tail = nullptr;
            }

            return true;
        }

        for (T node = _head; node != nullptr; node = node->next_intrusive_queue_node()) {
            // is sibling the node to remove?
            if (node->next_intrusive_queue_node() == removeNode) {
                if (removeNode == _tail) {
                    _tail = node;
                }

                // replace sibling
                node->set_next_intrusive_queue_node(removeNode->next_intrusive_queue_node());
                removeNode->set_next_intrusive_queue_node(nullptr);
                return true;
            }
        }

        return false;
    }

    struct Iterator {
        T node;
        Iterator(T v) :
            node(v) {
        }

        operator T() const {
            return node;
        }
        operator T &() {
            return node;
        }
        T operator*() const {
            return node;
        }
        Iterator &operator++() {
            if (node) {
                node = node->next_intrusive_queue_node();
            };

            return *this;
        }
    };

    Iterator begin() {
        return Iterator(_head);
    }
    Iterator end() {
        return Iterator(nullptr);
    }

private:
    T _head{nullptr};
    T _tail{nullptr};
};

template <class T>
class IntrusiveQueueNode {
private:
    friend IntrusiveQueue<T>;

    T next_intrusive_queue_node() const {
        return _next_intrusive_queue_node;
    }
    void set_next_intrusive_queue_node(T new_next) {
        _next_intrusive_queue_node = new_next;
    }

    T _next_intrusive_queue_node{nullptr};
};

// } // namespace nextpilot::container
