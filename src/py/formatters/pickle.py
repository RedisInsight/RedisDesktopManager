import pickle
import json

from .base import BaseFormatter


class PickleFormatter(BaseFormatter):

    read_only = False

    decode_format = "json"

    def decode(self, value):
        read_only = self.read_only
        deserialized = ''
        error = ''

        try:
            deserialized = pickle.loads(value)
        except pickle.UnpicklingError as e:
            read_only = True
            error = 'Value cannot be deserialized with pickle: {} ' \
                    '(value: {})'.format(e, value)

        return {
            'output': json.dumps(deserialized, ensure_ascii=False),
            'read-only': read_only,
            'error': error
        }

    def encode(self, value):
        return pickle.dumps(json.loads(value))
