import base64
from datetime import datetime
import io
import json

import msgpack

from .base import BaseFormatter


class MsgpackFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        unpacked = ''
        error = ''

        try:
            unpacked = msgpack.unpackb(value, raw=False, timestamp=3)
        except msgpack.ExtraData as e:
            self.read_only = True

            buf = io.BytesIO(value)
            unpacker = msgpack.Unpacker(buf, raw=False)
            for data in unpacker:
                unpacked = data
                error = f'First object from the stream is shown, ' \
                        f'value was truncated by {len(e.extra)} bytes.'

        return {'output': json.dumps(unpacked, default=self.default),
                'error': error}

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
