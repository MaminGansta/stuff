


bool SAT_triangle(std::vector<vec2f>& const verts_origin1, vec3i faces1,
                  std::vector<vec2f>& const verts_origin2, vec3i faces2)
{

    for (int shape = 0; shape < 2; shape++)
    {
        std::vector<vec2f>* verts1 = &verts_origin1;
        std::vector<vec2f>* verts2 = &verts_origin2;

        if (shape == 1)
        {
            verts1 = &verts_origin2;
            verts2 = &verts_origin1;

            vec3i temp = faces1;
            faces1 = faces2;
            faces2 = temp;
        }

        for (int k = 0; k < 3; k++)
        {
            vec2f projection(-(verts1->at(faces1[k]).y - verts1->at((faces1[(k + 1) % 3])).y), verts1->at(faces1[k]).x - verts1->at(faces1[(k + 1) % 3]).x);
            double min1 = INFINITY, max1 = -INFINITY;
            int counter = 0;

            for (int a = 0; a < 3; a++)
            {
                double dotprd = dproduct(verts1->at(faces1[a]), projection);
                min1 = MIN(min1, dotprd);
                max1 = MAX(max1, dotprd);
            }

            double min2 = INFINITY, max2 = -INFINITY;
      
            for (int a = 0; a < 3; a++)
            {
                double dotprd = dproduct(verts2->at(faces2[a]), projection);
                min2 = MIN(min2, dotprd);
                max2 = MAX(max2, dotprd);
            }
      
            if (!(max2 >= min1&& max1 >= min2))
                return false;
        }
    }

    return true;
}


bool SAT_shape(std::vector<vec2f>& const shape1, std::vector<vec3i>& const faces1,
               std::vector<vec2f>& const shape2, std::vector<vec3i>& const faces2)
{
    for (int i = 0; i < faces1.size(); i++)
        for (int j = 0; j < faces2.size(); j++)
            if (SAT_triangle(shape1, faces1[i], shape2, faces2[j])) // check for each triangle
            {
                overlapped.push_back(std::pair<int, int>(cur_shape, j));
                //return true;
            }

    return false;
}









int orientation(vec2f p, vec2f q, vec2f r)
{
    float val = (q.y - p.y) * (r.x - q.x) -
        (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear 
    return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

std::vector<vec2f> convexHull(std::vector<vec2f>& points)
{
    // There must be at least 3 points 
    if (points.size() < 3) return points;

    // Initialize Result 
    std::vector<vec2f> hull;

    // Find the leftmost point 
    int l = 0;
    for (int i = 1; i < points.size(); i++)
        if (points[i].x < points[l].x)
            l = i;

    int p = l, q;
    do
    {
        hull.push_back(points[p]);

        q = (p + 1) % points.size();
        for (int i = 0; i < points.size(); i++)
        {
            if (orientation(points[p], points[i], points[q]) == 2)
                q = i;
        }

        p = q;

    } while (p != l);  // While we don't come to first point 

    return hull;
}
