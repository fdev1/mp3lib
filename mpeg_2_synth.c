/*
 * mpeg_2_synth.c
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#include "compiler.h"
#include "mpeg_2_fractional.h"
#include "mpeg_2.h"
#include "mpeg_2_synth.h"

/*
// polyphase filterbank and PCM output buffers
*/
#if 1 && (defined(__XC16__) || defined(__C30__))
#if defined(__dsPIC33E__)
	__eds__ mp3_frac __attribute__((section(".mp3_filterbank"), space(ymemory), eds)) mp3_filterbank[2][2][2][16][8];
#else
	mp3_frac __attribute__((section(".mp3_filterbank"), space(ymemory))) mp3_filterbank[2][2][2][16][8];
#endif
unsigned int __attribute__((__near__, section(".mp3"))) mp3_phase;
#else
mp3_frac mp3_filterbank[2][2][2][16][8];
unsigned int mp3_phase;
#endif

#if defined(__XC16__) || defined(__C30__)
	#if 1 || !defined(NDEBUG)
		#define MP3_SECTION_D				".mp3_D"
	#else
		#define MP3_SECTION_D				".mp3_psv0"
	#endif
#endif

/*
// Di coefficients
// ===============
// This is based on table 3-B.3 of ISO but it has been re-ordered in
// order to take advantage of MAC prefetchs on the dsPIC and all values
// have been scaled up by 1 bit and saturated.
*/
#if defined(__XC16__) || defined(__C30__)
mp3_frac const __attribute__((space(psv), section(MP3_SECTION_D), address(0xD244))) mp3_D[17][32] = 
#else
mp3_frac const mp3_D[17][32] = 
#endif
{
	#include "D.dat"
};

/*
// initialize pcm synthesizer
*/
void mp3_synth_init(void)
{
	mp3_synth_mute();
	mp3_phase = 0;
}

/*
// zero out synth filter buffers
*/
#if !defined(__XC16__) && !defined(__C30__)
void mp3_synth_mute(void)
{
	memset(mp3_filterbank, 0, (2 * 2 * 2 * 16 * 8 * sizeof(mp3_frac)));
}
#endif

