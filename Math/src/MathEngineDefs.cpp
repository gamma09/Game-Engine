#include "MathEngineDefs.h"

#ifdef MATH_NO_SIMD

	Math128::Math128()
	{
		// Do nothing
	}

	Math128::Math128(const float f) :
		f0(f),
		f1(f),
		f2(f),
		f3(f)
	{
		// Do nothing
	}

	Math128::Math128(const float _f0, const float _f1, const float _f2, const float _f3) :
		f0(_f0),
		f1(_f1),
		f2(_f2),
		f3(_f3)
	{
		// Do nothing
	}

	Math128::Math128(const Math128& m) :
		f0(m.f0),
		f1(m.f1),
		f2(m.f2),
		f3(m.f3)
	{
		// Do nothing
	}

	Math128& Math128::operator=(const Math128& m)
	{
		this->f0 = m.f0;
		this->f1 = m.f1;
		this->f2 = m.f2;
		this->f3 = m.f3;

		return *this;
	}

	Math128::~Math128()
	{
		// Do nothing
	}

#endif
