/******************************
** Tsunagari Tile Engine     **
** math.cpp                  **
** Copyright 2011 OmegaSDG   **
******************************/

#include "math.h"

template<class T>
vec3<T>::vec3()
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
	return this;
}

template<class T>
vec3<T>& vec3<T>::operator -=(const vec3<T>& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return this;
}

template<class T>
vec3<T>& vec3<T>::operator *=(T coefficient)
{
	x *= coefficient;
	y *= coefficient;
	z *= coefficient;
	return this;
}

