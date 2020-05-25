import json
import unittest

from ddt import ddt, data
import phpserialize

from src.py.formatters.phpserialize import PhpSerializeFormatter


@ddt
class TestPhpSerializeFormatter(unittest.TestCase):
    formatter = PhpSerializeFormatter()

    @data(
        'test',
        {'a': 1, 'b': 'ъъъ', 'c': None, 'd': '✓', 'e': {'f': {'g': '🔫',
                                                              'h': '喂'}}}
    )
    def test_decode(self, val):
        expected_output = json.dumps(val, ensure_ascii=False)
        serialized_val = phpserialize.dumps(val)

        actual_output = self.formatter.decode(serialized_val)

        self.assertEqual(actual_output, expected_output)

    def test_encode(self):
        val = json.dumps({'喂': 'test'})
        expected_output = phpserialize.dumps({'喂': 'test'})

        output = self.formatter.encode(val)
        self.assertEqual(output, expected_output)
