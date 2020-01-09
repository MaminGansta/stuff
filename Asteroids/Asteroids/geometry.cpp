template <typename T>
struct vec2
{
	union
	{
		struct { T x, y; };
		T raw[2];
	};

	inline vec2() : x(0), y(0) {}
	inline vec2(T x, T y) : x(x), y(y) {}

	inline vec2<T> operator + (vec2<T> other)
	{
		return 	vec2<T> (x + other.x, y + other.y);
	}

	inline vec2<T> operator - (vec2<T> other)
	{
		return 	vec2<T>(x - other.x, y - other.y);
	}

	inline vec2<T> operator * (float scalar)
	{
		return	vec2<T>(x * scalar, y * scalar);
	}

	vec2<T>& operator += (vec2<T> point)
	{
		x += point.x;
		y += point.y;
		return *this;
	}

	vec2<T>& operator *= (float lenght)
	{
		x *= lenght;
		y *= lenght;
		return *this;
	}

	inline bool operator == (vec2<T> other)
	{
		return x == other.x && y == other.y;
	}

	inline bool operator == (vec2<T>& const other) const
	{
		return x == other.x && y == other.y;
	}

	float norm()
	{
		return std::sqrt(x * x + y * y);
	}

	inline vec2<T>& normalize_yourself()
	{
		*this = (*this) * (1.0f / norm());
		return *this;
	}

	inline vec2<T> normalize()
	{
		return (*this) * (1.0f / norm());
	}

	inline T& operator [] (int inx)
	{
		return raw[inx];
	}
};


template <typename T>
struct vec3
{
	union
	{
		struct { T x, y, z; };
		T raw[3];
	};

	inline vec3() : x(0), y(0), z(0) {}
	inline vec3(T x, T y, T z) : x(x), y(y), z(z) {}
	template <typename U>
	inline vec3(vec2<U> v) : x(v.x), y(v.y), z(0) {}

	inline vec3<T> operator + (vec3<T> b)
	{
		return vec3<T>(x + b.x, y + b.y, z + b.z);
	}

	inline vec3<T> operator - (vec3<T> b)
	{
		return vec3<T>(x - b.x, y - b.y, z - b.z);
	}

	inline vec3<T> operator * (float scalar)
	{
		return vec3<T>(x * scalar, y * scalar, z * scalar);
	}

	inline T& operator [] (int inx)
	{
		return raw[inx];
	}

	float norm()
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	inline vec3<T>& normalize()
	{
		*this = (*this) * (1.0f / norm());
		return *this;
	}
};

typedef vec2<int> vec2i;
typedef vec2<float> vec2f;

typedef vec3<int> vec3i;
typedef vec3<float> vec3f;

template <typename T>
inline float dproduct(vec3<T> a, vec3<T> b)
{
	float res = 0;
	for (int i = 0; i < 3; i++)
		res += a[i] * b[i];

	return res;
}

template <typename T>
inline double dproduct(vec2<T> a, vec2<T> b)
{
	double res = 0;
	for (int i = 0; i < 2; i++)
		res += a[i] * b[i];

	return res;
}

template <typename T>
inline void cproduct(vec3<T> a, vec3<T> b, vec3<T>* res)
{
	res->x = a.y * b.z - a.z * b.y;
	res->y = a.z * b.x - a.x * b.z;
	res->z = a.x * b.y - a.y * b.x;
}

template <typename T>
vec3<T> cross(vec3<T> v1, vec3<T> v2) {
	return vec3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}


inline bool barycentric(vec3f A, vec3f B, vec3f C, vec3i P, vec3f* out) {
	vec3f s[2];
	for (int i = 0; i < 2; i++) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	vec3f u = cross(s[0], s[1]);
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

inline bool fbarycentric(vec3f A, vec3f B, vec3f C, vec3f P, vec3f* out)
{
	vec3f s[2];
	for (int i = 0; i < 2; i++)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	vec3f u = cross(s[0], s[1]);
	//if (std::abs(u[2]) > 1e-2)
	{
		out->x = 1.0f - (u.x + u.y) / u.z;
		out->y = u.y / u.z;
		out->z = u.x / u.z;
		if ((out->x > 0 && out->y > 0 && out->z >= 0) &&
		    (out->x + out->y + out->z < 1.0f))
			return true;
	}
	return false;
}



#define identity 1
#define nil 0


template <typename T>
struct Matrix22
{
	T x[2][2] = { {0, 0}, {0, 0} };

	Matrix22(int type)
	{
		if (type == identity)
		{
			x[0][0] = 1;
			x[1][1] = 1;
		}
	}

	Matrix22(T x00, T x01, T x10, T x11)
	{
		x[0][0] = x00;
		x[0][1] = x01;
		x[1][0] = x10;
		x[1][1] = x11;
	}

	Matrix22(vec2<T> v)
	{
		x[0][0] = v[0];
		x[1][0] = v[1];
	}

	T* operator [](int i) { return x[i]; }

	Matrix22<T> operator + (Matrix22<T> mat)
	{
		Matrix22<T> out(nil);
		for (int i = 0; i < 2; i++)
			for (int j = 0; j < 2; j++)
				out[i][j] = mat[i][j] + x[i][j];

		return out;
	}

	Matrix22<T> operator * (Matrix22<T> b)
	{
		Matrix22<T> c(nil);
		for (uint8_t i = 0; i < 2; ++i)
			for (uint8_t j = 0; j < 2; ++j)
				c[i][j] = x[i][0] * b[0][j] + x[i][1] * b[1][j];

		return c;
	}

	inline vec2<T> operator * (vec2<T> v)
	{
		vec2<T> out;
		out.x = v.x * x[0][0] + v.y * x[0][1];
		out.y = v.x * x[1][0] + v.y * x[1][1];
		return out;
	}

	vec2<T> to_vec2()
	{
		return vec2<T>(x[0][0], x[1][0]);
	}

	vec3<T> to_vec3()
	{
		return vec3<T>(x[0][0], x[1][0], 0);
	}
};

typedef Matrix22<float> Matrix22f;
