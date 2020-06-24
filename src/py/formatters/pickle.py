import json
import pickle

import numpy as np
import pandas as pd

from .base import BaseFormatter


class PickleFormatter(BaseFormatter):

    decode_format = "json"

    def decode(self, value):
        read_only = self.read_only
        decode_format = self.decode_format
        deserialized = ''
        error = ''

        try:
            deserialized = pickle.loads(value)
        except pickle.UnpicklingError as e:
            read_only = True
            error = 'Value cannot be deserialized with pickle: {} ' \
                    '(value: {})'.format(e, value)

        if isinstance(deserialized, pd.Series):
            output = f'{str(type(deserialized))[1:-1]}\n' \
                     f'{deserialized.to_string()}'
            decode_format = 'plain_text'
        elif isinstance(deserialized, pd.DataFrame):
            html = deserialized.to_html(render_links=True, border=0)
            output = self.format_html_output(deserialized, html)
            decode_format = 'html'
        elif isinstance(deserialized, np.ndarray):
            html = pd.DataFrame(deserialized).to_html(render_links=True,
                                                      border=0)
            output = self.format_html_output(deserialized, html)
            decode_format = 'html'
        else:
            output = json.dumps(deserialized,
                                default=self.default,
                                ensure_ascii=False)
        return {
            'output': output,
            'decode-format': decode_format,
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
            return o.to_json()
        if isinstance(o, pd.DataFrame):
            return json.loads(o.to_json(orient='index', date_format='iso'))
        else:
            return str(o)

    @staticmethod
    def format_html_output(data, html):
        style = '<style type="text/css">' \
                'th, td { padding: 5px 15px 5px 0px; ' \
                'text-align: left; }</style>'
        return '{}<p><b>{}</b></p>{}'.format(style, str(type(data))[1:-1], html)
