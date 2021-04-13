import json
import pickle

try:
    import numpy as np
    import pandas as pd
    numpy_support = True
except ImportError:
    numpy_support = False

from .base import BaseFormatter


class PickleFormatter(BaseFormatter):

    decode_format = "json"

    def decode(self, value):
        def get_json_output(deserialized_object):
            return json.dumps(deserialized_object,
                              default=self.default,
                              ensure_ascii=False)

        read_only = self.read_only
        decode_format = self.decode_format
        deserialized = ''
        output = ''
        error = ''

        try:
            deserialized = pickle.loads(value)
        except pickle.UnpicklingError as e:
            read_only = True
            error = 'Value cannot be deserialized with pickle: {} ' \
                    '(value: {})'.format(e, value)

        if numpy_support:
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
                output = get_json_output(deserialized)
        else:
            output = get_json_output(deserialized)

        return {
            'output': output,
            'decode-format': decode_format,
            'read-only': read_only,
            'error': error
        }

    @staticmethod
    def default(o):
        if numpy_support:
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
        else:
            return str(o)

    @staticmethod
    def format_html_output(data, html):
        style = '<style type="text/css">' \
                'th, td { padding: 5px 15px 5px 0px; ' \
                'text-align: left; }</style>'
        return '{}<p><b>{}</b></p>{}'.format(style, str(type(data))[1:-1], html)
