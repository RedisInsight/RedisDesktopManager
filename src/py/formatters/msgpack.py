import base64
import io
import json

import msgpack

from .base import BaseFormatter


class MsgpackFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        read_only = self.read_only
        unpacked = ''
        error = ''

        try:
            unpacked = msgpack.loads(value, raw=False, strict_map_key=False)
        except msgpack.ExtraData as e:
            read_only = True

            buf = io.BytesIO(value)
            unpacker = msgpack.Unpacker(buf, raw=False, strict_map_key=False)
            for data in unpacker:
                unpacked = data
                error = ('First object from the stream is shown, value was '
                         'truncated by {extra_len} bytes.'
                         .format(extra_len=len(e.extra)))
                break

        return {
            'output': json.dumps(unpacked,
                                 default=self.default,
                                 ensure_ascii=False),
            'read-only': read_only,
            'error': error
        }

    def encode(self, value):
        return msgpack.dumps(json.loads(value))

    @staticmethod
    def default(o):
        if isinstance(o, msgpack.Timestamp):
            return o.to_datetime().isoformat()

        elif isinstance(o, bytes):
            try:
                return o.decode()
            except UnicodeDecodeError:
                return base64.b64encode(o)
        else:
            return str(o)
