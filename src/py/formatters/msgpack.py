import base64
from datetime import datetime
import json

import msgpack

from .base import BaseFormatter


class MsgpackFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        try:
            unpacked = msgpack.unpackb(value, raw=False, timestamp=3)
        except Exception as e:
            return json.dumps(f'Formatting error: {e}')

        try:
            return json.dumps(unpacked, default=self.default)
        except Exception as e:
            return json.dumps(e)

    def encode(self, value):
        return msgpack.packb(json.loads(value))

    def default(self, o):
        self.read_only = True

        if isinstance(o, msgpack.Timestamp):
            return o.to_datetime().isoformat()

        elif isinstance(o, datetime):
            return o.isoformat()

        elif isinstance(o, bytes):
            try:
                return o.decode()
            except UnicodeDecodeError:
                return base64.b64encode(o)
        else:
            return str(o)
