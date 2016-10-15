#ifndef _TRI_HEXDUMP_H
#define _TRI_HEXDUMP_H

#include "core.h"

enum {
	DUMP_PREFIX_NONE,
	DUMP_PREFIX_ADDRESS,
	DUMP_PREFIX_OFFSET
};

extern const char hex_asc[];
extern const char hex_asc_upper[];

extern int hex_to_bin(char ch);
extern int hex2bin(u8 *dst, const char *src, size_t count);
extern char *bin2hex(char *dst, const void *src, size_t count);
extern void hex_dump_to_buffer(const void *buf, size_t len, int rowsize,
			int groupsize, char *linebuf, size_t linebuflen,
			bool ascii);
extern void fprint_hex_dump(FILE *fp, const char *level, const char *prefix_str,
			    int prefix_type, int rowsize, int groupsize,
			    const void *buf, size_t len, bool ascii);

#define print_hex_dump(level, prefix_str, prefix_type, rowsize,		\
		       groupsize, buf, len, ascii)			\
		fprint_hex_dump(stdout, level, prefix_str, prefix_type, \
				rowsize, groupsize, buf, len, ascii)

#endif /* _TRI_HEXDUMP_H */
