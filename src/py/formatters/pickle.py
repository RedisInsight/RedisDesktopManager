import pickle
import json

from .base import BaseFormatter


class PickleFormatter(BaseFormatter):

    decode_format = "json"

    def decode(self, value):
        return json.dumps(pickle.loads(value))
