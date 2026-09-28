import zlib;
import core.stdc.stdio : printf;
import core.stdc.string : strlen, memcmp;

void main()
{
    const(char)* original = "The quick brown fox jumps over the lazy dog. ".ptr;
    uint originalLen = cast(uint) strlen(original);

    ubyte[256] compressed;
    uLongf compressedLen = compressed.length;
    int rc = compress(compressed.ptr, &compressedLen, cast(const(ubyte)*) original, originalLen);
    assert(rc == Z_OK, "compress() failed");
    printf("compressed %u bytes down to %lu\n", originalLen, compressedLen);

    ubyte[256] decompressed;
    uLongf decompressedLen = decompressed.length;
    rc = uncompress(decompressed.ptr, &decompressedLen, compressed.ptr, compressedLen);
    assert(rc == Z_OK, "uncompress() failed");
    assert(decompressedLen == originalLen, "round-trip length mismatch");
    assert(memcmp(decompressed.ptr, original, originalLen) == 0, "round-trip content mismatch");

    printf("ZLIB ZERO-WRAPPER ROUNDTRIP PASSED\n");
}
