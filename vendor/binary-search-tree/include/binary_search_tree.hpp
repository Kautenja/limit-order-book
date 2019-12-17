// A binary search tree data structure.
// Copyright 2019 Christian Kauten
//

#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP

namespace BST {

/// A generic type key binary search tree node
template<typename Key>
struct Node {
    /// the comparable key for the node
    Key key;
    /// the parent node (nullptr indicates root node)
    Node* parent;
    /// the left child
    Node* left;
    /// the right child
    Node* right;

    /// Create a new binary search tree node with 0 key.
    Node() :
        key(),
        parent(nullptr),
        left(nullptr),
        right(nullptr) { }

    /// Create a new binary search tree node with given key.
    ///
    /// @param key_ the key to create the node with
    ///
    explicit Node(Key key_) :
        key(key_),
        parent(nullptr),
        left(nullptr),
        right(nullptr) { }
};

/// Insert a node into a tree.
///
/// @param root the root node of the tree to add the node to
/// @param node the node to add to the tree
/// @undefined root is non-root node, i.e., root.parent != nullptr
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

/// Remove a node from a tree.
///
/// @param root a pointer to the root node of the tree to remove the node from
/// @param node the node to remove from the tree
/// @undefined root is non-root node, i.e., root.parent != nullptr
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

/// Return the maximum node of a binary search tree.
///
/// @param root the root node of the tree to search
/// @undefined root is non-root node, i.e., root.parent != nullptr
///
template<typename Key>
inline Node<Key>* max(Node<Key>* root) {
    while (root->right != nullptr)  // iterate over right side
        root = root->right;
    return root;
}

/// Return the minimum node of a binary search tree.
///
/// @param root the root node of the tree to search
/// @undefined root is non-root node, i.e., root.parent != nullptr
///
template<typename Key>
inline Node<Key>* min(Node<Key>* root) {
    while (root->left != nullptr)  // iterate over left side
        root = root->left;
    return root;
}

}  // namespace BST

#endif  // BINARY_SEARCH_TREE_HPP
