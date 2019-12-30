

struct Line
{
	vec2f pos;
	vec2f dir;
	float lenght{ 0 };

	inline Line() = default;
	inline Line(vec2f pos, vec2f dir, float lenght = 1) : pos(pos), dir(dir), lenght(lenght) {}

};

// shapes
// bound all dots with lines
inline void make_shape(vec2f* p, int size, std::vector<Line>& lines)
{
	for (int i = 0; i < size - 1; i++)
	{
		vec2f dir(p[i + 1].x - p[i].x, p[i + 1].y - p[i].y);
		lines.push_back(Line(p[i], dir, 1));
	}

	// bound end wiht start dot
	vec2f dir(p[0].x - p[size - 1].x, p[0].y - p[size - 1].y);
	lines.push_back(Line(p[size - 1], dir, 1));
}



int mod(int num, int m)
{
	return num >= 0 ? num % m : m + (num % m);
}

// lie under or uper (together) then y
bool lie_together(Line f, Line s, float y)
{
	float max, min;
	max = f.pos.y;
	min = f.pos.y;

	if (f.pos.y + f.dir.y < min)
		min = f.pos.y + f.dir.y;

	if (f.pos.y + f.dir.y > max)
		max = f.pos.y + f.dir.y;


	if (s.pos.y + s.dir.y < min)
		min = s.pos.y + s.dir.y;

	if (s.pos.y + s.dir.y > max)
		max = s.pos.y + s.dir.y;

	if (max > y && min < y)
		return false;
	else
		return true;
}

// check if angle convex
bool ifconvex(std::vector<Line>& shape, Line ray)
{
	// pos and neg intersictions
	int positive = 0, negative = 0;

	// nessecery to prevent repeats if ray casts into the midle of angle(two vectors intersection in this case)
	std::map<float, Line> duble;

	for (Line side : shape)
	{
		// not the angle from ray going
		if (side.pos == ray.pos || side.pos + side.dir == ray.pos) continue;
		// non parallel
		if (side.dir.normalize() == ray.dir.normalize() || side.dir.normalize() * (-1) == ray.dir.normalize()) continue;

		float T2 = (ray.dir.x * (side.pos.y - ray.pos.y) + ray.dir.y * (ray.pos.x - side.pos.x)) /
			(side.dir.x * ray.dir.y - side.dir.y * ray.dir.x);

		if (T2 >= 0 && T2 <= 1)
		{
			float T1 = (side.pos.x + side.dir.x * T2 - ray.pos.x) / ray.dir.x;
			if (T1 < 1 && T1 >= 0)
			{
				if (duble.count(T1) == 0)
				{
					positive++;
					duble.insert(std::make_pair(T1, side));
				}
				else
				{
					if (lie_together(duble[T1], side, ray.pos.y))
						positive++;
				}
			}
			else if (T1 >= -1 && T1 <= 0)
			{
				if (duble.count(T1) == 0)
				{
					negative++;
					duble.insert(std::make_pair(T1, side));
				}
				else
				{
					if (lie_together(duble[T1], side, ray.pos.y))
						negative++;
				}
			}
		}
	}

	positive = positive % 2;
	negative = negative % 2;

	// 100% convex
	if (positive == 0 && negative == 0)
		return true;
	else if (positive == 1 && negative == 0)
		return true;


	return false;
}


// vec3i means set of 3 positive integers(faces of tringle), the nums of verts in shape array
std::vector<vec3i> triangulate(std::vector<vec2f>& const shape)
{
	int i = 0;
	bool dot_flag = false;
	int actives = shape.size();
	std::vector<vec3i> faces;
	std::vector<int> verts;
	for (int i = 0; i < actives; i++)
		verts.push_back(i);

	std::vector<Line> shape_lines;
	make_shape(shape.data(), shape.size(), shape_lines);

	
	while (actives > 3)
	{
		float dot = dproduct(shape[verts[mod(i + 1, actives)]] - shape[verts[mod(i, actives)]], shape[verts[mod(i - 1, actives)]] - shape[verts[mod(i, actives)]]);

		int dir = sgn((shape[verts[mod(i + 1, actives)]] - shape[verts[mod(i, actives)]] + shape[verts[mod(i - 1, actives)]] - shape[verts[mod(i, actives)]]).x);
		bool conv =  ifconvex(shape_lines, Line(shape[verts[i]], vec2f(dir, 0)));

		if ((dot_flag || dot >= 0) && conv)
		{
			for (int k = 0; k < shape.size(); k++)
			{
				vec3f out;
				if (fbarycentric(shape[verts[mod(i, actives)]], shape[verts[mod(i - 1, actives)]], shape[verts[mod(i + 1, actives)]], shape[k], &out)) // return true if vertex of shape lie in a triangle
				{
					i = mod(i + 1, actives); // next vertext
					break;
				}

				if (k == shape.size() - 1)
				{
					vec3i push(verts[i], verts[mod(i - 1, actives)], verts[mod(i + 1, actives)]);
					faces.push_back(push); // write this triangle and delete the ear
					verts.erase(verts.begin() + i);
					--actives;
					dot_flag = false;

					// update shape_lines
					std::vector<vec2f> temp;
					for (int m = 0; m < verts.size(); m++)
						temp.push_back(shape[verts[m]]);
					shape_lines.clear();
					make_shape(temp.data(), temp.size(), shape_lines);
				}
			}
		}
		else
		{
			// we have not angles less then 90*
			if (i == actives - 1)
				dot_flag = true;
			i = mod(i + 1, actives);
		}
	}

	faces.push_back(vec3i(verts[0], verts[1], verts[2]));
	return std::move(faces);
}
