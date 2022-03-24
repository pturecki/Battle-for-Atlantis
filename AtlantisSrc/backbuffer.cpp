#include "atlantis.h"


HGDIOBJ         TBackBuffer::Bitmap             = NULL;
HDC             TBackBuffer::BitmapDC           = NULL;
HGDIOBJ         TBackBuffer::OldBitmap          = NULL;
BYTE*           TBackBuffer::Buffer             = NULL;
BYTE*           TBackBuffer::Map                = NULL;
BYTE            TBackBuffer::AColor             = 0;
BYTE            TBackBuffer::BkColor            = 0;
TTxtJust        TBackBuffer::TextJustifyX       = TXT_LEFT;
TTxtJust        TBackBuffer::TextJustifyY       = TXT_TOP;
int             TBackBuffer::TextAddX           = 2;
int             TBackBuffer::TextAddY           = 0;

char            TBackBuffer::TxtBuf[10]         = { 0 };
TFontColors     TBackBuffer::MapFC              = { { 0 } };
TFont*          TBackBuffer::MapFont            = NULL;


BYTE TxtColors[16] = { 210,
                       186,
                        20,
                        29,
                        22,
                        20,
                       164,
                       228,
                        48,
                        87,
                       204,
                       161,
                       165,
                       192,
                       202,
                       132 };


TBackBuffer::TBackBuffer(HWND window)
{
        Bitmap = static_cast<HBITMAP>(LoadImage(NULL, "bg.dat",
                IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));

        BITMAP bm;
        GetObject(Bitmap, sizeof(BITMAP), &bm);
        Buffer = static_cast<BYTE*>(bm.bmBits);

        HDC windc = GetDC(window);
        BitmapDC = CreateCompatibleDC(windc);
        ReleaseDC(window, windc);

        Map = new BYTE[800*600];

        DWORD* dest = reinterpret_cast<DWORD*>(Map);
        DWORD* src = reinterpret_cast<DWORD*>(Buffer);
        for (int i = 0; i < 800*600/4; i++)
          *dest++ = *src++;

        AColor = 15;
        BkColor = 0;

        MapFont = CreateFontFile("svga_f30.fnt", 0);     
        FillMemory(&MapFC, 1024, 0);
}

TBackBuffer::~TBackBuffer()
{
        DeleteFont(MapFont);

        DeleteObject(Bitmap);

        DeleteDC(BitmapDC);

        delete[] Map;
}

void TBackBuffer::Show(HDC dc)
{
        SelectBitmap();

        PAINTSTRUCT ps;
        BeginPaint(Game->GetWindow(), &ps);
        BitBlt(dc, 0, 0, 800, 600, BitmapDC, 0, 0, SRCCOPY);
        EndPaint(Game->GetWindow(), &ps);

        UnselectBitmap();
}

void TBackBuffer::DrawMap()
{
        CopyMemory(Buffer, Map, 800*600);

        if (Game->Started())
        {
          for (int i = 0; i < 48; i++)
            DrawCC(&::Map[i]);
          DrawArmy(Game->Field1);
        }  
}

void TBackBuffer::LineX(int x, int y1, int y2)
{
        BYTE* ptr = Buffer + (599 - y2) * 800 + x;

        for (int y = y1; y <= y2; y++)
          *ptr = AColor, ptr += 800;
}

void TBackBuffer::LineY(int y, int x1, int x2)
{
        BYTE* ptr = Buffer + (599 - y) * 800 + x1;

        for (int x = x1; x <= x2; x++)
          *ptr++ = AColor;
}

void TBackBuffer::Rect(int x1, int y1, int x2, int y2)
{
        LineX(x1, y1, y2);
        LineX(x2, y1, y2);
        LineY(y1, x1, x2);
        LineY(y2, x1, x2);
}

void TBackBuffer::RectFill(int x1, int y1, int x2, int y2, BYTE c)
{
        BYTE SavAColor = AColor;
        SetAColor(c);

        for (int y = y1; y <= y2; y++)
          LineY(y, x1, x2);

        SetAColor(SavAColor);
}


void TBackBuffer::Circle(int x, int y, int r)
{
        int d, xx, yy;
        d = 3 - (r << 1);
        xx = 0;
        yy = r;
        while (yy >= xx)
        {
          PutPixel(x + xx, y + yy, AColor);
          PutPixel(x - xx, y + yy, AColor);
          PutPixel(x + xx, y - yy, AColor);
          PutPixel(x - xx, y - yy, AColor);
          PutPixel(x + yy, y + xx, AColor);
          PutPixel(x - yy, y + xx, AColor);
          PutPixel(x + yy, y - xx, AColor);
          PutPixel(x - yy, y - xx, AColor);
          xx++;
          if (d < 0)
            d += (xx << 2) + 6;
          else
            d += ((xx - yy) << 2) + 10, yy--;
        }
}

