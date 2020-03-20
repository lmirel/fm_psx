//util.c
#include "util.h"

#include <math.h>
#include <string.h>
#include <time.h>

#include "pad.h"

#define S_PI 3.14159265f
#define D_PI 6.28318531f


int NPad (int btn)
{
	if (new_pad & btn)
		return 1;
	return 0;
}

int PPad (int btn)
{
	if (old_pad & btn)
        return 1;
	return 0;
}

#define PPAD_SKIP   5   //skip old pad report every 5 calls to reduce speed
int APad (int btn)
{
    static int pps = PPAD_SKIP;
	if (NPad(btn))
    {
        pps = PPAD_SKIP;
		return 1;
    }
	if (PPad(btn))
    {
        if (pps--)
            return 0;
        pps = PPAD_SKIP;
        return 1;
    }
	return 0;
}

void DrawRect2d (float x, float y, float z, float w, float h, u32 rgba)
{
    tiny3d_SetPolygon (TINY3D_QUADS);
    tiny3d_VertexPos (x, y, z);
    tiny3d_VertexColor (rgba);
    tiny3d_VertexPos (x + w, y, z);
    tiny3d_VertexPos (x + w, y + h, z);
    tiny3d_VertexPos (x, y + h, z);
    tiny3d_End ();
}

#if 0
unsigned long get_millis ()
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (unsigned long)((unsigned long long)tv.tv_sec * CLK_TCK +
                    (unsigned long long)tv.tv_usec * (1000000L / CLK_TCK));
}
#endif

int fps_update ()
{
    static time_t times = 0, timee;
    static int fpsv = -1;
    static int fpsh = 0;
    //
    if (times == 0)
        time (&times);
    time (&timee);
    //
    if (timee > times)
    {
        fpsh = fpsv;
        times = timee;
        fpsv = -1;
    }
    fpsv++;
    //
    return fpsh;
}
