from .binary import BinaryFormatter
from .cbor import CBORFormatter
from .msgpack import MsgpackFormatter
from .phpserialize import PhpSerializeFormatter
from .pickle import PickleFormatter


ENABLED_FORMATTERS = {
    "binary": BinaryFormatter(),
    "cbor": CBORFormatter(),
    "msgpack": MsgpackFormatter(),
    "php": PhpSerializeFormatter(),
    "pickle": PickleFormatter(),
}


def get_formatters_list():
    return list(ENABLED_FORMATTERS.keys())


def decode(name, value):
    formatter = ENABLED_FORMATTERS[name]

    error = ""
    try:
        result = formatter.decode(value)
        read_only = formatter.read_only
        decode_format = formatter.decode_format

        if type(result) is dict:
            result_dict = result
            result = result_dict.get('output', '')
            error = result_dict.get('error', error)
            read_only = result_dict.get('read-only', read_only)
            decode_format = result_dict.get('decode-format', decode_format)

    except Exception as e:
        read_only = True
        error = (
            "Embedded formatter %s error: %s (value: %s)"
            % (name, str(e), value)
        )
        result = ""

    return [error, result, read_only, decode_format]


def validate(name, value):
    return ENABLED_FORMATTERS[name].validate(value)


def encode(name, value):
    formatter = ENABLED_FORMATTERS[name]

    if formatter.read_only:
        return ["Formatter %s doesn't support encoding" % name]

    error = ""

    try:
        result = formatter.encode(value)
    except Exception as e:
        error = (
                "Embedded formatter %s error: %s (value: %s)"
                % (name, str(e), value)
        )
        result = ""

    return [error, result]
