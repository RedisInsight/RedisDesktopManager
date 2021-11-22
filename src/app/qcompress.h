#pragma once
#include <QByteArray>
#include <QString>

namespace qcompress {

enum {
  UNKNOWN,
  GZIP,
  LZ4,
  ZSTD,
  /*
   * MAGENTO session is build on top of the following php lib to store
   *compressed sessions:
   * https://github.com/colinmollenhour/php-redis-session-abstract/blob/5f399c53534cd1fe07460407e510590840b2c6d0/src/Cm/RedisSession/Handler.php#L822-L828
   **/
  MAGENTO_SESSION_GZIP,
  MAGENTO_SESSION_LZ4,
  MAGENTO_SESSION_SNAPPY,
  // MAGENTO_SESSION_ZSTD, // ZSTD is not yet supported -
  // https://github.com/colinmollenhour/php-redis-session-abstract/issues/42

  /*
   * MAGENTO CACHE
   * https://github.com/colinmollenhour/Cm_Cache_Backend_Redis/blob/a9c4a5ae6001e04097aa7302abd89c9496c563e0/Cm/Cache/Backend/Redis.php#L1202-L1207
   */
  MAGENTO_CACHE_GZIP,
  MAGENTO_CACHE_LZ4,
  MAGENTO_CACHE_ZSTD,
  MAGENTO_CACHE_SNAPPY,

  SNAPPY,
  GZIP_PHP,
  LZ4_RAW,
  BROTLI
};

unsigned guessFormat(const QByteArray& val);

QString nameOf(unsigned alg);

QByteArray decompress(const QByteArray& val, unsigned algo);

QByteArray compress(const QByteArray& val, unsigned algo);

}  // namespace qcompress
