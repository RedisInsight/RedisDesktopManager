#include "qcompress.h"
#include <zlib.h>

#define ZLIB_WINDOW_BIT 15 + 16
#define ZLIB_CHUNK_SIZE 32 * 1024
#define ZLIB_LEVEL 6

unsigned qcompress::guessFormat(const QByteArray &val) {
  if (val.size() > 2 && val.startsWith(QByteArray::fromHex("x1fx8b"))) {
    return qcompress::GZIP;
  }

  return qcompress::UNKNOWN;
}

QByteArray gzipDecode(const QByteArray &val) {
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  QByteArray output;

  int ret = inflateInit2(&strm, ZLIB_WINDOW_BIT);

  if (ret != Z_OK) return QByteArray();

  const char *input_data = val.data();
  int input_data_left = val.length();

  do {
    int chunk_size = qMin(ZLIB_CHUNK_SIZE, input_data_left);

    if (chunk_size <= 0) break;

    strm.next_in = (unsigned char *)input_data;
    strm.avail_in = chunk_size;

    input_data += chunk_size;
    input_data_left -= chunk_size;

    do {
      char out[ZLIB_CHUNK_SIZE];

      strm.next_out = (unsigned char *)out;
      strm.avail_out = ZLIB_CHUNK_SIZE;

      ret = inflate(&strm, Z_NO_FLUSH);

      switch (ret) {
        case Z_NEED_DICT:
          ret = Z_DATA_ERROR;
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
        case Z_STREAM_ERROR:
          inflateEnd(&strm);
          return QByteArray();
      }

      int have = (ZLIB_CHUNK_SIZE - strm.avail_out);

      if (have > 0) output.append((char *)out, have);
    } while (strm.avail_out == 0);
  } while (ret != Z_STREAM_END);

  inflateEnd(&strm);

  if (ret == Z_STREAM_END) {
    return output;
  } else {
    return QByteArray();
  }
}

QByteArray qcompress::decompress(const QByteArray &val) {
  switch (guessFormat(val)) {
    case qcompress::GZIP:
      return gzipDecode(val);
    default:
      return QByteArray();
  }
}

QByteArray gzipEncode(const QByteArray &val) {
  int flush = 0;

  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  QByteArray output;

  int ret = deflateInit2(&strm, qMax(-1, qMin(9, ZLIB_LEVEL)), Z_DEFLATED,
                         ZLIB_WINDOW_BIT, 8, Z_DEFAULT_STRATEGY);

  if (ret != Z_OK) return output;

  const char *input_data = val.data();
  int input_data_left = val.length();

  do {
    int chunk_size = qMin(ZLIB_CHUNK_SIZE, input_data_left);

    strm.next_in = (unsigned char *)input_data;
    strm.avail_in = chunk_size;

    input_data += chunk_size;
    input_data_left -= chunk_size;

    flush = (input_data_left <= 0 ? Z_FINISH : Z_NO_FLUSH);

    do {
      char out[ZLIB_CHUNK_SIZE];

      strm.next_out = (unsigned char *)out;
      strm.avail_out = ZLIB_CHUNK_SIZE;

      ret = deflate(&strm, flush);

      if (ret == Z_STREAM_ERROR) {
        deflateEnd(&strm);
        return QByteArray();
      }

      int have = (ZLIB_CHUNK_SIZE - strm.avail_out);

      if (have > 0) output.append((char *)out, have);
    } while (strm.avail_out == 0);
  } while (flush != Z_FINISH);

  (void)deflateEnd(&strm);

  if (ret == Z_STREAM_END) {
    return output;
  } else {
    return QByteArray();
  }
}

QByteArray qcompress::compress(const QByteArray &val, unsigned algo) {
  switch (algo) {
    case qcompress::GZIP:
      return gzipEncode(val);
    default:
      return QByteArray();
  }
}

QString qcompress::nameOf(unsigned alg) {
  switch (alg) {
    case GZIP:
      return "gzip";
    case UNKNOWN:
    default:
      return "unknown";
  }
}
