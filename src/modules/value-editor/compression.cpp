#include "compression.h"

/**
 * @brief Attempt to detect if the blob is compressed using the standard GZIP algorithm. May return false positives
 * @param input The buffer to be compressed
 * @return @c true if the input buffer is compressed.
 */
bool ValueEditor::Compression::isCompressed(QByteArray input) 
{
	return !input.isEmpty() && input.size() > 2 && (unsigned char)input.at(0) == 0x1f && (unsigned char)input.at(1) == 0x8b;
}

/**
 * @brief Compresses the given buffer using the standard GZIP algorithm
 * @param input The buffer to be compressed
 * @param output The result of the compression
 * @param level The compression level to be used (@c 0 = no compression, @c 9 = max, @c -1 = default)
 * @return @c true if the compression was successful, @c false otherwise
 */
bool ValueEditor::Compression::compress(QByteArray input, QByteArray &output, int level)
{
    output.clear();

    if(input.length())
    {
        int flush = 0;

        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        int ret = deflateInit2(&strm, qMax(-1, qMin(9, level)), Z_DEFLATED, WINDOW_BIT, 8, Z_DEFAULT_STRATEGY);

        if (ret != Z_OK)
            return false;

        char *input_data = input.data();
        int input_data_left = input.length();

        do {
            int chunk_size = qMin(CHUNK_SIZE, input_data_left);

            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            input_data += chunk_size;
            input_data_left -= chunk_size;

            flush = (input_data_left <= 0 ? Z_FINISH : Z_NO_FLUSH);

            do {
                char out[CHUNK_SIZE];

                strm.next_out = (unsigned char*)out;
                strm.avail_out = CHUNK_SIZE;

                ret = deflate(&strm, flush);

                if(ret == Z_STREAM_ERROR)
                {
                    deflateEnd(&strm);
                    return false;
                }

                int have = (CHUNK_SIZE - strm.avail_out);

                if(have > 0)
                    output.append((char*)out, have);
            } while (strm.avail_out == 0);
        } while (flush != Z_FINISH);

        (void)deflateEnd(&strm);

        return ret == Z_STREAM_END;
    }
    return true;
}

/**
 * @brief Decompresses the given buffer using the standard GZIP algorithm
 * @param input The buffer to be decompressed
 * @param output The result of the decompression
 * @return @c true if the decompression was successfull, @c false otherwise
 */
bool ValueEditor::Compression::decompress(QByteArray input, QByteArray &output)
{
    output.clear();

    if(input.length() > 0)
    {
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = 0;
        strm.next_in = Z_NULL;

        int ret = inflateInit2(&strm, WINDOW_BIT);

        if (ret != Z_OK)
            return false;

        char *input_data = input.data();
        int input_data_left = input.length();

        do {
            int chunk_size = qMin(CHUNK_SIZE, input_data_left);

            if(chunk_size <= 0)
                break;

            strm.next_in = (unsigned char*)input_data;
            strm.avail_in = chunk_size;

            input_data += chunk_size;
            input_data_left -= chunk_size;

            do {
                char out[CHUNK_SIZE];

                strm.next_out = (unsigned char*)out;
                strm.avail_out = CHUNK_SIZE;

                ret = inflate(&strm, Z_NO_FLUSH);

                switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                case Z_STREAM_ERROR:
                    inflateEnd(&strm);
                    return false;
                }

                int have = (CHUNK_SIZE - strm.avail_out);

                if(have > 0)
                    output.append((char*)out, have);
            } while (strm.avail_out == 0);
        } while (ret != Z_STREAM_END);

        inflateEnd(&strm);

        return ret == Z_STREAM_END;
    }
    return true;
}
