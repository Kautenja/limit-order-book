// A doubly linked list data structure.
//
// Copyright (c) 2019 Christian Kauten
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

namespace DLL {

/// @brief An intrusive list node whose links borrow caller-owned nodes.
struct Node {
    /// @brief The previous item in the doubly linked list, or nullptr.
    Node* prev;
    /// @brief The next item in the doubly linked list, or nullptr.
    Node* next;
    /// @brief Initialize a detached doubly linked list node.
    Node() : prev(nullptr), next(nullptr) { }
};

/// @brief Append a detached caller-owned node by traversing the list in O(N).
///
/// @param head the head node to push onto
/// @param node the node to push onto the head node
/// @pre head is a valid pointer and node is non-null and detached
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

/// @brief Append a detached caller-owned node through a known tail in O(1).
///
/// @param tail the tail node to append onto
/// @param node the node to append to the tail node
/// @pre tail points to a nonempty list's tail; node is non-null and detached
///
inline void append(Node** tail, Node* node) {
    (*tail)->next = node;
    node->prev = *tail;
    *tail = node;
}

/// @brief Unlink a node and clear its links in O(1), without deleting it.
///
/// @param head the head of the list to update if node is the head
/// @param tail the tail of the list to update if node is the tail
/// @param node the node to remove from the doubly linked list
/// @pre head and tail point to the list endpoints and node belongs to the list
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

/// @brief Return whether the doubly linked list is empty in O(1).
///
/// @param head the head of the doubly linked list
/// @returns true if the list is empty, false otherwise
///
inline bool is_empty(Node* head) { return head == nullptr; }

/// @brief Count the list's nodes in O(N).
///
/// @param head the head of the list to get the size of
/// @returns the number of elements in the list
/// @pre the number of nodes fits in int
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
