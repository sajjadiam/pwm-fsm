/* ===========================================================================

@file    sevenseg_map.h

@brief   Segment bit masks and ASCII lookup for 7-segment library

=========================================================================== */
#ifndef SEVENSEG_MAP_H
#define SEVENSEG_MAP_H

#include "sevenseg_config.h"


/* Segment mask definitions based on configuration */
#if (SEGMENT_NUM == 7 && SEGMENT_STRUCTURE == LSB_FIRST)
	#define SEG_A  (1U << 6)
	#define SEG_B  (1U << 5)
	#define SEG_C  (1U << 4)
	#define SEG_D  (1U << 3)
	#define SEG_E  (1U << 2)
	#define SEG_F  (1U << 1)
	#define SEG_G  (1U << 0)
#elif (SEGMENT_NUM == 7 && SEGMENT_STRUCTURE == MSB_FIRST)
	#define SEG_A  (1U << 0)
	#define SEG_B  (1U << 1)
	#define SEG_C  (1U << 2)
	#define SEG_D  (1U << 3)
	#define SEG_E  (1U << 4)
	#define SEG_F  (1U << 5)
	#define SEG_G  (1U << 6)
#elif (SEGMENT_NUM == 8 && SEGMENT_STRUCTURE == LSB_FIRST)
	#define SEG_A   (1U << 7)
	#define SEG_B   (1U << 6)
	#define SEG_C   (1U << 5)
	#define SEG_D   (1U << 4)
	#define SEG_E   (1U << 3)
	#define SEG_F   (1U << 2)
	#define SEG_G   (1U << 1)
	#define SEG_DP  (1U << 0)
#elif (SEGMENT_NUM == 8 && SEGMENT_STRUCTURE == MSB_FIRST)
	#define SEG_A   (1U << 0)
	#define SEG_B   (1U << 1)
	#define SEG_C   (1U << 2)
	#define SEG_D   (1U << 3)
	#define SEG_E   (1U << 4)
	#define SEG_F   (1U << 5)
	#define SEG_G   (1U << 6)
	#define SEG_DP  (1U << 7)
#else
	#error "Invalid SEGMENT_NUM or SEGMENT_STRUCTURE"
#endif

/**

@brief ASCII-to-segment lookup table

Maps ASCII characters to 7-segment bit patterns
*/
static const uint8_t segmentMapASCII[128] = {
	['0'] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,    			//0xFC
	['1'] =              SEG_B | SEG_C,                       			//0x60
	['2'] = SEG_A | SEG_B |      SEG_D | SEG_E |     SEG_G,   			//0xDA
	['3'] = SEG_A | SEG_B | SEG_C | SEG_D |         SEG_G,   				//0xF2
	['4'] =       SEG_B | SEG_C |         SEG_F | SEG_G,     				//0x66
	['5'] = SEG_A |      SEG_C | SEG_D |     SEG_F | SEG_G,   			//0xB6
	['6'] = SEG_A |      SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, 		//0xBE
	['7'] = SEG_A | SEG_B | SEG_C,                            			//0xE0
	['8'] = SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, 	//0xFE
	['9'] = SEG_A | SEG_B | SEG_C | SEG_D |     SEG_F | SEG_G, 			//0xF6
		
	['A'] = SEG_A | SEG_B | SEG_C |     SEG_E | SEG_F | SEG_G, 			//0xEE
	['b'] =            SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,  			//0x3E
	['C'] = SEG_A |            SEG_D | SEG_E | SEG_F,          			//0x9C
	['c'] =           SEG_D | SEG_E | SEG_G,                   			//0x1A
	['d'] =      SEG_B | SEG_C | SEG_D | SEG_E |     SEG_G,    			//0x7A
	['E'] = SEG_A |      SEG_D | SEG_E | SEG_F | SEG_G,        			//0x9E
	['F'] = SEG_A |            SEG_E | SEG_F | SEG_G,          			//0x8E
	['g'] = SEG_A | SEG_B | SEG_C | SEG_D |     SEG_F | SEG_G, 			//0xF6
	['H'] =      SEG_B | SEG_C |     SEG_E | SEG_F | SEG_G,    			//0x6E
	['I'] =             SEG_B | SEG_C,                         			//0x60
	['J'] =             SEG_B | SEG_C | SEG_D,                 			//0x70
	['L'] =           SEG_D | SEG_E | SEG_F,                   			//0x1C
	['n'] =           SEG_C |     SEG_E | SEG_G,               			//0x2A
	['o'] =           SEG_C | SEG_D | SEG_E | SEG_G,           			//0x3A
	['P'] = SEG_A | SEG_B |     SEG_E | SEG_F | SEG_G,         			//0xCE
	['q'] = SEG_A | SEG_B | SEG_C |         SEG_F | SEG_G,     			//0xE6
	['r'] =            SEG_E | SEG_G,                         			//0x0A
	['S'] = SEG_A |      SEG_C | SEG_D |     SEG_F | SEG_G,    			//0xB6
	['t'] =           SEG_D | SEG_E | SEG_F | SEG_G,           			//0x1E
	['U'] =      SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,        			//0x7C
	['y'] =      SEG_B | SEG_C | SEG_D |     SEG_F | SEG_G,    			//0x76
		
	['-'] =                   SEG_G,                           			//0x02
	['='] =           SEG_D |       SEG_G,                     			//0x12
	[' '] = 0x00,                                              			//0x00
#ifdef SEG_DP			
	['.'] = SEG_DP,                                            			//0x01
#endif // SEG_DP
};

#endif // SEVENSEG_MAP_H