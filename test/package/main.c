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
    delete_(book);
    return status;
}
