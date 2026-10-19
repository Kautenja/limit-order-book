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

#include <catch2/catch_test_macros.hpp>
#include "doubly_linked_list.hpp"

// ---------------------------------------------------------------------------
// MARK: LOB::DLL::push_back
// ---------------------------------------------------------------------------

SCENARIO("LOB::DLL: nodes are pushed onto the queue") {
    GIVEN("a series of node pointers") {
        LOB::DLL::Node node1_storage{};
        auto node1 = &node1_storage;
        LOB::DLL::Node node2_storage{};
        auto node2 = &node2_storage;
        LOB::DLL::Node node3_storage{};
        auto node3 = &node3_storage;
        LOB::DLL::Node* head = nullptr;
        WHEN("node1 is pushed onto head") {
            LOB::DLL::push_back(&head, node1);
            THEN("head is set to node 1") {
                REQUIRE(head == node1);
            }
        }
        WHEN("node2 is pushed onto node1") {
            LOB::DLL::push_back(&head, node1);
            LOB::DLL::push_back(&head, node2);
            THEN("head is set to node 1") {
                REQUIRE(head == node1);
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == nullptr);
                REQUIRE(node2->prev == node1);
            }
        }
        WHEN("node3 is pushed onto node2") {
            LOB::DLL::push_back(&head, node1);
            LOB::DLL::push_back(&head, node2);
            LOB::DLL::push_back(&head, node3);
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

// ---------------------------------------------------------------------------
// MARK: LOB::DLL::append
// ---------------------------------------------------------------------------

SCENARIO("LOB::DLL: nodes are appended to the queue") {
    GIVEN("a series of node pointers") {
        LOB::DLL::Node node1_storage{};
        auto node1 = &node1_storage;
        LOB::DLL::Node node2_storage{};
        auto node2 = &node2_storage;
        LOB::DLL::Node node3_storage{};
        auto node3 = &node3_storage;
        LOB::DLL::Node* tail = nullptr;
        WHEN("node2 is appended to node1") {
            LOB::DLL::push_back(&tail, node1);
            LOB::DLL::append(&tail, node2);
            THEN("tail is set to node 1") {
                REQUIRE(tail == node2);
                REQUIRE(node1->next == node2);
                REQUIRE(node1->prev == nullptr);
                REQUIRE(node2->next == nullptr);
                REQUIRE(node2->prev == node1);
            }
        }
        WHEN("node3 is appended to node2") {
            LOB::DLL::push_back(&tail, node1);
            LOB::DLL::append(&tail, node2);
            LOB::DLL::append(&tail, node3);
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

// ---------------------------------------------------------------------------
// MARK: LOB::DLL::remove
// ---------------------------------------------------------------------------

TEST_CASE("LOB::DLL: remove a single node") {
    LOB::DLL::Node node1_storage{};
    auto node1 = &node1_storage;
    LOB::DLL::Node* head = nullptr;
    LOB::DLL::Node* tail = node1;
    LOB::DLL::push_back(&head, node1);
    LOB::DLL::remove(&head, &tail, node1);
    REQUIRE(head == nullptr);
    REQUIRE(tail == nullptr);
    REQUIRE(node1->next == nullptr);
    REQUIRE(node1->prev == nullptr);
}

SCENARIO("LOB::DLL: nodes are pushed onto the queue then removed") {
    GIVEN("a series of node pointers") {
        LOB::DLL::Node node1_storage{};
        auto node1 = &node1_storage;
        LOB::DLL::Node node2_storage{};
        auto node2 = &node2_storage;
        LOB::DLL::Node node3_storage{};
        auto node3 = &node3_storage;
        LOB::DLL::Node* head = nullptr;
        LOB::DLL::Node* tail = node3;
        LOB::DLL::push_back(&head, node1);
        LOB::DLL::push_back(&head, node2);
        LOB::DLL::push_back(&head, node3);
        WHEN("node1 is removed") {
            LOB::DLL::remove(&head, &tail, node1);
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
            LOB::DLL::remove(&head, &tail, node2);
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
            LOB::DLL::remove(&head, &tail, node3);
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

// ---------------------------------------------------------------------------
// MARK: LOB::DLL::is_empty
// ---------------------------------------------------------------------------

TEST_CASE("LOB::DLL: return the correct emptiness flag") {
    LOB::DLL::Node node1_storage{};
    auto node1 = &node1_storage;
    LOB::DLL::Node* head = nullptr;
    REQUIRE(LOB::DLL::is_empty(head));
    LOB::DLL::push_back(&head, node1);
    REQUIRE_FALSE(LOB::DLL::is_empty(head));
}

// ---------------------------------------------------------------------------
// MARK: LOB::DLL::count
// ---------------------------------------------------------------------------

TEST_CASE("LOB::DLL: return the correct size") {
    LOB::DLL::Node node1_storage{};
    auto node1 = &node1_storage;
    LOB::DLL::Node node2_storage{};
    auto node2 = &node2_storage;
    LOB::DLL::Node node3_storage{};
    auto node3 = &node3_storage;
    LOB::DLL::Node* head = nullptr;
    REQUIRE(LOB::DLL::count(head) == 0);
    LOB::DLL::push_back(&head, node1);
    REQUIRE(LOB::DLL::count(head) == 1);
    LOB::DLL::push_back(&head, node2);
    REQUIRE(LOB::DLL::count(head) == 2);
    LOB::DLL::push_back(&head, node3);
    REQUIRE(LOB::DLL::count(head) == 3);
}
