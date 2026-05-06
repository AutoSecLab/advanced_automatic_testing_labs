#include <tiffio.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

#define FIXED_SIZE 528
#define COUNTER_NUMBER 1
#define MIN_SEED_SIZE 536
const unsigned counter_size[COUNTER_NUMBER] = { 8 };

#define NEW_DATA_LEN 4096


#define MIN(x,y) ((x < y) ? x : y)


extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t Size) {
	if (Size < MIN_SEED_SIZE) return 0;
	const char *char_p_cs0 = "jawruyqltusifmlgphvs.bin";
	char char_p_cs1[512];
	memset(char_p_cs1, 0x0, sizeof(char_p_cs1));
	__uint64_t __uint64_t_s1[1];
	memset(__uint64_t_s1, 0x0, sizeof(__uint64_t_s1));
	TIFF *TIFF_p_h0[1] = { 0 };
	TIFF *TIFF_p_h0_shadow[1] = { 0 };
	__uint64_t __uint64_t_s0[1];
	memset(__uint64_t_s0, 0x0, sizeof(__uint64_t_s0));
	size_t size_t_s0[1];
	memset(size_t_s0, 0x0, sizeof(size_t_s0));
	memcpy(char_p_cs1, data, sizeof(char_p_cs1));data += sizeof(char_p_cs1);
	char_p_cs1[sizeof(char_p_cs1) - 1] = 0;
	memcpy(__uint64_t_s1, data, sizeof(__uint64_t_s1));data += sizeof(__uint64_t_s1);
	memcpy(__uint64_t_s0, data, sizeof(__uint64_t_s0));data += sizeof(__uint64_t_s0);
	//file init
	memcpy(size_t_s0, data, sizeof(size_t_s0));data += sizeof(size_t_s0);
	FILE *p2 = fopen(char_p_cs0, "w");
	fwrite(data, size_t_s0[0], 1, p2);
	fclose(p2);data += size_t_s0[0];

	TIFF_p_h0[0] =  TIFFOpen((const char * )char_p_cs0, (const char * )char_p_cs1);
	TIFF_p_h0_shadow[0] = TIFF_p_h0[0];
	if (TIFF_p_h0[0] == 0) goto clean_up;
	__uint64_t_s0[0] =  TIFFTileSize64(TIFF_p_h0[0]);
	__uint64_t_s0[0] =  TIFFTileSize64(TIFF_p_h0[0]);
	__uint64_t_s1[0] =  TIFFTileSize64(TIFF_p_h0[0]);
	TIFFClose(TIFF_p_h0[0]);
	TIFF_p_h0[0] = 0;
	TIFF_p_h0_shadow[0] = 0;

clean_up:
	if (TIFF_p_h0_shadow[0] != 0) TIFFCleanup(TIFF_p_h0[0]);

	return 0;
}
