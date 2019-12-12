
// some geometry

template <typename T>
struct Matrix22
{
	T x[2][2] = { {0, 0}, {0, 0} };

	Matrix22(int type)
	{
		if (type = identity)
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

	T* operator [](int i) { return x[i]; }

	Matrix22<T> operator * (Matrix22<T> b)
	{
		Matrix22<T> c;
		for (uint8_t i = 0; i < 2; ++i)
			for (uint8_t j = 0; j < 2; ++j)
				c[i][j] = x[i][0] * b[0][j] + x[i][1] * b[1][j];

		return c;
	}

	inline Vec2<T> operator * (Vec2<T> v)
	{
		Vec2<T> out;
		out.x = v.x * x[0][0] + v.y * x[0][1];
		out.y = v.x * x[1][0] + v.y * x[1][1];
		return out;
	}
};

typedef Matrix22<float> Matrix22f;


inline Matrix22f rotationMatrix(float angle)
{
	return Matrix22f(cosf(angle), -sinf(angle), sinf(angle), cosf(angle));
}

// game structs----------------------------------------------------------------------

struct Ship
{
	std::vector<Vec2f> pts{Vec2f(0, 0.02), Vec2f(-0.01, -0.01), Vec2f(0.01, -0.01)};
	Vec2f  center;
	float dir{PI / 2};
	float speed_x{0};
	float speed_y{0};
	float speed_dif{0.001};

	void move(float x, float y)
	{
		for (int i = 0; i < 3; i++)
			pts[i].x += x, pts[i].y += y;

		center.x += x, center.y += y;
	}
	
};

struct Asteroid
{
	std::vector<Vec2f> pts;
	Vec2f center;
	float speed_x{0};
	float speed_y{0};
};

// rotate shap
inline void rotate_shape(std::vector<Vec2f>& shape, Vec2f center, Matrix22f r)
{
	for (Vec2f& v : shape)
		v = (r * (v - center)) + center;
}