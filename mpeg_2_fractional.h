/*
 * mpeg_2_fractional.h
 *
 * Copyright 2014 Fernando Rodriguez (support@fernansoft.com). 
 * All rights reserved
 *
 */

#ifndef MP3_FRACTIONAL_H
#define MP3_FRACTIONAL_H

/*
// 16 bit signed fractional type
*/
#if defined(__XC16__) || defined(__C30__)
	typedef int mp3_frac;
#elif defined(__XC32__) || defined(_MSC_VER)
	typedef short mp3_frac;
#elif __GNUC__
	typedef short mp3_frac;
#else
	#error "Fractional type not defined."
#endif

#define MP3_FRACBITS			(14)
#define MP3_FRAC_MIN			((mp3_frac) -0x8000)
#define MP3_FRAC_MAX			((mp3_frac) +0x7fff)
#define MP3_FRAC_ONE			((mp3_frac) +0x4000)

#if (defined(__XC16__) || defined(__C30__))
	#if 1
		extern mp3_frac __result __attribute__((__near__));

		#if 1
		#pragma GCC diagnostic ignored "-Wvolatile-register-var"
		#define MP3_FRAC_MUL(x, y)					\
		({										\
			volatile register int _acc asm("A");		\
			_acc = __builtin_mpy(x, y, 0, 0, 0, 0, 0, 0);	\
			__result = __builtin_sacr(_acc, MP3_FRACBITS - 16);		\
			__result; 							\
		})
		#else
		#define MP3_FRAC_MUL(x, y)					\
		({										\
			asm	volatile							\
			(									\
				"mpy %1 * %2, b\n"				\
				"sac.r b, #%3,%0\n"			\
				/*"mov w5, %0"*/					\
				: "=r" (__result)								\
				: "z" (x), "z" (y), "i" (MP3_FRACBITS - 16)			\
				: /*"w5",*/ 			\
			); 									\
			__result; 							\
		})
		#endif
		/*
		// these are no longer needed since all signal
		// processing is done in assembly
		*/
		#define MP3_FRAC_MPY(x, y)	__asm__("mpy %0 * %1, a\n" : : "z" (x), "z" (y))
		#define MP3_FRAC_MAC(x, y)	__asm__("mac %0 * %1, a\n" : : "z" (x), "z" (y))
		#define MP3_FRAC_NEG()		__asm__("neg a")
		#define MP3_FRAC_SAC()					\
		({ 									\
			__asm__							\
			(								\
				"sac.r a, #%1, %0" 			\
				: "=r" (__result) 			\
				: "i" (MP3_FRACBITS - 16) \
			); 								\
			__result;						\
		})
		#define MP3_FRAC_SAC_SHIFT(scale)			\
		({ 									\
			__asm__							\
			(								\
				"sac.r a, #%1, %0" 			\
				: "=r" (__result) 			\
				: "i" ((MP3_FRACBITS - 16) + scale) \
			); 								\
			__result;						\
		})
	#else
	
		#define ANSI_C
	
		extern long acc;
	
		#ifndef ANSI_C
			#define MP3_FRAC_MUL(x, y)					\
			({										\
				long _p = (long) (x) * (long) (y);	\
				_p >>= MP3_FRACBITS;				\
				if (_p > MP3_FRAC_MAX)					\
				{									\
					_p = MP3_FRAC_MAX;					\
				}									\
				else if (_p < MP3_FRAC_MIN)			\
				{									\
					_p = MP3_FRAC_MIN;					\
				}									\
				(mp3_frac) _p;					\
			})
		#else
			static inline mp3_frac MP3_FRAC_MUL(mp3_frac x, mp3_frac y)
			{
				long _p = (long) (x) * (long) (y);
				_p >>= MP3_FRACBITS;
				//if (_p > MP3_FRAC_MAX)
				//{
				//	_p = MP3_FRAC_MAX;
				//}
				//else if (_p < MP3_FRAC_MIN)
				//{
				//	_p = MP3_FRAC_MIN;
				//}
				return (mp3_frac) _p;
			}
		#endif
		#define MP3_FRAC_MPY(x, y) 	acc  = ((long) (x)) * ((long) (y))
		#define MP3_FRAC_MAC(x, y) 	acc += ((long) (x)) * ((long) (y))
		#define MP3_FRAC_NEG() 		acc = -acc
		#ifndef ANSI_C
			#define MP3_FRAC_SAC()						\
			({										\
				acc >>= MP3_FRACBITS;				\
				if (acc > MP3_FRAC_MAX)				\
				{									\
					acc = MP3_FRAC_MAX;				\
				}									\
				else if (acc < MP3_FRAC_MIN)			\
				{									\
					acc = MP3_FRAC_MIN;				\
				}									\
				(mp3_frac) acc;			\
			})
		#else
			static inline mp3_frac MP3_FRAC_SAC()
			{
				acc >>= MP3_FRACBITS;
				//if (acc > MP3_FRAC_MAX)
				//{
				//	acc = MP3_FRAC_MAX;
				//}
				//else if (acc < MP3_FRAC_MIN)
				//{
				//	acc = MP3_FRAC_MIN;
				//}
				return (mp3_frac) acc;
			}
		#endif
		#ifndef ANSI_C
			#define MP3_FRAC_SAC_SHIFT(scale)			\
			({										\
				acc >>= (MP3_FRACBITS + scale);		\
				if (acc > MP3_FRAC_MAX)				\
				{									\
					acc = MP3_FRAC_MAX;				\
				}									\
				else if (acc < MP3_FRAC_MIN)			\
				{									\
					acc = MP3_FRAC_MIN;				\
				}									\
				(mp3_frac) acc;					\
			})
		#else
			static inline mp3_frac MP3_FRAC_SAC_SHIFT(int scale)
			{
				acc >>= (MP3_FRACBITS + scale);
				//if (acc > MP3_FRAC_MAX)
				//{
				//	acc = MP3_FRAC_MAX;
				//}
				//else if (acc < MP3_FRAC_MIN)
				//{
				//	acc = MP3_FRAC_MIN;
				//}
				return (mp3_frac) acc;
			}
		#endif

	#endif

