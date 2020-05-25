import cbor
import json
from .base import BaseFormatter


class CBORFormatter(BaseFormatter):

    decode_format = "json"

    def decode(self, value):
        return json.dumps(cbor.loads(value), ensure_ascii=False)
