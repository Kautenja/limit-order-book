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
        self.assertEqual(0, book.size_at(100))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(0, book.size())


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
        self.assertEqual(1, book.size_at(50))
        self.assertEqual(1, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(1, book.size())


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
        self.assertEqual(0, book.size_at(100))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(0, book.size())


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
        self.assertEqual(1, book.size_at(50))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(1, book.size_buy())
        self.assertEqual(1, book.size())


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
        self.assertEqual(0, book.size_at(100))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(0, book.size())


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
        self.assertEqual(0, book.size_at(100))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(0, book.size())


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
        self.assertEqual(0, book.size_at(100))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(0, book.size())


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
        self.assertEqual(1, book.size_at(50))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(1, book.size_buy())
        self.assertEqual(1, book.size())


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
        self.assertEqual(1, book.size_at(50))
        self.assertEqual(1, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(1, book.size())


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
        self.assertEqual(0, book.size_at(100))
        self.assertEqual(0, book.size_sell())
        self.assertEqual(0, book.size_buy())
        self.assertEqual(0, book.size())
