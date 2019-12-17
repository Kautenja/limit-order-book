"""Test cases for the lob module."""
from unittest import TestCase
from .. import lob


class ShouldInitializeLimitOrderBook(TestCase):
    def test(self):
        book = lob.LimitOrderBook()
        self.assertIsInstance(book, lob.LimitOrderBook)
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
        book = lob.LimitOrderBook()


class ShouldCancelSellLimitOrder(TestCase):
    def test(self):
        book = lob.LimitOrderBook()


class ShouldPlaceBuyLimitOrder(TestCase):
    def test(self):
        book = lob.LimitOrderBook()


class ShouldCancelBuyLimitOrder(TestCase):
    def test(self):
        book = lob.LimitOrderBook()


class ShouldPlaceSellMarketOrderEmptyBook(TestCase):
    def test(self):
        book = lob.LimitOrderBook()


class ShouldPlaceBuyMarketOrderEmptyBook(TestCase):
    def test(self):
        book = lob.LimitOrderBook()


class ShouldPlaceSellMarketOrderAndMatch(TestCase):
    def test(self):
        book = lob.LimitOrderBook()


class ShouldPlaceBuyMarketOrderAndMatch(TestCase):
    def test(self):
        book = lob.LimitOrderBook()