/*
// fast in[32]->out[32] DCT
*/
#if !defined(__XC16__) && !defined(__C30__)
static void mp3_dct32
(
	mp3_frac const in[32], 
	unsigned int slot,
	mp3_frac lo[16][8], 
	mp3_frac hi[16][8]
)
{	
	mp3_frac t0,   t1,   t2,   t3,   t4,   t5,   t6,   t7;
	mp3_frac t8,   t9,   t10,  t11,  t12,  t13,  t14,  t15;
	mp3_frac t16,  t17,  t18,  t19,  t20,  t21,  t22,  t23;
	mp3_frac t24,  t25,  t26,  t27,  t28,  t29,  t30,  t31;
	mp3_frac t32,  t33,  t34,  t35,  t36,  t37,  t38,  t39;
	mp3_frac t40,  t41,  t42,  t43,  t44,  t45,  t46,  t47;
	mp3_frac t48,  t49,  t50,  t51,  t52,  t53,  t54,  t55;
	mp3_frac t56,  t57,  t58,  t59,  t60,  t61,  t62,  t63;
	mp3_frac t64,  t65,  t66,  t67,  t68,  t69,  t70,  t71;
	mp3_frac t72,  t73,  t74,  t75,  t76,  t77,  t78,  t79;
	mp3_frac t80,  t81,  t82,  t83,  t84,  t85,  t86,  t87;
	mp3_frac t88,  t89,  t90,  t91,  t92,  t93,  t94,  t95;
	mp3_frac t96,  t97,  t98,  t99,  t100, t101, t102, t103;
	mp3_frac t104, t105, t106, t107, t108, t109, t110, t111;
	mp3_frac t112, t113, t114, t115, t116, t117, t118, t119;
	mp3_frac t120, t121, t122, t123, t124, t125, t126, t127;
	mp3_frac t128, t129, t130, t131, t132, t133, t134, t135;
	mp3_frac t136, t137, t138, t139, t140, t141, t142, t143;
	mp3_frac t144, t145, t146, t147, t148, t149, t150, t151;
	mp3_frac t152, t153, t154, t155, t156, t157, t158, t159;
	mp3_frac t160, t161, t162, t163, t164, t165, t166, t167;
	mp3_frac t168, t169, t170, t171, t172, t173, t174, t175;
	mp3_frac t176;
	
	//slot >>= 1;

	/* costab[i] = cos(PI / (2 * 32) * i) */
	#define costab1			0x1ff6
	#define costab2			0x1fd9
	#define costab3			0x1fa7
	#define costab4			0x1f63
	#define costab5			0x1f0a
	#define costab6			0x1e9f
	#define costab7			0x1e21
	#define costab8			0x1d90
	#define costab9			0x1ced
	#define costab10		0x1c39
	#define costab11		0x1b73
	#define costab12		0x1a9b
	#define costab13		0x19b4
	#define costab14		0x18bd
	#define costab15		0x17b6
	#define costab16		0x16a1
	#define costab17		0x157d
	#define costab18		0x144d
	#define costab19		0x1310
	#define costab20		0x11c7
	#define costab21		0x1074
	#define costab22		0x0f16
	#define costab23		0x0daf
	#define costab24		0x0c3f
	#define costab25		0x0ac8
	#define costab26		0x094a
	#define costab27		0x07c6
	#define costab28		0x063e
	#define costab29		0x04b2
	#define costab30		0x0323
	#define costab31		0x0192
	
	t0   = in[0]  + in[31];  
	t16  = MP3_FRAC_MUL(in[0]  - in[31], costab1);
	t1   = in[15] + in[16];  
	t17  = MP3_FRAC_MUL(in[15] - in[16], costab31);
	t41  = t16 + t17;
	t59  = MP3_FRAC_MUL(t16 - t17, costab2);
	t33  = t0  + t1;
	t50  = MP3_FRAC_MUL(t0  - t1,  costab2);
	t2   = in[7]  + in[24];  
	t18  = MP3_FRAC_MUL(in[7]  - in[24], costab15);
	t3   = in[8]  + in[23];  
	t19  = MP3_FRAC_MUL(in[8]  - in[23], costab17);
	t42  = t18 + t19;
	t60  = MP3_FRAC_MUL(t18 - t19, costab30);
	t34  = t2  + t3;
	t51  = MP3_FRAC_MUL(t2  - t3,  costab30);
	t4   = in[3]  + in[28];  
	t20  = MP3_FRAC_MUL(in[3]  - in[28], costab7);
	t5   = in[12] + in[19];  
	t21  = MP3_FRAC_MUL(in[12] - in[19], costab25);
	t43  = t20 + t21;
	t61  = MP3_FRAC_MUL(t20 - t21, costab14);
	t35  = t4  + t5;
	t52  = MP3_FRAC_MUL(t4  - t5,  costab14);
	t6   = in[4]  + in[27];  
	t22  = MP3_FRAC_MUL(in[4]  - in[27], costab9);
	t7   = in[11] + in[20];  
	t23  = MP3_FRAC_MUL(in[11] - in[20], costab23);
	t44  = t22 + t23;
	t62  = MP3_FRAC_MUL(t22 - t23, costab18);
	t36  = t6  + t7;
	t53  = MP3_FRAC_MUL(t6  - t7,  costab18);
	t8   = in[1]  + in[30];  
	t24  = MP3_FRAC_MUL(in[1]  - in[30], costab3);
	t9   = in[14] + in[17];  
	t25  = MP3_FRAC_MUL(in[14] - in[17], costab29);
	t45  = t24 + t25;
	t63  = MP3_FRAC_MUL(t24 - t25, costab6);
	t37  = t8  + t9;
	t54  = MP3_FRAC_MUL(t8  - t9,  costab6);
	t10  = in[6]  + in[25];  
	t26  = MP3_FRAC_MUL(in[6]  - in[25], costab13);
	t11  = in[9]  + in[22];  
	t27  = MP3_FRAC_MUL(in[9]  - in[22], costab19);
	t46  = t26 + t27;
	t64  = MP3_FRAC_MUL(t26 - t27, costab26);
	t38  = t10 + t11;
	t55  = MP3_FRAC_MUL(t10 - t11, costab26);
	t12  = in[2]  + in[29];  
	t28  = MP3_FRAC_MUL(in[2]  - in[29], costab5);
	t13  = in[13] + in[18];  
	t29  = MP3_FRAC_MUL(in[13] - in[18], costab27);
	t47  = t28 + t29;
	t65  = MP3_FRAC_MUL(t28 - t29, costab10);
	t39  = t12 + t13;
	t56  = MP3_FRAC_MUL(t12 - t13, costab10);
	t14  = in[5]  + in[26];  
	t30  = MP3_FRAC_MUL(in[5]  - in[26], costab11);
	t15  = in[10] + in[21];  
	t31  = MP3_FRAC_MUL(in[10] - in[21], costab21);
	t48  = t30 + t31;
	t66  = MP3_FRAC_MUL(t30 - t31, costab22);
	t40  = t14 + t15;
	t57  = MP3_FRAC_MUL(t14 - t15, costab22);
	t69  = t33 + t34;  
	t89  = MP3_FRAC_MUL(t33 - t34, costab4);
	t70  = t35 + t36;  
	t90  = MP3_FRAC_MUL(t35 - t36, costab28);
	t71  = t37 + t38;  
	t91  = MP3_FRAC_MUL(t37 - t38, costab12);
	t72  = t39 + t40;  
	t92  = MP3_FRAC_MUL(t39 - t40, costab20);
	t73  = t41 + t42;  
	t94  = MP3_FRAC_MUL(t41 - t42, costab4);
	t74  = t43 + t44;  
	t95  = MP3_FRAC_MUL(t43 - t44, costab28);
	t75  = t45 + t46;  
	t96  = MP3_FRAC_MUL(t45 - t46, costab12);
	t76  = t47 + t48;  
	t97  = MP3_FRAC_MUL(t47 - t48, costab20);
	t78  = t50 + t51;  
	t100 = MP3_FRAC_MUL(t50 - t51, costab4);
	t79  = t52 + t53;  
	t101 = MP3_FRAC_MUL(t52 - t53, costab28);
	t80  = t54 + t55;  
	t102 = MP3_FRAC_MUL(t54 - t55, costab12);
	t81  = t56 + t57;  
	t103 = MP3_FRAC_MUL(t56 - t57, costab20);
	t83  = t59 + t60;  
	t106 = MP3_FRAC_MUL(t59 - t60, costab4);
	t84  = t61 + t62;  
	t107 = MP3_FRAC_MUL(t61 - t62, costab28);
	t85  = t63 + t64;  
	t108 = MP3_FRAC_MUL(t63 - t64, costab12);
	t86  = t65 + t66;  
	t109 = MP3_FRAC_MUL(t65 - t66, costab20);
	t113 = t69  + t70;
	t114 = t71  + t72;
	
	/*  0 */ hi[15][slot] = t113 + t114;
	/* 16 */ lo[ 0][slot] = MP3_FRAC_MUL(t113 - t114, costab16);
	
	t115 = t73  + t74;
	t116 = t75  + t76;
	t32  = t115 + t116;
	
	/*  1 */ hi[14][slot] = t32;
	
	t118 = t78  + t79;
	t119 = t80  + t81;
	t58  = t118 + t119;
	
	/*  2 */ hi[13][slot] = t58;
	
	t121 = t83  + t84;
	t122 = t85  + t86;
	t67  = t121 + t122;
	
	t49  = (t67 * 2) - t32;
	
	/*  3 */ hi[12][slot] = (t49);
	
	t125 = t89  + t90;
	t126 = t91  + t92;
	t93  = t125 + t126;
	
	/*  4 */ hi[11][slot] = (t93);
	
	t128 = t94  + t95;
	t129 = t96  + t97;
	t98  = t128 + t129;
	t68  = (t98 * 2) - t49;
	
	/*  5 */ hi[10][slot] = (t68);
	
	t132 = t100 + t101;
	t133 = t102 + t103;
	t104 = t132 + t133;
	t82  = (t104 * 2) - t58;
	
	/*  6 */ hi[ 9][slot] = (t82);
	
	t136 = t106 + t107;
	t137 = t108 + t109;
	t110 = t136 + t137;
	t87  = (t110 * 2) - t67;
	t77  = (t87 * 2) - t68;
	
	/*  7 */ hi[ 8][slot] = (t77);
	
	t141 = MP3_FRAC_MUL(t69 - t70, costab8);
	t142 = MP3_FRAC_MUL(t71 - t72, costab24);
	t143 = t141 + t142;
	
	/*  8 */ hi[ 7][slot] = (t143);
	/* 24 */ lo[ 8][slot] = ((MP3_FRAC_MUL(t141 - t142, costab16) * 2) - t143);
	
	t144 = MP3_FRAC_MUL(t73 - t74, costab8);
	t145 = MP3_FRAC_MUL(t75 - t76, costab24);
	t146 = t144 + t145;
	
	t88  = (t146 * 2) - t77;
	
	/*  9 */ hi[ 6][slot] = (t88);
	
	t148 = MP3_FRAC_MUL(t78 - t79, costab8);
	t149 = MP3_FRAC_MUL(t80 - t81, costab24);
	t150 = t148 + t149;
	
	t105 = (t150 * 2) - t82;
	
	/* 10 */ hi[ 5][slot] = (t105);
	
	t152 = MP3_FRAC_MUL(t83 - t84, costab8);
	t153 = MP3_FRAC_MUL(t85 - t86, costab24);
	t154 = t152 + t153;
	t111 = (t154 * 2) - t87;
	t99  = (t111 * 2) - t88;
	
	/* 11 */ hi[ 4][slot] = (t99);
	
	t157 = MP3_FRAC_MUL(t89 - t90, costab8);
	t158 = MP3_FRAC_MUL(t91 - t92, costab24);
	t159 = t157 + t158;
	t127 = (t159 * 2) - t93;
	
	/* 12 */ hi[ 3][slot] = (t127);
	
	t160 = (MP3_FRAC_MUL(t125 - t126, costab16) * 2) - t127;
	
	/* 20 */ lo[ 4][slot] = (t160);
	/* 28 */ lo[12][slot] = ((((MP3_FRAC_MUL(t157 - t158, costab16) * 2) - t159) * 2) - t160);
	
	t161 = MP3_FRAC_MUL(t94 - t95, costab8);
	t162 = MP3_FRAC_MUL(t96 - t97, costab24);
	t163 = t161 + t162;
	t130 = (t163 * 2) - t98;
	t112 = (t130 * 2) - t99;
	
	/* 13 */ hi[ 2][slot] = (t112);
	
	t164 = (MP3_FRAC_MUL(t128 - t129, costab16) * 2) - t130;
	t166 = MP3_FRAC_MUL(t100 - t101, costab8);
	t167 = MP3_FRAC_MUL(t102 - t103, costab24);
	t168 = t166 + t167;
	t134 = (t168 * 2) - t104;
	t120 = (t134 * 2) - t105;
	
	/* 14 */ hi[ 1][slot] = (t120);
	
	t135 = (MP3_FRAC_MUL(t118 - t119, costab16) * 2) - t120;
	
	/* 18 */ lo[ 2][slot] = (t135);
	
	t169 = (MP3_FRAC_MUL(t132 - t133, costab16) * 2) - t134;
	t151 = (t169 * 2) - t135;
	
	/* 22 */ lo[ 6][slot] = (t151);
	
	t170 = (((MP3_FRAC_MUL(t148 - t149, costab16) * 2) - t150) * 2) - t151;
	
	/* 26 */ lo[10][slot] = (t170);
	/* 30 */ lo[14][slot] = ((((((MP3_FRAC_MUL(t166 - t167, costab16) * 2) - t168) * 2) - t169) * 2) - t170);
	
	t171 = MP3_FRAC_MUL(t106 - t107, costab8);
	t172 = MP3_FRAC_MUL(t108 - t109, costab24);
	t173 = t171 + t172;
	t138 = (t173 * 2) - t110;
	t123 = (t138 * 2) - t111;
	t139 = (MP3_FRAC_MUL(t121 - t122, costab16) * 2) - t123;
	t117 = (t123 * 2) - t112;
	
	/* 15 */ hi[ 0][slot] = (t117);
	
	t124 = (MP3_FRAC_MUL(t115 - t116, costab16) * 2) - t117;
	
	/* 17 */ lo[ 1][slot] = (t124);
	
	t131 = (t139 * 2) - t124;
	
	/* 19 */ lo[ 3][slot] = (t131);
	
	t140 = (t164 * 2) - t131;
	
	/* 21 */ lo[ 5][slot] = (t140);
	
	t174 = (MP3_FRAC_MUL(t136 - t137, costab16) * 2) - t138;
	t155 = (t174 * 2) - t139;
	t147 = (t155 * 2) - t140;
	
	/* 23 */ lo[ 7][slot] = (t147);
	
	t156 = (((MP3_FRAC_MUL(t144 - t145, costab16) * 2) - t146) * 2) - t147;
	
	/* 25 */ lo[ 9][slot] = (t156);
	
	t175 = (((MP3_FRAC_MUL(t152 - t153, costab16) * 2) - t154) * 2) - t155;
	t165 = (t175 * 2) - t156;
	
	/* 27 */ lo[11][slot] = (t165);

	t176 = (((((MP3_FRAC_MUL(t161 - t162, costab16) * 2) - t163) * 2) - t164) * 2) - t165;
	
	/* 29 */ lo[13][slot] = (t176);
	/* 31 */ lo[15][slot] = 
		((((((((MP3_FRAC_MUL(t171 - t172, costab16) * 2) - t173) * 2) - t174) * 2) - t175) * 2) - t176);
}
#endif

