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
	vec2();
	vec2(const vec2<T>& other);
	vec2(T x, T y);

	vec2& operator +=(const vec2<T>& other);
	vec2& operator -=(const vec2<T>& other);
	vec2& operator *=(const vec2<T>& other);
	vec2& operator *=(T coefficient);
	vec2& operator /=(const vec2<T>& other);
	vec2& operator /=(T coefficient);

	T x, y;
};


template<class T>
class vec3
{
public:
	vec3();
	vec3(const vec3<T>& other);
	vec3(T x, T y, T z);

	vec3& operator +=(const vec3<T>& other);
	vec3& operator -=(const vec3<T>& other);
	vec3& operator *=(const vec3<T>& other);
	vec3& operator *=(T coefficient);
	vec3& operator /=(const vec3<T>& other);
	vec3& operator /=(T coefficient);

	T x, y, z;
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

// For template member definitions.
#include "math.cpp"

#endif

