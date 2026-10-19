"""Test cases for the lob module."""
from unittest import TestCase
from .. import limit_order_book


class ShouldInitializeLimitOrderBook(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        self.assertIsInstance(book, limit_order_book.LimitOrderBook)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())

#
# MARK: limit
#

class ShouldPlaceSellLimitOrder(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        self.assertEqual(50, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(50, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(100, book.volume_sell())
        self.assertEqual(100, book.volume_sell(50))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(100, book.volume())
        self.assertEqual(100, book.volume(50))
        self.assertEqual(1, book.count_at(50))
        self.assertEqual(1, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(1, book.count())


class ShouldPlaceSellLimitOrderByValue(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit(False, 1, 100, 50)
        self.assertEqual(50, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(50, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(100, book.volume_sell())
        self.assertEqual(100, book.volume_sell(50))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(100, book.volume())
        self.assertEqual(100, book.volume(50))
        self.assertEqual(1, book.count_at(50))
        self.assertEqual(1, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(1, book.count())


class ShouldPlaceBuyLimitOrder(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_buy(1, 100, 50)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(50, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(50, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(50))
        self.assertEqual(100, book.volume_buy())
        self.assertEqual(100, book.volume_buy(50))
        self.assertEqual(100, book.volume())
        self.assertEqual(100, book.volume(50))
        self.assertEqual(1, book.count_at(50))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(1, book.count_buy())
        self.assertEqual(1, book.count())


class ShouldPlaceBuyLimitOrderByValue(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit(True, 1, 100, 50)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(50, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(50, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(50))
        self.assertEqual(100, book.volume_buy())
        self.assertEqual(100, book.volume_buy(50))
        self.assertEqual(100, book.volume())
        self.assertEqual(100, book.volume(50))
        self.assertEqual(1, book.count_at(50))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(1, book.count_buy())
        self.assertEqual(1, book.count())

#
# MARK: limit match
#

class ShouldIgnoreDuplicateLiveLimitOrderIDs(TestCase):
    def test_duplicates_leave_both_sides_unchanged(self):
        for side in (False, True):
            for duplicate_side in (False, True):
                for price in (50, 100, 200):
                    for dispatch in (False, True):
                        with self.subTest(side=side, duplicate_side=duplicate_side,
                                          price=price, dispatch=dispatch):
                            book = limit_order_book.LimitOrderBook()
                            opposite_price = 200 if side else 50
                            book.limit(side, 3, 5, 100)
                            book.limit(not side, 9, 6, opposite_price)
                            if dispatch:
                                book.limit(duplicate_side, 3, 9, price)
                            elif duplicate_side:
                                book.limit_buy(3, 9, price)
                            else:
                                book.limit_sell(3, 9, price)
                            self.assertTrue(book.has(3))
                            self.assertTrue(book.has(9))
                            self.assertEqual(2, book.count())
                            self.assertEqual(1, book.count_buy())
                            self.assertEqual(1, book.count_sell())
                            self.assertEqual(11, book.volume())
                            self.assertEqual(5 if side else 6, book.volume_buy())
                            self.assertEqual(6 if side else 5, book.volume_sell())
                            self.assertEqual(1, book.count_at(100))
                            self.assertEqual(5, book.volume(100))
                            self.assertEqual(1, book.count_at(opposite_price))
                            self.assertEqual(6, book.volume(opposite_price))
                            self.assertEqual(100, book.best(side))
                            self.assertEqual(opposite_price, book.best(not side))

    def test_fifo_and_cancellation_remain_valid(self):
        for side in (False, True):
            with self.subTest(side=side):
                book = limit_order_book.LimitOrderBook()
                book.limit(side, 3, 5, 100)
                book.limit(side, 3, 5, 100)
                book.limit(side, 7, 4, 100)
                book.cancel(3)
                self.assertEqual(1, book.count())
                self.assertEqual(4, book.volume(100))
                book.cancel(7)
                self.assertEqual(0, book.count())
                self.assertEqual(0, book.volume())
                self.assertEqual(0, book.best_buy())
                self.assertEqual(0, book.best_sell())
                book.limit(side, 3, 5, 100)
                book.limit(side, 7, 4, 100)
                book.limit(side, 3, 9, 101)
                book.market(not side, 8, 6)
                self.assertFalse(book.has(3))
                self.assertTrue(book.has(7))
                self.assertEqual(1, book.count_at(100))
                self.assertEqual(3, book.volume(100))
                self.assertEqual(100, book.best(side))
                self.assertEqual(0, book.best(not side))


class ShouldReuseRemovedLimitOrderIDs(TestCase):
    def test(self):
        for side in (False, True):
            for removal in ('cancel', 'fill', 'clear', 'cross'):
                with self.subTest(side=side, removal=removal):
                    book = limit_order_book.LimitOrderBook()
                    book.limit(side, 3, 5, 100)
                    if removal == 'cancel':
                        book.cancel(3)
                    elif removal == 'fill':
                        book.market(not side, 8, 5)
                    elif removal == 'clear':
                        book.clear()
                    else:
                        book.cancel(3)
                        book.limit(not side, 8, 5, 100)
                        book.limit(side, 3, 5, 100)
                    self.assertFalse(book.has(3))
                    book.limit(not side, 3, 7, 101)
                    self.assertTrue(book.has(3))
                    self.assertEqual(1, book.count())
                    self.assertEqual(7, book.volume())
                    self.assertEqual(1, book.count_at(101))
                    self.assertEqual(7, book.volume(101))
                    self.assertEqual(0, book.best(side))
                    self.assertEqual(101, book.best(not side))


class ShouldMatchSellLimitOrderWithIncomingBuy(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        book.limit_buy(2, 100, 50)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(50))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(50))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(50))
        self.assertEqual(0, book.count_at(50))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())


class ShouldMatchBuyLimitOrderWithIncomingSell(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_buy(1, 100, 50)
        book.limit_sell(2, 100, 50)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(50))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(50))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(50))
        self.assertEqual(0, book.count_at(50))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())

#
# MARK: cancel
#

class ShouldCancelSellLimitOrder(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        self.assertTrue(book.has(1))
        book.cancel(1)
        self.assertFalse(book.has(1))
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())


class ShouldCancelBuyLimitOrder(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_buy(1, 100, 50)
        self.assertTrue(book.has(1))
        book.cancel(1)
        self.assertFalse(book.has(1))
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())


class ShouldIgnoreCancelOfUnknownOrder(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        book.cancel(2)
        self.assertTrue(book.has(1))
        self.assertFalse(book.has(2))
        self.assertEqual(50, book.best_sell())
        self.assertEqual(100, book.volume_sell())
        self.assertEqual(1, book.count())

#
# MARK: market
#

class ShouldPlaceSellMarketOrderEmptyBook(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.market_sell(1, 100)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())


class ShouldPlaceBuyMarketOrderEmptyBook(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.market_buy(1, 100)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())


class ShouldPlaceSellMarketOrderAndMatch(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_buy(1, 100, 50)
        book.market_sell(1, 10)
        self.assertEqual(0, book.best_sell())
        self.assertEqual(50, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(50, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(90, book.volume_buy())
        self.assertEqual(90, book.volume_buy(50))
        self.assertEqual(90, book.volume())
        self.assertEqual(90, book.volume(50))
        self.assertEqual(1, book.count_at(50))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(1, book.count_buy())
        self.assertEqual(1, book.count())


class ShouldPlaceBuyMarketOrderAndMatch(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        book.market_buy(1, 10)
        self.assertEqual(50, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(50, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(90, book.volume_sell())
        self.assertEqual(90, book.volume_sell(50))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(50))
        self.assertEqual(90, book.volume())
        self.assertEqual(90, book.volume(50))
        self.assertEqual(1, book.count_at(50))
        self.assertEqual(1, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(1, book.count())

#
# MARK: clear
#

class ShouldClearSellLimitOrders(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        book.limit_sell(2, 100, 50)
        book.limit_sell(3, 100, 50)
        self.assertTrue(book.has(1))
        self.assertTrue(book.has(2))
        self.assertTrue(book.has(3))
        book.clear()
        self.assertFalse(book.has(1))
        self.assertFalse(book.has(2))
        self.assertFalse(book.has(3))
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())


class ShouldClearBuyLimitOrders(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        book.limit_sell(1, 100, 50)
        book.limit_sell(2, 100, 50)
        book.limit_sell(3, 100, 50)
        self.assertTrue(book.has(1))
        self.assertTrue(book.has(2))
        self.assertTrue(book.has(3))
        book.clear()
        self.assertFalse(book.has(1))
        self.assertFalse(book.has(2))
        self.assertFalse(book.has(3))
        self.assertEqual(0, book.best_sell())
        self.assertEqual(0, book.best_buy())
        self.assertEqual(0, book.best(False))
        self.assertEqual(0, book.best(True))
        self.assertEqual(0, book.volume_sell())
        self.assertEqual(0, book.volume_sell(100))
        self.assertEqual(0, book.volume_buy())
        self.assertEqual(0, book.volume_buy(100))
        self.assertEqual(0, book.volume())
        self.assertEqual(0, book.volume(100))
        self.assertEqual(0, book.count_at(100))
        self.assertEqual(0, book.count_sell())
        self.assertEqual(0, book.count_buy())
        self.assertEqual(0, book.count())

#
# MARK: volume
#

class ShouldReportVolumeAboveUInt32Max(TestCase):
    def test(self):
        book = limit_order_book.LimitOrderBook()
        # the largest quantity that fits in a 32-bit Quantity
        quantity = 2**32 - 1
        book.limit_sell(1, quantity, 100)
        book.limit_sell(2, quantity, 100)
        book.limit_sell(3, quantity, 101)
        book.limit_buy(4, quantity, 50)
        book.limit_buy(5, quantity, 50)
        self.assertEqual(2 * quantity, book.volume_sell(100))
        self.assertEqual(3 * quantity, book.volume_sell())
        self.assertEqual(2 * quantity, book.volume_buy(50))
        self.assertEqual(2 * quantity, book.volume_buy())
        self.assertEqual(2 * quantity, book.volume(100))
        self.assertEqual(2 * quantity, book.volume(50))
        self.assertEqual(5 * quantity, book.volume())
        self.assertEqual(2, book.count_at(100))
        self.assertEqual(3, book.count_sell())
        self.assertEqual(2, book.count_buy())
        self.assertEqual(5, book.count())
