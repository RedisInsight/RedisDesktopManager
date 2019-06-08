class BaseFormatter(object):

    read_only = True

    decode_format = "plain_text"

    def decode(self, value):
        raise NotImplementedError()

    def encode(self, value):
        raise NotImplementedError()

    def validate(self, value):
        try:
            self.decode(value)
            return True
        except Exception:
            return False