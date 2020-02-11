//util.c
#include "util.h"

#include <math.h>
#include <string.h>

#define S_PI 3.14159265f
#define D_PI 6.28318531f

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
