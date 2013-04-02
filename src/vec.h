/*********************************
** Tsunagari Tile Engine        **
** vec.h                        **
** Copyright 2011-2012 OmegaSDG **
*********************************/

// "OmegaSDG" is defined as Michael D. Reiley and Paul Merrill.

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#ifndef VEC_H
#define VEC_H

/**
 * Virtual integer coordinate.
 *
 * x and y are the same as a physical integer coordinate.
 * z is a virtual layer depth within an Area.
 */
struct vicoord
{
	vicoord() {}
	vicoord(int x, int y, double z): x(x), y(y), z(z) {}

	int x, y;
	double z;
};

struct icube {
	icube(int x1, int y1, int z1, int x2, int y2, int z2)
		: x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2) {}

	int x1, y1, z1;
	int x2, y2, z2;
};

template<class T>
class vec2
{
public:
	T x, y;

	vec2()
	{
	}

	vec2(const vec2<T>& other)
		: x(other.x), y(other.y)
	{
	}

	vec2(T x, T y)
		: x(x), y(y)
	{
	}

	vec2<T>& operator +=(const vec2<T>& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	vec2<T>& operator -=(const vec2<T>& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	vec2<T>& operator *=(const vec2<T>& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	vec2<T>& operator *=(T coefficient)
	{
		x *= coefficient;
		y *= coefficient;
		return *this;
	}

	vec2<T>& operator /=(const vec2<T>& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	vec2<T>& operator /=(T coefficient)
	{
		x /= coefficient;
		y /= coefficient;
		return *this;
	}

	operator bool()
	{
		return x || y;
	}
};


template<class T>
class vec3
{
public:
	T x, y, z;

	vec3()
	{
	}

	vec3(const vec3<T>& other)
		: x(other.x), y(other.y), z(other.z)
	{
	}

	vec3(T x, T y, T z)
		: x(x), y(y), z(z)
	{
	}

	vec3<T>& operator +=(const vec3<T>& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	vec3<T>& operator -=(const vec3<T>& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	vec3<T>& operator *=(const vec3<T>& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	vec3<T>& operator *=(T coefficient)
	{
		x *= coefficient;
		y *= coefficient;
		z *= coefficient;
		return *this;
	}

	vec3<T>& operator /=(const vec3<T>& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	vec3<T>& operator /=(T coefficient)
	{
		x /= coefficient;
		y /= coefficient;
		z /= coefficient;
		return *this;
	}

	operator bool()
	{
		return x || y || z;
	}
};

template<class T>
vec2<T> operator+(const vec2<T>& a, const vec2<T>& b)
{
	vec2<T> c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}

template<class T>
vec3<T> operator+(const vec3<T>& a, const vec3<T>& b)
{
	vec3<T> c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
	return c;
}

template<class T>
vec2<T> operator-(const vec2<T>& a, const vec2<T>& b)
{
	vec2<T> c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	return c;
}

template<class T>
vec3<T> operator-(const vec3<T>& a, const vec3<T>& b)
{
	vec3<T> c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
}

template<class T>
vec2<T> operator*(const vec2<T>& a, const vec2<T>& b)
{
	vec2<T> c;
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	return c;
}

template<class T>
vec3<T> operator*(const vec3<T>& a, const vec3<T>& b)
{
	vec3<T> c;
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
	return c;
}

template<class T, class CO>
vec2<T> operator*(const vec2<T>& a, CO co)
{
	vec2<T> c;
	c.x = a.x * (T)co;
	c.y = a.y * (T)co;
	return c;
}

template<class T, class CO>
vec3<T> operator*(const vec3<T>& a, CO co)
{
	vec3<T> c;
	c.x = a.x * (T)co;
	c.y = a.y * (T)co;
	c.z = a.z * (T)co;
	return c;
}

template<class T, class CO>
vec2<T> operator*(CO co, const vec2<T>& a)
{
	return a * co;
}

template<class T, class CO>
vec3<T> operator*(CO co, const vec3<T>& a)
{
	return a * co;
}

template<class T>
vec2<T> operator/(const vec2<T>& a, const vec2<T>& b)
{
	vec2<T> c;
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	return c;
}

template<class T>
vec3<T> operator/(const vec3<T>& a, const vec3<T>& b)
{
	vec3<T> c;
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
	return c;
}

template<class T, class CO>
vec2<T> operator/(const vec2<T>& a, CO co)
{
	vec2<T> c;
	c.x = a.x / (T)co;
	c.y = a.y / (T)co;
	return c;
}

template<class T, class CO>
vec3<T> operator/(const vec3<T>& a, CO co)
{
	vec3<T> c;
	c.x = a.x / (T)co;
	c.y = a.y / (T)co;
	c.z = a.z / (T)co;
	return c;
}

template<class T, class CO>
vec2<T> operator/(CO co, const vec2<T>& a)
{
	vec2<T> c;
	c.x = (T)co / a.x;
	c.y = (T)co / a.y;
	return c;
}

template<class T, class CO>
vec3<T> operator/(CO co, const vec3<T>& a)
{
	vec3<T> c;
	c.x = (T)co / a.x;
	c.y = (T)co / a.y;
	c.z = (T)co / a.z;
	return c;
}

template<class T>
bool operator==(const vec2<T>& a, const vec2<T>& b)
{
	return a.x == b.x && a.y == b.y;
}

template<class T>
bool operator==(const vec3<T>& a, const vec3<T>& b)
{
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

//! Integer vector.
typedef vec2<int> ivec2;
typedef vec3<int> ivec3;

//! Real vector.
typedef vec2<double> rvec2;
typedef vec3<double> rvec3;

//! Coordinates.
typedef ivec3 icoord;
typedef rvec3 rcoord;


void exportVecs();

#endif

