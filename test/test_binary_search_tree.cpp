// Test cases for the binary search tree module.
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

#include <catch2/catch_test_macros.hpp>
#include "binary_search_tree.hpp"

// ---------------------------------------------------------------------------
// MARK: constructor
// ---------------------------------------------------------------------------

TEST_CASE("BST: initialize BST::Node with default constructor") {
    REQUIRE_NOTHROW(BST::Node<double>{});
}

TEST_CASE("BST: BST::Node with default constructor has default key") {
    BST::Node<double> tree;
    REQUIRE(0 == tree.key);
    REQUIRE(nullptr == tree.parent);
    REQUIRE(nullptr == tree.left);
    REQUIRE(nullptr == tree.right);
}

TEST_CASE("BST: initialize BST::Node with key constructor") {
    REQUIRE_NOTHROW(BST::Node<double>{4});
}

TEST_CASE("BST: BST::Node with key constructor has key") {
    auto tree = BST::Node<double>(4);
    REQUIRE(4 == tree.key);
    REQUIRE(nullptr == tree.parent);
    REQUIRE(nullptr == tree.left);
    REQUIRE(nullptr == tree.right);
}

// ---------------------------------------------------------------------------
// MARK: BST::insert
// ---------------------------------------------------------------------------

SCENARIO("BST: insert node with same key") {
    GIVEN("a series of binary search tree nodes") {
        double key = 1;
        BST::Node<double> node1_storage{key};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{key};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{key};
        auto node3 = &node3_storage;
        REQUIRE(BST::max(node1) == node1);
        REQUIRE(BST::min(node1) == node1);
        WHEN("node1 is inserted into nullptr") {
            BST::Node<double>* root = nullptr;
            BST::insert(&root, node1);
            THEN("the nodes point in the correct directions") {
                REQUIRE(root == node1);
            }
        }
        WHEN("node2 is inserted into node1") {
            BST::insert(&node1, node2);
            THEN("the nodes point in the correct directions") {
                REQUIRE(node1->parent == nullptr);
                REQUIRE(node1->left == nullptr);
                REQUIRE(node1->right == node2);
                REQUIRE(node2->parent == node1);
                REQUIRE(node2->left == nullptr);
                REQUIRE(node2->right == nullptr);
                REQUIRE(BST::max(node1) == node2);
                REQUIRE(BST::min(node1) == node1);
            }
        }
        WHEN("node3 is inserted into node1 after node2") {
            BST::insert(&node1, node2);
            BST::insert(&node1, node3);
            THEN("the nodes point in the correct directions") {
                REQUIRE(node1->parent == nullptr);
                REQUIRE(node1->left == nullptr);
                REQUIRE(node1->right == node2);
                REQUIRE(node2->parent == node1);
                REQUIRE(node2->left == nullptr);
                REQUIRE(node2->right == node3);
                REQUIRE(node3->parent == node2);
                REQUIRE(node3->left == nullptr);
                REQUIRE(node3->right == nullptr);
                REQUIRE(BST::max(node1) == node3);
                REQUIRE(BST::min(node1) == node1);
            }
        }
    }
}

SCENARIO("BST: insert node with greater key") {
    GIVEN("a series of binary search tree nodes") {
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        WHEN("node2 is inserted into node1") {
            BST::insert(&node1, node2);
            THEN("the nodes point in the correct directions") {
                REQUIRE(node1->parent == nullptr);
                REQUIRE(node1->left == nullptr);
                REQUIRE(node1->right == node2);
                REQUIRE(node2->parent == node1);
                REQUIRE(node2->left == nullptr);
                REQUIRE(node2->right == nullptr);
                REQUIRE(BST::max(node1) == node2);
                REQUIRE(BST::min(node1) == node1);
            }
        }
        WHEN("node3 is inserted into node1 after node2") {
            BST::insert(&node1, node2);
            BST::insert(&node1, node3);
            THEN("the nodes point in the correct directions") {
                REQUIRE(node1->parent == nullptr);
                REQUIRE(node1->left == nullptr);
                REQUIRE(node1->right == node2);
                REQUIRE(node2->parent == node1);
                REQUIRE(node2->left == nullptr);
                REQUIRE(node2->right == node3);
                REQUIRE(node3->parent == node2);
                REQUIRE(node3->left == nullptr);
                REQUIRE(node3->right == nullptr);
                REQUIRE(BST::max(node1) == node3);
                REQUIRE(BST::min(node1) == node1);
            }
        }
    }
}

