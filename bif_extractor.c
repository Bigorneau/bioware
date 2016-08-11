#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

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

/*
 * https://github.com/xoreos/xoreos-docs/blob/master/specs/bioware/KeyBIF_Format.pdf
 * http://witcher.wikia.com/wiki/KEY_BIF_V1.1_format
 */
enum e_res_type {
	RES_TYPE_RES = 0, /* misc GFF resources */
	RES_TYPE_BMP = 1, /* Microsoft Windows Bitmap */
	RES_TYPE_MVE = 2,
	RES_TYPE_TGA = 3, /* Targa Graphics Format */
	RES_TYPE_WAV = 4, /* Waveform Audio File */
	RES_TYPE_PLT = 6, /* Bioware Packed Layer Texture */
	RES_TYPE_INI = 7, /* Windows INI */
	RES_TYPE_MP3 = 8, /* MP3 */
	RES_TYPE_MPG = 9, /* MPEG */
	RES_TYPE_TXT = 10, /* Text file */
	RES_TYPE_XML = 11, /* XML */
	RES_TYPE_PLH = 2000,
	RES_TYPE_TEX = 2001,
	RES_TYPE_MDL = 2002, /* Model */
	RES_TYPE_THG = 2003,
	RES_TYPE_FNT = 2005, /* Font */
	RES_TYPE_LUA = 2007, /* Lua script source code */
	RES_TYPE_SLT = 2008,
	RES_TYPE_NSS = 2009, /* NWScript source code */
	RES_TYPE_NCS = 2010, /* NWScript bytecode */
	RES_TYPE_MOD = 2011, /* Module */
	RES_TYPE_ARE = 2012, /* Area (GFF) */
	RES_TYPE_SET = 2013, /* Tileset (unused in KOTOR?) */
	RES_TYPE_IFO = 2014, /* Module information */
	RES_TYPE_BIC = 2015, /* Character sheet (unused) */
	RES_TYPE_WOK = 2016, /* walk-mesh */
	RES_TYPE_2DA = 2017, /* 2-dimensional array */
	RES_TYPE_TLK = 2018, /* conversation file */
	RES_TYPE_TXI = 2022, /* texture information */
	RES_TYPE_GIT = 2023, /* dynamic area information, game instance file,
	                        all area and objects that are scriptable */
	RES_TYPE_BTI = 2024,
	RES_TYPE_UTI = 2025, /* item blueprint */
	RES_TYPE_BTC = 2026,
	RES_TYPE_UTC = 2027, /* creature blueprint */
	RES_TYPE_DLG = 2029, /* dialogue */
	RES_TYPE_ITP = 2030, /* tile blueprint pallet file */
	RES_TYPE_BTT = 2031,
	RES_TYPE_UTT = 2032, /* trigger blueprint */
	RES_TYPE_DDS = 2033, /* compressed texture file */
	RES_TYPE_BTS = 2034,
	RES_TYPE_UTS = 2035, /* sound blueprint */
	RES_TYPE_LTR = 2036, /* letter combo probability inf */
	RES_TYPE_GFF = 2037, /* generic file format */
	RES_TYPE_FAC = 2038, /* faction file */
	RES_TYPE_BTE = 2039,
	RES_TYPE_UTE = 2040, /* encounter blueprint */
	RES_TYPE_BTD = 2041,
	RES_TYPE_UTD = 2042, /* door blueprint */
	RES_TYPE_BTP = 2043,
	RES_TYPE_UTP = 2044, /* placeable object blueprint */
	RES_TYPE_DFT = 2045, /* default values file (text-ini) */
	RES_TYPE_GIC = 2046, /* game instance comments */
	RES_TYPE_GUI = 2047, /* GUI definition (GFF) */
	RES_TYPE_CSS = 2048,
	RES_TYPE_CCS = 2049,
	RES_TYPE_BTM = 2050,
	RES_TYPE_UTM = 2051, /* store merchant blueprint */
	RES_TYPE_DWK = 2052, /* door walkmesh */
	RES_TYPE_PWK = 2053, /* placeable object walkmesh */
	RES_TYPE_BTG = 2054,
	RES_TYPE_JRL = 2056, /* journal */
	RES_TYPE_SAV = 2057, /* saved game (ERF) */
	RES_TYPE_UTW = 2058, /* waypoint blueprint */
	RES_TYPE_4PC = 2059,
	RES_TYPE_SSF = 2060, /* sound set file */
	RES_TYPE_BIK = 2063, /* movie file (bik format) */
	RES_TYPE_NDB = 2064, /* script debugger file */
	RES_TYPE_PTM = 2065, /* plot manager/plot instance */
	RES_TYPE_PTT = 2066, /* plot wizard blueprint */
	RES_TYPE_NCM = 2067,
	RES_TYPE_MFX = 2068,
	RES_TYPE_MAT = 2069,
	RES_TYPE_MDB = 2070, /* not the standard MDB, multiple file formats present
	                        despite same type */
	RES_TYPE_SAY = 2071,
	RES_TYPE_TTF = 2072, /* standard .ttf font files */
	RES_TYPE_TTC = 2073,
	RES_TYPE_CUT = 2074, /* cutscene? (GFF) */
	RES_TYPE_KA = 2075, /* karma file (XML) */
	RES_TYPE_JPG = 2076, /* jpg image */
	RES_TYPE_ICO = 2077, /* standard windows .ico files */
	RES_TYPE_OGG = 2078, /* ogg vorbis sound file */
	RES_TYPE_SPT = 2079,
	RES_TYPE_SPW = 2080,
	RES_TYPE_WFX = 2081, /* woot effect class (XML) */
	RES_TYPE_UGM = 2082, /* 2082 ??? [textures00.bif] */
	RES_TYPE_QDB = 2083, /* quest database (GFF v3.38) */
	RES_TYPE_QST = 2084, /* quest (GFF) */
	RES_TYPE_NPC = 2085,
	RES_TYPE_SPN = 2086,
	RES_TYPE_UTX = 2087, /* spawn point? (GFF) */
	RES_TYPE_MMD = 2088,
	RES_TYPE_SMM = 2089,
	RES_TYPE_UTA = 2090, /* uta (GFF) */
	RES_TYPE_MDE = 2091,
	RES_TYPE_MDV = 2092,
	RES_TYPE_MDA = 2093,
	RES_TYPE_MBA = 2094,
	RES_TYPE_OCT = 2095,
	RES_TYPE_BFX = 2096,
	RES_TYPE_PDB = 2097,
	RES_TYPE_TWS = 2098, /* TheWitcherSave */
	RES_TYPE_PVS = 2099,
	RES_TYPE_CFX = 2100,
	RES_TYPE_LUC = 2101, /* compiled lua script */
	RES_TYPE_PRB = 2103,
	RES_TYPE_CAM = 2104,
	RES_TYPE_VDS = 2105,
	RES_TYPE_BIN = 2106,
	RES_TYPE_WOB = 2107,
	RES_TYPE_API = 2108,
	RES_TYPE_PROP = 2109, /* properties */
	RES_TYPE_PNG = 2110,

	RES_TYPE_BIG = 9995,
	RES_TYPE_ERF = 9997, /* bncapsulated resource format */
	RES_TYPE_BIF = 9998,
	RES_TYPE_KEY = 9999,
};

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
	char filename[80];

	memset((void*)buf, 0, sizeof(buf));
	for (i = 0; i < bif->header.variable_res_count; i++) {
		if (bif->vrt[i].type != RES_TYPE_WAV)
			continue;

		sprintf(filename, "data/output/%08u.wav", bif->vrt[i].id);
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