void TBackBuffer::CircleFill(int x, int y, int r, BYTE c)
{
        BYTE SavAColor = AColor;
        SetAColor(c);

        int d, xx, yy;
        d = 3 - (r << 1);
        xx = 0;
        yy = r;
        while (yy >= xx)
        {
          LineY(y + yy, x - xx, x + xx);
          LineY(y - yy, x - xx, x + xx);
          LineY(y + xx, x - yy, x + yy);
          LineY(y - xx, x - yy, x + yy);
          xx++;
          if (d < 0)
            d += (xx << 2) + 6;
          else
            d += ((xx - yy) << 2) + 10, yy--;
        }

        SetAColor(SavAColor);
}

void TBackBuffer::RectFillAdd(int x1, int y1, int x2, int y2, char add)
{
        BYTE* ptr = Buffer + (599 - y2) * 800 + x1;
        int addline = 800 - (x2 - x1 + 1);

        for (int y = y1; y <= y2; y++)
        {
          for (int x = x1; x <= x2; x++)
            *ptr++ += add;
          ptr += addline;  
            
        }    
}

TImage* TBackBuffer::CreateImage(int sx, int sy)
{
        TImage* img = new TImage;

        img->SizeX = sx;
        img->SizeY = sy;
        img->Bits  = new BYTE[sx * sy];

        return img;
}

TImage* TBackBuffer::CreateImageFile(char* fname, DWORD fpos)
{
        HANDLE f = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return 0;

        DWORD bread;
        int sx, sy;

        SetFilePointer(f, fpos, NULL, FILE_BEGIN);
        ReadFile(f, &sx, sizeof sx, &bread, NULL);
        ReadFile(f, &sy, sizeof sy, &bread, NULL);

        TImage* img = CreateImage(sx, sy);

        ReadFile(f, img->Bits, sx * sy, &bread, NULL);

        CloseHandle(f);

        return img;
}

void TBackBuffer::DeleteImage(TImage* img)
{
        delete[] img->Bits;
        delete img;
}

void TBackBuffer::GetImage(int x1, int y1, int x2, int y2, TImage* img)
{
        int sx      = x2 - x1 + 1;
        int sy      = y2 - y1 + 1;
        int lineadd = -(800 + sx);
        BYTE* src   = Buffer + (599 - y1) * 800 + x1;
        BYTE* dest  = img->Bits;

        for (int y = 0; y < sy; y++)
        {
          for (int x = 0; x < sx; x++)
            *dest++ = *src++;
          src += lineadd;
        }
}

void TBackBuffer::PutImage(int x, int y, TImage* img, TImgFlags flags)
{
        int sx      = img->SizeX;
        int sy      = img->SizeY;
        int lineadd = -(800 + sx);
        BYTE* dest  = Buffer + (599 - y /*- img->SizeY*/) * 800 + x;
        BYTE* src  = img->Bits;

        BYTE c;
        int xx, yy;

        switch (flags) {

          case IMG_COPY :
            for (yy = 0; yy < sy; yy++)
            {
              for (xx = 0; xx < sx; xx++)
                *dest++ = *src++;
              dest += lineadd;
            }
            break;

          case IMG_TRANSP :
            for (yy = 0; yy < sy; yy++)
            {
              for (xx = 0; xx < sx; xx++)
              if ((c = *src++) == BkColor)
                dest++;
              else
                *dest++ = c;
              dest += lineadd;
            }
            break;
        }
}

