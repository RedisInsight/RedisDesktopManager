class BaseFormatter(object):

    read_only = True

    decode_format = "plain_text"

    def decode(self, value):
        raise NotImplementedError()

    def encode(self, value):
        raise NotImplementedError()

    def validate(self, value):
        try:
            result = self.decode(value)

            if type(result) is dict:
                err = result.get('error', '')
                return [err == '', err]
            else:
                return [True, ""]
        except Exception as e:
            return [False, str(e)]
