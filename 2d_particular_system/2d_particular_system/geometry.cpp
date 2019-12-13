#pragma once
#include <cmath>

template <typename T>
struct Vec2
{
	union
	{
		struct { T x, y; };
		T raw[2];
	};

	inline Vec2() : x(0), y(0) {}
	inline Vec2(T x, T y) : x(x), y(y) {}

	inline Vec2<T> operator + (Vec2<T> other)
	{
		return 	Vec2<T> (x + other.x, y + other.y);
	}

	inline Vec2<T> operator * (float scalar)
	{
		return	Vec2<T>(x * scalar, y * scalar);
	}

	Vec2<T>& operator += (Vec2<T> point)
	{
		x += point.x;
		y += point.y;
		return *this;
	}

	Vec2<T>& operator *= (T lenght)
	{
		x *= lenght;
		y *= lenght;
		return *this;
	}

	float norm()
	{
		return std::sqrt(x * x + y * y);
	}

	inline Vec2<T>& normalize()
	{
		*this = (*this) * (1.0f / norm());
		return *this;
	}


	inline T& operator [] (int inx)
	{
		return raw[inx];
	}
};


template <typename T>
struct Vec3
{
	union
	{
		struct { T x, y, z; };
		T raw[3];
	};

	inline Vec3() : x(0), y(0), z(0) {}
	inline Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
	template <typename U>
	inline Vec3(Vec2<U> v) : x(v.x), y(v.y), z(0) {}

	inline Vec3<T>& operator + (Vec3<T> other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	inline Vec3<T>& operator - (Vec3<T> other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	inline Vec3<T>& operator * (float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	inline T& operator [] (int inx)
	{
		return raw[inx];
	}

	float norm()
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	inline Vec3<T>& normalize()
	{
		*this = (*this) * (1.0f / norm());
		return *this;
	}

};

typedef Vec2<int> Vert2i;
typedef Vec2<double> Vert2f;
typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2f;

typedef Vec3<int> Vert3i;
typedef Vec3<float> Vert3f;
typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;

template <typename T>
inline float dproduct(Vec3<T> a, Vec3<T> b)
{
	float res = 0;
	for (int i = 0; i < 3; i++)
		res += a[i] * b[i];

	return res;
}

template <typename T>
inline int dproduct(Vec2<T> a, Vec2<T> b)
{
	int res = 0;
	for (int i = 0; i < 2; i++)
		res += a[i] * b[i];

	return res;
}

template <typename T>
inline void cproduct(Vec3<T> a, Vec3<T> b, Vec3<T>* res)
{
	res->x = a.y * b.z - a.z * b.y;
	res->y = a.z * b.x - a.x * b.z;
	res->z = a.x * b.y - a.y * b.x;
}

template <typename T>
Vec3<T> cross(Vec3<T> v1, Vec3<T> v2) {
	return Vec3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}


inline bool barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3i P, Vec3f* out) {
	Vec3f s[2];
	for (int i = 0; i < 2; i++) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2)
	{
		out->x = 1.0f - (u.x + u.y) / u.z;
		out->y = u.y / u.z;
		out->z = u.x / u.z;
		if (out->x >= 0 && out->y >= 0 && out->z >= 0)
			return true;
	}
	return false;
}