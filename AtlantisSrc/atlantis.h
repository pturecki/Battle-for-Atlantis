
//#define DEBUG_MOVE
//#define DEBUG_FLD
//#define DEBUG_MODE

#ifdef DEBUG_MODE
extern bool DEBUG_0;
#endif

#include <windows.h>
//#include <values.h>
#include <math.h>
#include <time.h>



#pragma warning( disable : 6031 ) // Return value ignored :
#pragma warning( disable : 4244 ) // 'argument': conversion from '' to '', possible loss of data
#pragma warning( disable : 4996 ) // This function or variable may be unsafe

#ifndef MAXINT
#define MAXINT 0x7fffffff
#endif

#define M_PI 3.14f

inline int random(int n)
{
	return n != 0 ? rand() % n : 0;
}

inline void randomize()
{
	srand(time(NULL));
}






class TGame;

#pragma pack( push )
#pragma pack( 1 )

typedef         struct {
                  int           SizeX;
                  int           SizeY;
                  BYTE*         Bits;
                } TImage;

typedef         struct {
                  WORD          Type;
                  DWORD         Size;    // razem z nag³ówkiem
                  WORD          Height;
                  WORD          Descent; // tak jak pod Windows
                  WORD          Leading;
                  BYTE          DefChar;
                  WORD          MaxCharWidth;
                  WORD          AveCharWidth;
                  BYTE          CharWidth[256];
                  WORD          CharPtr[256];
                  BYTE*         Chars;
                } TFont;

typedef         BYTE            TFontColors[32][32];

typedef         struct {
                  int           Player;
                  int           Army;
                  int           Island;         // numer wyspy, do której nale¿y pole
                  int           Win[4];         // prostok¹t ograniczaj¹cy pole
                  BYTE*         Colors;         // wsk. do kolorów pola
                  BYTE*         ColorsSel;      // podœwietlone pole
                  int           Neighbours[8];
                  TImage*       Img;
                  int           CC_x, CC_y;     // x, y centrum dow.
                  int           Txt_x, Txt_y;   // x, y napisów
                  unsigned char  CommandCentre   : 1;
                  unsigned char  IslandEdge      : 1;
                } TField;

#pragma pack( pop )


extern TGame*   Game;
extern BYTE     ColorsDat[17][4];
extern TField   Map[48];
extern int      NeighboursNum[48];
extern bool     DemoMode;
extern HCURSOR  Cursor1;
extern HCURSOR  Cursor2;


inline bool InWindow(int x, int y, int* w)
{
        return x >= w[0] && x <= w[2] && y >= w[1] && y <= w[3];
}

void DeleteComponent(int* arr, int comp, int size);


#include "backbuffer.h"
#include "play.h"
#include "menu.h"
#include "game.h"

//#include "atlantis.rc"

