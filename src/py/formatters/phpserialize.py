import phpserialize
import json

from .base import BaseFormatter


class PhpSerializeFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        read_only = self.read_only
        deserialized = ''
        error = ''

        try:
            deserialized = phpserialize.loads(
                value, decode_strings=True, object_hook=phpserialize.phpobject)

        except ValueError as e:
            read_only = True
            error = 'Value cannot be unserialized: {} (value: {})'.format(
                e, value)

        return {
            'output': json.dumps(deserialized, ensure_ascii=False,
                                 default=self.default),
            'read-only': read_only,
            'error': error
        }

    def encode(self, value):
        return phpserialize.dumps(json.loads(value))

    @staticmethod
    def default(o):
        if isinstance(o, phpserialize.phpobject):
            return o._asdict()
