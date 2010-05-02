/* Copyright (C) 2010 Wildfire Games.
 * This file is part of 0 A.D.
 *
 * 0 A.D. is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * 0 A.D. is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with 0 A.D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_FIXED_VECTOR2D
#define INCLUDED_FIXED_VECTOR2D

#include "maths/Fixed.h"
#include "maths/Sqrt.h"

class CFixedVector2D
{
private:
	typedef CFixed_23_8 fixed;

public:
	fixed X, Y;

	CFixedVector2D() { }

	CFixedVector2D(fixed X, fixed Y) : X(X), Y(Y) { }

	/// Vector equality
	bool operator==(const CFixedVector2D& v) const
	{
		return (X == v.X && Y == v.Y);
	}

	/// Vector inequality
	bool operator!=(const CFixedVector2D& v) const
	{
		return (X != v.X || Y != v.Y);
	}

	/// Vector addition
	CFixedVector2D operator+(const CFixedVector2D& v) const
	{
		return CFixedVector2D(X + v.X, Y + v.Y);
	}

	/// Vector subtraction
	CFixedVector2D operator-(const CFixedVector2D& v) const
	{
		return CFixedVector2D(X - v.X, Y - v.Y);
	}

	/// Negation
	CFixedVector2D operator-() const
	{
		return CFixedVector2D(-X, -Y);
	}

	/// Vector addition
	CFixedVector2D& operator+=(const CFixedVector2D& v)
	{
		*this = *this + v;
		return *this;
	}

	/// Vector subtraction
	CFixedVector2D& operator-=(const CFixedVector2D& v)
	{
		*this = *this - v;
		return *this;
	}

	/// Scalar multiplication by an integer
	CFixedVector2D operator*(int n) const
	{
		return CFixedVector2D(X*n, Y*n);
	}

	/**
	 * Multiply by a CFixed. Likely to overflow if both numbers are large,
	 * so we use an ugly name instead of operator* to make it obvious.
	 */
	CFixedVector2D Multiply(fixed n) const
	{
		return CFixedVector2D(X.Multiply(n), Y.Multiply(n));
	}

	/**
	 * Returns the length of the vector.
	 * Will not overflow if the result can be represented as type 'fixed'.
	 */
	fixed Length() const
	{
		// Do intermediate calculations with 64-bit ints to avoid overflows
		i64 x = (i64)X.GetInternalValue();
		i64 y = (i64)Y.GetInternalValue();
		u64 xx = (u64)(x * x);
		u64 yy = (u64)(y * y);
		u64 d2 = xx + yy;
		CheckUnsignedAdditionOverflow(d2, xx, L"Overflow in CFixedVector2D::Length() part 1")

		u32 d = isqrt64(d2);

		CheckU32CastOverflow(d, i32, L"Overflow in CFixedVector2D::Length() part 2")
		fixed r;
		r.SetInternalValue((i32)d);
		return r;
	}

	bool IsZero() const
	{
		return (X.IsZero() && Y.IsZero());
	}

	/**
	 * Normalize the vector so that length is close to 1.
	 * If length is 0, does nothing.
	 * WARNING: The fixed-point numbers only have 8-bit fractional parts, so
	 * a normalized vector will be very imprecise.
	 */
	void Normalize()
	{
		if (!IsZero())
		{
			fixed l = Length();
			X = X / l;
			Y = Y / l;
		}
	}

	/**
	 * Normalize the vector so that length is close to n.
	 * If length is 0, does nothing.
	 */
	void Normalize(fixed n)
	{
		if (n.IsZero())
		{
			X = Y = fixed::FromInt(0);
			return;
		}

		fixed l = Length();
		// TODO: work out whether this is giving decent precision
		fixed d = l / n;
		if (!d.IsZero())
		{
			X = X / d;
			Y = Y / d;
		}
	}

	/**
	 * Compute the dot product of this vector with another.
	 */
	fixed Dot(const CFixedVector2D& v)
	{
		i64 x = (i64)X.GetInternalValue() * (i64)v.X.GetInternalValue();
		i64 y = (i64)Y.GetInternalValue() * (i64)v.Y.GetInternalValue();
		CheckSignedAdditionOverflow(i64, x, y, L"Overflow in CFixedVector2D::Dot() part 1", L"Underflow in CFixedVector2D::Dot() part 1")
		i64 sum = x + y;
		sum >>= fixed::fract_bits;

		CheckCastOverflow(sum, i32, L"Overflow in CFixedVector2D::Dot() part 2", L"Underflow in CFixedVector2D::Dot() part 2")
		fixed ret;
		ret.SetInternalValue((i32)sum);
		return ret;
	}

	CFixedVector2D Perpendicular()
	{
		return CFixedVector2D(Y, -X);
	}
};

#endif // INCLUDED_FIXED_VECTOR2D