#elif defined(__XC32__)
#if 1
	static inline mp3_frac MP3_FRAC_MUL(mp3_frac x, mp3_frac y)
	{
		int _p = (int) x * (int) y;
		_p += (1 << (MP3_FRACBITS - 1));
		_p >>= MP3_FRACBITS;
		
		//if (_p > MP3_FRAC_MAX)
		//{
		//	_p = MP3_FRAC_MAX;
		//}
		//else if (_p < MP3_FRAC_MIN)
		//{
		//	_p = MP3_FRAC_MIN;
		//}
		

		return (mp3_frac) _p;
	}
	extern long acc;
	#define MP3_FRAC_MPY(x, y)			((acc)  = MP3_FRAC_MUL((x), (y)))
	#define MP3_FRAC_MAC(x, y)			((acc) += MP3_FRAC_MUL((x), (y)))
	#define MP3_FRAC_NEG()				((acc)  = -(acc))
	static inline mp3_frac MP3_FRAC_SAC()
	{
		//if (acc > MP3_FRAC_MAX)
		//{
		//	acc = MP3_FRAC_MAX;
		//}
		//else if (acc < MP3_FRAC_MIN)
		//{
		//	acc = MP3_FRAC_MIN;
		//}
		return (mp3_frac) acc;
	}
	static inline MP3_FRAC_SAC_SHIFT(int scale)	
	{
		//if (acc > MP3_FRAC_MAX)
		//{
		//	acc = MP3_FRAC_MAX;
		//}
		//else if (acc < MP3_FRAC_MIN)
		//{
		//	acc = MP3_FRAC_MIN;
		//}
		return ((mp3_frac) acc) << -scale;
	}
