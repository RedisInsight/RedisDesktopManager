import json
import pickle

import numpy as np
import pandas as pd

from .base import BaseFormatter


class PickleFormatter(BaseFormatter):

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
            'output': json.dumps(deserialized,
                                 default=self.default,
                                 ensure_ascii=False),
            'read-only': read_only,
            'error': error
        }

    @staticmethod
    def default(o):
        if isinstance(o, pd.Timestamp):
            return o.isoformat()
        if isinstance(o, np.ndarray):
            return o.tolist()
        if isinstance(o, pd.Series):
            return o.to_list()
        if isinstance(o, pd.DataFrame):
            return json.loads(o.to_json(orient='index', date_format='iso'))
        else:
            return str(o)
