// A doubly linked list data structure.
// Copyright 2019 Christian Kauten
//

#include "catch.hpp"
#include "doubly_linked_list.hpp"

//
// MARK: DLL::push_back
//

SCENARIO("nodes are pushed onto the queue") {
    GIVEN("a series of node pointers") {
        auto node1 = new DLL::Node();
        auto node2 = new DLL::Node();
        auto node3 = new DLL::Node();
        DLL::Node* head = nullptr;
        WHEN("node1 is pushed onto head") {
            DLL::push_back(&head, node1);
            THEN("head is set to node 1") {
                REQUIRE(head == node1);
            }
        }
        WHEN("node2 is pushed onto node1") {
            DLL::push_back(&head, node1);
            DLL::push_back(&head, node2);
            THEN("head is set to node 1") {
                REQUIRE(head == node1);
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == nullptr);
                REQUIRE(node2->prev == node1);
            }
        }
        WHEN("node3 is pushed onto node2") {
            DLL::push_back(&head, node1);
            DLL::push_back(&head, node2);
            DLL::push_back(&head, node3);
            THEN("head is set to node 1") {
                REQUIRE(head == node1);
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == node3);
                REQUIRE(node2->prev == node1);
                REQUIRE(node3->next == nullptr);
                REQUIRE(node3->prev == node2);
            }
        }
    }
}

//
// MARK: DLL::append
//

SCENARIO("nodes are appended to the queue") {
    GIVEN("a series of node pointers") {
        auto node1 = new DLL::Node();
        auto node2 = new DLL::Node();
        auto node3 = new DLL::Node();
        DLL::Node* tail = nullptr;
        WHEN("node2 is appended to node1") {
            DLL::push_back(&tail, node1);
            DLL::append(&tail, node2);
            THEN("tail is set to node 1") {
                REQUIRE(tail == node2);
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == nullptr);
                REQUIRE(node2->prev == node1);
            }
        }
        WHEN("node3 is appended to node2") {
            DLL::push_back(&tail, node1);
            DLL::append(&tail, node2);
            DLL::append(&tail, node3);
            THEN("tail is set to node 1") {
                REQUIRE(tail == node3);
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == node3);
                REQUIRE(node2->prev == node1);
                REQUIRE(node3->next == nullptr);
                REQUIRE(node3->prev == node2);
            }
        }
    }
}

//
// MARK: DLL::remove
//

TEST_CASE("remove a single node") {
    auto node1 = new DLL::Node();
    DLL::Node* head = nullptr;
    DLL::Node* tail = node1;
    DLL::push_back(&head, node1);
    DLL::remove(&head, &tail, node1);
    REQUIRE(head == nullptr);
    REQUIRE(tail == nullptr);
    REQUIRE(node1->next == nullptr);
    REQUIRE(node1->prev == nullptr);
}

SCENARIO("nodes are pushed onto the queue then removed") {
    GIVEN("a series of node pointers") {
        auto node1 = new DLL::Node();
        auto node2 = new DLL::Node();
        auto node3 = new DLL::Node();
        DLL::Node* head = nullptr;
        DLL::Node* tail = node3;
        DLL::push_back(&head, node1);
        DLL::push_back(&head, node2);
        DLL::push_back(&head, node3);
        WHEN("node1 is removed") {
            DLL::remove(&head, &tail, node1);
            THEN("node2 is head") {
                REQUIRE(head == node2);
            }
            THEN("node3 is tail") {
                REQUIRE(tail == node3);
            }
            THEN("pointers are set") {
                REQUIRE(node1->next == nullptr);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == node3);
                REQUIRE(node2->prev == nullptr);
                REQUIRE(node3->next == nullptr);
                REQUIRE(node3->prev == node2);
            }
        }
        WHEN("node2 is removed") {
            DLL::remove(&head, &tail, node2);
            THEN("node1 is head") {
                REQUIRE(head == node1);
            }
            THEN("node3 is tail") {
                REQUIRE(tail == node3);
            }
            THEN("pointers are set") {
                REQUIRE(node1->next == node3);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == nullptr);
                REQUIRE(node2->prev == nullptr);
                REQUIRE(node3->next == nullptr);
                REQUIRE(node3->prev == node1);
            }
        }
        WHEN("node3 is removed") {
            DLL::remove(&head, &tail, node3);
            THEN("node1 is head") {
                REQUIRE(head == node1);
            }
            THEN("node2 is tail") {
                REQUIRE(tail == node2);
            }
            THEN("pointers are set") {
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == nullptr);
                REQUIRE(node2->prev == node1);
                REQUIRE(node3->next == nullptr);
                REQUIRE(node3->prev == nullptr);
            }
        }
    }
}

//
// MARK: DLL::is_empty
//

TEST_CASE("return the correct emptiness flag") {
    auto node1 = new DLL::Node();
    DLL::Node* head = nullptr;
    REQUIRE(DLL::is_empty(head));
    DLL::push_back(&head, node1);
    REQUIRE_FALSE(DLL::is_empty(head));
}

//
// MARK: DLL::count
//

TEST_CASE("return the correct size") {
    auto node1 = new DLL::Node();
    auto node2 = new DLL::Node();
    auto node3 = new DLL::Node();
    DLL::Node* head = nullptr;
    REQUIRE(DLL::count(head) == 0);
    DLL::push_back(&head, node1);
    REQUIRE(DLL::count(head) == 1);
    DLL::push_back(&head, node2);
    REQUIRE(DLL::count(head) == 2);
    DLL::push_back(&head, node3);
    REQUIRE(DLL::count(head) == 3);
}