void TBackBuffer::SaveImage(char* fname, DWORD fpos, TImage* img)
{
        HANDLE f = CreateFile(fname, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;

        DWORD bwrite;

        SetFilePointer(f, fpos, NULL, FILE_BEGIN);
        WriteFile(f, &img->SizeX, sizeof img->SizeX, &bwrite, NULL);
        WriteFile(f, &img->SizeY, sizeof img->SizeY, &bwrite, NULL);
        WriteFile(f, img->Bits, img->SizeX * img->SizeY, &bwrite, NULL);

        CloseHandle(f);
}

TFont* TBackBuffer::CreateFontFile(char* fname, DWORD fpos)
{
        HANDLE f = CreateFile(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return 0;

        DWORD bread;
        WORD type;
        DWORD size;

        SetFilePointer(f, fpos, NULL, FILE_BEGIN);
        ReadFile(f, &type, sizeof type, &bread, NULL);
        ReadFile(f, &size, sizeof size, &bread, NULL);

        TFont* font = new TFont;
        font->Chars = new BYTE[size - sizeof(TFont) + sizeof font->Chars];

        SetFilePointer(f, fpos, NULL, FILE_BEGIN);
        ReadFile(f, font, sizeof(TFont) - sizeof font->Chars, &bread, NULL);
        ReadFile(f, font->Chars, size - sizeof(TFont) + sizeof font->Chars, &bread, NULL);

        CloseHandle(f);

        return font;
}

void TBackBuffer::DeleteFont(TFont* font)
{
        delete[] font->Chars;
        delete font;
}

int TBackBuffer::GetTextWidth(TFont* font, char* txt)
{
        if (*txt == 0)
          return 0;

        int width = -TextAddX;

        while (*txt)
          width += font->CharWidth[*reinterpret_cast<BYTE*>(txt)] + TextAddX, txt++;

        return width;
}

int TBackBuffer::GetTextHeight(TFont* font)
{
        return font->Height;
}

int TBackBuffer::WriteText(TFont* font, int x, int y, char* txt, TFontColors* fc)
{
        int curx, cury;

        switch (TextJustifyX) {
          case TXT_LEFT :       curx = x; break;
          case TXT_CENTER :     curx = x - (GetTextWidth(font, txt) >> 1); break;
          case TXT_RIGHT :      curx = x - GetTextWidth(font, txt); break;
        }

        switch (TextJustifyY) {
          case TXT_TOP :        cury = y; break;
          case TXT_CENTER :     cury = y - (GetTextHeight(font) >> 1); break;
          case TXT_BOTTOM :     cury = y - GetTextHeight(font); break;
          case TXT_LEADING :    cury = y - font->Leading; break;
          case TXT_DESCENT :    cury = y - GetTextHeight(font) + font->Descent; break;
        }

        while (*txt)
          curx = WriteChar(font, curx, cury, *txt, fc), cury += TextAddY, txt++;

        return curx;
}

int TBackBuffer::WriteChar(TFont* font, int x, int y, BYTE c, TFontColors* fc)
{
        int width     = font->CharWidth[c];
        int height    = font->Height;
        int bline     = (width - 1) / 8 + 1;           // szerokoœæ znaku w bajtach
        DWORD* ptr    = reinterpret_cast<DWORD*>(font->Chars + font->CharPtr[c] - sizeof(TFont) + sizeof font->Chars);
        BYTE* colors  = reinterpret_cast<BYTE*>(fc);
        int addcolors = 32 - width;
        int adddest   = -(800 + width);
        BYTE* dest    = Buffer + (599 - y) * 800 + x;

        DWORD bit;
        DWORD mask;

        for (int yy = 0; yy < height; yy++)
        {
          mask = *ptr;
          bit = 1;
          for (int xx = 0; xx < width; xx++)
          {
            if (mask & bit)
              *dest = *colors;
            dest++;
            colors++;
            bit <<= 1;
          }
          dest += adddest;
          colors += addcolors;
          ptr = (DWORD*)(reinterpret_cast<BYTE*>(ptr) + bline);
        }

        return x + width + TextAddX;
}

void TBackBuffer::DrawCC(TField* fld)
{
        if (!fld->CommandCentre)
          return;

        CircleFill(fld->CC_x, fld->CC_y, 10, 0);
        CircleFill(fld->CC_x, fld->CC_y, 8, 215);
}

void TBackBuffer::DrawField(TField* fld, bool sel)
{
        int sx       = fld->Img->SizeX;
        int sy       = fld->Img->SizeY;
        int lineadd  = -(800 + sx);
        BYTE* dest   = Buffer + (599 - fld->Win[1]) * 800 + fld->Win[0];
        BYTE* src    = fld->Img->Bits;
        BYTE* colors = sel ? fld->ColorsSel : fld->Colors;

        BYTE c;

        for (int yy = 0; yy < sy; yy++)
        {
          for (int xx = 0; xx < sx; xx++)
          if ((c = *src++) == BkColor)
            dest++;
          else
            *dest++ = colors[c];
          dest += lineadd;
        }

        DrawCC(fld);

        int w = 18, h = GetTextHeight(MapFont);

        SetTextJustify(TXT_CENTER, TXT_TOP);

        RectFill(fld->Txt_x - w + 1, fld->Txt_y, fld->Txt_x + w - 1, fld->Txt_y + h - 1,
                 TxtColors[Game->Colors[fld->Player]]);
        SetAColor(0);
        Rect(fld->Txt_x - w, fld->Txt_y - 1, fld->Txt_x + w, fld->Txt_y + h);

        if (fld->Army > 0)
        {
          itoa(max(fld->Army, 1), TxtBuf, 10);
          WriteText(MapFont, fld->Txt_x, fld->Txt_y, TxtBuf, &MapFC);
        }  
}

void TBackBuffer::UpdateMap(TField* fld)
{
        fld->Colors = ColorsDat[Game->Colors[fld->Player]];

        int sx       = fld->Img->SizeX;
        int sy       = fld->Img->SizeY;
        int lineadd  = -(800 + sx);
        BYTE* dest   = Map + (599 - fld->Win[1]) * 800 + fld->Win[0];
        BYTE* src    = fld->Img->Bits;
        BYTE* colors = fld->Colors;

        BYTE c;

        for (int yy = 0; yy < sy; yy++)
        {
          for (int xx = 0; xx < sx; xx++)
          if ((c = *src++) == BkColor)
            dest++;
          else
            *dest++ = colors[c];
          dest += lineadd;
        }
}

void TBackBuffer::DrawArmy(int fld_rect_sel)
{
        int w = 18, h = GetTextHeight(MapFont);

        SetTextJustify(TXT_CENTER, TXT_TOP);
        for (int i = 0; i < 48; i++)
        {
          RectFill(::Map[i].Txt_x - w + 1, ::Map[i].Txt_y,
                   ::Map[i].Txt_x + w - 1, ::Map[i].Txt_y + h - 1,
                   TxtColors[Game->Colors[::Map[i].Player]]);
          if (i == fld_rect_sel)
          {
            int t = Game->GetTime() % 200;
            SetAColor(static_cast<BYTE>(t <= 100 ? (225 + t / 10) : (245 - t / 10)));
          }
          else
            SetAColor(0);
          Rect(::Map[i].Txt_x - w, ::Map[i].Txt_y - 1, ::Map[i].Txt_x + w, ::Map[i].Txt_y + h);

#ifdef DEBUG_FLD
          char bb[100];

          itoa(::Map[i].Army, TxtBuf, 10);
          itoa(i, bb, 10);
          strcat(bb, "-");
          strcat(bb, TxtBuf);
          WriteText(MapFont, ::Map[i].Txt_x, ::Map[i].Txt_y, bb, &MapFC);
#else
          itoa(max(::Map[i].Army, 1), TxtBuf, 10);
          WriteText(MapFont, ::Map[i].Txt_x, ::Map[i].Txt_y, TxtBuf, &MapFC);
#endif
        }
}

void TBackBuffer::DrawWaitField(TField* fld)
{
        int sx       = fld->Img->SizeX;
        int sy       = fld->Img->SizeY;
        int lineadd  = -(800 + sx);
        BYTE* dest   = Buffer + (599 - fld->Win[1]) * 800 + fld->Win[0];
        BYTE* src    = fld->Img->Bits;

        for (int yy = 0; yy < sy; yy++)
        {
          for (int xx = 0; xx < sx; xx++)
          if (*src++ == BkColor)
            dest++;
          else
            *dest++ = 215;
          dest += lineadd;
        }

        DrawCC(fld);

        int w = 18, h = GetTextHeight(MapFont);

        SetTextJustify(TXT_CENTER, TXT_TOP);

        RectFill(fld->Txt_x - w + 1, fld->Txt_y, fld->Txt_x + w - 1, fld->Txt_y + h - 1,
                 TxtColors[Game->Colors[fld->Player]]);
        SetAColor(0);
        Rect(fld->Txt_x - w, fld->Txt_y - 1, fld->Txt_x + w, fld->Txt_y + h);

#ifdef DEBUG_FLD
        char bb[100];

        itoa(fld->Army, TxtBuf, 10);
        for (int i = 0; i < 48; i++)
        if (::Map[i].Img->Bits == fld->Img->Bits)
          itoa(i, bb, 10);
        strcat(bb, "-");
        strcat(bb, TxtBuf);
        WriteText(MapFont, fld->Txt_x, fld->Txt_y, bb, &MapFC);
#else
        itoa(max(fld->Army, 1), TxtBuf, 10);
        WriteText(MapFont, fld->Txt_x, fld->Txt_y, TxtBuf, &MapFC);
#endif
}