SCENARIO("BST: insert node with lesser key") {
    GIVEN("a series of binary search tree nodes") {
        BST::Node<double> node1_storage{3};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{1};
        auto node3 = &node3_storage;
        WHEN("node2 is inserted into node1") {
            BST::insert(&node1, node2);
            THEN("the nodes point in the correct directions") {
                REQUIRE(node1->parent == nullptr);
                REQUIRE(node1->left == node2);
                REQUIRE(node1->right == nullptr);
                REQUIRE(node2->parent == node1);
                REQUIRE(node2->left == nullptr);
                REQUIRE(node2->right == nullptr);
                REQUIRE(BST::max(node1) == node1);
                REQUIRE(BST::min(node1) == node2);
            }
        }
        WHEN("node3 is inserted into node1 after node2") {
            BST::insert(&node1, node2);
            BST::insert(&node1, node3);
            THEN("the nodes point in the correct directions") {
                REQUIRE(node1->parent == nullptr);
                REQUIRE(node1->left == node2);
                REQUIRE(node1->right == nullptr);
                REQUIRE(node2->parent == node1);
                REQUIRE(node2->left == node3);
                REQUIRE(node2->right == nullptr);
                REQUIRE(node3->parent == node2);
                REQUIRE(node3->left == nullptr);
                REQUIRE(node3->right == nullptr);
                REQUIRE(BST::max(node1) == node1);
                REQUIRE(BST::min(node1) == node3);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: BST::remove
// ---------------------------------------------------------------------------

/*
tree shape (single node):
*
*/
SCENARIO("BST: remove from a single node") {
    GIVEN("a tree and a single node") {
        BST::Node<double> node_storage{3};
        auto node = &node_storage;
        auto node_copy = node;
        WHEN("the node is removed") {
            BST::remove(&node, node);
            THEN("the node pointers are correct") {
                REQUIRE(nullptr == node);
            }
            THEN("the removed node is not deleted") {
                REQUIRE(nullptr != node_copy);
                REQUIRE(nullptr == node_copy->parent);
                REQUIRE(nullptr == node_copy->left);
                REQUIRE(nullptr == node_copy->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   2
*/
SCENARIO("BST: remove from a balanced three-node tree") {
    GIVEN("a tree with V shape") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node2);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);
        WHEN("the left node is removed") {
            BST::remove(&node1, node0);
            THEN(R"(
the shape is correct:
1
 \
  2
                )") {
                // removed node should still have pointers
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);
                // tree should be correct
                REQUIRE(nullptr == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);
            }
        }
        WHEN("the right node is removed") {
            BST::remove(&node1, node2);
            THEN(R"(
the shape is correct:
  1
 /
0
                )") {
                // removed node should still have pointers
                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);
                // tree should be correct
                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(nullptr == node1->right);

                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);
            }
        }
        WHEN("the root node is removed") {
            auto note1copy = node1;
            BST::remove(&node1, node1);
            THEN(R"(
the shape is correct:
  2
 /
0
                )") {
                REQUIRE(node1 == node2);
                // removed node should still have pointers
                REQUIRE(nullptr == note1copy->parent);
                REQUIRE(node0 == note1copy->left);
                REQUIRE(node2 == note1copy->right);
                // tree should be correct
                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);
            }
        }
    }
}

/*
tree shape:
0
 \
  1
   \
    2
*/
SCENARIO("BST: remove from a right chain") {
    GIVEN("a tree with right leg shape") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        // create the tree
        BST::insert(&node0, node1);
        BST::insert(&node0, node2);
        // check the tree
        REQUIRE(nullptr == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(node1 == node0->right);

        REQUIRE(node0 == node1->parent);
        REQUIRE(nullptr == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);
        WHEN("the root node is removed") {
            BST::remove(&node0, node0);
            THEN(R"(
the shape is correct:
1
 \
  2
                )") {
                REQUIRE(nullptr == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);
            }
        }
        WHEN("the middle node is removed") {
            BST::remove(&node0, node1);
            THEN(R"(
the shape is correct:
0
 \
  2
                )") {
                REQUIRE(nullptr == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(node2 == node0->right);

                REQUIRE(node0 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);
            }
        }
        WHEN("the leaf node is removed") {
            BST::remove(&node0, node2);
            THEN(R"(
the shape is correct:
0
 \
  1
                )") {
                REQUIRE(nullptr == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(node1 == node0->right);

                REQUIRE(node0 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(nullptr == node1->right);
            }
        }
    }
}