#else
	#define mp3_frac_long int
	extern mp3_frac_long acc;

	static inline mp3_frac MP3_FRAC_MUL(mp3_frac x, mp3_frac y)
	{
		mp3_frac_long _x = (mp3_frac_long) x;
		mp3_frac_long _y = (mp3_frac_long) y;
		mp3_frac_long _p = _x * _y;
		_p >>= MP3_FRACBITS;
		_p += (1 << (MP3_FRACBITS - 1));
		/*
		if (_p > MP3_FRAC_MAX)
		{
			_p = MP3_FRAC_MAX;
		}
		else if (_p < MP3_FRAC_MIN)
		{
			_p = MP3_FRAC_MIN;
		}
		*/
		return (mp3_frac) _p;
	}
	#define MP3_FRAC_MPY(x, y) 	acc  = ((mp3_frac_long) x) * ((mp3_frac_long) y)
	#define MP3_FRAC_MAC(x, y) 	acc += ((mp3_frac_long) x) * ((mp3_frac_long) y)
	#define MP3_FRAC_NEG() 		acc = -acc;
	static inline mp3_frac MP3_FRAC_SAC()
	{
		acc += (1 << (MP3_FRACBITS - 1));
		acc >>= MP3_FRACBITS;
		/*
		if (acc > MP3_FRAC_MAX)
		{
			acc = MP3_FRAC_MAX;
		}
		else if (acc < MP3_FRAC_MIN)
		{
			acc = MP3_FRAC_MIN;
		}
		*/
		return (mp3_frac) acc;
	}
	static i mp3_frac MP3_FRAC_SAC_SHIFT(int scale)
	{
		acc += (1 << (MP3_FRACBITS - 1));
		acc >>= (MP3_FRACBITS + scale);
		/*
		if (acc > MP3_FRAC_MAX)
		{
			acc = MP3_FRAC_MAX;
		}
		else if (acc < MP3_FRAC_MIN)
		{
			acc = MP3_FRAC_MIN;
		}
		*/
		return (mp3_frac) acc;
	}
#endif

#elif defined(_MSC_VER)
#if 1 && !defined(__GNUC__)
	static __forceinline mp3_frac MP3_FRAC_MUL(mp3_frac x, mp3_frac y)
	{
		int _p = (int) x * (int) y;
		_p += (1 << (MP3_FRACBITS - 1));
		_p >>= MP3_FRACBITS;
		
		//if (_p > MP3_FRAC_MAX)
		//{
		//	_p = MP3_FRAC_MAX;
		//}
		//else if (_p < MP3_FRAC_MIN)
		//{
		//	_p = MP3_FRAC_MIN;
		//}
		

		return (mp3_frac) _p;
	}
	extern __int64 acc;
	#define MP3_FRAC_MPY(x, y)			((acc)  = MP3_FRAC_MUL((x), (y)))
	#define MP3_FRAC_MAC(x, y)			((acc) += MP3_FRAC_MUL((x), (y)))
	#define MP3_FRAC_NEG()				((acc)  = -(acc))
	static __forceinline mp3_frac MP3_FRAC_SAC()
	{
		//if (acc > MP3_FRAC_MAX)
		//{
		//	acc = MP3_FRAC_MAX;
		//}
		//else if (acc < MP3_FRAC_MIN)
		//{
		//	acc = MP3_FRAC_MIN;
		//}
		return (mp3_frac) acc;
	}
	static __forceinline MP3_FRAC_SAC_SHIFT(int scale)	
	{
		//if (acc > MP3_FRAC_MAX)
		//{
		//	acc = MP3_FRAC_MAX;
		//}
		//else if (acc < MP3_FRAC_MIN)
		//{
		//	acc = MP3_FRAC_MIN;
		//}
		return ((mp3_frac) acc) << -scale;
	}
#else
	#define mp3_frac_long __int32
	extern __int32 acc;

	static __forceinline mp3_frac MP3_FRAC_MUL(mp3_frac x, mp3_frac y)
	{
		mp3_frac_long _x = (mp3_frac_long) x;
		mp3_frac_long _y = (mp3_frac_long) y;
		mp3_frac_long _p = _x * _y;
		_p >>= MP3_FRACBITS;
		_p += (1 << (MP3_FRACBITS - 1));
		/*
		if (_p > MP3_FRAC_MAX)
		{
			_p = MP3_FRAC_MAX;
		}
		else if (_p < MP3_FRAC_MIN)
		{
			_p = MP3_FRAC_MIN;
		}
		*/
		return (mp3_frac) _p;
	}
	#define MP3_FRAC_MPY(x, y) 	acc  = ((mp3_frac_long) x) * ((mp3_frac_long) y)
	#define MP3_FRAC_MAC(x, y) 	acc += ((mp3_frac_long) x) * ((mp3_frac_long) y)
	#define MP3_FRAC_NEG() 		acc = -acc;
	static __forceinline mp3_frac MP3_FRAC_SAC()
	{
		acc += (1 << (MP3_FRACBITS - 1));
		acc >>= MP3_FRACBITS;
		/*
		if (acc > MP3_FRAC_MAX)
		{
			acc = MP3_FRAC_MAX;
		}
		else if (acc < MP3_FRAC_MIN)
		{
			acc = MP3_FRAC_MIN;
		}
		*/
		return (mp3_frac) acc;
	}
	static __inline mp3_frac MP3_FRAC_SAC_SHIFT(int scale)
	{
		acc += (1 << (MP3_FRACBITS - 1));
		acc >>= (MP3_FRACBITS + scale);
		/*
		if (acc > MP3_FRAC_MAX)
		{
			acc = MP3_FRAC_MAX;
		}
		else if (acc < MP3_FRAC_MIN)
		{
			acc = MP3_FRAC_MIN;
		}
		*/
		return (mp3_frac) acc;
	}
