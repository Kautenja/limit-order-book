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
// MARK: DLL::push_back
// ---------------------------------------------------------------------------

SCENARIO("DLL: nodes are pushed onto the queue") {
    GIVEN("a series of node pointers") {
        DLL::Node node1_storage{};
        auto node1 = &node1_storage;
        DLL::Node node2_storage{};
        auto node2 = &node2_storage;
        DLL::Node node3_storage{};
        auto node3 = &node3_storage;
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

// ---------------------------------------------------------------------------
// MARK: DLL::append
// ---------------------------------------------------------------------------

SCENARIO("DLL: nodes are appended to the queue") {
    GIVEN("a series of node pointers") {
        DLL::Node node1_storage{};
        auto node1 = &node1_storage;
        DLL::Node node2_storage{};
        auto node2 = &node2_storage;
        DLL::Node node3_storage{};
        auto node3 = &node3_storage;
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

// ---------------------------------------------------------------------------
// MARK: DLL::remove
// ---------------------------------------------------------------------------

TEST_CASE("DLL: remove a single node") {
    DLL::Node node1_storage{};
    auto node1 = &node1_storage;
    DLL::Node* head = nullptr;
    DLL::Node* tail = node1;
    DLL::push_back(&head, node1);
    DLL::remove(&head, &tail, node1);
    REQUIRE(head == nullptr);
    REQUIRE(tail == nullptr);
    REQUIRE(node1->next == nullptr);
    REQUIRE(node1->prev == nullptr);
}

SCENARIO("DLL: nodes are pushed onto the queue then removed") {
    GIVEN("a series of node pointers") {
        DLL::Node node1_storage{};
        auto node1 = &node1_storage;
        DLL::Node node2_storage{};
        auto node2 = &node2_storage;
        DLL::Node node3_storage{};
        auto node3 = &node3_storage;
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

// ---------------------------------------------------------------------------
// MARK: DLL::is_empty
// ---------------------------------------------------------------------------

TEST_CASE("DLL: return the correct emptiness flag") {
    DLL::Node node1_storage{};
    auto node1 = &node1_storage;
    DLL::Node* head = nullptr;
    REQUIRE(DLL::is_empty(head));
    DLL::push_back(&head, node1);
    REQUIRE_FALSE(DLL::is_empty(head));
}

// ---------------------------------------------------------------------------
// MARK: DLL::count
// ---------------------------------------------------------------------------

TEST_CASE("DLL: return the correct size") {
    DLL::Node node1_storage{};
    auto node1 = &node1_storage;
    DLL::Node node2_storage{};
    auto node2 = &node2_storage;
    DLL::Node node3_storage{};
    auto node3 = &node3_storage;
    DLL::Node* head = nullptr;
    REQUIRE(DLL::count(head) == 0);
    DLL::push_back(&head, node1);
    REQUIRE(DLL::count(head) == 1);
    DLL::push_back(&head, node2);
    REQUIRE(DLL::count(head) == 2);
    DLL::push_back(&head, node3);
    REQUIRE(DLL::count(head) == 3);
}
