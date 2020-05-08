import msgpack
import json

from .base import BaseFormatter


class MsgpackFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        try:
            return json.dumps(msgpack.unpackb(value, raw=False),
                              default=self.default)
        except Exception as e:
            return json.dumps(e)

    def encode(self, value):
        return msgpack.packb(json.loads(value))

    def default(self, o):
        self.read_only = True
        if isinstance(o, msgpack.Timestamp):

            return str(o)
