/* zlib.h -- interface of the 'zlib' general purpose compression library
   version 1.1.4, March 11th, 2002

   Minimal subset used by DALib's compression wrapper, matching the layout and
   calling convention of the zlib statically linked into the shipped binary.
*/

#ifndef ZLIB_H
#define ZLIB_H

#define ZLIB_VERSION "1.1.4"

#ifdef __cplusplus
extern "C" {
#endif

/* Windows DLL builds of zlib export with __stdcall (ZEXPORT). */
#define ZEXPORT __stdcall

typedef unsigned char  Bytef;
typedef unsigned int   uInt;
typedef unsigned long  uLong;
typedef void          *voidpf;

typedef voidpf (*alloc_func)(voidpf opaque, uInt items, uInt size);
typedef void   (*free_func)(voidpf opaque, voidpf address);

struct internal_state;

typedef struct z_stream_s {
    Bytef    *next_in;   /* next input byte */
    uInt      avail_in;  /* number of bytes available at next_in */
    uLong     total_in;  /* total nb of input bytes read so far */

    Bytef    *next_out;  /* next output byte should be put there */
    uInt      avail_out; /* remaining free space at next_out */
    uLong     total_out; /* total nb of bytes output so far */

    char     *msg;       /* last error message, NULL if no error */
    struct internal_state *state; /* not visible by applications */

    alloc_func zalloc;   /* used to allocate the internal state */
    free_func  zfree;    /* used to free the internal state */
    voidpf     opaque;   /* private data object passed to zalloc and zfree */

    int        data_type; /* best guess about the data type: ascii or binary */
    uLong      adler;     /* adler32 value of the uncompressed data */
    uLong      reserved;  /* reserved for future use */
} z_stream;

typedef z_stream *z_streamp;

/* constants */
#define Z_NO_FLUSH      0
#define Z_FINISH        4

#define Z_OK            0
#define Z_STREAM_END    1

#define Z_DEFAULT_COMPRESSION  (-1)

#define Z_NULL  0

extern int ZEXPORT deflate(z_streamp strm, int flush);
extern int ZEXPORT deflateEnd(z_streamp strm);
extern int ZEXPORT deflateReset(z_streamp strm);

extern int ZEXPORT inflate(z_streamp strm, int flush);
extern int ZEXPORT inflateEnd(z_streamp strm);
extern int ZEXPORT inflateReset(z_streamp strm);

extern int ZEXPORT deflateInit_(z_streamp strm, int level,
                                const char *version, int stream_size);
extern int ZEXPORT inflateInit_(z_streamp strm,
                                const char *version, int stream_size);

#define deflateInit(strm, level) \
        deflateInit_((strm), (level), ZLIB_VERSION, sizeof(z_stream))
#define inflateInit(strm) \
        inflateInit_((strm), ZLIB_VERSION, sizeof(z_stream))

#ifdef __cplusplus
}
#endif

#endif /* ZLIB_H */
