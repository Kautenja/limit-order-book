// A consumer C program that matches two orders through the C API library.
//
// Copyright (c) 2026 Christian Kauten
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "lib_lob.h"
#include <stddef.h>

int main(void) {
    int status = 0;
    LOB_LimitOrderBook* book = new_();
    if (book == NULL) return 1;
    limit(book, false, 1, 100, 50);
    limit(book, true, 2, 40, 50);  // crosses and partially fills
    if (best_sell(book) != 50) status = 1;
    if (volume_sell(book) != 60) status = 1;
    if (count(book) != 1) status = 1;
    if (get(book, 1) == NULL) status = 1;
    if (get(book, 2) != NULL) status = 1;  // order filled
    cancel(book, 2);  // unknown IDs are ignored
    cancel(book, 1);
    if (has(book, 1) || count(book) != 0) status = 1;
    for (int side = 0; side < 2; ++side) {
        limit(book, side, 3, 5, 100);
        const LOB_Order* original = get(book, 3);
        limit_buy(book, 3, 9, 101);
        limit_sell(book, 3, 9, 99);
        limit(book, side, 3, 5, 100);
        if (get(book, 3) != original) status = 1;
        if (count(book) != 1 || count_at(book, 100) != 1) status = 1;
        if (volume(book) != 5 || volume_price(book, 100) != 5) status = 1;
        if (best(book, side) != 100 || best(book, !side) != 0) status = 1;
        limit(book, side, 7, 4, 100);
        cancel(book, 3);
        if (has(book, 3) || count(book) != 1) status = 1;
        if (volume(book) != 4) status = 1;
        cancel(book, 7);
        if (count(book) != 0 || volume(book) != 0) status = 1;
        if (best_buy(book) != 0 || best_sell(book) != 0) status = 1;
        // Reuse removed IDs and verify that a duplicate retains FIFO priority.
        limit(book, side, 3, 5, 100);
        limit(book, side, 7, 4, 100);
        limit(book, side, 3, 9, 101);
        market(book, !side, 8, 6);
        if (has(book, 3) || !has(book, 7)) status = 1;
        if (count(book) != 1 || volume(book) != 3) status = 1;
        clear(book);
    }
    delete_(book);
    return status;
}
