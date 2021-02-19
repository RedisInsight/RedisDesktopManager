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
        # Example from #4789
        b'O:8:"stdClass":2:{s:3:"foo";s:3:"bar";s:3:"bar";s:3:"baz";}',

        # Example from #4942
        b'cookieInfo|i:1;isWholesale|i:0;storageOnly|i:1;isLogged|i:0;'
        b'itemListType|s:3:"std";itemListNum|i:64;search|a:1:{s:5:"group";'
        b's:1:"2";}sr22|a:2:{s:3:"sex";s:7:"0,1,2,3";s:4:"size";s:68:'
        b'"17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,'
        b'37,38,39";}'

        # Foo(foo=Bar(bar=Foo(foo={'a': 1, 'b': 2}), open=False))
        b'O:3:"Foo":1:{s:3:"foo";O:3:"Bar":2:{s:3:"bar";O:3:"Foo":1:{s:3:"foo";'
        b'a:2:{s:1:"a";i:1;s:1:"b";i:2;}}s:4:"open";b:0;}}',

        # Foo(foo=Bar(bar='baz', open=True))
        b'O:3:"Foo":1:{s:3:"foo";O:3:"Bar":2:{s:3:"bar";s:3:"baz";s:4:"open";'
        b'b:1;}}'

        # Foo(foo=Bar(bar=[1, 2], open=True))
        b'O:3:"Foo":1:{s:3:"foo";O:3:"Bar":2:{s:3:"bar";a:2:{i:0;i:1;i:1;i:2;}'
        b's:4:"open";b:1;}}'
    )
    def test_decode(self, val):
        if type(val) == bytes:
            serialized_val = val
            val = phpserialize.loads(val, decode_strings=True,
                                     object_hook=phpserialize.phpobject)
            if not isinstance(val, dict):
                val = val._asdict()
        else:
            serialized_val = phpserialize.dumps(val)
        expected_output = json.dumps(val, ensure_ascii=False,
                                     default=self.formatter.default)

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
