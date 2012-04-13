/*********************************
** Tsunagari Tile Engine        **
** vec.h                        **
** Copyright 2011-2012 OmegaSDG **
*********************************/

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

//! 3D cube type.
struct icube_t {
	int x1, x2;
	int y1, y2;
	int z1, z2;
};

//! cube_t constructor.
icube_t icube(int x1, int y1, int z1,
              int x2, int y2, int z2);

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

