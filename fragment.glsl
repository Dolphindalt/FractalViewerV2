#version 400

uniform double screen_ratio;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform double zoom;
uniform int iterations;

dvec2 point = dvec2(1.0, -1.0);

out vec4 colorOut;

const vec3 color_map[] = 
{
    {0.0,  0.0,  0.0},
    {0.26, 0.18, 0.06},
    {0.1,  0.03, 0.1},
    {0.04, 0.0,  0.18},
    {0.02, 0.02, 0.29},
    {0.0,  0.03, 0.37},
    {0.04, 0.13, 0.60},
    {0.07, 0.32, 0.75},
    {0.22, 0.49, 0.82},
    {0.52, 0.71, 0.9},
    {0.82, 0.92, 0.97},
    {0.94, 0.91, 0.75},
    {0.97, 0.79, 0.37},
    {1.0,  0.60, 0.0},
    {0.8,  0.5,  0.0},
    {0.6,  0.30, 0.0},
    {0.41, 0.2,  0.01}
};

void main()
{
    dvec2 z, c;
    c.x = screen_ratio * (gl_FragCoord.x / screen_size.x - 0.5);
    c.y = (gl_FragCoord.y / screen_size.y - 0.5);

    c.x /= zoom;
    c.y /= zoom;

    c.x += center.x;
    c.y += center.y;

    int i;
    double dist = 1E20;
    for(i = 0; i < iterations; i++)
    {
        double x = (z.x * z.x - z.y * z.y) + c.x;
        double y = (z.y * z.x + z.x * z.y) + c.y;
        if((x*x + y*y) > 2.0) break;
        z.x = x;
        z.y = y;

        dist = min(dist, length(z - point));
    }

    if(i == iterations)
    {
        colorOut = vec4(vec3(0.0), 1.0);
    }
    else
    {
        float d = float(dist);
        colorOut = vec4(dist, sin(d*20), cos(d)*10, 1.0);
    }
}