#endif
#elif defined(__GNUC__)

#define ANSI_C

extern long acc;

#ifndef ANSI_C
	#define MP3_FRAC_MUL(x, y)					\
	({										\
		long _p = (long) (x) * (long) (y);	\
		_p >>= MP3_FRACBITS;				\
		if (_p > MP3_FRAC_MAX)					\
		{									\
			_p = MP3_FRAC_MAX;					\
		}									\
		else if (_p < MP3_FRAC_MIN)			\
		{									\
			_p = MP3_FRAC_MIN;					\
		}									\
		(mp3_frac) _p;					\
	})
#else
	static inline mp3_frac MP3_FRAC_MUL(mp3_frac x, mp3_frac y)
	{
		long _p = (long) (x) * (long) (y);
		_p >>= MP3_FRACBITS;
		//if (_p > MP3_FRAC_MAX)
		//{
		//	_p = MP3_FRAC_MAX;
		//}
		//else if (_p < MP3_FRAC_MIN)
		//{
		//	_p = MP3_FRAC_MIN;
		//}
		return (mp3_frac) _p;
	}
#endif
#define MP3_FRAC_MPY(x, y) 	acc  = ((long) (x)) * ((long) (y))
#define MP3_FRAC_MAC(x, y) 	acc += ((long) (x)) * ((long) (y))
#define MP3_FRAC_NEG() 		acc = -acc
#ifndef ANSI_C
	#define MP3_FRAC_SAC()						\
	({										\
		acc >>= MP3_FRACBITS;				\
		if (acc > MP3_FRAC_MAX)				\
		{									\
			acc = MP3_FRAC_MAX;				\
		}									\
		else if (acc < MP3_FRAC_MIN)			\
		{									\
			acc = MP3_FRAC_MIN;				\
		}									\
		(mp3_frac) acc;			\
	})
#else
	static inline mp3_frac MP3_FRAC_SAC()
	{
		acc >>= MP3_FRACBITS;
		//if (acc > MP3_FRAC_MAX)
		//{
		//	acc = MP3_FRAC_MAX;
		//}
		//else if (acc < MP3_FRAC_MIN)
		//{
		//	acc = MP3_FRAC_MIN;
		//}
		return (mp3_frac) acc;
	}
#endif
#ifndef ANSI_C
	#define MP3_FRAC_SAC_SHIFT(scale)			\
	({										\
		acc >>= (MP3_FRACBITS + scale);		\
		if (acc > MP3_FRAC_MAX)				\
		{									\
			acc = MP3_FRAC_MAX;				\
		}									\
		else if (acc < MP3_FRAC_MIN)			\
		{									\
			acc = MP3_FRAC_MIN;				\
		}									\
		(mp3_frac) acc;					\
	})
#else
	static inline mp3_frac MP3_FRAC_SAC_SHIFT(int scale)
	{
		acc >>= (MP3_FRACBITS + scale);
		//if (acc > MP3_FRAC_MAX)
		//{
		//	acc = MP3_FRAC_MAX;
		//}
		//else if (acc < MP3_FRAC_MIN)
		//{
		//	acc = MP3_FRAC_MIN;
		//}
		return (mp3_frac) acc;
	}
#endif


#else
	#error "Fixed point math routines not implemented."
#endif

#endif
