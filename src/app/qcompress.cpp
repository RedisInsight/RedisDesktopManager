#include "qcompress.h"

#include <lz4frame.h>
#include <zlib.h>

#include <QDebug>

#define ZLIB_WINDOW_BIT 15 + 16
#define ZLIB_CHUNK_SIZE 32 * 1024
#define ZLIB_LEVEL 6

struct LZ4FCleanUp {
  static inline void cleanup(LZ4F_dctx *p) { LZ4F_freeDecompressionContext(p); }
};

unsigned qcompress::guessFormat(const QByteArray &val) {
  if (val.size() > 2 && val.startsWith(QByteArray::fromHex("x1fx8b"))) {
    return qcompress::GZIP;
  } else if (val.size() > 4 &&
             val.startsWith(QByteArray::fromHex("x04x22x4dx18"))) {
    LZ4F_dctx *lz4_dctx = nullptr;
    LZ4F_createDecompressionContext(&lz4_dctx, LZ4F_VERSION);

    if (!lz4_dctx) {
      qWarning() << "LZ4 error. Cannot initialize context";
      return qcompress::UNKNOWN;
    }

    QScopedPointer<LZ4F_dctx, LZ4FCleanUp> dctx(lz4_dctx);

    LZ4F_frameInfo_t lz4_frameinfo;
    size_t buffSize = val.size();

    size_t res = LZ4F_getFrameInfo(dctx.data(), &lz4_frameinfo,
                                   static_cast<const void *>(val.constData()),
                                   static_cast<size_t *>(&buffSize));

    if (LZ4F_isError(res)) {
      qWarning() << "LZ4 error. Cannot retrive frame info";
      return qcompress::UNKNOWN;
    }

    if (lz4_frameinfo.contentSize > 0) {
      return qcompress::LZ4;
    }
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

QByteArray lz4Decode(const QByteArray &val) {
  LZ4F_dctx *lz4_dctx = nullptr;
  LZ4F_createDecompressionContext(&lz4_dctx, LZ4F_VERSION);

  if (!lz4_dctx) {
    qWarning() << "LZ4 error. Cannot initialize context";
    return QByteArray();
  }

  QScopedPointer<LZ4F_dctx, LZ4FCleanUp> dctx(lz4_dctx);

  LZ4F_frameInfo_t lz4_frameinfo;
  size_t buffSize = val.size();

  size_t res = LZ4F_getFrameInfo(dctx.data(), &lz4_frameinfo,
                                 static_cast<const void *>(val.constData()),
                                 static_cast<size_t *>(&buffSize));

  if (LZ4F_isError(res)) {
    qWarning() << "LZ4 error. Cannot retrive frame info";
    return QByteArray();
  }

  size_t contentSize = lz4_frameinfo.contentSize;
  size_t srcSize = val.size();

  if (!(0 < contentSize && contentSize <= 255 * srcSize)) {
    return QByteArray();
  }

  QByteArray dst(contentSize, '\x00');
  size_t dstSize = dst.size();

  static constexpr const LZ4F_decompressOptions_t opt{};

  res = LZ4F_decompress(dctx.data(), dst.data(), &dstSize,
                        val.data() + buffSize, &srcSize, &opt);

  if (LZ4F_isError(res)) {
    qWarning() << "LZ4 error. Cannot decode frame" << LZ4F_getErrorName(res);
    return QByteArray();
  }

  return dst;
}

QByteArray qcompress::decompress(const QByteArray &val) {
  switch (guessFormat(val)) {
    case qcompress::GZIP:
      return gzipDecode(val);
    case qcompress::LZ4:
      return lz4Decode(val);
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

QByteArray lz4Encode(const QByteArray &val) {
  QByteArray dst;
  LZ4F_preferences_t opt{};
  opt.frameInfo.contentSize = val.size();
  dst.resize(LZ4F_compressFrameBound(val.size(), &opt));

  size_t res =
      LZ4F_compressFrame(dst.data(), dst.size(), val.data(), val.size(), &opt);

  if (LZ4F_isError(res)) {
    qWarning() << "LZ4 error. Cannot compress frame" << LZ4F_getErrorName(res);
    return QByteArray();
  }

  return dst;
}

QByteArray qcompress::compress(const QByteArray &val, unsigned algo) {
  switch (algo) {
    case qcompress::GZIP:
      return gzipEncode(val);
    case qcompress::LZ4:
      return lz4Encode(val);
    default:
      return QByteArray();
  }
}

QString qcompress::nameOf(unsigned alg) {
  switch (alg) {
    case GZIP:
      return "gzip";
    case LZ4:
      return "lz4";
    case UNKNOWN:
    default:
      return "unknown";
  }
}
