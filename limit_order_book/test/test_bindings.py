"""Regression tests for Python conversion, matching, and native ownership."""
import gc
import weakref
from unittest import TestCase

from limit_order_book import LimitOrderBook
from limit_order_book.limit_order_book import LimitOrderBook as ModuleBook


class IndexValue:
    """An integer-like value supported by the original ctypes conversion."""

    def __init__(self, value):
        self.value = value

    def __index__(self):
        return self.value


class ShouldPreservePythonInterface(TestCase):
    def test_imports_keywords_defaults_and_return_types(self):
        self.assertIs(LimitOrderBook, ModuleBook)
        book = LimitOrderBook()
        self.assertIsNone(book.limit(side=False, order_id=1, quantity=10, price=100))
        self.assertIs(book.has(order_id=1), True)
        self.assertIs(book.has(order_id=2), False)
        for query in (book.volume, book.volume_sell, book.volume_buy):
            self.assertEqual(query(), query(price=None))
            self.assertIs(type(query()), int)
        self.assertIsNone(book.market(side=True, order_id=2, quantity=4))
        self.assertEqual(book.volume(price=100), 6)
        self.assertEqual(book.best(side=False), 100)
        self.assertEqual(book.count_at(price=100), 1)
        self.assertIsNone(book.cancel(order_id=1))
        self.assertIsNone(book.cancel(order_id=1))
        self.assertIsNone(book.clear())

    def test_full_unsigned_widths_and_wrapping(self):
        book = LimitOrderBook()
        maximum = 2**64 - 1
        book.limit_sell(-1, -1, -1)
        book.limit_sell(2**64, 2**32 + 2, 2**64 - 1)
        self.assertTrue(book.has(maximum))
        self.assertTrue(book.has(0))
        self.assertEqual(book.best_sell(), maximum)
        self.assertEqual(book.volume_sell(-1), 2**32 + 1)
        self.assertEqual(book.count_at(maximum), 2)
        book.market_buy(2**64 + 1, 2**32 + 1)
        self.assertEqual(book.volume(), 2**32)
        book.cancel(-1)
        self.assertEqual(book.volume(), 2)
        book.cancel(2**64)
        self.assertEqual(book.count(), 0)

    def test_integer_index_protocol_and_side_truthiness(self):
        book = LimitOrderBook()
        book.limit([], IndexValue(1), IndexValue(10), IndexValue(100))
        self.assertTrue(book.has(IndexValue(1)))
        self.assertEqual(book.volume_sell(IndexValue(100)), 10)
        self.assertEqual(book.count_at(IndexValue(100)), 1)
        book.market('buy', IndexValue(2), IndexValue(4))
        self.assertEqual(book.volume(), 6)
        book.cancel(IndexValue(1))
        book.limit([True], 3, 8, 99)
        self.assertEqual(book.best([True]), 99)
        self.assertEqual(book.best([]), 0)

    def test_invalid_arguments_do_not_mutate_book(self):
        book = LimitOrderBook()
        book.limit_sell(1, 10, 100)
        for invalid in (1.5, '1', None, object()):
            for method, args in (
                (book.limit_sell, (invalid, 1, 100)),
                (book.limit_buy, (2, invalid, 100)),
                (book.limit, (True, 2, 1, invalid)),
                (book.market_buy, (2, invalid)),
                (book.market_sell, (invalid, 1)),
                (book.market, (True, invalid, 1)),
                (book.has, (invalid,)),
                (book.cancel, (invalid,)),
                (book.count_at, (invalid,)),
            ):
                with self.subTest(method=method.__name__, value=invalid):
                    with self.assertRaises(TypeError):
                        method(*args)
            self.assertTrue(book.has(1))
            self.assertEqual(book.volume(), 10)
            self.assertEqual(book.count(), 1)

    def test_price_time_priority_on_both_sides(self):
        for side in (False, True):
            with self.subTest(side=side):
                book = LimitOrderBook()
                worse = 101 if not side else 99
                book.limit(side, 1, 10, worse)
                book.limit(side, 2, 10, 100)
                book.limit(side, 3, 10, 100)
                book.market(not side, 4, 15)
                self.assertFalse(book.has(2))
                self.assertTrue(book.has(3))
                self.assertTrue(book.has(1))
                self.assertEqual(book.volume(100), 5)
                book.limit(not side, 5, 10, 100)
                self.assertFalse(book.has(3))
                self.assertTrue(book.has(5))
                self.assertEqual(book.volume(worse), 10)
                self.assertEqual(book.volume(100), 5)
                book.clear()
                book.limit(side, 2, 1, 100)
                self.assertTrue(book.has(2))

    def test_reinitialization_and_independent_books(self):
        first, second = LimitOrderBook(), LimitOrderBook()
        first.limit_sell(1, 10, 100)
        second.limit_buy(1, 20, 99)
        first.__init__()
        self.assertEqual(first.count(), 0)
        self.assertEqual(second.volume(), 20)
        first.limit_sell(1, 30, 101)
        self.assertEqual(first.volume(), 30)
        self.assertEqual(second.best_buy(), 99)

    def test_subclasses_attributes_weakrefs_and_cycles(self):
        class NamedBook(LimitOrderBook):
            def __init__(self, name):
                super().__init__()
                self.name = name

        book = NamedBook('orders')
        book.limit_sell(1, 10, 100)
        book.cycle = book
        self.assertEqual(book.name, 'orders')
        reference = weakref.ref(book)
        del book
        gc.collect()
        self.assertIsNone(reference())

    def test_failed_subclass_initialization_is_collectible(self):
        references = []

        class FailingBook(LimitOrderBook):
            def __init__(self):
                self.limit_sell(1, 10, 100)
                references.append(weakref.ref(self))
                raise ValueError('initialization failed')

        with self.assertRaisesRegex(ValueError, 'initialization failed'):
            FailingBook()
        gc.collect()
        self.assertIsNone(references[0]())
