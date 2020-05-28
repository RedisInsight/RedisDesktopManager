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
                                                              'h': '喂'}}},
        b'O:8:"stdClass":2:{s:3:"foo";s:3:"bar";s:3:"bar";s:3:"baz";}'
    )
    def test_decode(self, val):
        if type(val) == bytes:
            serialized_val = val
            val = phpserialize.loads(val, decode_strings=True,
                                     object_hook=phpserialize.phpobject)
            expected_output = json.dumps(val._asdict(), ensure_ascii=False)
        else:
            serialized_val = phpserialize.dumps(val)
            expected_output = json.dumps(val, ensure_ascii=False)

        formatter_response_dict = self.formatter.decode(serialized_val)

        self.assertIn('output', formatter_response_dict)

        actual_output = formatter_response_dict['output']
        self.assertEqual(actual_output, expected_output)

    @data(
        'test',
        {'喂': 'test'},
    )
    def test_encode(self, val):
        formatter_input = json.dumps(val, ensure_ascii=False)
        expected_output = phpserialize.dumps(val)

        output = self.formatter.encode(formatter_input)
        self.assertEqual(output, expected_output)
