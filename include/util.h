//util.h
#include <tiny3d.h>

// change to 2D context ( virtual size of the screen is 848.0 x 512.0)
//for 8x8 font, we can split the screen into 106x64 chars - 2 panes w53chars
//we leave 8 lines for progress/status/info below the panes
#define PANEL_W 53
#define PANEL_H 60
#define STATUS_H 4

#define CBSIZE  256     //char buffer size

int fps_update ();
void DrawRect2d (float x, float y, float z, float w, float h, u32 rgba);
int NPad (int btn);
int PPad (int btn);
int APad (int btn);
