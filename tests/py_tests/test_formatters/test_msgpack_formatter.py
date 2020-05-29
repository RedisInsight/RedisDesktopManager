import io
import json
import unittest

from ddt import ddt, data
import msgpack

from src.py.formatters.msgpack import MsgpackFormatter


@ddt
class TestMsgpackFormatter(unittest.TestCase):
    formatter = MsgpackFormatter()

    @data(
        [1, 2, 3, 4],
        [0, [25, 636905376000000000, 636906075333708700, 55.0, None]],
        [3925794820, 0, msgpack.Timestamp(seconds=1587539993,
                                          nanoseconds=518021200), False],
        [1, msgpack.ExtType(code=1, data=b'text')],
        [1, msgpack.ExtType(code=1, data=b'\x94\x01\x02\x03\x04')],

        # Example from #4781
        b'\xdc\x00\x1f\xcd9\x07\x10\xcf\x08\xd8\x03\x9e\x8f\xf53t\xcd\x01\xae'
        b'\xce\x00\x08\x82:\xa47626\xcc\xca&\xcfH\xd8\x03\xa1\xc4C^\xba\xcfH'
        b'\xd8\x03\x9e\x90\x81\xbf\x01G\xc2\xcd\x05C\xce\x00\x02|Z\xc2\xc2\xcc'
        b'\xe5\xcb@kD\xc97r\x82+\x0b\xcb@S\xc0\x00\x00\x00\x00\x00\xa11'
        b'\xb4hhjjk c \xd0\xbf\xd0\xbe\xd1\x87\xd1\x82\xd0\xbe\xd0\xb9\x01\x85'
        b'\x04\xce\x00\x01\x98\xa9\x03\xce\x00\x02\xdfm\x02\xce\x00\x02\xbc]'
        b'\x01\xce\x00\x02\xa9Q\x00\xce\x00\x0b\x1b\xc5\xcd\x16n\xc0\xc0\xc2'
        b'\xc0\x92\x00\x91\x0b\xc0'
    )
    def test_decode(self, val):
        if type(val) == bytes:
            msgpacked_val = val
            val = msgpack.loads(val, raw=False, strict_map_key=False)
        else:
            msgpacked_val = msgpack.dumps(val)

        expected_output = json.dumps(val, default=self.formatter.default,
                                     ensure_ascii=False)

        formatter_response_dict = self.formatter.decode(msgpacked_val)

        self.assertIn('output', formatter_response_dict)

        actual_output = formatter_response_dict['output']
        self.assertEqual(actual_output, expected_output)

    @data(
        {'valid': ['valid', 'bytes'], 'extra': ['extra', 'bytes']},
        {'valid': [], 'extra': ['extra', 'bytes']},
        {'valid': msgpack.Timestamp(1, 1), 'extra': ['extra', 'bytes']},
    )
    def test_decode_stream(self, stream):
        expected_output = json.dumps(stream['valid'],
                                     default=self.formatter.default,
                                     ensure_ascii=False)
        buf = io.BytesIO()

        buf.write(msgpack.dumps(stream['valid'], use_bin_type=True))
        buf.write(msgpack.dumps(stream['extra'], use_bin_type=True))

        broken_val = buf.getvalue()[:-5]
        formatter_response_dict = self.formatter.decode(broken_val)

        self.assertIn('output', formatter_response_dict)

        actual_output = formatter_response_dict['output']
        self.assertEqual(actual_output, expected_output)

    def test_encode(self):
        val = json.dumps('test')
        expected_output = msgpack.dumps('test')

        output = self.formatter.encode(val)
        self.assertEqual(output, expected_output)
