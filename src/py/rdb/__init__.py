import calendar

import rdbtools
from rdbtools.encodehelpers import STRING_ESCAPE_UTF8, STRING_ESCAPE_RAW

VALID_TYPES = ("hash", "set", "string", "list", "sortedset")


def process_command(callback, path_to_rdb, db,
                    include_keys_pattern,
                    exclude_keys_pattern,
                    key_types):
    filters = {}

    if db:
        filters['dbs'] = [int(db)]

    if include_keys_pattern:
        filters['keys'] = include_keys_pattern

    if exclude_keys_pattern:
        filters['not_keys'] = exclude_keys_pattern

    if key_types:
        filters['types'] = []
        for x in key_types:
            if not x in VALID_TYPES:
                raise ValueError(
                    'Invalid type provided - %s. '
                    'Expected one of %s' % (x, (", ".join(VALID_TYPES)))
                )
            else:
                filters['types'].append(x)

    parser = rdbtools.RdbParser(callback=callback, filters=filters)
    parser.parse(path_to_rdb)


def rdb_list_keys(path_to_rdb, db,
                  include_keys_pattern=None,
                  exclude_keys_pattern=None,
                  key_types=None):
    class KeysOnlyCallback(rdbtools.RdbCallback):
        def __init__(self, string_escape=None):
            super(KeysOnlyCallback, self).__init__(string_escape)
            self._out = set()

        def _keyout(self, key):
            self._out.add(self.encode_key(key))

        def set(self, key, value, expiry, info):
            self._keyout(key)

        def start_hash(self, key, length, expiry, info):
            self._keyout(key)

        def hset(self, key, field, value):
            self._keyout(key)

        def start_set(self, key, cardinality, expiry, info):
            self._keyout(key)

        def sadd(self, key, member):
            self._keyout(key)

        def start_list(self, key, expiry, info):
            self._keyout(key)

        def rpush(self, key, value):
            self._keyout(key)

        def start_sorted_set(self, key, length, expiry, info):
            self._keyout(key)

        def zadd(self, key, score, member):
            self._keyout(key)

        def start_stream(self, key, listpacks_count, expiry, info):
            self._keyout(key)

        def start_module(self, key, module_name, expiry, info):
            self._keyout(key)
            return False

        def keys(self):
            return list(self._out)

    callback = KeysOnlyCallback(string_escape=STRING_ESCAPE_UTF8)

    process_command(callback, path_to_rdb, db,
                    include_keys_pattern,
                    exclude_keys_pattern,
                    key_types)

    return callback.keys()


def rdb_export_as_commands(path_to_rdb, db,
                           include_keys_pattern=None,
                           exclude_keys_pattern=None,
                           key_types=None):
    def _unix_timestamp(dt):
        return calendar.timegm(dt.utctimetuple())

    class CommandsCallback(rdbtools.RdbCallback):
        def __init__(self, string_escape=None):
            super(CommandsCallback, self).__init__(string_escape)
            self._commands = []
            self.reset()

        def reset(self):
            self._expires = {}

        def set_expiry(self, key, dt):
            self._expires[key] = dt

        def get_expiry_seconds(self, key):
            if key in self._expires:
                return _unix_timestamp(self._expires[key])
            return None

        def expires(self, key):
            return key in self._expires

        def pre_expiry(self, key, expiry):
            if expiry is not None:
                self.set_expiry(key, expiry)

        def post_expiry(self, key):
            if self.expires(key):
                self.expireat(key, self.get_expiry_seconds(key))

        def emit(self, *args):
            self._commands.append(args)

        def start_database(self, db_number):
            self.reset()
            self.select(db_number)

        # String handling

        def set(self, key, value, expiry, info):
            self.pre_expiry(key, expiry)
            self.emit(b'SET', key, value)
            self.post_expiry(key)

        # Hash handling

        def start_hash(self, key, length, expiry, info):
            self.pre_expiry(key, expiry)

        def hset(self, key, field, value):
            self.emit(b'HSET', key, field, value)

        def end_hash(self, key):
            self.post_expiry(key)

        # Set handling

        def start_set(self, key, cardinality, expiry, info):
            self.pre_expiry(key, expiry)

        def sadd(self, key, member):
            self.emit(b'SADD', key, member)

        def end_set(self, key):
            self.post_expiry(key)

        # List handling

        def start_list(self, key, expiry, info):
            self.pre_expiry(key, expiry)

        def rpush(self, key, value):
            self.emit(b'RPUSH', key, value)

        def end_list(self, key, info):
            self.post_expiry(key)

        # Sorted set handling

        def start_sorted_set(self, key, length, expiry, info):
            self.pre_expiry(key, expiry)

        def zadd(self, key, score, member):
            self.emit(b'ZADD', key, score, member)

        def end_sorted_set(self, key):
            self.post_expiry(key)

        # streams and modules, not currently supported

        def start_stream(self, key, listpacks_count, expiry, info):
            # TODO send RESTORE command
            pass

        def start_module(self, key, module_name, expiry, info):
            # TODO send RESTORE command
            return False

        # Other misc commands

        def select(self, db_number):
            self.emit(b'SELECT', db_number)

        def expireat(self, key, timestamp):
            self.emit(b'EXPIREAT', key, timestamp)

        def commands(self):
            return self._commands

    callback = CommandsCallback(string_escape=STRING_ESCAPE_RAW)

    process_command(callback, path_to_rdb, db,
                    include_keys_pattern,
                    exclude_keys_pattern,
                    key_types)

    return callback.commands()
