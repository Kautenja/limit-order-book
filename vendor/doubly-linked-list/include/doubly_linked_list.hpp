// A doubly linked list data structure.
// Copyright 2019 Christian Kauten
//

#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

namespace DLL {

/// A generic type key binary search tree node
struct Node {
    /// the previous item in the doubly linked list
    Node* prev;
    /// the next item in the doubly linked list
    Node* next;
    /// Initialize a new doubly linked list.
    Node() : prev(nullptr), next(nullptr) { }
};

/// Push a node onto a double linked list.
///
/// @param head the head node to push onto
/// @param node the node to push onto the head node
///
inline void push_back(Node** head, Node* node) {
    if (*head == nullptr) {  // linked list is empty
        *head = node;
        return;
    }
    // find the end of the list
    auto last = *head;
    while (last->next != nullptr)
        last = last->next;
    // attach the node
    last->next = node;
    node->prev = last;
}

/// Push a node onto a double linked list.
///
/// @param tail the tail node to append onto
/// @param node the node to append to the tail node
///
inline void append(Node** tail, Node* node) {
    (*tail)->next = node;
    node->prev = *tail;
    *tail = node;
}

/// Remove the given node from the doubly linked list
///
/// @param head the head of the list to update if node is the head
/// @param tail the tail of the list to update if node is the tail
/// @param node the node to remove from the doubly linked list
///
inline void remove(Node** head, Node** tail, Node* node) {
    if (node->prev != nullptr)  // has previous node
        node->prev->next = node->next;
    if (node->next != nullptr)  // has next node
        node->next->prev = node->prev;
    if (*head == node)  // removing the head node
        *head = node->next;
    if (*tail == node)  // removing the tail node
        *tail = node->prev;
    node->next = nullptr;
    node->prev = nullptr;
}

/// Return true if the double linked list is empty.
///
/// @param head the head of the doubly linked list
/// @returns true if the list is empty, false otherwise
///
inline bool is_empty(Node* head) { return head == nullptr; }

/// Return the number of elements in the given dynamic linked list
///
/// @param head the head of the list to get the size of
/// @returns the number of elements in the list
///
inline int count(Node* head) {
    int count = 0;
    while (head != nullptr) {  // head is not null
        head = head->next;
        count++;
    }
    return count;
}

}  // namespace DLL

#endif  // DOUBLY_LINKED_LIST_HPP
