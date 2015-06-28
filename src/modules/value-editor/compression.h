#pragma once 

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <zlib.h>
#include <QByteArray>

#define WINDOW_BIT 15 + 16
#define CHUNK_SIZE 32 * 1024

namespace ValueEditor {

class Compression
{
public:
	static bool isCompressed(QByteArray input);
    static bool compress(QByteArray input, QByteArray &output, int level = -1);
    static bool decompress(QByteArray input, QByteArray &output);
};

}
#endif // COMPRESSOR_H