/*
// synthesize PCM output
*/
#if !defined(__XC16__) && !defined(__C30__)
void mp3_synth(unsigned int nch, unsigned int ns)
{
	#define SCALEBITS 	(-1)

	unsigned int ph, ch, s, sb, pe, po;
	mp3_frac *p_out1, *p_out2;
	mp3_frac (*filter)[2][2][16][8];
	mp3_frac (*sbsample)[36][32];
	mp3_frac (*fe)[8], (*fx)[8], (*fo)[8];
	mp3_frac const (*p_D)[32], *ptr;
	
	for (ch = 0; ch < nch; ch++) 
	{
		sbsample = &mp3_out->sb_samples[ch];
		filter = &mp3_filterbank[ch];
		ph = mp3_phase;
		p_out1 = mp3_out->pcm_samples[ch];

		for (s = 0; s < ns; ++s) 
		{
			mp3_dct32((*sbsample)[s], ph >> 1, (*filter)[0][ph & 1], (*filter)[1][ph & 1]);
	
			pe = ph & ~1;
			po = ((ph - 1) & 0xf) | 1;

			fe = &(*filter)[0][ ph & 1][0];
			fx = &(*filter)[0][~ph & 1][0];
			fo = &(*filter)[1][~ph & 1][0];

			p_D = &mp3_D[0];

			ptr = *p_D + po;
			MP3_FRAC_MPY((*fx)[0], ptr[16]);
			MP3_FRAC_MAC((*fx)[1], ptr[14]);
			MP3_FRAC_MAC((*fx)[2], ptr[12]);
			MP3_FRAC_MAC((*fx)[3], ptr[10]);
			MP3_FRAC_MAC((*fx)[4], ptr[ 8]);
			MP3_FRAC_MAC((*fx)[5], ptr[ 6]);
			MP3_FRAC_MAC((*fx)[6], ptr[ 4]);
			MP3_FRAC_MAC((*fx)[7], ptr[ 2]);
			MP3_FRAC_NEG();

			ptr = *p_D + pe;
			MP3_FRAC_MAC((*fe)[0], ptr[16]);
			MP3_FRAC_MAC((*fe)[1], ptr[14]);
			MP3_FRAC_MAC((*fe)[2], ptr[12]);
			MP3_FRAC_MAC((*fe)[3], ptr[10]);
			MP3_FRAC_MAC((*fe)[4], ptr[ 8]);
			MP3_FRAC_MAC((*fe)[5], ptr[ 6]);
			MP3_FRAC_MAC((*fe)[6], ptr[ 4]);
			MP3_FRAC_MAC((*fe)[7], ptr[ 2]);

			*p_out1++ = MP3_FRAC_SAC_SHIFT(SCALEBITS);
			
			p_out2 = p_out1 + 30;
	
			for (sb = 1; sb < 16; sb++) 
			{
				fe++;
				p_D++;

				ptr = *p_D + po;
				MP3_FRAC_MPY((*fo)[0], ptr[16]);
				MP3_FRAC_MAC((*fo)[1], ptr[14]);
				MP3_FRAC_MAC((*fo)[2], ptr[12]);
				MP3_FRAC_MAC((*fo)[3], ptr[10]);
				MP3_FRAC_MAC((*fo)[4], ptr[ 8]);
				MP3_FRAC_MAC((*fo)[5], ptr[ 6]);
				MP3_FRAC_MAC((*fo)[6], ptr[ 4]);
				MP3_FRAC_MAC((*fo)[7], ptr[ 2]);
				MP3_FRAC_NEG();
				
				ptr = *p_D + pe;
				MP3_FRAC_MAC((*fe)[7], ptr[ 2]);
				MP3_FRAC_MAC((*fe)[6], ptr[ 4]);
				MP3_FRAC_MAC((*fe)[5], ptr[ 6]);
				MP3_FRAC_MAC((*fe)[4], ptr[ 8]);
				MP3_FRAC_MAC((*fe)[3], ptr[10]);
				MP3_FRAC_MAC((*fe)[2], ptr[12]);
				MP3_FRAC_MAC((*fe)[1], ptr[14]);
				MP3_FRAC_MAC((*fe)[0], ptr[16]);
				
				*p_out1++ = MP3_FRAC_SAC_SHIFT(SCALEBITS);
				
				ptr = *p_D - pe;
				MP3_FRAC_MPY((*fe)[0], ptr[15]);
				MP3_FRAC_MAC((*fe)[1], ptr[17]);
				MP3_FRAC_MAC((*fe)[2], ptr[19]);
				MP3_FRAC_MAC((*fe)[3], ptr[21]);
				MP3_FRAC_MAC((*fe)[4], ptr[23]);
				MP3_FRAC_MAC((*fe)[5], ptr[25]);
				MP3_FRAC_MAC((*fe)[6], ptr[27]);
				MP3_FRAC_MAC((*fe)[7], ptr[29]);
				
				ptr = *p_D - po;
				MP3_FRAC_MAC((*fo)[7], ptr[29]);
				MP3_FRAC_MAC((*fo)[6], ptr[27]);
				MP3_FRAC_MAC((*fo)[5], ptr[25]);
				MP3_FRAC_MAC((*fo)[4], ptr[23]);
				MP3_FRAC_MAC((*fo)[3], ptr[21]);
				MP3_FRAC_MAC((*fo)[2], ptr[19]);
				MP3_FRAC_MAC((*fo)[1], ptr[17]);
				MP3_FRAC_MAC((*fo)[0], ptr[15]);
				
				*p_out2-- = MP3_FRAC_SAC_SHIFT(SCALEBITS);
				
				fo++;
			}
	
			p_D++;
	
			ptr = *p_D + po;
			MP3_FRAC_MPY((*fo)[0], ptr[16]);
			MP3_FRAC_MAC((*fo)[1], ptr[14]);
			MP3_FRAC_MAC((*fo)[2], ptr[12]);
			MP3_FRAC_MAC((*fo)[3], ptr[10]);
			MP3_FRAC_MAC((*fo)[4], ptr[ 8]);
			MP3_FRAC_MAC((*fo)[5], ptr[ 6]);
			MP3_FRAC_MAC((*fo)[6], ptr[ 4]);
			MP3_FRAC_MAC((*fo)[7], ptr[ 2]);
			MP3_FRAC_NEG();

			*p_out1 = MP3_FRAC_SAC_SHIFT(SCALEBITS);
			p_out1 += 16;

			ph = (ph + 1) & 0xF;
		}
	}
	mp3_phase = ph;
}
#endif

