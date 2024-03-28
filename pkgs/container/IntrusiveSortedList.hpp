/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

/**
 * @file IntrusiveSortedList.hpp
 *
 * An intrusive linked list where nodes are sorted on insertion.
 */

#pragma once

#include <stdlib.h>

template <class T>
class IntrusiveSortedListNode {
public:
    void setSortedSibling(T sibling) {
        _sorted_list_node_sibling = sibling;
    }
    const T getSortedSibling() const {
        return _sorted_list_node_sibling;
    }

protected:
    T _sorted_list_node_sibling{nullptr};
};

template <class T>
class IntrusiveSortedList {
public:
    void add(T newNode) {
        if (_head == nullptr) {
            // list is empty, add as head
            _head = newNode;
            return;

        } else {
            if (*newNode <= *_head) {
                newNode->setSortedSibling(_head);
                _head = newNode;
                return;
            }

            // find last node and add to end
            T node = _head;

            while (node != nullptr && node->getSortedSibling() != nullptr) {
                if (*newNode <= *node->getSortedSibling()) {
                    // insert newNode
                    newNode->setSortedSibling(node->getSortedSibling());
                    node->setSortedSibling(newNode);
                    return;
                }

                node = node->getSortedSibling();
            }

            // reached the end, add
            node->setSortedSibling(newNode);
        }
    }

    bool remove(T removeNode) {
        if (removeNode == nullptr) {
            return false;
        }

        // base case
        if (removeNode == _head) {
            if (_head != nullptr) {
                _head = _head->getSortedSibling();
            }

            removeNode->setSortedSibling(nullptr);

            return true;
        }

        for (T node = _head; node != nullptr; node = node->getSortedSibling()) {
            // is sibling the node to remove?
            if (node->getSortedSibling() == removeNode) {
                // replace sibling
                if (node->getSortedSibling() != nullptr) {
                    node->setSortedSibling(node->getSortedSibling()->getSortedSibling());

                } else {
                    node->setSortedSibling(nullptr);
                }

                removeNode->setSortedSibling(nullptr);

                return true;
            }
        }

        return false;
    }

    struct Iterator {
        T node;
        explicit Iterator(T v) :
            node(v) {
        }

        operator T() const {
            return node;
        }
        operator T &() {
            return node;
        }
        const T &operator*() const {
            return node;
        }
        Iterator &operator++() {
            if (node) {
                node = node->getSortedSibling();
            }

            return *this;
        }
    };

    Iterator begin() {
        return Iterator(_head);
    }
    Iterator end() {
        return Iterator(nullptr);
    }

    bool empty() const {
        return _head == nullptr;
    }

    size_t size() const {
        size_t sz = 0;

        for (T node = _head; node != nullptr; node = node->getSortedSibling()) {
            sz++;
        }

        return sz;
    }

    void deleteNode(T node) {
        if (remove(node)) {
            // only delete if node was successfully removed
            delete node;
        }
    }

    void clear() {
        T node = _head;

        while (node != nullptr) {
            T next = node->getSortedSibling();
            delete node;
            node = next;
        }

        _head = nullptr;
    }

protected:
    T _head{nullptr};
};
