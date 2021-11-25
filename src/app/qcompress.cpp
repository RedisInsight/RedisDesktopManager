#include "qcompress.h"

#include <brotli/decode.h>
#include <brotli/encode.h>
#include <lz4.h>
#include <lz4frame.h>
#include <snappy.h>
#include <zlib.h>
#include <zstd.h>

#include <QDebug>

#define ZLIB_WINDOW_BIT 15 + 16
#define ZLIB_PHP_WINDOW_BIT 15
#define ZLIB_CHUNK_SIZE 32 * 1024
#define ZLIB_LEVEL 6

#define ZSTD_LEVEL 1

#define BROTLI_BUFFER_SIZE 32 * 1024

struct LZ4FCleanUp {
  static inline void cleanup(LZ4F_dctx *p) { LZ4F_freeDecompressionContext(p); }
};

struct ZSTDCleanUp {
  static inline void cleanup(ZSTD_DCtx *p) { ZSTD_freeDCtx(p); }
};

QByteArray gzipDecode(const QByteArray &val, int windowBits) {
  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  QByteArray output;

  int ret = inflateInit2(&strm, windowBits);

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

QByteArray lz4RawDecode(const QByteArray &val) {
  int offset = sizeof(int);

  if (val.size() < offset) {
    return QByteArray();
  }

  int dataSize;
  memcpy(&dataSize, val.data(), offset);

  QByteArray dst(dataSize, '\x00');

  auto res = LZ4_decompress_safe(val.constData() + offset, dst.data(),
                                 val.size() - offset, dst.capacity());

  if (res < 0) {
    qWarning() << "LZ4 raw decoding error";
    return QByteArray();
  }

  return dst;
}

QByteArray lz4RawEncode(const QByteArray &val) {
  int maxSize = LZ4_compressBound(val.size());

  QByteArray dst(maxSize, '\x00');

  int res = LZ4_compress_default(val.constData(), dst.data(), val.size(),
                                 dst.capacity());

  if (res == 0) {
    qWarning() << "LZ4 raw decoding error";
    return QByteArray();
  }

  return dst;
}

QByteArray lz4FrameDecode(const QByteArray &val) {
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

QByteArray gzipEncode(const QByteArray &val, int windowBits) {
  int flush = 0;

  z_stream strm;
  strm.zalloc = Z_NULL;
  strm.zfree = Z_NULL;
  strm.opaque = Z_NULL;
  strm.avail_in = 0;
  strm.next_in = Z_NULL;
  QByteArray output;

  int ret = deflateInit2(&strm, qMax(-1, qMin(9, ZLIB_LEVEL)), Z_DEFLATED,
                         windowBits, 8, Z_DEFAULT_STRATEGY);

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

QByteArray lz4FrameEncode(const QByteArray &val) {
  QByteArray dst;
  LZ4F_preferences_t opt{};
  opt.frameInfo.contentSize = val.size();
  size_t expectedSize = LZ4F_compressFrameBound(val.size(), &opt);
  dst.resize(expectedSize);

  size_t res =
      LZ4F_compressFrame(dst.data(), dst.size(), val.data(), val.size(), &opt);

  if (LZ4F_isError(res)) {
    qWarning() << "LZ4 error. Cannot compress frame" << LZ4F_getErrorName(res);
    return QByteArray();
  }

  if (expectedSize > res) {
      dst.resize(res);
  }

  return dst;
}

QByteArray zstdDecode(const QByteArray &val) {
  size_t buffSize = val.size();
  auto decompressedSize = ZSTD_getFrameContentSize(
      static_cast<const void *>(val.constData()), buffSize);

  if (decompressedSize == 0UL || decompressedSize == ZSTD_CONTENTSIZE_ERROR) {
    return QByteArray();
  }

  size_t srcSize = val.size();  

  ZSTD_DCtx *const zstd_dctx = ZSTD_createDCtx();

  if (!zstd_dctx) {
    qWarning() << "ZSTD error. Cannot initialize context";
    return QByteArray();
  }

  QScopedPointer<ZSTD_DCtx, ZSTDCleanUp> dctx(zstd_dctx);

  QByteArray dst(decompressedSize, '\x00');
  size_t dstSize = dst.size();

  size_t const res =
      ZSTD_decompress(static_cast<void *>(dst.data()), dstSize,
                      static_cast<const void *>(val.data()), srcSize);

  if (ZSTD_isError(res)) {
    qWarning() << "ZSTD error. Cannot decode frame" << ZSTD_getErrorName(res);
    return QByteArray();
  }

  dst.resize(res);

  return dst;
}

QByteArray zstdEncode(const QByteArray &val) {
  QByteArray dst;
  dst.resize(ZSTD_compressBound(val.size()));

  size_t res =
      ZSTD_compress(dst.data(), dst.size(), val.data(), val.size(), ZSTD_LEVEL);

  if (ZSTD_isError(res)) {
    qWarning() << "ZSTD error. Cannot compress frame" << ZSTD_getErrorName(res);
    return QByteArray();
  }

  dst.resize(res);

  return dst;
}

QByteArray snappyDecode(const QByteArray &val) {
  size_t size = 0;

  bool res = snappy::GetUncompressedLength(val.constData(), val.size(), &size);

  if (!res) {
    qWarning() << "Snappy error: Cannot get uncompressed size";
    QByteArray();
  }

  std::string output;

  res = snappy::Uncompress(val.constData(), val.size(), &output);

  if (!res) {
    qWarning() << "Snappy error: Cannot uncompress buffer";
    QByteArray();
  }

  return QByteArray::fromStdString(output);
}

QByteArray snappyEncode(const QByteArray &val) {
  std::string output;

  bool res = snappy::Compress(val.constData(), val.size(), &output);

  if (!res) {
    qWarning() << "Snappy error: Cannot compress buffer";
    QByteArray();
  }

  return QByteArray::fromStdString(output);
}

QByteArray brotliDecode(const QByteArray &val)
{
    auto decoder = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

    if (!decoder) {
        qWarning() << "BROTLI: Cannot create decoder";
        return QByteArray();
    }

    QByteArray dst;
    dst.resize(BROTLI_BUFFER_SIZE);

    size_t availableIn = val.size(), availableOut = dst.size();
    const uint8_t* nextIn = reinterpret_cast<const uint8_t*>(val.constData());
    uint8_t* nextOut = reinterpret_cast<uint8_t*>(dst.data());
    BrotliDecoderResult itResult;

    do {
      itResult = BrotliDecoderDecompressStream(
          decoder, &availableIn, &nextIn, &availableOut, &nextOut, nullptr);
      if (itResult == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
        size_t offset = dst.size() - availableOut;
        availableOut += dst.size();
        dst.resize(dst.size() * 2);
        nextOut = reinterpret_cast<uint8_t *>(dst.data()) + offset;
        itResult = BROTLI_DECODER_RESULT_SUCCESS;
      }

      if (itResult != BROTLI_DECODER_RESULT_SUCCESS) {
          qWarning() << "Brotli: Invalid input";
          return QByteArray();
      }
    } while (!(availableIn == 0 &&
               itResult == BROTLI_DECODER_RESULT_SUCCESS));

    if (itResult == BROTLI_DECODER_RESULT_SUCCESS)
        dst.resize(dst.size() - availableOut);

    BrotliDecoderDestroyInstance(decoder);
    return dst;
}

QByteArray brotliEncode(const QByteArray &val)
{
    auto encoder = BrotliEncoderCreateInstance(nullptr, nullptr, nullptr);

    if (!encoder) {
        qWarning() << "BROTLI: Cannot create encoder";
        return QByteArray();
    }

    QByteArray dst;
    dst.resize(BROTLI_BUFFER_SIZE);

    size_t availableIn = val.size(), available_out = dst.size();
    const uint8_t* nextIn = reinterpret_cast<const uint8_t*>(val.constData());
    uint8_t* nextOut = reinterpret_cast<uint8_t*>(dst.data());
    size_t totalOut = 0;

    int itResult;

    do
    {
        itResult = BrotliEncoderCompressStream
        (
            encoder, BROTLI_OPERATION_FINISH,
            &availableIn, &nextIn, &available_out, &nextOut, &totalOut
        );
    }
    while (!(availableIn == 0 && BrotliEncoderIsFinished(encoder)));

    if (itResult == BROTLI_TRUE) {
        dst.resize(totalOut);
    }

    BrotliEncoderDestroyInstance(encoder);
    return dst;
}

bool validateLZ4Frame(const QByteArray &val) {
  const auto magicHeader = QByteArray::fromHex("x04x22x4dx18");

  if (!val.startsWith(magicHeader)) return false;

  LZ4F_dctx *lz4_dctx = nullptr;
  LZ4F_createDecompressionContext(&lz4_dctx, LZ4F_VERSION);

  if (!lz4_dctx) {
    qWarning() << "LZ4 error. Cannot initialize context";
    return false;
  }

  QScopedPointer<LZ4F_dctx, LZ4FCleanUp> dctx(lz4_dctx);

  LZ4F_frameInfo_t lz4_frameinfo;
  size_t buffSize = val.size();

  size_t res = LZ4F_getFrameInfo(dctx.data(), &lz4_frameinfo,
                                 static_cast<const void *>(val.constData()),
                                 static_cast<size_t *>(&buffSize));

  if (LZ4F_isError(res)) {
    qWarning() << "LZ4 error. Cannot retrive frame info";
    return false;
  }

  return lz4_frameinfo.contentSize > 0;
}

bool validateZSTDFrame(const QByteArray &val) {
  const auto magicHeader = QByteArray::fromHex("x28xB5x2FxFD");

  if (!val.startsWith(magicHeader)) {
    return false;
  }
  size_t buffSize = val.size();
  unsigned long long decompressedSize = ZSTD_getFrameContentSize(
      static_cast<const void *>(val.constData()), buffSize);

  return !(decompressedSize == ZSTD_CONTENTSIZE_ERROR);
}

bool validateGZip(const QByteArray &val, int from = 0) {
  return val.indexOf(QByteArray::fromHex("x1fx8b"), from) == 0;
}

bool validateSnappyFrame(const QByteArray &val) {
  return snappy::IsValidCompressedBuffer(val.constData(), val.size());
}

bool isMagentoCacheFormat(unsigned f) {
  return qcompress::MAGENTO_CACHE_GZIP <= f &&
         f <= qcompress::MAGENTO_CACHE_SNAPPY;
}

QHash<unsigned, QByteArray> knownMagentoFormats() {
  return {
      {qcompress::MAGENTO_CACHE_GZIP, "gz"},
      {qcompress::MAGENTO_SESSION_GZIP, "gz"},
      {qcompress::MAGENTO_CACHE_LZ4, "l4"},
      {qcompress::MAGENTO_SESSION_LZ4, "l4"},
      {qcompress::MAGENTO_CACHE_ZSTD, "zs"},
      {qcompress::MAGENTO_SESSION_SNAPPY, "sn"},
      {qcompress::MAGENTO_CACHE_SNAPPY, "sn"},
  };
}

static const QHash<unsigned, QByteArray> magentoFormats = knownMagentoFormats();

QByteArray magentoPrefix(unsigned f) {
  if (!magentoFormats.contains(f)) {
    return QByteArray();
  }

  QByteArray id = magentoFormats[f];

  if (isMagentoCacheFormat(f)) {
    id += QByteArray(":") + QByteArray::fromHex("x1fx8b");
  } else {
    id = QByteArray(":") + id + QByteArray(":");
  }

  return id;
}

unsigned qcompress::guessFormat(const QByteArray &val) {
  if (val.size() > 4) {
    auto mFormats = magentoFormats.keys();

    QByteArray prefix;

    for (auto f : qAsConst(mFormats)) {
      prefix = magentoPrefix(f);

      if (val.startsWith(prefix)) {
        return f;
      }
    }
  }

  if (val.size() > 2 && validateGZip(val)) {
    return qcompress::GZIP;
  } else if (val.size() > 4 && validateLZ4Frame(val)) {
    return qcompress::LZ4;
  } else if (val.size() > 4 && validateZSTDFrame(val)) {
    return qcompress::ZSTD;
  } else if (val.size() > 10 && validateSnappyFrame(val)) {
    return qcompress::SNAPPY;
  }

  return qcompress::UNKNOWN;
}

QByteArray qcompress::compress(const QByteArray &val, unsigned algo) {
  switch (algo) {
    case qcompress::GZIP:
      return gzipEncode(val, ZLIB_WINDOW_BIT);
    case qcompress::MAGENTO_SESSION_GZIP:
    case qcompress::MAGENTO_CACHE_GZIP:
      return magentoPrefix(algo) + gzipEncode(val, ZLIB_PHP_WINDOW_BIT);
    case qcompress::LZ4:
      return lz4FrameEncode(val);
    case qcompress::MAGENTO_SESSION_LZ4:
    case qcompress::MAGENTO_CACHE_LZ4:
      return magentoPrefix(algo) + lz4RawEncode(val);
    case qcompress::ZSTD:
      return zstdEncode(val);
    case qcompress::MAGENTO_CACHE_ZSTD:
      return magentoPrefix(algo) + zstdEncode(val);
    case qcompress::SNAPPY:
      return snappyEncode(val);
    case qcompress::MAGENTO_CACHE_SNAPPY:
    case qcompress::MAGENTO_SESSION_SNAPPY:
      return magentoPrefix(algo) + snappyEncode(val);
    case qcompress::BROTLI:
      return brotliEncode(val);
    default:
      return QByteArray();
  }
}

QByteArray qcompress::decompress(const QByteArray &val, unsigned format) {
  int offset = 0;

  if (magentoFormats.contains(format)) {
    offset = magentoPrefix(format).size();
  }

  switch (format) {
    case qcompress::GZIP:
      return gzipDecode(val, ZLIB_WINDOW_BIT);
    case qcompress::MAGENTO_SESSION_GZIP:
    case qcompress::MAGENTO_CACHE_GZIP:
    case qcompress::GZIP_PHP:
      return gzipDecode(val.mid(offset), ZLIB_PHP_WINDOW_BIT);
    case qcompress::LZ4:
      return lz4FrameDecode(val);
    case qcompress::MAGENTO_SESSION_LZ4:
    case qcompress::MAGENTO_CACHE_LZ4:
    case qcompress::LZ4_RAW:
      return lz4RawDecode(val.mid(offset));
    case qcompress::ZSTD:
      return zstdDecode(val);
    case qcompress::MAGENTO_CACHE_ZSTD:
      return zstdDecode(val.mid(offset));
    case qcompress::SNAPPY:
      return snappyDecode(val);
    case qcompress::MAGENTO_CACHE_SNAPPY:
    case qcompress::MAGENTO_SESSION_SNAPPY:
      return snappyDecode(val.mid(offset));
    case qcompress::BROTLI:
      return brotliDecode(val);
    default:
      return QByteArray();
  }
}

QString qcompress::nameOf(unsigned alg) {
  switch (alg) {
    case qcompress::GZIP:
      return "gzip";
    case qcompress::LZ4:
      return "lz4";
    case qcompress::MAGENTO_SESSION_GZIP:
      return "magento-session-gzip";
    case qcompress::MAGENTO_SESSION_LZ4:
      return "magento-session-lz4";
    case qcompress::MAGENTO_CACHE_GZIP:
      return "magento-cache-gzip";
    case qcompress::MAGENTO_CACHE_LZ4:
      return "magento-cache-lz4";
    case qcompress::MAGENTO_CACHE_ZSTD:
      return "magento-cache-zstd";
    case qcompress::MAGENTO_CACHE_SNAPPY:
      return "magento-cache-snappy";
    case qcompress::MAGENTO_SESSION_SNAPPY:
      return "magento-session-snappy";
    case qcompress::ZSTD:
      return "ZSTD";
    case qcompress::SNAPPY:
      return "Snappy";
    case qcompress::GZIP_PHP:
      return "PHP gzcompress";
    case qcompress::LZ4_RAW:
      return "LZ4 Raw";
    case qcompress::BROTLI:
      return "Brotli";
    case qcompress::UNKNOWN:
    default:
      return "unknown";
  }
}
