#include "test_compression.h"
#include "value-editor/compression.h"

void TestCompression::testCompression()
{
#ifdef Q_OS_WIN
    QSKIP("SKIP ON Windows");
#endif
    unsigned char hello_world[] = {
      0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xcb, 0x48,
      0xcd, 0xc9, 0xc9, 0x57, 0x28, 0xcf, 0x2f, 0xca, 0x49, 0x51, 0x04, 0x00,
      0x6d, 0xc2, 0xb4, 0x03, 0x0c, 0x00, 0x00, 0x00
    };
    QByteArray plainText("hello world!");
    QByteArray compressedString((const char*)hello_world, 32);

    bool actualResult = ValueEditor::Compression::isCompressed(compressedString);
    QCOMPARE(true, actualResult);

    QByteArray actualUncompressedResult;
    actualResult = ValueEditor::Compression::decompress(compressedString, actualUncompressedResult);
    QCOMPARE(true, actualResult);
    QCOMPARE(plainText, actualUncompressedResult);

    QByteArray actualCompressedResult;
    actualResult = ValueEditor::Compression::compress(plainText, actualCompressedResult);
    QCOMPARE(true, actualResult);
    QCOMPARE(compressedString, actualCompressedResult);
}
