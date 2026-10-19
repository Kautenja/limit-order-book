// A binary search tree data structure.
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

#ifndef LOB_BINARY_SEARCH_TREE_HPP_
#define LOB_BINARY_SEARCH_TREE_HPP_

/// @brief Components of the limit order book.
namespace LOB {

/// @brief Intrusive binary search tree nodes and pointer operations.
namespace BST {

/// @brief An intrusive binary search tree node with a comparable key.
///
/// @tparam Key the comparable key type
/// @details Links borrow caller-owned nodes. The tree does not balance itself.
///
template<typename Key>
struct Node {
    /// @brief The comparable key for the node.
    Key key;
    /// @brief The parent node (nullptr indicates the root node).
    Node* parent;
    /// @brief The left child, or nullptr.
    Node* left;
    /// @brief The right child, or nullptr.
    Node* right;

    /// @brief Create a detached node with a value-initialized key.
    Node() :
        key(),
        parent(nullptr),
        left(nullptr),
        right(nullptr) { }

    /// @brief Create a detached node with the given key.
    ///
    /// @param key_ the key to create the node with
    ///
    explicit Node(Key key_) :
        key(key_),
        parent(nullptr),
        left(nullptr),
        right(nullptr) { }
};

/// @brief Insert a detached caller-owned node in O(H) tree height.
///
/// @tparam Key the comparable key type
/// @param root the root node of the tree to add the node to
/// @param node the node to add to the tree
/// @pre root points to an empty tree or its root; node is non-null and detached
/// @details Equal keys follow the right branch. No nodes are allocated.
///
template<typename Key>
void insert(Node<Key>** root, Node<Key>* node) {
    if (*root == nullptr) {  // root is null, set pointer
        *root = node;
        return;
    }
    auto current = *root;
    while (current != nullptr) {  // find the bottom of the tree
        if (node->key < current->key) {  // node key is less, left child
            if (current->left == nullptr) {  // no left child, add one
                current->left = node;
                node->parent = current;
                return;
            }
            // move to the left node
            current = current->left;
        } else {  // node key is greater, right child
            if (current->right == nullptr) {  // no right child, add one
                current->right = node;
                node->parent = current;
                return;
            }
            // move to the right node
            current = current->right;
        }
    }
}

/// @brief Unlink a caller-owned node in O(H) tree height without deleting it.
///
/// @tparam Key the comparable key type
/// @param root a pointer to the root node of the tree to remove the node from
/// @param node the node to remove from the tree
/// @pre root points to the tree's root and node belongs to that tree
/// @details The removed node's own links are not reset.
///
template<typename Key>
void remove(Node<Key>** root, Node<Key>* node) {
    // check for children
    bool no_left = node->left == nullptr;
    bool no_right = node->right == nullptr;
    if (no_left && no_right) {  // leaf node
        if (*root == node)  // root node
            *root = nullptr;
        else if (node->parent->left == node)
            node->parent->left = nullptr;
        else
            node->parent->right = nullptr;
    } else if (no_right) {  // has sole left child
        node->left->parent = node->parent;
        if (*root == node)  // root node
            *root = node->left;
        else if (node->parent->left == node)  // left child of parent
            node->parent->left = node->left;
        else  // right child of parent
            node->parent->right = node->left;
    } else if (no_left) {  // has sole right child
        node->right->parent = node->parent;
        if (*root == node)  // root node
            *root = node->right;
        else if (node->parent->left == node)  // left child of parent
            node->parent->left = node->right;
        else  // right child of parent
            node->parent->right = node->right;
    } else {  // left and right child
        if (node->right->left == nullptr) {  // right child has no left branch
            // move the right child to the removed node's position
            node->right->parent = node->parent;
            node->right->left = node->left;
            node->left->parent = node->right;
            if (*root == node)  // removing the root node, update root pointer
                *root = node->right;
            else if (node->parent->left == node)  // left child of parent
                node->parent->left = node->right;
            else  // right child of parent
                node->parent->right = node->right;
        } else {  // right child has left branch
            // find min node in right child
            auto min_node = min(node->right);
            if (min_node->right != nullptr) {  // node has right child
                min_node->parent->left = min_node->right;
                min_node->right->parent = min_node->parent;
            } else if (min_node != node->right) {  // node has no right child
                min_node->parent->left = nullptr;
            }
            // move the min node to the removed node's position
            min_node->parent = node->parent;
            min_node->left = node->left;
            min_node->left->parent = min_node;
            min_node->right = node->right;
            min_node->right->parent = min_node;
            if (*root == node)  // removing the root node, update root pointer
                *root = min_node;
            else if (node->parent->left == node)  // left child of parent
                node->parent->left = min_node;
            else  // right child of parent
                node->parent->right = min_node;
        }
    }
}

/// @brief Return the maximum node of a nonempty subtree in O(H) tree height.
///
/// @tparam Key the comparable key type
/// @param root the root node of the tree to search
/// @returns a borrowed pointer to the node with the largest key
/// @pre root is non-null
///
template<typename Key>
inline Node<Key>* max(Node<Key>* root) {
    while (root->right != nullptr)  // iterate over right side
        root = root->right;
    return root;
}

/// @brief Return the minimum node of a nonempty subtree in O(H) tree height.
///
/// @tparam Key the comparable key type
/// @param root the root node of the tree to search
/// @returns a borrowed pointer to the node with the smallest key
/// @pre root is non-null
///
template<typename Key>
inline Node<Key>* min(Node<Key>* root) {
    while (root->left != nullptr)  // iterate over left side
        root = root->left;
    return root;
}

}  // namespace BST
}  // namespace LOB

#endif  // LOB_BINARY_SEARCH_TREE_HPP_
