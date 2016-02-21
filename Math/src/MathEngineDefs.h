#pragma once

#include "MathEngineConfig.h"

#include <cassert>

#if defined(MATH_SIMD) && defined(MATH_NO_SIMD)
#error Cannot define both MATH_SIMD and MATH_NO_SIMD simultaneously.
#endif

#if !defined(MATH_SIMD) && !defined(MATH_NO_SIMD)
#error Must define either MATH_SIMD or MATH_NO_SIMD.
#endif

#ifdef MATH_SIMD

	#include <xmmintrin.h>
	#include <smmintrin.h>

	#define M128_TYPE __m128

	#define CHECK_ALIGNMENT_16(ptr) GameAssert(((unsigned long) (ptr) & (16-1)) == 0)

	inline const M128_TYPE operator+(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return _mm_add_ps(lhs, rhs);
	}

	inline const M128_TYPE operator-(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return _mm_sub_ps(lhs, rhs);
	}

	inline const M128_TYPE operator*(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return _mm_mul_ps(lhs, rhs);
	}

	inline const M128_TYPE operator*(const M128_TYPE& lhs, const float& rhs)
	{
		M128_TYPE rhs128 = _mm_set1_ps(rhs);
		return _mm_mul_ps(lhs, rhs128);
	}

	inline const M128_TYPE operator/(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return _mm_div_ps(lhs, rhs);
	}

	inline void operator+=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs = _mm_add_ps(lhs, rhs);
	}

	inline void operator-=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs = _mm_sub_ps(lhs, rhs);
	}
	
	inline void operator*=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs = _mm_mul_ps(lhs, rhs);
	}

	inline void operator*=(M128_TYPE& lhs, const float& rhs)
	{
		M128_TYPE val = _mm_set1_ps(rhs);
		lhs = _mm_mul_ps(lhs, val);
	}

	inline void operator/=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs = _mm_div_ps(lhs, rhs);
	}

	// This version of dot product is faster than _mm_dp_ps intrinsic
	inline void _dot_product_ps(const M128_TYPE& m1, const M128_TYPE& m2, float& result)
	{
		M128_TYPE r = _mm_mul_ps(m1, m2);
		r = _mm_hadd_ps(r, r);
		r = _mm_hadd_ps(r, r);
		_mm_store_ss(&result, r);
	}

	// Shamelessly stolen from StackOverflow
	// Much more efficient than the way I was doing it
	// Takes advantage of the fact that the difference between a negative float
	// and a positive float is the highest order bit
	inline const M128_TYPE _mm_abs_ps(const M128_TYPE& x)
	{
		static const __m128 sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
		return _mm_andnot_ps(sign_mask, x);
	}

#else // MATH_NO_SIMD
	
	#include <stdint.h>
	#include <math.h>

	struct Math128
	{
		Math128();
		Math128(const float f);
		Math128(const float f1, const float f2, const float f3, const float f4);
		Math128(const Math128& m);
		Math128& operator=(const Math128& m);
		~Math128();

		// Suppress warnings regarding anonymous union/struct
#pragma warning(disable : 4201)
		union
		{
			float m128_f32[4];

			struct
			{
				float f0;
				float f1;
				float f2;
				float f3;
			};
		};
	};

	#define M128_TYPE Math128

	#define CHECK_ALIGNMENT_16(ptr)






	inline const M128_TYPE operator+(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return M128_TYPE(lhs.f0 + rhs.f0, lhs.f1 + rhs.f1, lhs.f2 + rhs.f2, lhs.f3 + rhs.f3);
	}

	inline const M128_TYPE operator-(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return M128_TYPE(lhs.f0 - rhs.f0, lhs.f1 - rhs.f1, lhs.f2 - rhs.f2, lhs.f3 - rhs.f3);
	}

	inline const M128_TYPE operator*(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return M128_TYPE(lhs.f0 * rhs.f0, lhs.f1 * rhs.f1, lhs.f2 * rhs.f2, lhs.f3 * rhs.f3);
	}

	inline const M128_TYPE operator*(const M128_TYPE& lhs, const float& rhs)
	{
		return M128_TYPE(lhs.f0 * rhs, lhs.f1 * rhs, lhs.f2 * rhs, lhs.f3 * rhs);
	}

	inline const M128_TYPE operator/(const M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		return M128_TYPE(lhs.f0 / rhs.f0, lhs.f1 / rhs.f1, lhs.f2 / rhs.f2, lhs.f3 / rhs.f3);
	}

	inline void operator+=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs.f0 += rhs.f0;
		lhs.f1 += rhs.f1;
		lhs.f2 += rhs.f2;
		lhs.f3 += rhs.f3;
	}

	inline void operator-=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs.f0 -= rhs.f0;
		lhs.f1 -= rhs.f1;
		lhs.f2 -= rhs.f2;
		lhs.f3 -= rhs.f3;
	}

	inline void operator*=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs.f0 *= rhs.f0;
		lhs.f1 *= rhs.f1;
		lhs.f2 *= rhs.f2;
		lhs.f3 *= rhs.f3;
	}

	inline void operator*=(M128_TYPE& lhs, const float& rhs)
	{
		lhs.f0 *= rhs;
		lhs.f1 *= rhs;
		lhs.f2 *= rhs;
		lhs.f3 *= rhs;
	}

	inline void operator/=(M128_TYPE& lhs, const M128_TYPE& rhs)
	{
		lhs.f0 /= rhs.f0;
		lhs.f1 /= rhs.f1;
		lhs.f2 /= rhs.f2;
		lhs.f3 /= rhs.f3;
	}

	inline void _dot_product_ps(const M128_TYPE& m1, const M128_TYPE& m2, float& result)
	{
		M128_TYPE r = m1;
		r *= m2;

		result = r.f0;
		result += r.f1;
		result += r.f2;
		result += r.f3;
	}

	inline const M128_TYPE _mm_abs_ps(const M128_TYPE& m)
	{
		return M128_TYPE(fabsf(m.f0), fabsf(m.f1), fabsf(m.f2), fabsf(m.f3));
	}
	
#endif