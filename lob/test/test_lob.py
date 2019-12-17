"""Test cases for the lob module."""
from unittest import TestCase
from .. import lob


class ShouldInitializeLimitOrderBook(TestCase):
    def test(self):
        self.assertIsInstance(lob.LimitOrderBook(), lob.LimitOrderBook)
