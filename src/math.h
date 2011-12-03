/******************************
** Tsunagari Tile Engine     **
** math.h                    **
** Copyright 2011 OmegaSDG   **
******************************/

#ifndef MATH_H
#define MATH_H

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

//! Integer vector.
typedef vec2<int> ivec2;
typedef vec3<int> ivec3;

//! Real vector.
typedef vec2<double> rvec2;
typedef vec3<double> rvec3;

//! Coordinates.
typedef ivec3 icoord;
typedef rvec3 rcoord;

#endif

