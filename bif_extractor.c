#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "pack.h"

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

/*
 * BIF v1 structure
 *   20 bytes - header
 *   16 bytes - variable resource table
 *   20 bytes - fixed resource table (not implemented)
 *   x        - variable resource data
 *   x        - fixed resource data
 */
struct bifv1 {
	/*
	 * BIF v1 - Header
	 *   4 uint8_t - file type (BIFF)
	 *   4 uint8_t - version (V1)
	 *   uint32_t  - variable resource count
	 *   uint32_t  - fixed resource count
	 *   uint32_t  - variable table offset
	 */
	struct bifv1_header {
		uint8_t signature[4 + 1];
		uint8_t version[4 + 1];
		uint32_t variable_res_count;
		uint32_t fixed_res_count;
		uint32_t variable_table_offset;
	} header;

	/*
	 * BIF v1 - Variable resource table
	 *   uint32_t  - ID
	 *   uint32_t  - offset
	 *   uint32_t  - file size
	 *   uint32_t  - resource type
	 */
	struct bifv1_vrt {
		uint32_t id;
		uint32_t offset;
		uint32_t size;
		uint32_t type;
	} *vrt; /* use variable resource count for size */
};

#define HEXDUMP_COLS 8
static void hexdump(void *mem, unsigned int len)
{
	unsigned int i, j;

	for(i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++)
	{
		/* print offset */
		if(i % HEXDUMP_COLS == 0)
		{
			printf("0x%06x: ", i);
		}

		/* print hex data */
		if(i < len)
		{
			printf("%02x ", 0xFF & ((char*)mem)[i]);
		}
		else /* end of block, just aligning for ASCII dump */
		{
			printf("   ");
		}

		/* print ASCII dump */
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1))
		{
			for(j = i - (HEXDUMP_COLS - 1); j <= i; j++)
			{
				if(j >= len) /* end of block, not really printing */
				{
					putchar(' ');
				}
				else if(isprint(((char*)mem)[j])) /* printable char */
				{
					putchar(0xFF & ((char*)mem)[j]);        
				}
				else /* other char */
				{
					putchar('.');
				}
			}
			putchar('\n');
		}
	}
}

static int bifv1_read_header(struct bifv1 * bif, uint8_t * buf)
{

	hexdump(buf, 4);
	memcpy(bif->header.signature, buf, 4);
	bif->header.signature[4] = '\0';
	buf += 4;

	hexdump(buf, 4);
	memcpy(bif->header.version, buf, 4);
	bif->header.version[4] = '\0';
	buf += 4;

	hexdump(buf, 4);
	bif->header.variable_res_count = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	hexdump(buf, 4);
	bif->header.fixed_res_count = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	hexdump(buf, 4);
	bif->header.variable_table_offset = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	printf("signature: %s\n", bif->header.signature);
	printf("version: %s\n", bif->header.version);
	printf("variable_res_count: 0x%08x\n", bif->header.variable_res_count);
	printf("fixed_res_count: 0x%08x\n", bif->header.fixed_res_count);
	printf("variable_table_offset: 0x%08x\n", bif->header.variable_table_offset);
	return 0;
}

static int bifv1_read_variable_resource_table(uint8_t *buf, struct bifv1_vrt * vrt)
{
	hexdump(buf, 4);
	vrt->id = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;
	hexdump(buf, 4);
	vrt->offset = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;
	hexdump(buf, 4);
	vrt->size = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;
	hexdump(buf, 4);
	vrt->type = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	printf("id: %u\n", vrt->id);
	printf("offset: %u\n", vrt->offset);
	printf("size: %u\n", vrt->size);
	printf("type: %u\n", vrt->type);
	return 0;
}

/*
 * BIF v1 - Fixed resource table
 *   uint32_t  - ID
 *   uint32_t  - offset
 *   uint32_t  - part count
 *   uint32_t  - file size
 *   uint32_t  - resource type
 */
static int bifv1_read_fixed_resource_table(uint8_t *buf)
{
	uint32_t id, offset, count, size, type;

	hexdump(buf, 4);
	id = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	hexdump(buf, 4);
	offset = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	hexdump(buf, 4);
	count = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	hexdump(buf, 4);
	size = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	hexdump(buf, 4);
	type = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + buf[0];
	buf += 4;

	printf("id: %u\n", id);
	printf("offset: %u\n", offset);
	printf("count: %u\n", count);
	printf("size: %u\n", size);
	printf("type: %u\n", type);
	return 0;
}

/*
 * BIF v1 - Variable resourse data
 *   <x>
 */
/*
 * BIF v1 - Fixed resource data
 *   <y>
 */



int bifv1_extract_data(int fd, struct bifv1 * bif)
{
	uint32_t i;
	uint8_t buf[1024];
	ssize_t n, size, written;
	int fd2;
	char filename[80], *ext;

	memset((void*)buf, 0, sizeof(buf));
	for (i = 0; i < bif->header.variable_res_count; i++) {
		ext = resouce_to_extension(bif->vrt[i].type);
		sprintf(filename, "data/output/%08u.%s", bif->vrt[i].id, ext);

		if ((fd2 = open(filename, O_RDWR|O_CREAT)) < 0) {
			printf("open: %d, %s\n", errno, strerror(errno));
			return -2;
		}

		lseek(fd, bif->vrt[i].offset, SEEK_SET);

		written = 0;
		size = bif->vrt[i].size;
		while (written < size) {
			printf("size: %u, written: %u\n", size, written);
			n = read(fd, buf, sizeof(buf));
			n = write(fd2, buf, MIN(size - written, n));
			written += n;
		}
		close(fd2);
	}

	return 0;
}

int main(const int agc, const char *argv[])
{
	int fd;
	uint8_t buf[512];
	uint32_t i;
	ssize_t n;
	struct bifv1 bif;

	if ((fd = open("data/test.bif", O_RDONLY)) < 0) {
		printf("open: %d, %s\n", errno, strerror(errno));
		return -1;
	}

	memset((void*)buf, 0, sizeof(buf));
	n = read(fd, buf, 20);
	bifv1_read_header(&bif, buf);

	lseek(fd, bif.header.variable_table_offset, SEEK_SET);

	bif.vrt = (struct bifv1_vrt *) malloc(bif.header.variable_res_count * sizeof(struct bifv1_vrt));

	for (i = 0; i < bif.header.variable_res_count; i++) {
		n = read(fd, buf, 16);
		bifv1_read_variable_resource_table(buf, &bif.vrt[i]);
	}

	for (i = 0; i < bif.header.fixed_res_count; i++) {
		n = read(fd, buf, 20);
		bifv1_read_fixed_resource_table(buf);
	}

	bifv1_extract_data(fd, &bif);

	free(bif.vrt);
	close(fd);
	return 0;
}
