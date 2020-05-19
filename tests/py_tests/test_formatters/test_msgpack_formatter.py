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
    )
    def test_decode(self, val):
        expected_output = json.dumps(val, default=self.formatter.default)
        msgpacked_val = msgpack.packb(val)

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
                                     default=self.formatter.default)
        buf = io.BytesIO()

        buf.write(msgpack.packb(stream['valid'], use_bin_type=True))
        buf.write(msgpack.packb(stream['extra'], use_bin_type=True))

        broken_val = buf.getvalue()[:-5]
        formatter_response_dict = self.formatter.decode(broken_val)

        self.assertIn('output', formatter_response_dict)

        actual_output = formatter_response_dict['output']
        self.assertEqual(actual_output, expected_output)

    def test_encode(self):
        val = json.dumps('test')
        expected_output = msgpack.packb('test')

        output = self.formatter.encode(val)
        self.assertEqual(output, expected_output)
