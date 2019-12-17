#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <random>
#include <vector>
#include "catch.hpp"
#include "binary_search_tree.hpp"

//
// MARK: BST::insert nodes
//

/// BST::insert nodes with strictly increasing key.
///
/// @param count the number of nodes to insert
///
inline void insert_strictly_increasing_nodes(int count) {
    BST::Node<int>* head(nullptr);
    for (int i = 0; i < count; i++)
        BST::insert(&head, new BST::Node<int>(i));
}

TEST_CASE("BST::insert nodes with strictly increasing key") {
    BENCHMARK("10 nodes")    { insert_strictly_increasing_nodes(10); };
    BENCHMARK("100 nodes")   { insert_strictly_increasing_nodes(100); };
    BENCHMARK("1000 nodes")  { insert_strictly_increasing_nodes(1000); };
    BENCHMARK("10000 nodes") { insert_strictly_increasing_nodes(10000); };
    // BENCHMARK("100000 nodes") { insert_strictly_increasing_nodes(100000); };
}

/// BST::insert nodes with random key.
///
/// @param count the number of nodes to insert
/// @param mean the mean value of the distribution of keys
/// @param variance the variance of the distributions of keys
///
inline void insert_random_nodes(
    int count,
    double mean = 500,
    double variance = 20
) {
    BST::Node<double>* head(nullptr);
    auto generator = std::default_random_engine();
    auto key_distribution = std::normal_distribution<double>(mean, variance);
    for (int i = 0; i < count; i++)
        BST::insert(&head, new BST::Node<double>(key_distribution(generator)));
}

TEST_CASE("BST::insert nodes with random key") {
    BENCHMARK("10 nodes")      { insert_random_nodes(10); };
    BENCHMARK("100 nodes")     { insert_random_nodes(100); };
    BENCHMARK("1000 nodes")    { insert_random_nodes(1000); };
    BENCHMARK("10000 nodes")   { insert_random_nodes(10000); };
    BENCHMARK("100000 nodes")  { insert_random_nodes(100000); };
    // BENCHMARK("1000000 nodes") { insert_random_nodes(1000000); };
}

//
// MARK: BST::remove nodes
//

/// BST::insert nodes with random key.
///
/// @param head the reference to the head node pointer
/// @param nodes the reference to the vector of nodes to remove
/// @param count the number of nodes to insert
/// @param mean the mean value of the distribution of keys
/// @param variance the variance of the distributions of keys
///
inline void insert_random_nodes(
    BST::Node<double>** head,
    std::vector<BST::Node<double>*>& nodes,
    int count,
    double mean = 500,
    double variance = 20
) {
    auto generator = std::default_random_engine();
    auto key_distribution = std::normal_distribution<double>(mean, variance);
    for (int i = 0; i < count; i++) {
        auto node = new BST::Node<double>(key_distribution(generator));
        BST::insert(head, node);
        nodes.push_back(node);
    }
    std::shuffle(nodes.begin(), nodes.end(), generator);
}

/// Remove all the nodes from a binary search tree
///
/// @param head the head of the binary search tree
/// @param nodes the vector of nodes in the binary search tree
///
inline void remove_nodes_randomly(
    BST::Node<double>** head,
    std::vector<BST::Node<double>*>& nodes
) {
    if (nodes.size() == 0)
        throw "passed empty node list!";
    while (nodes.size()) {
        BST::remove(head, nodes.back());
        nodes.pop_back();
    }
}

TEST_CASE("randomly remove nodes") {
    BST::Node<double>* head(nullptr);
    std::vector<BST::Node<double>*> nodes;
    BENCHMARK("10 nodes") {
        insert_random_nodes(&head, nodes, 10);
        remove_nodes_randomly(&head, nodes);
    };
    BENCHMARK("100 nodes") {
        insert_random_nodes(&head, nodes, 100);
        remove_nodes_randomly(&head, nodes);
    };
    BENCHMARK("1000 nodes") {
        insert_random_nodes(&head, nodes, 1000);
        remove_nodes_randomly(&head, nodes);
    };
    BENCHMARK("10000 nodes") {
        insert_random_nodes(&head, nodes, 10000);
        remove_nodes_randomly(&head, nodes);
    };
    BENCHMARK("100000 nodes") {
        insert_random_nodes(&head, nodes, 100000);
        remove_nodes_randomly(&head, nodes);
    };
    // BENCHMARK("1000000 nodes") {
    //     insert_random_nodes(head, nodes, 1000000);
    //     remove_nodes_randomly(head, nodes);
    // };
}
