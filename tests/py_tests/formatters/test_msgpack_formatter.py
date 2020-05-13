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
    def test_formatting(self, val):
        msgpack_val = msgpack.packb(val)
        formatted = self.formatter.decode(msgpack_val)
        json_val = json.dumps(val, default=self.formatter.default)

        self.assertEqual(formatted, json_val)
