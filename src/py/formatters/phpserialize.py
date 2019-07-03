import phpserialize
import json

from .base import BaseFormatter


class PhpSerializeFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        return json.dumps(phpserialize.loads(value, decode_strings=True))

    def encode(self, value):
        return phpserialize.dumps(json.loads(value))