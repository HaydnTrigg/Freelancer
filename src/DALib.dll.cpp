#include "zlib.h"

// zlib compression/decompression wrapper.
//
//   +0x00  vtable
//   +0x04  z_stream m_deflate   (56 bytes)
//   +0x3C  z_stream m_inflate   (56 bytes)
//   +0x74  uLong    m_compressedSize     (last deflate total_out)
//   +0x78  uLong    m_decompressedSize   (last inflate total_out)
class Compressor
{
public:
    Compressor();
    virtual ~Compressor();

    bool Compress(void *src, uInt srcLen, void *dst, uInt dstLen);
    bool Decompress(void *src, uInt srcLen, void *dst, uInt dstLen);

private:
    z_stream m_deflate;
    z_stream m_inflate;
    uLong    m_compressedSize;
    uLong    m_decompressedSize;
};

Compressor::Compressor()
{
    m_deflate.zalloc = Z_NULL;
    m_deflate.zfree = Z_NULL;
    m_deflate.opaque = Z_NULL;
    deflateInit(&m_deflate, 6);

    m_inflate.zalloc = Z_NULL;
    m_inflate.zfree = Z_NULL;
    m_inflate.opaque = Z_NULL;
    m_inflate.next_in = Z_NULL;
    m_inflate.avail_in = 0;
    inflateInit(&m_inflate);

    m_decompressedSize = 0;
    m_compressedSize = 0;
}

Compressor::~Compressor()
{
    deflateEnd(&m_deflate);
    inflateEnd(&m_inflate);
}

bool Compressor::Compress(void *src, uInt srcLen, void *dst, uInt dstLen)
{
    if (srcLen < 0x20)
        return false;

    m_deflate.next_in = (Bytef *)src;
    m_deflate.avail_in = srcLen;
    m_deflate.next_out = (Bytef *)dst;
    m_deflate.avail_out = dstLen;

    int ret = deflate(&m_deflate, Z_FINISH);
    m_compressedSize = m_deflate.total_out;
    deflateReset(&m_deflate);
    return ret == Z_STREAM_END;
}

bool Compressor::Decompress(void *src, uInt srcLen, void *dst, uInt dstLen)
{
    m_inflate.next_in = (Bytef *)src;
    m_inflate.avail_in = srcLen;
    m_inflate.next_out = (Bytef *)dst;
    m_inflate.avail_out = dstLen;

    int ret = inflate(&m_inflate, Z_FINISH);
    m_decompressedSize = m_inflate.total_out;
    inflateReset(&m_inflate);
    return ret == Z_STREAM_END;
}
