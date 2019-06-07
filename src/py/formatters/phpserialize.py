import phpserialize
import json

from .base import BaseFormatter


class PhpSerializeFormatter(BaseFormatter):

    decode_format = "json"

    def decode(self, value):
        return json.dumps(phpserialize.loads(value, decode_strings=True))
