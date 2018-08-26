#version 400

uniform int selection;
uniform double screen_ratio;
uniform dvec2 screen_size;
uniform dvec2 center;
uniform dvec2 orbit_trap;
uniform double zoom;
uniform int iterations;

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

vec4 point_orbit_trap(dvec2 c)
{
    dvec2 z;
    int i;
    double dist = 1E20;
    for(i = 0; i < iterations; i++)
    {
        double x = (z.x * z.x - z.y * z.y) + c.x;
        double y = (z.y * z.x + z.x * z.y) + c.y;

        if((x*x + y*y) > 4.0) break;
        z.x = x;
        z.y = y;

        dist = min(dist, length(z - orbit_trap));
    }
    float d = float(dist);
    if(iterations == i) return vec4(vec3(0.0), 1.0);
    return vec4(dist, sin(d*20), cos(d)*10, 1.0);
}

vec4 pickover_stalks(dvec2 c)
{
    dvec2 z;
    int i;
    double dist = 1E20;
    for(i = 0; i < iterations; i++)
    {
        double x = (z.x * z.x - z.y * z.y) + c.x;
        double y = (z.y * z.x + z.x * z.y) + c.y;

        if((x*x + y*y) > 4.0) break;
        z.x = x;
        z.y = y;

        double dx = abs(z.x - orbit_trap.x);
        double dy = abs(z.y - orbit_trap.y);
        double smallest = min(dx, dy);
        dist = min(dist, smallest);
    }
    float d = float(dist);
    return vec4(dist, sin(d*20), sin(d)*10, 1.0);
}

vec4 iteration_map(dvec2 c)
{
    dvec2 z;
    int i;
    for(i = 0; i < iterations; i++)
    {
        double x = (z.x * z.x - z.y * z.y) + c.x;
        double y = (z.y * z.x + z.x * z.y) + c.y;
        if((x*x + y*y) > 4.0) break;
        z.x = x;
        z.y = y;
    }
    double t = double(i) / double(iterations);
    uint row_i = (i * 100 / iterations % 17);
    return vec4((i == iterations ? vec3(0.0) : color_map[row_i]), 1.0);
}

vec4 normalized_colors(dvec2 c)
{
    dvec2 z;
    int i;
    for(i = 0; i < iterations; i++)
    {
        double x = (z.x * z.x - z.y * z.y) + c.x;
        double y = (z.y * z.x + z.x * z.y) + c.y;
        if((x*x + y*y) > 4.0) break;
        z.x = x;
        z.y = y;
    }
    
    if(iterations == i)
    {
        return vec4(vec3(0.0), 1.0);
    }

    float mu = (i + 1 - log(log(float(abs(z))) / float(log(2)))) / float(iterations);
    return vec4(mu+1.0, mu+0.5, mu, 1.0);
}

void main()
{
    dvec2 c;
    c.x = screen_ratio * (gl_FragCoord.x / screen_size.x - 0.5);
    c.y = screen_ratio * (gl_FragCoord.y / screen_size.y - 0.5);

    c.x /= zoom;
    c.y /= zoom;

    c.x += center.x;
    c.y += center.y;

    if(selection == 0)
    {
        colorOut = pickover_stalks(c);
    }
    else if(selection == 1)
    {
        colorOut = iteration_map(c);
    }
    else if(selection == 2)
    {
        colorOut = point_orbit_trap(c);
    }
    else if(selection == 3)
    {
        colorOut = normalized_colors(c);
    }
}