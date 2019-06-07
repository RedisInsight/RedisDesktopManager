import bitstring

from .base import BaseFormatter


class BinaryFormatter(BaseFormatter):

    def decode(self, value):
        return bitstring.BitArray(value).bin
