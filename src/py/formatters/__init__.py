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
    except Exception as e:
        error = (
            "Embedded formatter %s error: %s (value: %s)"
            % (name, str(e), value)
        )
        result = ""

    return [error, result, formatter.read_only, formatter.decode_format]


def validate(name, value):
    return ENABLED_FORMATTERS[name].validate(value)


def encode(name, value):
    pass
