#ifndef __INCLUDE_BACKBUFFER__
#define __INCLUDE_BACKBUFFER__

// trochê dziwnie s¹ obliczane adresy punktów -> to dlatego, ¿e normalna bitmapa jest odbita w pionie

enum TImgFlags {
        IMG_COPY        = 1,
        IMG_TRANSP      = 2 };

enum TTxtJust {
        TXT_LEFT        = 0,
        TXT_RIGHT       = 1,
        TXT_TOP         = 2,
        TXT_CENTER      = 3,
        TXT_BOTTOM      = 4,
        TXT_LEADING     = 5,
        TXT_DESCENT     = 6 };


class TBackBuffer {
        TBackBuffer(const TBackBuffer& bb);     // ¿eby nie kopiowaæ
public:
        TBackBuffer(HWND window);
        ~TBackBuffer();

        static void Show(HDC dc);               // skopiuj do dc
        static void DrawMap();                  // narysuj w buforze mapkê

        static void SelectBitmap();
        static void UnselectBitmap();
        static HDC GetBitmapDC();

        static void SetAColor(BYTE c);
        static void SetBkColor(BYTE c);

        static BYTE GetPixel(int x, int y);
        static void PutPixel(int x, int y, BYTE c);
        static void LineX(int x, int y1, int y2);
        static void LineY(int y, int x1, int x2);
        static void Rect(int x1, int y1, int x2, int y2);
        static void RectFill(int x1, int y1, int x2, int y2, BYTE c);
        static void Circle(int x, int y, int r);
        static void CircleFill(int x, int y, int r, BYTE c);
        static void RectFillAdd(int x1, int y1, int x2, int y2, char add);

        static TImage* CreateImage(int sx, int sy);
        static TImage* CreateImageFile(char* fname, DWORD fpos);
        static void DeleteImage(TImage* img); 
        static void GetImage(int x1, int y1, int x2, int y2, TImage* img);
        static void PutImage(int x, int y, TImage* img, TImgFlags flags);
        static void SaveImage(char* fname, DWORD fpos, TImage* img);
        static BYTE GetImagePixel(TImage* img, int x, int y);

        static TFont* CreateFontFile(char* fname, DWORD fpos);
        static void DeleteFont(TFont* font);
        static int GetTextWidth(TFont* font, char* txt);
        static int GetTextHeight(TFont* font);
        static int WriteText(TFont* font, int x, int y, char* txt, TFontColors* fc);
        static void SetTextJustify(TTxtJust justx, TTxtJust justy);
        static void SetTextAdd(int ax, int ay);

        static void DrawCC(TField* fld);
        static void DrawField(TField* fld, bool sel);
        static void UpdateMap(TField* fld);
        static void DrawArmy(int fld_rect_sel);
        static void DrawWaitField(TField* fld);

        static int WriteChar(TFont* font, int x, int y, BYTE c, TFontColors* fc);

protected:
        static HGDIOBJ          Bitmap;
        static HDC              BitmapDC;
        static HGDIOBJ          OldBitmap;
        static BYTE*            Buffer;
        static BYTE*            Map;
        static BYTE             AColor;
        static BYTE             BkColor;
        static TTxtJust         TextJustifyX;
        static TTxtJust         TextJustifyY;
        static int              TextAddX;
        static int              TextAddY;

        static char             TxtBuf[10];
        static TFontColors      MapFC;
        static TFont*           MapFont; 
};


inline void TBackBuffer::SelectBitmap()
{
        OldBitmap = (HGDIOBJ)SelectObject(BitmapDC, Bitmap);
}

inline void TBackBuffer::UnselectBitmap()
{
        SelectObject(BitmapDC, OldBitmap);
}

inline HDC TBackBuffer::GetBitmapDC()
{
        return BitmapDC;
}

inline void TBackBuffer::SetAColor(BYTE c)
{
        AColor = c;
}

inline void TBackBuffer::SetBkColor(BYTE c)
{
        BkColor = c;
}

inline BYTE TBackBuffer::GetPixel(int x, int y)
{
        return Buffer[(599 - y) * 800 + x];
}

inline void TBackBuffer::PutPixel(int x, int y, BYTE c)
{
        Buffer[(599 - y) * 800 + x] = c;
}

inline BYTE TBackBuffer::GetImagePixel(TImage* img, int x, int y)
{
        return img->Bits[y * img->SizeX + x];
}

inline void TBackBuffer::SetTextJustify(TTxtJust justx, TTxtJust justy)
{
        TextJustifyX = justx;
        TextJustifyY = justy;
}

inline void TBackBuffer::SetTextAdd(int ax, int ay)
{
        TextAddX = ax;
        TextAddY = ay;
}


#endif