/*
tree shape:
    2
   /
  1
 /
0
*/
SCENARIO("BST: remove from a left chain") {
    GIVEN("a tree with left leg shape") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        // create the tree
        BST::insert(&node2, node1);
        BST::insert(&node2, node0);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(node2 == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(nullptr == node1->right);

        REQUIRE(nullptr == node2->parent);
        REQUIRE(node1 == node2->left);
        REQUIRE(nullptr == node2->right);
        WHEN("the root node is removed") {
            BST::remove(&node2, node2);
            THEN(R"(
the shape is correct:
  1
 /
0
                )") {
                REQUIRE(node2 == node1);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(nullptr == node1->right);

                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);
            }
        }
        WHEN("the middle node is removed") {
            BST::remove(&node2, node1);
            THEN(R"(
the shape is correct:
  2
 /
0
                )") {
                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);
            }
        }
        WHEN("the leaf node is removed") {
            BST::remove(&node2, node0);
            THEN(R"(
the shape is correct:
  2
 /
1
                )") {
                REQUIRE(nullptr == node2->parent);
                REQUIRE(node1 == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node2 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(nullptr == node1->right);
            }
        }
    }
}

/*
tree shape:
0
 \
  3
 /
1
 \
  2
*/
SCENARIO("BST: remove from a right-left-right chain") {
    GIVEN("a tree with right zigzag--shaped limit tree (buy)") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        // create the tree
        BST::insert(&node0, node3);
        BST::insert(&node0, node1);
        BST::insert(&node0, node2);
        // check the tree
        REQUIRE(nullptr == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(node3 == node0->right);

        REQUIRE(node3 == node1->parent);
        REQUIRE(nullptr == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(node0 == node3->parent);
        REQUIRE(node1 == node3->left);
        REQUIRE(nullptr == node3->right);
        WHEN("node 0 is removed") {
            BST::remove(&node0, node0);
            THEN(R"(
the shape is correct:
  3
 /
1
 \
  2
                )") {
                REQUIRE(node0 == node3);

                REQUIRE(node3 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(nullptr == node3->parent);
                REQUIRE(node1 == node3->left);
                REQUIRE(nullptr == node3->right);
            }
        }
        WHEN("node 3 is removed") {
            BST::remove(&node0, node3);
            THEN(R"(
the shape is correct:
0
 \
  1
   \
    2
                )") {
                REQUIRE(nullptr == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(node1 == node0->right);

                REQUIRE(node0 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);
            }
        }
        WHEN("node 1 is removed") {
            BST::remove(&node0, node1);
            THEN(R"(
the shape is correct:
0
 \
  3
 /
2
                )") {
                REQUIRE(nullptr == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(node3 == node0->right);

                REQUIRE(node3 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node0 == node3->parent);
                REQUIRE(node2 == node3->left);
                REQUIRE(nullptr == node3->right);
            }
        }
    }
}

/*
tree shape:
  3
 /
0
 \
  2
 /
1
*/
SCENARIO("BST: remove from a left-right-left chain") {
    GIVEN("a tree with left zigzag--shape") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        // create the tree
        BST::insert(&node3, node0);
        BST::insert(&node3, node2);
        BST::insert(&node3, node1);
        // check the tree
        REQUIRE(node3 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(node2 == node0->right);

        REQUIRE(node2 == node1->parent);
        REQUIRE(nullptr == node1->left);
        REQUIRE(nullptr == node1->right);

        REQUIRE(node0 == node2->parent);
        REQUIRE(node1 == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(nullptr == node3->parent);
        REQUIRE(node0 == node3->left);
        REQUIRE(nullptr == node3->right);
        WHEN("the root node is removed") {
            BST::remove(&node3, node3);
            THEN(R"(
the shape is correct:
0
 \
  2
 /
1
                )") {
                REQUIRE(node3 == node0);

                REQUIRE(nullptr == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(node2 == node0->right);

                REQUIRE(node2 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(nullptr == node1->right);

                REQUIRE(node0 == node2->parent);
                REQUIRE(node1 == node2->left);
                REQUIRE(nullptr == node2->right);
            }
        }
        WHEN("the root left node is removed") {
            BST::remove(&node3, node0);
            THEN(R"(
the shape is correct:
    3
   /
  2
 /
1
                )") {
                REQUIRE(node2 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(nullptr == node1->right);

                REQUIRE(node3 == node2->parent);
                REQUIRE(node1 == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(nullptr == node3->parent);
                REQUIRE(node2 == node3->left);
                REQUIRE(nullptr == node3->right);
            }
        }
        WHEN("node 2 is removed") {
            BST::remove(&node3, node2);
            THEN(R"(
the shape is correct:
  3
 /
0
 \
  1
                )") {
                REQUIRE(node3 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(node1 == node0->right);

                REQUIRE(node0 == node1->parent);
                REQUIRE(nullptr == node1->left);
                REQUIRE(nullptr == node1->right);

                REQUIRE(nullptr == node3->parent);
                REQUIRE(node0 == node3->left);
                REQUIRE(nullptr == node3->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   2
     \
      3
*/
SCENARIO("BST: remove from a right child with a right leaf") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node2);
        BST::insert(&node1, node3);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(node3 == node2->right);

        REQUIRE(node2 == node3->parent);
        REQUIRE(nullptr == node3->left);
        REQUIRE(nullptr == node3->right);
        WHEN("the root node is removed") {
            BST::remove(&node1, node1);
            THEN(R"(
the shape is correct:
  2
 / \
0   3
                )") {
                REQUIRE(node1 == node2);

                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(node3 == node2->right);

                REQUIRE(node2 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   4
   /
  3
 /
2
*/
SCENARIO("BST: remove from a deep left successor") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node4);
        BST::insert(&node1, node3);
        BST::insert(&node1, node2);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node4 == node1->right);

        REQUIRE(node3 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(node4 == node3->parent);
        REQUIRE(node2 == node3->left);
        REQUIRE(nullptr == node3->right);

        REQUIRE(node1 == node4->parent);
        REQUIRE(node3 == node4->left);
        REQUIRE(nullptr == node4->right);
        WHEN("the root node is removed") {
            BST::remove(&node1, node1);
            THEN(R"(
the shape is correct:
  2
 / \
0   4
   /
  3
                )") {
                REQUIRE(node1 == node2);

                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(node4 == node2->right);

                REQUIRE(node4 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);

                REQUIRE(node2 == node4->parent);
                REQUIRE(node3 == node4->left);
                REQUIRE(nullptr == node4->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   5
   /
  4
 /
2
 \
  3
*/
SCENARIO("BST: remove from a deep successor with a right child") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node5);
        BST::insert(&node1, node4);
        BST::insert(&node1, node2);
        BST::insert(&node1, node3);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node5 == node1->right);

        REQUIRE(node4 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(node3 == node2->right);

        REQUIRE(node2 == node3->parent);
        REQUIRE(nullptr == node3->left);
        REQUIRE(nullptr == node3->right);

        REQUIRE(node5 == node4->parent);
        REQUIRE(node2 == node4->left);
        REQUIRE(nullptr == node4->right);

        REQUIRE(node1 == node5->parent);
        REQUIRE(node4 == node5->left);
        REQUIRE(nullptr == node5->right);
        WHEN("the root node is removed") {
            BST::remove(&node1, node1);
            THEN(R"(
the shape is correct:
  2
 / \
0   5
   /
  4
 /
3
                )") {
                REQUIRE(node1 == node2);

                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(node5 == node2->right);

                REQUIRE(node4 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);

                REQUIRE(node5 == node4->parent);
                REQUIRE(node3 == node4->left);
                REQUIRE(nullptr == node4->right);

                REQUIRE(node2 == node5->parent);
                REQUIRE(node4 == node5->left);
                REQUIRE(nullptr == node5->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   3
   /
  2
*/
SCENARIO("BST: remove from an immediate left successor") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node3);
        BST::insert(&node1, node2);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node3 == node1->right);

        REQUIRE(node3 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(node1 == node3->parent);
        REQUIRE(node2 == node3->left);
        REQUIRE(nullptr == node3->right);
        WHEN("the root node is removed") {
            BST::remove(&node1, node1);
            THEN(R"(
the shape is correct:
  2
 / \
0   3
                )") {
                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(node3 == node2->right);

                REQUIRE(node2 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   4
   /
  2
   \
    3
*/
SCENARIO("BST: remove from a successor with a right child") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node4);
        BST::insert(&node1, node2);
        BST::insert(&node1, node3);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node4 == node1->right);

        REQUIRE(node4 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(node3 == node2->right);

        REQUIRE(node2 == node3->parent);
        REQUIRE(nullptr == node3->left);
        REQUIRE(nullptr == node3->right);

        REQUIRE(node1 == node4->parent);
        REQUIRE(node2 == node4->left);
        REQUIRE(nullptr == node4->right);
        WHEN("the root node is removed") {
            BST::remove(&node1, node1);
            THEN(R"(
the shape is correct:
  2
 / \
0   4
   /
  3
                )") {
                REQUIRE(node2 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node2->parent);
                REQUIRE(node0 == node2->left);
                REQUIRE(node4 == node2->right);

                REQUIRE(node4 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);

                REQUIRE(node2 == node4->parent);
                REQUIRE(node3 == node4->left);
                REQUIRE(nullptr == node4->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   5
   /
  3
 / \
2   4
*/
SCENARIO("BST: remove from a successor with a sibling") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node5);
        BST::insert(&node1, node3);
        BST::insert(&node1, node2);
        BST::insert(&node1, node4);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node5 == node1->right);

        REQUIRE(node3 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(node5 == node3->parent);
        REQUIRE(node2 == node3->left);
        REQUIRE(node4 == node3->right);

        REQUIRE(node3 == node4->parent);
        REQUIRE(nullptr == node4->left);
        REQUIRE(nullptr == node4->right);

        REQUIRE(node1 == node5->parent);
        REQUIRE(node3 == node5->left);
        REQUIRE(nullptr == node5->right);
        WHEN("node 3 is removed") {
            BST::remove(&node1, node3);
            THEN(R"(
the shape is correct:
  1
 / \
0   5
   /
  4
 /
2
                )") {
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(node5 == node1->right);

                REQUIRE(node4 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node5 == node4->parent);
                REQUIRE(node2 == node4->left);
                REQUIRE(nullptr == node4->right);

                REQUIRE(node1 == node5->parent);
                REQUIRE(node4 == node5->left);
                REQUIRE(nullptr == node5->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   2
     \
      4
     / \
    3   5
*/
SCENARIO("BST: remove from a right subtree with two leaves") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node2);
        BST::insert(&node1, node4);
        BST::insert(&node1, node5);
        BST::insert(&node1, node3);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(node4 == node2->right);

        REQUIRE(node4 == node3->parent);
        REQUIRE(nullptr == node3->left);
        REQUIRE(nullptr == node3->right);

        REQUIRE(node2 == node4->parent);
        REQUIRE(node3 == node4->left);
        REQUIRE(node5 == node4->right);

        REQUIRE(node4 == node5->parent);
        REQUIRE(nullptr == node5->left);
        REQUIRE(nullptr == node5->right);
        WHEN("node 4 is removed") {
            BST::remove(&node1, node4);
            THEN(R"(
the shape is correct:
  1
 / \
0   2
     \
      5
     /
    3
                )") {
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(node5 == node2->right);

                REQUIRE(node5 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);

                REQUIRE(node2 == node5->parent);
                REQUIRE(node3 == node5->left);
                REQUIRE(nullptr == node5->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   6
   /
  3
 / \
2   5
   /
  4
*/
SCENARIO("BST: remove from a nested successor") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        BST::Node<double> node6_storage{6};
        auto node6 = &node6_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node6);
        BST::insert(&node1, node3);
        BST::insert(&node1, node2);
        BST::insert(&node1, node5);
        BST::insert(&node1, node4);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node6 == node1->right);

        REQUIRE(node3 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(node6 == node3->parent);
        REQUIRE(node2 == node3->left);
        REQUIRE(node5 == node3->right);

        REQUIRE(node5 == node4->parent);
        REQUIRE(nullptr == node4->left);
        REQUIRE(nullptr == node4->right);

        REQUIRE(node3 == node5->parent);
        REQUIRE(node4 == node5->left);
        REQUIRE(nullptr == node5->right);

        REQUIRE(node1 == node6->parent);
        REQUIRE(node3 == node6->left);
        REQUIRE(nullptr == node6->right);
        WHEN("node 3 is removed") {
            BST::remove(&node1, node3);
            THEN(R"(
the shape is correct:
  1
 / \
0   6
   /
  4
 / \
2   5
                )") {
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(node6 == node1->right);

                REQUIRE(node4 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node6 == node4->parent);
                REQUIRE(node2 == node4->left);
                REQUIRE(node5 == node4->right);

                REQUIRE(node4 == node5->parent);
                REQUIRE(nullptr == node5->left);
                REQUIRE(nullptr == node5->right);

                REQUIRE(node1 == node6->parent);
                REQUIRE(node4 == node6->left);
                REQUIRE(nullptr == node6->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   2
     \
      4
     / \
    3   6
       /
      5
*/
SCENARIO("BST: remove from a nested right subtree") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        BST::Node<double> node6_storage{6};
        auto node6 = &node6_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node2);
        BST::insert(&node1, node4);
        BST::insert(&node1, node3);
        BST::insert(&node1, node6);
        BST::insert(&node1, node5);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(node4 == node2->right);

        REQUIRE(node4 == node3->parent);
        REQUIRE(nullptr == node3->left);
        REQUIRE(nullptr == node3->right);

        REQUIRE(node2 == node4->parent);
        REQUIRE(node3 == node4->left);
        REQUIRE(node6 == node4->right);

        REQUIRE(node6 == node5->parent);
        REQUIRE(nullptr == node5->left);
        REQUIRE(nullptr == node5->right);

        REQUIRE(node4 == node6->parent);
        REQUIRE(node5 == node6->left);
        REQUIRE(nullptr == node6->right);
        WHEN("node 4 is removed") {
            BST::remove(&node1, node4);
            THEN(R"(
the shape is correct:
  1
 / \
0   2
     \
      5
     / \
    3   6
                )") {
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(node5 == node2->right);

                REQUIRE(node5 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);

                REQUIRE(node2 == node5->parent);
                REQUIRE(node3 == node5->left);
                REQUIRE(node6 == node5->right);

                REQUIRE(node5 == node6->parent);
                REQUIRE(nullptr == node6->left);
                REQUIRE(nullptr == node6->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   7
   /
  3
 / \
2   6
   /
  5
 /
4
*/
SCENARIO("BST: remove from a twice-nested successor") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        BST::Node<double> node6_storage{6};
        auto node6 = &node6_storage;
        BST::Node<double> node7_storage{7};
        auto node7 = &node7_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node7);
        BST::insert(&node1, node3);
        BST::insert(&node1, node2);
        BST::insert(&node1, node6);
        BST::insert(&node1, node5);
        BST::insert(&node1, node4);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node7 == node1->right);

        REQUIRE(node3 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(nullptr == node2->right);

        REQUIRE(node7 == node3->parent);
        REQUIRE(node2 == node3->left);
        REQUIRE(node6 == node3->right);

        REQUIRE(node5 == node4->parent);
        REQUIRE(nullptr == node4->left);
        REQUIRE(nullptr == node4->right);

        REQUIRE(node6 == node5->parent);
        REQUIRE(node4 == node5->left);
        REQUIRE(nullptr == node5->right);

        REQUIRE(node3 == node6->parent);
        REQUIRE(node5 == node6->left);
        REQUIRE(nullptr == node6->right);

        REQUIRE(node1 == node7->parent);
        REQUIRE(node3 == node7->left);
        REQUIRE(nullptr == node7->right);
        WHEN("node 3 is removed") {
            BST::remove(&node1, node3);
            THEN(R"(
the shape is correct:
  1
 / \
0   7
   /
  4
 / \
2   6
   /
  5
                )") {
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(node7 == node1->right);

                REQUIRE(node4 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(nullptr == node2->right);

                REQUIRE(node7 == node4->parent);
                REQUIRE(node2 == node4->left);
                REQUIRE(node6 == node4->right);

                REQUIRE(node6 == node5->parent);
                REQUIRE(nullptr == node5->left);
                REQUIRE(nullptr == node5->right);

                REQUIRE(node4 == node6->parent);
                REQUIRE(node5 == node6->left);
                REQUIRE(nullptr == node6->right);

                REQUIRE(node1 == node7->parent);
                REQUIRE(node4 == node7->left);
                REQUIRE(nullptr == node7->right);
            }
        }
    }
}

/*
tree shape:
  1
 / \
0   2
     \
      4
     / \
    3   7
       /
      6
     /
    5
*/
SCENARIO("BST: remove from a twice-nested right subtree") {
    GIVEN("nodes") {
        BST::Node<double> node0_storage{0};
        auto node0 = &node0_storage;
        BST::Node<double> node1_storage{1};
        auto node1 = &node1_storage;
        BST::Node<double> node2_storage{2};
        auto node2 = &node2_storage;
        BST::Node<double> node3_storage{3};
        auto node3 = &node3_storage;
        BST::Node<double> node4_storage{4};
        auto node4 = &node4_storage;
        BST::Node<double> node5_storage{5};
        auto node5 = &node5_storage;
        BST::Node<double> node6_storage{6};
        auto node6 = &node6_storage;
        BST::Node<double> node7_storage{7};
        auto node7 = &node7_storage;
        // create the tree
        BST::insert(&node1, node0);
        BST::insert(&node1, node2);
        BST::insert(&node1, node4);
        BST::insert(&node1, node3);
        BST::insert(&node1, node7);
        BST::insert(&node1, node6);
        BST::insert(&node1, node5);
        // check the tree
        REQUIRE(node1 == node0->parent);
        REQUIRE(nullptr == node0->left);
        REQUIRE(nullptr == node0->right);

        REQUIRE(nullptr == node1->parent);
        REQUIRE(node0 == node1->left);
        REQUIRE(node2 == node1->right);

        REQUIRE(node1 == node2->parent);
        REQUIRE(nullptr == node2->left);
        REQUIRE(node4 == node2->right);

        REQUIRE(node4 == node3->parent);
        REQUIRE(nullptr == node3->left);
        REQUIRE(nullptr == node3->right);

        REQUIRE(node2 == node4->parent);
        REQUIRE(node3 == node4->left);
        REQUIRE(node7 == node4->right);

        REQUIRE(node6 == node5->parent);
        REQUIRE(nullptr == node5->left);
        REQUIRE(nullptr == node5->right);

        REQUIRE(node7 == node6->parent);
        REQUIRE(node5 == node6->left);
        REQUIRE(nullptr == node6->right);

        REQUIRE(node4 == node7->parent);
        REQUIRE(node6 == node7->left);
        REQUIRE(nullptr == node7->right);
        WHEN("node 4 is removed") {
            BST::remove(&node1, node4);
            THEN(R"(
the shape is correct:
  1
 / \
0   2
     \
      5
     / \
    3   7
       /
      6
                )") {
                REQUIRE(node1 == node0->parent);
                REQUIRE(nullptr == node0->left);
                REQUIRE(nullptr == node0->right);

                REQUIRE(nullptr == node1->parent);
                REQUIRE(node0 == node1->left);
                REQUIRE(node2 == node1->right);

                REQUIRE(node1 == node2->parent);
                REQUIRE(nullptr == node2->left);
                REQUIRE(node5 == node2->right);

                REQUIRE(node5 == node3->parent);
                REQUIRE(nullptr == node3->left);
                REQUIRE(nullptr == node3->right);

                REQUIRE(node2 == node5->parent);
                REQUIRE(node3 == node5->left);
                REQUIRE(node7 == node5->right);

                REQUIRE(node7 == node6->parent);
                REQUIRE(nullptr == node6->left);
                REQUIRE(nullptr == node6->right);

                REQUIRE(node5 == node7->parent);
                REQUIRE(node6 == node7->left);
                REQUIRE(nullptr == node7->right);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// MARK: polymorphism
// ---------------------------------------------------------------------------

TEST_CASE("BST: should subclass BST::Node (C-style casts)") {
    struct ArbNode : BST::Node<int> { };
    ArbNode* root = nullptr;
    ArbNode node;
    BST::insert((BST::Node<int>**) &root, (BST::Node<int>*) &node);
    REQUIRE(root == &node);
}

TEST_CASE("BST: should subclass BST::Node (C++ casts)") {
    struct ArbNode : BST::Node<int> { };
    ArbNode* root = nullptr;
    ArbNode node;
    BST::insert(
        reinterpret_cast<BST::Node<int>**>(&root),
        static_cast<BST::Node<int>*>(&node)
    );
    REQUIRE(root == &node);
}
