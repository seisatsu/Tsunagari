/******************************
** Tsunagari Tile Engine     **
** math.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include "math.h"

/*
 * vec2
 */

template<class T>
vec2<T>::vec2()
{
}

template<class T>
vec2<T>::vec2(const vec2<T>& other)
	: x(other.x), y(other.y)
{
}

template<class T>
vec2<T>::vec2(T x, T y)
	: x(x), y(y)
{
}

template<class T>
vec2<T>& vec2<T>::operator +=(const vec2<T>& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

template<class T>
vec2<T>& vec2<T>::operator -=(const vec2<T>& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

template<class T>
vec2<T>& vec2<T>::operator *=(const vec2<T>& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}

template<class T>
vec2<T>& vec2<T>::operator *=(T coefficient)
{
	x *= coefficient;
	y *= coefficient;
	return *this;
}

template<class T>
vec2<T>& vec2<T>::operator /=(const vec2<T>& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}

template<class T>
vec2<T>& vec2<T>::operator /=(T coefficient)
{
	x /= coefficient;
	y /= coefficient;
	return *this;
}


/*
 * vec3
 */

template<class T>
vec3<T>::vec3()
{
}

template<class T>
vec3<T>::vec3(const vec3<T>& other)
	: x(other.x), y(other.y), z(other.z)
{
}

template<class T>
vec3<T>::vec3(T x, T y, T z)
	: x(x), y(y), z(z)
{
}

template<class T>
vec3<T>& vec3<T>::operator +=(const vec3<T>& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

template<class T>
vec3<T>& vec3<T>::operator -=(const vec3<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}

template<class T>
vec3<T>& vec3<T>::operator *=(const vec3<T>& other)
{
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this;
}

template<class T>
vec3<T>& vec3<T>::operator *=(T coefficient)
{
	x *= coefficient;
	y *= coefficient;
	z *= coefficient;
	return *this;
}

template<class T>
vec3<T>& vec3<T>::operator /=(const vec3<T>& other)
{
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}

template<class T>
vec3<T>& vec3<T>::operator /=(T coefficient)
{
	x /= coefficient;
	y /= coefficient;
	z /= coefficient;
	return *this;
}

