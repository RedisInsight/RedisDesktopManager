#pragma once
#include <QByteArray>
#include <QString>

namespace qcompress {

enum { UNKNOWN, GZIP, LZ4, ZSTD };

unsigned guessFormat(const QByteArray& val);

QString nameOf(unsigned alg);

QByteArray decompress(const QByteArray& val);

QByteArray compress(const QByteArray& val, unsigned algo);

}  // namespace qcompress
