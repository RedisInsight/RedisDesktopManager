import msgpack
import json

from .base import BaseFormatter


class MsgpackFormatter(BaseFormatter):

    decode_format = "json"

    def decode(self, value):
        return json.dumps(msgpack.unpackb(value, raw=False))
