#pragma once

#include "MgsFunction.hpp"
#include "types.hpp"

void PsxCpp_ForceLink();

struct P_TAG
{
    unsigned addr : 24;
    unsigned len : 8; // Tagged pointer bits?
    BYTE r0, g0, b0, code;
};
MGS_ASSERT_SIZEOF(P_TAG, 8);


#define setlen( p, _len) (((P_TAG *)(p))->len  = (u_char)(_len))
#define setaddr(p, _addr) (((P_TAG *)(p))->addr = (u_long)(_addr))
#define setcode(p, _code) (((P_TAG *)(p))->code = (u_char)(_code))

#define setRGB0(p,_r0,_g0,_b0) (p)->r0 = _r0,(p)->g0 = _g0,(p)->b0 = _b0
#define setPolyFT4(p) setlen(p, 9), setcode(p, 0x2c)

struct PSX_RECT // Should be called RECT but will clash with windows.h for now
{
    s16 x1, y1, x2, y2;
};
MGS_ASSERT_SIZEOF(PSX_RECT, 8);

struct DR_ENV
{
    DWORD tag;
    DWORD code[15];
};
MGS_ASSERT_SIZEOF(DR_ENV, 0x40);

struct DRAWENV
{
    PSX_RECT clip;
    WORD offx;
    WORD offy;
    PSX_RECT textureWindow;
    BYTE texturePage;
    BYTE dtd;
    BYTE dfe;
    BYTE isbg;
    BYTE r0;
    BYTE g0;
    BYTE b0;
    DR_ENV dr_env;
};
MGS_ASSERT_SIZEOF(DRAWENV, 0x5C);

struct POLY_FT4
{
    DWORD   tag;
    BYTE    r0, g0, b0, code;
    short   x0, y0;
    BYTE    u0, v0;
    WORD    clut;
    short   x1, y1;
    BYTE    u1, v1;
    WORD    tpage;
    short   x2, y2;
    BYTE    u2, v2;
    WORD    pad1;
    short   x3, y3;
    BYTE    u3, v3;
    WORD    pad2;
};
MGS_ASSERT_SIZEOF(POLY_FT4, 40);

struct POLY_F4
{
    DWORD tag;      // Pointer to the next primitive
    BYTE r0, g0, b0; // RGB color values
    BYTE code;       // Primitive ID(reserved)
    short x0, y0;    // Vertex coordinates 
    short x1, y1;    // Vertex coordinates 
    short x2, y2;    // Vertex coordinates 
    short x3, y3;    // Vertex coordinates
};
MGS_ASSERT_SIZEOF(POLY_F4, 0x18);


struct DISPENV
{
    PSX_RECT disp;   // Display area within frame buffer.Width: 256, 320, 384, 512, or 640. Height : 240 or 480.
    PSX_RECT screen; // Output screen display area.It is calculated without regard to the value of
                     // disp, using the standard monitor screen upper left - hand point(0, 0) and
                     // lower right - hand point(256, 240).
    BYTE isinter;    // Interlace mode flag. 0: non - interlace; 1: interlace
    BYTE isrgb24;    // 24 - bit mode flag. 0: 16 - bit mode; 1: 24 - bit mode
    BYTE pad0, pad1; // Reserved by system
};
MGS_ASSERT_SIZEOF(DISPENV, 20);

struct VECTOR
{
    int field_0_x;
    int field_4_y;
    int field_8_z;
    int padding;
};
MGS_ASSERT_SIZEOF(VECTOR, 0x10);

struct SVECTOR
{
    short int field_0_x;
    short int field_2_y;
    short int field_4_z;
    short int padding;
};
MGS_ASSERT_SIZEOF(SVECTOR, 0x8);

MGS_VAR_EXTERN(DISPENV, gDispEnv_6BECF0);
MGS_VAR_EXTERN(DRAWENV, gDrawEnv_6C0E98);

signed int CC Resetgraph_AndPrintPsxStructureSizes(int mode);
int CC SetGraphDebug(int debug);
void CC SetDispMask(int mask);
int CC ClearImage(PSX_RECT* pRect, BYTE r, BYTE g, BYTE b);
int CC Psx_OpenEvent(int desc, int spec, int mode, int func); // Psx_ to avoid windows.h name clash
void CC PsxGpuDebug_44A4D0();
DRAWENV* CC Renderer_DRAWENV_Init_401888(DRAWENV* pDrawEnv, __int16 clipX1, __int16 clipY1, __int16 clipX2, __int16 clipY2);
