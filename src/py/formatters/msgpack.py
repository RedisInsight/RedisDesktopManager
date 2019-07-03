import msgpack
import json

from .base import BaseFormatter


class MsgpackFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        return json.dumps(msgpack.unpackb(value, raw=False))

    def encode(self, value):
        return msgpack.packb(json.loads(value))