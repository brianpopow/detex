/*

Copyright (c) 2015 Harm Hanemaaijer <fgenfb@yahoo.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/

#include "detex.h"

// Define an array to speed up clamping of values to the ranges 0 to 255.

static const uint8_t clamp_table[255 + 256 + 256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
	33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
	49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
	65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
	97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112,
	113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128,
	129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144,
	145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160,
	161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176,
	177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192,
	193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
	209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
	225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240,
	241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255 };

static const int complement3bitshifted_table[8] = {
	0, 8, 16, 24, -32, -24, -16, -8
};

static const int modifier_table[8][4] = {
	{ 2, 8, -2, -8 },
	{ 5, 17, -5, -17 },
	{ 9, 29, -9, -29 },
	{ 13, 42, -13, -42 },
	{ 18, 60, -18, -60 },
	{ 24, 80, -24, -80 },
	{ 33, 106, -33, -106 },
	{ 47, 183, -47, -183 }
};

static DETEX_INLINE_ONLY uint8_t clamp(int x) {
	return clamp_table[x + 255];
}

static DETEX_INLINE_ONLY int clamp2047(int x) {
	if (x < 0)
		return 0;
	if (x > 2047)
		return 2047;
	return x;
}

static DETEX_INLINE_ONLY int clamp1023_signed(int x) {
	if (x < - 1023)
		return - 1023;
	if (x > 1023)
		return 1023;
	return x;
}

// This function calculates the 3-bit complement value in the range -4 to 3 of a three bit
// representation. The result is arithmetically shifted 3 places to the left before returning.
static DETEX_INLINE_ONLY int complement3bitshifted(int x) {
	return complement3bitshifted_table[x];
}

static DETEX_INLINE_ONLY int complement3bitshifted_slow(int x) {
	if (x & 4)
		return ((x & 3) - 4) << 3;	// Note: shift is arithmetic.
	return x << 3;
}

static DETEX_INLINE_ONLY int complement3bit(int x) {
	if (x & 4)
		return ((x & 3) - 4);
	return x;
}

// Define inline function to speed up ETC1 decoding.

static DETEX_INLINE_ONLY void ProcessPixelETC1(uint8_t i, uint32_t pixel_index_word,
uint32_t table_codeword, int *base_color_subblock, uint8_t *pixel_buffer) {
	int pixel_index = ((pixel_index_word & (1 << i)) >> i)
		| ((pixel_index_word & (0x10000 << i)) >> (16 + i - 1));
	int r, g, b;
	int modifier = modifier_table[table_codeword][pixel_index];
	r = clamp(base_color_subblock[0] + modifier);
	g = clamp(base_color_subblock[1] + modifier);
	b = clamp(base_color_subblock[2] + modifier);
	uint32_t *buffer = (uint32_t *)pixel_buffer;
	buffer[(i & 3) * 4 + ((i & 12) >> 2)] =
		detexPack32RGB8Alpha0xFF(r, g, b);
}

/* Decompress a 64-bit 4x4 pixel texture block compressed using the ETC1 */
/* format. */
bool detexDecompressBlockETC1(uint8_t *bitstring, uint32_t mode_mask,
uint32_t flags, uint8_t *pixel_buffer) {
	int differential_mode = bitstring[3] & 2;
	if (differential_mode) {
		if ((mode_mask & DETEX_MODE_MASK_ETC_DIFFERENTIAL) == 0)
			return false;
	}
	else
		if ((mode_mask & DETEX_MODE_MASK_ETC_INDIVIDUAL) == 0)
			return false;
	int flipbit = bitstring[3] & 1;
	int base_color_subblock1[3];
	int base_color_subblock2[3];
	if (differential_mode) {
		base_color_subblock1[0] = (bitstring[0] & 0xF8);
		base_color_subblock1[0] |= ((base_color_subblock1[0] & 224) >> 5);
		base_color_subblock1[1] = (bitstring[1] & 0xF8);
		base_color_subblock1[1] |= (base_color_subblock1[1] & 224) >> 5;
		base_color_subblock1[2] = (bitstring[2] & 0xF8);
		base_color_subblock1[2] |= (base_color_subblock1[2] & 224) >> 5;
		base_color_subblock2[0] = (bitstring[0] & 0xF8);			// 5 highest order bits.
		base_color_subblock2[0] += complement3bitshifted(bitstring[0] & 7);	// Add difference.
		if (base_color_subblock2[0] & 0xFF07)					// Check for overflow.
			return false;
		base_color_subblock2[0] |= (base_color_subblock2[0] & 224) >> 5;	// Replicate.
		base_color_subblock2[1] = (bitstring[1] & 0xF8);
		base_color_subblock2[1] += complement3bitshifted(bitstring[1] & 7);
		if (base_color_subblock2[1] & 0xFF07)
			return false;
		base_color_subblock2[1] |= (base_color_subblock2[1] & 224) >> 5;
		base_color_subblock2[2] = (bitstring[2] & 0xF8);
		base_color_subblock2[2] += complement3bitshifted(bitstring[2] & 7);
		if (base_color_subblock2[2] & 0xFF07)
			return false;
		base_color_subblock2[2] |= (base_color_subblock2[2] & 224) >> 5;
	}
	else {
		base_color_subblock1[0] = (bitstring[0] & 0xF0);
		base_color_subblock1[0] |= base_color_subblock1[0] >> 4;
		base_color_subblock1[1] = (bitstring[1] & 0xF0);
		base_color_subblock1[1] |= base_color_subblock1[1] >> 4;
		base_color_subblock1[2] = (bitstring[2] & 0xF0);
		base_color_subblock1[2] |= base_color_subblock1[2] >> 4;
		base_color_subblock2[0] = (bitstring[0] & 0x0F);
		base_color_subblock2[0] |= base_color_subblock2[0] << 4;
		base_color_subblock2[1] = (bitstring[1] & 0x0F);
		base_color_subblock2[1] |= base_color_subblock2[1] << 4;
		base_color_subblock2[2] = (bitstring[2] & 0x0F);
		base_color_subblock2[2] |= base_color_subblock2[2] << 4;
	}
	uint32_t table_codeword1 = (bitstring[3] & 224) >> 5;
	uint32_t table_codeword2 = (bitstring[3] & 28) >> 2;
	uint32_t pixel_index_word = ((uint32_t)bitstring[4] << 24) | ((uint32_t)bitstring[5] << 16) |
		((uint32_t)bitstring[6] << 8) | bitstring[7];
	if (flipbit == 0) {
		ProcessPixelETC1(0, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(1, pixel_index_word, table_codeword1,base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(2, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(3, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(4, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(5, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(6, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(7, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(8, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(9, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(10, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(11, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(12, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(13, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(14, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(15, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
	}
	else {
		ProcessPixelETC1(0, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(1, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(2, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(3, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(4, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(5, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(6, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(7, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(8, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(9, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(10, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(11, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(12, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(13, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC1(14, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC1(15, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
	}
	return true;
}

/* Return the internal mode of a ETC1 block. */
uint32_t detexGetModeETC1(const uint8_t *bitstring) {
	// Figure out the mode.
	if ((bitstring[3] & 2) == 0)
		// Individual mode.
		return 0;
	else
		return 1;
}

static const int etc2_distance_table[8] = { 3, 6, 11, 16, 23, 32, 41, 64 };

static void ProcessBlockETC2TOrHMode(const unsigned char *bitstring, int mode, uint8_t *pixel_buffer) {
	int base_color1_R, base_color1_G, base_color1_B;
	int base_color2_R, base_color2_G, base_color2_B;
	int paint_color_R[4], paint_color_G[4], paint_color_B[4];
	int distance;
	if (mode == DETEX_MODE_MASK_ETC_T) {
		// T mode.
		base_color1_R = ((bitstring[0] & 0x18) >> 1) | (bitstring[0] & 0x3);
		base_color1_R |= base_color1_R << 4;
		base_color1_G = bitstring[1] & 0xF0;
		base_color1_G |= base_color1_G >> 4;
		base_color1_B = bitstring[1] & 0x0F;
		base_color1_B |= base_color1_B << 4;
		base_color2_R = bitstring[2] & 0xF0;
		base_color2_R |= base_color2_R >> 4;
		base_color2_G = bitstring[2] & 0x0F;
		base_color2_G |= base_color2_G << 4;
		base_color2_B = bitstring[3] & 0xF0;
		base_color2_B |= base_color2_B >> 4;
		// index = (da << 1) | db
		distance = etc2_distance_table[((bitstring[3] & 0x0C) >> 1) | (bitstring[3] & 0x1)];
		paint_color_R[0] = base_color1_R;
		paint_color_G[0] = base_color1_G;
		paint_color_B[0] = base_color1_B;
		paint_color_R[2] = base_color2_R;
		paint_color_G[2] = base_color2_G;
		paint_color_B[2] = base_color2_B;
		paint_color_R[1] = clamp(base_color2_R + distance);
		paint_color_G[1] = clamp(base_color2_G + distance);
		paint_color_B[1] = clamp(base_color2_B + distance);
		paint_color_R[3] = clamp(base_color2_R - distance);
		paint_color_G[3] = clamp(base_color2_G - distance);
		paint_color_B[3] = clamp(base_color2_B - distance);
	}
	else {
		// H mode.
		base_color1_R = (bitstring[0] & 0x78) >> 3;
		base_color1_R |= base_color1_R << 4;
		base_color1_G = ((bitstring[0] & 0x07) << 1) | ((bitstring[1] & 0x10) >> 4);
		base_color1_G |= base_color1_G << 4;
		base_color1_B = (bitstring[1] & 0x08) | ((bitstring[1] & 0x03) << 1) | ((bitstring[2] & 0x80) >> 7);
		base_color1_B |= base_color1_B << 4;
		base_color2_R = (bitstring[2] & 0x78) >> 3;
		base_color2_R |= base_color2_R << 4;
		base_color2_G = ((bitstring[2] & 0x07) << 1) | ((bitstring[3] & 0x80) >> 7);
		base_color2_G |= base_color2_G << 4;
		base_color2_B = (bitstring[3] & 0x78) >> 3;
		base_color2_B |= base_color2_B << 4;
		// da is most significant bit, db is middle bit, least significant bit is
		// (base_color1 value >= base_color2 value).
		int base_color1_value = (base_color1_R << 16) + (base_color1_G << 8) + base_color1_B;
		int base_color2_value = (base_color2_R << 16) + (base_color2_G << 8) + base_color2_B;
		int bit;
		if (base_color1_value >= base_color2_value)
			bit = 1;
		else
			bit = 0;
		distance = etc2_distance_table[(bitstring[3] & 0x04) | ((bitstring[3] & 0x01) << 1) | bit];
		paint_color_R[0] = clamp(base_color1_R + distance);
		paint_color_G[0] = clamp(base_color1_G + distance);
		paint_color_B[0] = clamp(base_color1_B + distance);
		paint_color_R[1] = clamp(base_color1_R - distance);
		paint_color_G[1] = clamp(base_color1_G - distance);
		paint_color_B[1] = clamp(base_color1_B - distance);
		paint_color_R[2] = clamp(base_color2_R + distance);
		paint_color_G[2] = clamp(base_color2_G + distance);
		paint_color_B[2] = clamp(base_color2_B + distance);
		paint_color_R[3] = clamp(base_color2_R - distance);
		paint_color_G[3] = clamp(base_color2_G - distance);
		paint_color_B[3] = clamp(base_color2_B - distance);
	}
	uint32_t pixel_index_word = ((uint32_t)bitstring[4] << 24) | ((uint32_t)bitstring[5] << 16) |
		((uint32_t)bitstring[6] << 8) | bitstring[7];
	uint32_t *buffer = (uint32_t *)pixel_buffer;
	for (int i = 0; i < 16; i++) {
		int pixel_index = ((pixel_index_word & (1 << i)) >> i)			// Least significant bit.
			| ((pixel_index_word & (0x10000 << i)) >> (16 + i - 1));	// Most significant bit.
		int r = paint_color_R[pixel_index];
		int g = paint_color_G[pixel_index];
		int b = paint_color_B[pixel_index];
		buffer[(i & 3) * 4 + ((i & 12) >> 2)] = detexPack32RGB8Alpha0xFF(r, g, b);
	}
}

static void ProcessBlockETC2PlanarMode(const unsigned char *bitstring, uint8_t *pixel_buffer) {
	// Each color O, H and V is in 6-7-6 format.
	int RO = (bitstring[0] & 0x7E) >> 1;
	int GO = ((bitstring[0] & 0x1) << 6) | ((bitstring[1] & 0x7E) >> 1);
	int BO = ((bitstring[1] & 0x1) << 5) | (bitstring[2] & 0x18) | ((bitstring[2] & 0x03) << 1) |
		((bitstring[3] & 0x80) >> 7);
	int RH = ((bitstring[3] & 0x7B) >> 1) | (bitstring[3] & 0x1);
	int GH = (bitstring[4] & 0xFE) >> 1;
	int BH = ((bitstring[4] & 0x1) << 5) | ((bitstring[5] & 0xF8) >> 3);
	int RV = ((bitstring[5] & 0x7) << 3) | ((bitstring[6] & 0xE0) >> 5);
	int GV = ((bitstring[6] & 0x1F) << 2) | ((bitstring[7] & 0xC0) >> 6);
	int BV = bitstring[7] & 0x3F;
	RO = (RO << 2) | ((RO & 0x30) >> 4);	// Replicate bits.
	GO = (GO << 1) | ((GO & 0x40) >> 6);
	BO = (BO << 2) | ((BO & 0x30) >> 4);
	RH = (RH << 2) | ((RH & 0x30) >> 4);
	GH = (GH << 1) | ((GH & 0x40) >> 6);
	BH = (BH << 2) | ((BH & 0x30) >> 4);
	RV = (RV << 2) | ((RV & 0x30) >> 4);
	GV = (GV << 1) | ((GV & 0x40) >> 6);
	BV = (BV << 2) | ((BV & 0x30) >> 4);
	uint32_t *buffer = (uint32_t *)pixel_buffer;
	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++) {
			int r = clamp((x * (RH - RO) + y * (RV - RO) + 4 * RO + 2) >> 2);
			int g = clamp((x * (GH - GO) + y * (GV - GO) + 4 * GO + 2) >> 2);
			int b = clamp((x * (BH - BO) + y * (BV - BO) + 4 * BO + 2) >> 2);
			buffer[y * 4 + x] = detexPack32RGB8Alpha0xFF(r, g, b);
		}
}

/* Decompress a 64-bit 4x4 pixel texture block compressed using the ETC2 */
/* format. */
bool detexDecompressBlockETC2(uint8_t *bitstring, uint32_t mode_mask,
uint32_t flags, uint8_t *pixel_buffer) {
	// Figure out the mode.
	if ((bitstring[3] & 2) == 0) {
		// Individual mode.
		return detexDecompressBlockETC1(bitstring, mode_mask, flags,
			pixel_buffer);
	}
	if ((mode_mask & (~DETEX_MODE_MASK_ETC_INDIVIDUAL)) == 0)
		return false;
	int R = (bitstring[0] & 0xF8);
	R += complement3bitshifted(bitstring[0] & 7);
	int G = (bitstring[1] & 0xF8);
	G += complement3bitshifted(bitstring[1] & 7);
	int B = (bitstring[2] & 0xF8);
	B += complement3bitshifted(bitstring[2] & 7);
	if (R & 0xFF07) {
		// T mode.
		if ((mode_mask & DETEX_MODE_MASK_ETC_T) == 0)
			return false;
		ProcessBlockETC2TOrHMode(bitstring, DETEX_MODE_MASK_ETC_T,
			pixel_buffer);
		return true;
	}
	else
	if (G & 0xFF07) {
		// H mode.
		if ((mode_mask & DETEX_MODE_MASK_ETC_H) == 0)
			return false;
		ProcessBlockETC2TOrHMode(bitstring, DETEX_MODE_MASK_ETC_H,
			pixel_buffer);
		return true;
	}
	else
	if (B & 0xFF07) {
		// Planar mode.
		if ((mode_mask & DETEX_MODE_MASK_ETC_PLANAR) == 0)
			return false;
		ProcessBlockETC2PlanarMode(bitstring, pixel_buffer);
		return true;
	}
	else {
		// Differential mode.
		return detexDecompressBlockETC1(bitstring, mode_mask, flags,
			pixel_buffer);
	}
}

/* Return the internal mode of a ETC2 block. */
uint32_t detexGetModeETC2(const uint8_t *bitstring) {
	// Figure out the mode.
	if ((bitstring[3] & 2) == 0)
		// Individual mode.
		return 0;
	int R = (bitstring[0] & 0xF8);
	R += complement3bitshifted(bitstring[0] & 7);
	int G = (bitstring[1] & 0xF8);
	G += complement3bitshifted(bitstring[1] & 7);
	int B = (bitstring[2] & 0xF8);
	B += complement3bitshifted(bitstring[2] & 7);
	if (R & 0xFF07)
		// T mode.
		return 2;
	else
	if (G & 0xFF07)
		// H mode.
		return 3;
	else
	if (B & 0xFF07)
		// Planar mode.
		return 4;
	else
		// Differential mode.
		return 1;
}

static const int punchthrough_modifier_table[8][4] = {
	{ 0, 8, 0, -8 },
	{ 0, 17, 0, -17 },
	{ 0, 29, 0, -29 },
	{ 0, 42, 0, -42 },
	{ 0, 60, 0, -60 },
	{ 0, 80, 0, -80 },
	{ 0, 106, 0, -106 },
	{ 0, 183, 0, -183 }
};

static const uint32_t punchthrough_mask_table[4] = {
	0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF };

static DETEX_INLINE_ONLY void ProcessPixelETC2Punchthrough(uint8_t i, uint32_t pixel_index_word,
uint32_t table_codeword, int *base_color_subblock, uint8_t *pixel_buffer) {
	int pixel_index = ((pixel_index_word & (1 << i)) >> i)
		| ((pixel_index_word & (0x10000 << i)) >> (16 + i - 1));
	int r, g, b;
	int modifier = modifier_table[table_codeword][pixel_index];
	r = clamp(base_color_subblock[0] + modifier);
	g = clamp(base_color_subblock[1] + modifier);
	b = clamp(base_color_subblock[2] + modifier);
	uint32_t mask = punchthrough_mask_table[pixel_index];
	uint32_t *buffer = (uint32_t *)pixel_buffer;
	buffer[(i & 3) * 4 + ((i & 12) >> 2)] =
		detexPack32RGB8Alpha0xFF(r, g, b) & mask;
}


void ProcessBlockETC2PunchthroughDifferentialMode(const unsigned char *bitstring,
uint8_t *pixel_buffer) {
	int flipbit = bitstring[3] & 1;
	int base_color_subblock1[3];
	int base_color_subblock2[3];
	base_color_subblock1[0] = (bitstring[0] & 0xF8);
	base_color_subblock1[0] |= ((base_color_subblock1[0] & 224) >> 5);
	base_color_subblock1[1] = (bitstring[1] & 0xF8);
	base_color_subblock1[1] |= (base_color_subblock1[1] & 224) >> 5;
	base_color_subblock1[2] = (bitstring[2] & 0xF8);
	base_color_subblock1[2] |= (base_color_subblock1[2] & 224) >> 5;
	base_color_subblock2[0] = (bitstring[0] & 0xF8);				// 5 highest order bits.
	base_color_subblock2[0] += complement3bitshifted(bitstring[0] & 7);	// Add difference.
	base_color_subblock2[0] |= (base_color_subblock2[0] & 224) >> 5;		// Replicate.
	base_color_subblock2[1] = (bitstring[1] & 0xF8);
	base_color_subblock2[1] += complement3bitshifted(bitstring[1] & 7);
	base_color_subblock2[1] |= (base_color_subblock2[1] & 224) >> 5;
	base_color_subblock2[2] = (bitstring[2] & 0xF8);
	base_color_subblock2[2] += complement3bitshifted(bitstring[2] & 7);
	base_color_subblock2[2] |= (base_color_subblock2[2] & 224) >> 5;
	uint32_t table_codeword1 = (bitstring[3] & 224) >> 5;
	uint32_t table_codeword2 = (bitstring[3] & 28) >> 2;
	uint32_t pixel_index_word = ((uint32_t)bitstring[4] << 24) | ((uint32_t)bitstring[5] << 16) |
		((uint32_t)bitstring[6] << 8) | bitstring[7];
	if (flipbit == 0) {
		ProcessPixelETC2Punchthrough(0, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(1, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(2, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(3, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(4, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(5, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(6, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(7, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(8, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(9, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(10, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(11, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(12, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(13, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(14, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(15, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
	}
	else {
		ProcessPixelETC2Punchthrough(0, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(1, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(2, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(3, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(4, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(5, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(6, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(7, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(8, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(9, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(10, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(11, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(12, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(13, pixel_index_word, table_codeword1, base_color_subblock1, pixel_buffer);
		ProcessPixelETC2Punchthrough(14, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
		ProcessPixelETC2Punchthrough(15, pixel_index_word, table_codeword2, base_color_subblock2, pixel_buffer);
	}
}

static void ProcessBlockETC2PunchthroughTOrHMode(const unsigned char *bitstring,
int mode, uint8_t *pixel_buffer) {
	int base_color1_R, base_color1_G, base_color1_B;
	int base_color2_R, base_color2_G, base_color2_B;
	int paint_color_R[4], paint_color_G[4], paint_color_B[4];
	int distance;
	if (mode == DETEX_MODE_MASK_ETC_T) {
		// T mode.
		base_color1_R = ((bitstring[0] & 0x18) >> 1) | (bitstring[0] & 0x3);
		base_color1_R |= base_color1_R << 4;
		base_color1_G = bitstring[1] & 0xF0;
		base_color1_G |= base_color1_G >> 4;
		base_color1_B = bitstring[1] & 0x0F;
		base_color1_B |= base_color1_B << 4;
		base_color2_R = bitstring[2] & 0xF0;
		base_color2_R |= base_color2_R >> 4;
		base_color2_G = bitstring[2] & 0x0F;
		base_color2_G |= base_color2_G << 4;
		base_color2_B = bitstring[3] & 0xF0;
		base_color2_B |= base_color2_B >> 4;
		// index = (da << 1) | db
		distance = etc2_distance_table[((bitstring[3] & 0x0C) >> 1) | (bitstring[3] & 0x1)];
		paint_color_R[0] = base_color1_R;
		paint_color_G[0] = base_color1_G;
		paint_color_B[0] = base_color1_B;
		paint_color_R[2] = base_color2_R;
		paint_color_G[2] = base_color2_G;
		paint_color_B[2] = base_color2_B;
		paint_color_R[1] = clamp(base_color2_R + distance);
		paint_color_G[1] = clamp(base_color2_G + distance);
		paint_color_B[1] = clamp(base_color2_B + distance);
		paint_color_R[3] = clamp(base_color2_R - distance);
		paint_color_G[3] = clamp(base_color2_G - distance);
		paint_color_B[3] = clamp(base_color2_B - distance);
	}
	else {
		// H mode.
		base_color1_R = (bitstring[0] & 0x78) >> 3;
		base_color1_R |= base_color1_R << 4;
		base_color1_G = ((bitstring[0] & 0x07) << 1) | ((bitstring[1] & 0x10) >> 4);
		base_color1_G |= base_color1_G << 4;
		base_color1_B = (bitstring[1] & 0x08) | ((bitstring[1] & 0x03) << 1) | ((bitstring[2] & 0x80) >> 7);
		base_color1_B |= base_color1_B << 4;
		base_color2_R = (bitstring[2] & 0x78) >> 3;
		base_color2_R |= base_color2_R << 4;
		base_color2_G = ((bitstring[2] & 0x07) << 1) | ((bitstring[3] & 0x80) >> 7);
		base_color2_G |= base_color2_G << 4;
		base_color2_B = (bitstring[3] & 0x78) >> 3;
		base_color2_B |= base_color2_B << 4;
		// da is most significant bit, db is middle bit, least significant bit is
		// (base_color1 value >= base_color2 value).
		int base_color1_value = (base_color1_R << 16) + (base_color1_G << 8) + base_color1_B;
		int base_color2_value = (base_color2_R << 16) + (base_color2_G << 8) + base_color2_B;
		int bit;
		if (base_color1_value >= base_color2_value)
			bit = 1;
		else
			bit = 0;
		distance = etc2_distance_table[(bitstring[3] & 0x04) | ((bitstring[3] & 0x01) << 1) | bit];
		paint_color_R[0] = clamp(base_color1_R + distance);
		paint_color_G[0] = clamp(base_color1_G + distance);
		paint_color_B[0] = clamp(base_color1_B + distance);
		paint_color_R[1] = clamp(base_color1_R - distance);
		paint_color_G[1] = clamp(base_color1_G - distance);
		paint_color_B[1] = clamp(base_color1_B - distance);
		paint_color_R[2] = clamp(base_color2_R + distance);
		paint_color_G[2] = clamp(base_color2_G + distance);
		paint_color_B[2] = clamp(base_color2_B + distance);
		paint_color_R[3] = clamp(base_color2_R - distance);
		paint_color_G[3] = clamp(base_color2_G - distance);
		paint_color_B[3] = clamp(base_color2_B - distance);
	}
	uint32_t pixel_index_word = ((uint32_t)bitstring[4] << 24) | ((uint32_t)bitstring[5] << 16) |
		((uint32_t)bitstring[6] << 8) | bitstring[7];
	uint32_t *buffer = (uint32_t *)pixel_buffer;
	for (int i = 0; i < 16; i++) {
		int pixel_index = ((pixel_index_word & (1 << i)) >> i)			// Least significant bit.
			| ((pixel_index_word & (0x10000 << i)) >> (16 + i - 1));	// Most significant bit.
		int r = paint_color_R[pixel_index];
		int g = paint_color_G[pixel_index];
		int b = paint_color_B[pixel_index];
		uint32_t mask = punchthrough_mask_table[pixel_index];
		buffer[(i & 3) * 4 + ((i & 12) >> 2)] = (detexPack32RGB8Alpha0xFF(r, g, b)) & mask;
	}
}

/* Decompress a 64-bit 4x4 pixel texture block compressed using the */
/* ETC2_PUNCHTROUGH format. */
bool detexDecompressBlockETC2_PUNCHTHROUGH(uint8_t *bitstring,
uint32_t mode_mask, uint32_t flags, uint8_t *pixel_buffer) {
	int R = (bitstring[0] & 0xF8);
	R += complement3bitshifted(bitstring[0] & 7);
	int G = (bitstring[1] & 0xF8);
	G += complement3bitshifted(bitstring[1] & 7);
	int B = (bitstring[2] & 0xF8);
	B += complement3bitshifted(bitstring[2] & 7);
	int opaque = bitstring[3] & 2;
	if (opaque && (flags & DETEX_DECOMPRESS_FLAG_NON_OPAQUE_ONLY))
		return false;
	if (!opaque && (flags & DETEX_DECOMPRESS_FLAG_OPAQUE_ONLY))
		return false;
	if (R & 0xFF07) {
		// T mode.
		if ((mode_mask & DETEX_MODE_MASK_ETC_T) == 0)
			return false;
		if (opaque) {
			ProcessBlockETC2TOrHMode(bitstring, DETEX_MODE_MASK_ETC_T,
				pixel_buffer);
			return true;
		}
		// T mode with punchthrough alpha.
		ProcessBlockETC2PunchthroughTOrHMode(bitstring,
			DETEX_MODE_MASK_ETC_T, pixel_buffer);
		return true;
	}
	else
	if (G & 0xFF07) {
		// H mode.
		if ((mode_mask & DETEX_MODE_MASK_ETC_H) == 0)
			return false;
		if (opaque) {
			ProcessBlockETC2TOrHMode(bitstring, DETEX_MODE_MASK_ETC_H,
				pixel_buffer);
			return true;
		}
		// H mode with punchthrough alpha.
		ProcessBlockETC2PunchthroughTOrHMode(bitstring, DETEX_MODE_MASK_ETC_H,
			pixel_buffer);
		return true;
	}
	else
	if (B & 0xFF07) {
		// Planar mode.
		if ((mode_mask & DETEX_MODE_MASK_ETC_PLANAR) == 0)
			return false;
		// Opaque always set.
		if (flags & DETEX_DECOMPRESS_FLAG_NON_OPAQUE_ONLY)
			return false;
		ProcessBlockETC2PlanarMode(bitstring, pixel_buffer);
		return true;
	}
	else {
		// Differential mode.
		if (opaque)
			return detexDecompressBlockETC1(bitstring, mode_mask, flags,
				pixel_buffer);
		// Differential mode with punchthrough alpha.
		if ((mode_mask & DETEX_MODE_MASK_ETC_DIFFERENTIAL) == 0)
			return false;
		ProcessBlockETC2PunchthroughDifferentialMode(bitstring, pixel_buffer);
		return true;
	}
}


/* Return the internal mode of a ETC2_PUNCHTROUGH block. */
uint32_t detexGetModeETC2_PUNCHTHROUGH(const uint8_t *bitstring) {
	// Figure out the mode.
//	int opaque = bitstring[3] & 2;
	int R = (bitstring[0] & 0xF8);
	R += complement3bitshifted(bitstring[0] & 7);
	int G = (bitstring[1] & 0xF8);
	G += complement3bitshifted(bitstring[1] & 7);
	int B = (bitstring[2] & 0xF8);
	B += complement3bitshifted(bitstring[2] & 7);
	if (R & 0xFF07)
		// T mode.
		return 2;
	else if (G & 0xFF07)
		// H mode.
		return 3;
	else if (B & 0xFF07)
		// Planar mode.
		return 4;
	else
		// Differential mode.
		return 1;
}

