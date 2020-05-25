import phpserialize
import json

from .base import BaseFormatter


class PhpSerializeFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        self.read_only = self.__class__.read_only
        return json.dumps(phpserialize.loads(value, decode_strings=True),
                          ensure_ascii=False)

    def encode(self, value):
        return phpserialize.dumps(json.loads(value))
