
#include "atlantis.h"


//TBackBuffer*    TMenu::BackBuffer       = NULL;
TFont*          TMenu::Font             = NULL;
TFontColors     TMenu::FC               = { { 0 } };
TFontColors     TMenu::FCSel            = { { 0 } };
TFontColors     TMenu::FCGrey           = { { 0 } };
TFontColors     TMenu::FCText           = { { 0 } };
TMenuID         TMenu::MenuID           = Menu_Main;
char            TMenu::TxtBuf[20]       = { 0 };
DWORD           TMenu::Time             = 0;
int             TMenu::Anim             = 0;
int             TMenu::PCol[4]          = { 0 };
int             TMenu::DLevel           = 0;
bool            TMenu::FileError        = false;
THelpID         TMenu::HelpID           = Help_About;
TFont*          TMenu::HFont0           = NULL;

float           TMenu::_3d_d            = 100;
float           TMenu::_3d_v            = 0.01f;
float           TMenu::_3d_p3[MAXP][3]  = { { 0 } };
float           TMenu::_3d_p2[MAXP][3]  = { { 0 } };
int             TMenu::_3d_t            = 0;


int             MainWin[7][4] =         { { 600, 285, 790, 320 },
                                          { 600, 330, 790, 365 },
                                          { 600, 375, 790, 410 },
                                          { 600, 420, 790, 455 },
                                          { 600, 465, 790, 500 },
                                          { 600, 510, 790, 545 },
                                          { 600, 555, 790, 590 } };

int             NewWin[13][4] =         { { 600, 285, 660, 320 },
                                          { 730, 285, 790, 320 },
                                          { 600, 365, 660, 400 },
                                          { 730, 365, 790, 400 },
                                          { 600, 410, 660, 445 },
                                          { 730, 410, 790, 445 },
                                          { 600, 455, 660, 490 },
                                          { 730, 455, 790, 490 },
                                          { 600, 500, 660, 535 },
                                          { 730, 500, 790, 535 },
                                          { 600, 555, 660, 590 },
                                          { 670, 555, 790, 590 },
                                          { 450, 555, 590, 590 } };

int             SaveWin[2][4] =         { { 600, 450, 790, 485 },
                                          { 600, 505, 790, 540 } };

int             FileErrWin[4] =           { 600, 450, 790, 485 };

int             ColorsWin[10][4] =      { { 600, 365, 660, 400 },
                                          { 730, 365, 790, 400 },
                                          { 600, 410, 660, 445 },
                                          { 730, 410, 790, 445 },
                                          { 600, 455, 660, 490 },
                                          { 730, 455, 790, 490 },
                                          { 600, 500, 660, 535 },
                                          { 730, 500, 790, 535 },
                                          { 600, 555, 660, 590 },
                                          { 670, 555, 790, 590 } };

// pomoc: o programie, interfejs, zasady gry, ok
int             HelpWin[4][4] =         { {  10,  10, 210,  45 },
                                          { 220,  10, 420,  45 },
                                          { 430,  10, 630,  45 },
                                          { 640,  10, 790,  45 } };


TMenu::TMenu()
{
        Font = Game->GetBackBuffer()->CreateFontFile("svga_f22.fnt", 0);
        HFont0 = Game->GetBackBuffer()->CreateFontFile("svga_f7.fnt", 0); //8,18,

        for (int j = 0; j < 32; j++)
          for (int i = 0; i < 32; i++)
            FC[j][i] = static_cast<BYTE>(225 + j / 5);

        for (int j = 0; j < 32; j++)
          for (int i = 0; i < 32; i++)
            FCSel[j][i] = 215;

        for (int j = 0; j < 32; j++)
          for (int i = 0; i < 32; i++)
            FCGrey[j][i] = static_cast<BYTE>(226 + ((i * j) & 3));

        for (int j = 0; j < 32; j++)
          for (int i = 0; i < 32; i++)
            FCText[j][i] = 205;
}

TMenu::~TMenu()
{
        Game->GetBackBuffer()->DeleteFont(Font);
        Game->GetBackBuffer()->DeleteFont(HFont0);
}

void TMenu::MenuProcessMessage(UINT msg_id, WPARAM wparam, LPARAM lparam)
{
        if (!Game->MenuActive)
          return;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_MOUSEMOVE && msg_id != WM_KEYDOWN &&
            msg_id != WM_LBUTTONDBLCLK)
          return;

        switch (MenuID) {
          case Menu_Main        : MenuProcessMessage_Main(msg_id, wparam, lparam); break;
          case Menu_New         : MenuProcessMessage_New(msg_id, wparam, lparam); break;
          case Menu_Save        : MenuProcessMessage_Save(msg_id, wparam, lparam); break;
          case Menu_Load        : MenuProcessMessage_Load(msg_id, wparam, lparam); break;
          case Menu_Colors      : MenuProcessMessage_Colors(msg_id, wparam, lparam); break;
          case Menu_Help        : MenuProcessMessage_Help(msg_id, wparam, lparam); break;
        }
}

void TMenu::Tick(DWORD time)
{
        Time += time;

        Anim = (Time % 200) <= 100 ? (225 + (Time % 200) / 10) : (245 - (Time % 200) / 10);

        _3d_t = time;

        if (MenuID == Menu_Help)
        {
          _3d_Move();
          _3d_To2D();
          if ((_3d_v += 0.01f) > 3)   
            _3d_v = 3;
        }
}

void TMenu::DrawMenu()
{
        if (!Game->MenuActive)
          return;

        switch (MenuID) {
          case Menu_Main        : DrawMenu_Main();   break;
          case Menu_New         : DrawMenu_New();    break;
          case Menu_Save        : DrawMenu_Save();   break;
          case Menu_Load        : DrawMenu_Load();   break;
          case Menu_Colors      : DrawMenu_Colors(); break;
          case Menu_Help        : DrawMenu_Help();   break;
        }
}

void TMenu::DrawButton(int x1, int y1, int x2, int y2, char* txt)
{
        Game->GetBackBuffer()->RectFill(x1 + 1, y1 + 1, x2 - 1, y2 - 1, 72);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Rect(x1, y1, x2, y2);

        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_CENTER);
        Game->GetBackBuffer()->WriteText(Font, x1 + ((x2 - x1 + 1) >> 1),
                                               y1 + ((y2 - y1 + 1) >> 1),
                                               txt, &FC);
}

void TMenu::DrawButtonSel(int x1, int y1, int x2, int y2, char* txt)
{
        Game->GetBackBuffer()->RectFill(x1 + 1, y1 + 1, x2 - 1, y2 - 1, 72);
        //Game->GetBackBuffer()->SetAColor(static_cast<BYTE>(Anim));
		Game->GetBackBuffer()->SetAColor(215);
        Game->GetBackBuffer()->Rect(x1, y1, x2, y2);

        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_CENTER);
        Game->GetBackBuffer()->WriteText(Font, x1 + ((x2 - x1 + 1) >> 1),
                                               y1 + ((y2 - y1 + 1) >> 1),
                                               txt, &FCSel);
}

void TMenu::DrawButtonGrey(int x1, int y1, int x2, int y2, char* txt)
{
        Game->GetBackBuffer()->RectFill(x1 + 1, y1 + 1, x2 - 1, y2 - 1, 225);
        Game->GetBackBuffer()->SetAColor(228);
        Game->GetBackBuffer()->Rect(x1, y1, x2, y2);

        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_CENTER);
        Game->GetBackBuffer()->WriteText(Font, x1 + ((x2 - x1 + 1) >> 1),
                                               y1 + ((y2 - y1 + 1) >> 1),
                                               txt, &FCGrey);
}

void TMenu::DrawMenu_Main()
{
        static char* txt[] = { "NEW GAME",
                               "SAVE",
                               "LOAD",
                               "END TURN",
                               "COLORS",
                               "HELP",
                               "EXIT" };

        for (int i = 0; i < 7; i++)
        if (((!Game->Started() || Game->EndGame) && (i == 1 || i == 3 || i == 4)) ||
            (Game->Player->GetPlayer() > 0 && i != 6 && !Game->EndGame && !DemoMode) ||
            (DemoMode && (i == 1 || i == 3)))
          DrawButtonGrey(MainWin[i][0], MainWin[i][1], MainWin[i][2], MainWin[i][3], txt[i]);
        else
          if (InWindow(Game->GetMX(), Game->GetMY(), MainWin[i]))
            DrawButtonSel(MainWin[i][0], MainWin[i][1], MainWin[i][2], MainWin[i][3], txt[i]);
          else
            DrawButton(MainWin[i][0], MainWin[i][1], MainWin[i][2], MainWin[i][3], txt[i]);

        if (!Game->Started())
        {
          Game->GetBackBuffer()->RectFill(100, 250, 500, 350, 45);
          Game->GetBackBuffer()->SetAColor(215);
          Game->GetBackBuffer()->Rect(100, 250, 500, 350);
          Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
          Game->GetBackBuffer()->WriteText(Font, 300, 275, "To start a game select", &FCSel);
          Game->GetBackBuffer()->WriteText(Font, 300, 305, "NEW GAME or LOAD", &FCSel);
        }
}

void TMenu::DrawMenu_New()
{
        static char* txt[] = { "<", ">",
                               "<", ">",
                               "<", ">",
                               "<", ">",
                               "<", ">",
                               "OK", "CANCEL", "DEMO" };

        for (int i = 0; i < 13; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), NewWin[i]))
          DrawButtonSel(NewWin[i][0], NewWin[i][1], NewWin[i][2], NewWin[i][3], txt[i]);
        else
          DrawButton(NewWin[i][0], NewWin[i][1], NewWin[i][2], NewWin[i][3], txt[i]);

// poziom trudnoœci
        Game->GetBackBuffer()->RectFill(600, 255, 790, 275, 15);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Rect(600, 255, 790, 275);
        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        Game->GetBackBuffer()->WriteText(Font, 695, 254, "difficulty level", &FCText);

        Game->GetBackBuffer()->CircleFill(695, 304, 20, 13);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Circle(695, 304, 20);
        Game->GetBackBuffer()->WriteText(Font, 695, 295, itoa(DLevel + 1, TxtBuf, 10), &FCText);

// kolory:
        Game->GetBackBuffer()->RectFill(600, 335, 790, 355, 15);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Rect(600, 335, 790, 355);
        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        Game->GetBackBuffer()->WriteText(Font, 695, 334, "colors", &FCText);

        for (int c = 0; c < 4; c++)
        {
          for (int i = 0; i < 50; i++)
            for (int j = 0; j < 35; j++)
              Game->GetBackBuffer()->PutPixel(670 + i, 365 + j + c * 45,
                                              ColorsDat[PCol[c]][random(3) + 1]);
          Game->GetBackBuffer()->SetAColor(215);
          Game->GetBackBuffer()->Rect(670, 365 + c * 45, 720, 400 + c * 45);
        }

/*        if (!Game->Started())
        {
          Game->GetBackBuffer()->RectFill(100, 250, 500, 350, 45);
          Game->GetBackBuffer()->SetAColor(215);
          Game->GetBackBuffer()->Rect(100, 250, 500, 350);
          Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
          Game->GetBackBuffer()->WriteText(Font, 300, 275, "Aby zacz¥† gr© wybierz", &FCSel);
          Game->GetBackBuffer()->WriteText(Font, 300, 305, "NOWA GRA lub ODCZYTAJ", &FCSel);
        }*/
}

void TMenu::DrawMenu_Save()
{
        static char* txt[2] = { "SAVE", "CANCEL" };

        if (TxtBuf[0] == 0)
          DrawButtonGrey(SaveWin[0][0], SaveWin[0][1], SaveWin[0][2], SaveWin[0][3], txt[0]);
        else
          if (InWindow(Game->GetMX(), Game->GetMY(), SaveWin[0]))
            DrawButtonSel(SaveWin[0][0], SaveWin[0][1], SaveWin[0][2], SaveWin[0][3], txt[0]);
          else
            DrawButton(SaveWin[0][0], SaveWin[0][1], SaveWin[0][2], SaveWin[0][3], txt[0]);

        if (InWindow(Game->GetMX(), Game->GetMY(), SaveWin[1]))
          DrawButtonSel(SaveWin[1][0], SaveWin[1][1], SaveWin[1][2], SaveWin[1][3], txt[1]);
        else
          DrawButton(SaveWin[1][0], SaveWin[1][1], SaveWin[1][2], SaveWin[1][3], txt[1]);

        Game->GetBackBuffer()->RectFill(600, 355, 790, 375, 15);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Rect(600, 355, 790, 375);
        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        Game->GetBackBuffer()->WriteText(Font, 695, 354, "type file name", &FCText);

        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        int x = Game->GetBackBuffer()->WriteText(Font, 695, 390, TxtBuf, &FCSel) + 2;

        bool cursor = Time % 1000 < 500;
        if (cursor)
          Game->GetBackBuffer()->RectFill(x, 391, x + 1, 406, 215);
}

void TMenu::DrawMenu_Load()
{
        if (FileError)
        {
          if (InWindow(Game->GetMX(), Game->GetMY(), FileErrWin))
            DrawButtonSel(FileErrWin[0], FileErrWin[1], FileErrWin[2], FileErrWin[3], "OK");
          else
            DrawButton(FileErrWin[0], FileErrWin[1], FileErrWin[2], FileErrWin[3], "OK");

          Game->GetBackBuffer()->RectFill(600, 355, 790, 375, 15);
          Game->GetBackBuffer()->SetAColor(230);
          Game->GetBackBuffer()->Rect(600, 355, 790, 375);
          Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
          Game->GetBackBuffer()->WriteText(Font, 695, 354, "file error", &FCText);

          Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
          Game->GetBackBuffer()->WriteText(Font, 695, 390, TxtBuf, &FCSel);

          return;
        }

        static char* txt[2] = { "LOAD", "CANCEL" };

        if (TxtBuf[0] == 0)
          DrawButtonGrey(SaveWin[0][0], SaveWin[0][1], SaveWin[0][2], SaveWin[0][3], txt[0]);
        else
          if (InWindow(Game->GetMX(), Game->GetMY(), SaveWin[0]))
            DrawButtonSel(SaveWin[0][0], SaveWin[0][1], SaveWin[0][2], SaveWin[0][3], txt[0]);
          else
            DrawButton(SaveWin[0][0], SaveWin[0][1], SaveWin[0][2], SaveWin[0][3], txt[0]);

        if (InWindow(Game->GetMX(), Game->GetMY(), SaveWin[1]))
          DrawButtonSel(SaveWin[1][0], SaveWin[1][1], SaveWin[1][2], SaveWin[1][3], txt[1]);
        else
          DrawButton(SaveWin[1][0], SaveWin[1][1], SaveWin[1][2], SaveWin[1][3], txt[1]);

        Game->GetBackBuffer()->RectFill(600, 355, 790, 375, 15);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Rect(600, 355, 790, 375);
        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        Game->GetBackBuffer()->WriteText(Font, 695, 354, "type file name", &FCText);

        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        int x = Game->GetBackBuffer()->WriteText(Font, 695, 390, TxtBuf, &FCSel) + 2;

        bool cursor = Time % 1000 < 500;
        if (cursor)
          Game->GetBackBuffer()->RectFill(x, 391, x + 1, 406, 215);
}

void TMenu::DrawMenu_Colors()
{
        static char* txt[] = { "<", ">",
                               "<", ">",
                               "<", ">",
                               "<", ">",
                               "OK", "CANCEL" };

        for (int i = 0; i < 10; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), ColorsWin[i]))
          DrawButtonSel(ColorsWin[i][0], ColorsWin[i][1], ColorsWin[i][2], ColorsWin[i][3], txt[i]);
        else
          DrawButton(ColorsWin[i][0], ColorsWin[i][1], ColorsWin[i][2], ColorsWin[i][3], txt[i]);

// kolory:
        Game->GetBackBuffer()->RectFill(600, 335, 790, 355, 15);
        Game->GetBackBuffer()->SetAColor(230);
        Game->GetBackBuffer()->Rect(600, 335, 790, 355);
        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        Game->GetBackBuffer()->WriteText(Font, 695, 334, "colors", &FCText);

        for (int c = 0; c < 4; c++)
        {
          for (int i = 0; i < 50; i++)
            for (int j = 0; j < 35; j++)
              Game->GetBackBuffer()->PutPixel(670 + i, 365 + j + c * 45,
                                              ColorsDat[PCol[c]][random(3) + 1]);
          Game->GetBackBuffer()->SetAColor(215);
          Game->GetBackBuffer()->Rect(670, 365 + c * 45, 720, 400 + c * 45);
        }
}

void TMenu::DrawMenu_Help()
{
        Game->GetBackBuffer()->RectFill(0, 0, 799, 599, 0);

//        _3d_To2D();
        _3d_Draw();

        static char* txt[4] = { "ABOUT", "INTERFACE", "RULES", "OK" };

        for (int i = 0; i < 4; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), HelpWin[i]))
          DrawButtonSel(HelpWin[i][0], HelpWin[i][1], HelpWin[i][2], HelpWin[i][3], txt[i]);
        else
          DrawButton(HelpWin[i][0], HelpWin[i][1], HelpWin[i][2], HelpWin[i][3], txt[i]);

        Game->GetBackBuffer()->SetAColor(215);
        for (int i = 0; i < 3; i++)
        Game->GetBackBuffer()->Rect(HelpWin[HelpID][0] - i, HelpWin[HelpID][1] - i,
                                    HelpWin[HelpID][2] + i, HelpWin[HelpID][3] + i);

        switch (HelpID) {
          case Help_About       : DrawHelp_About();     break;
          case Help_Interface   : DrawHelp_Interface(); break;
          case Help_Rules       : DrawHelp_Rules();     break;
        }
}

void TMenu::MenuProcessMessage_Main(UINT msg_id, WPARAM, LPARAM)
{
        Game->MapActive = true;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_LBUTTONDBLCLK)
          return;

        for (int j, i = 0; i < 7; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), MainWin[i]) &&
            !(((!Game->Started() || Game->EndGame) && (i == 1 || i == 3 || i == 4)) ||
            (Game->Player->GetPlayer() > 0 && i != 6 && !Game->EndGame && !DemoMode) ||
            (DemoMode && (i == 1 || i == 3))))
          switch (i) {

            case 0 :
              MenuID = Menu_New;
              for (j = 0; j < 4; j++)
                PCol[j] = Game->Colors[j];
              DLevel = Game->DiffLevel;
              break;

            case 1 :
              MenuID = Menu_Save;
              TxtBuf[0] = 0;
              Game->Field2 = -1;
              break;

            case 2 :
              MenuID = Menu_Load;
              TxtBuf[0] = 0;
              Game->Field2 = -1;
              break;

            case 3 :
              Game->EndTurn();
              break;

            case 4 :
              MenuID = Menu_Colors;
              for (j = 0; j < 4; j++)
                PCol[j] = Game->Colors[j];
              break;

            case 5 :
              MenuID = Menu_Help;
              _3d_Init();
              _3d_To2D();
              break;

            case 6 :
              Game->Exit(); break;
              
          }
}

void TMenu::MenuProcessMessage_New(UINT msg_id, WPARAM, LPARAM)
{
        Game->MapActive = false;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_LBUTTONDBLCLK)
          return;

        for (int j, i = 0; i < 13; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), NewWin[i]))
          switch (i) {
            case 0  : if (DLevel > 0) DLevel--; break;
            case 1  : if (DLevel < 9) DLevel++; break;
            case 2  : if (PCol[0]-- == 0) PCol[0] = 15; break;
            case 3  : if (PCol[0]++ == 15) PCol[0] = 0; break;
            case 4  : if (PCol[1]-- == 0) PCol[1] = 15; break;
            case 5  : if (PCol[1]++ == 15) PCol[1] = 0; break;
            case 6  : if (PCol[2]-- == 0) PCol[2] = 15; break;
            case 7  : if (PCol[2]++ == 15) PCol[2] = 0; break;
            case 8  : if (PCol[3]-- == 0) PCol[3] = 15; break;
            case 9  : if (PCol[3]++ == 15) PCol[3] = 0; break;
            case 10 :
              DemoMode = false;
              for (j = 0; j < 4; j++)
                Game->Colors[j] = PCol[j];
              Game->DiffLevel = DLevel;
              Game->NewGame();
              MenuID = Menu_Main;
              Game->MapActive = true;
              break;
            case 11 :
              MenuID = Menu_Main;
              Game->MapActive= true;
              break;
            case 12 :
              DemoMode = true;
              for (j = 0; j < 4; j++)
                Game->Colors[j] = PCol[j];
              Game->DiffLevel = DLevel;
              Game->NewGame();
              MenuID = Menu_Main;
              Game->MapActive = true;
              break;
          }
}

void TMenu::MenuProcessMessage_Save(UINT msg_id, WPARAM wparam, LPARAM)
{
        Game->MapActive = false;

        for (int i = 0; i < 2; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), SaveWin[i]))
          if (msg_id == WM_LBUTTONDOWN ||
              msg_id == WM_LBUTTONDBLCLK)
          switch (i) {
            case 0 :
              if (TxtBuf[0] == 0)
                break;
              Game->Save(TxtBuf);
            case 1 :
              Game->MapActive = true;
              MenuID = Menu_Main;
              break;
          }

        if (msg_id != WM_KEYDOWN)
          return;

        int vkey = wparam;
        int key = MapVirtualKey(vkey, 2);

        if (vkey == VK_RETURN)
        {
          if (TxtBuf[0] != 0)
          {
            Game->Save(TxtBuf);
            Game->MapActive = true;
            MenuID = Menu_Main;
          }
        }
        else
        if (vkey == VK_ESCAPE)
        {
          Game->MapActive = true;
          MenuID = Menu_Main;
        }
        else
          if (vkey == VK_BACK)
          {
            if (strlen(TxtBuf) > 0)
              TxtBuf[strlen(TxtBuf) - 1] = 0;
          }
          else
          {
            int len = strlen(TxtBuf);
            if (len < 19)
            {
              TxtBuf[len] = static_cast<BYTE>(key);
              TxtBuf[len + 1] = 0;
            }
          }
}

static bool FileExists(char* fname)
{
        static char buf[255];
        strcpy(buf, "zapisy\\");
        strcat(buf, fname);
        strcat(buf, ".dat");

        HANDLE f = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return false;

        CloseHandle(f);
        return true;
}

void TMenu::MenuProcessMessage_Load(UINT msg_id, WPARAM wparam, LPARAM)
{
        Game->MapActive = false;

        if (FileError)
        {
          if (InWindow(Game->GetMX(), Game->GetMY(), FileErrWin))
            if (msg_id == WM_LBUTTONDOWN ||
                msg_id == WM_LBUTTONDBLCLK)
            {
              Game->MapActive = true;
              FileError = false;
              MenuID = Menu_Main;
              return;
            }
        }

        for (int i = 0; i < 2; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), SaveWin[i]))
          if (msg_id == WM_LBUTTONDOWN ||
              msg_id == WM_LBUTTONDBLCLK)
          switch (i) {
            case 0 :
              if (TxtBuf[0] == 0)
                break;
              FileError = !FileExists(TxtBuf);
              if (!FileError)
                Game->Load(TxtBuf);
              else
                return;
            case 1 :
              Game->MapActive = true;
              MenuID = Menu_Main;
              break;
          }

        if (msg_id != WM_KEYDOWN ||
            FileError)
          return;

        int vkey = wparam;
        int key = MapVirtualKey(vkey, 2);

        if (vkey == VK_RETURN)
        {
          if (TxtBuf[0] != 0)
          {
            FileError = !FileExists(TxtBuf);
            if (!FileError)
              Game->Load(TxtBuf);
            else
              return;
            Game->MapActive = true;
            MenuID = Menu_Main;
          }
        }
        else
        if (vkey == VK_ESCAPE)
        {
          Game->MapActive = true;
          MenuID = Menu_Main;
        }
        else
          if (vkey == VK_BACK)
          {
            if (strlen(TxtBuf) > 0)
              TxtBuf[strlen(TxtBuf) - 1] = 0;
          }
          else
          {
            int len = strlen(TxtBuf);
            if (len < 19)
            {
              TxtBuf[len] = static_cast<BYTE>(key);
              TxtBuf[len + 1] = 0;
            }
          }
}

void TMenu::MenuProcessMessage_Colors(UINT msg_id, WPARAM, LPARAM)
{
        Game->MapActive = false;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_LBUTTONDBLCLK)
          return;

        for (int j, i = 0; i < 10; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), ColorsWin[i]))
          switch (i) {
            case 0  : if (PCol[0]-- == 0) PCol[0] = 15; break;
            case 1  : if (PCol[0]++ == 15) PCol[0] = 0; break;
            case 2  : if (PCol[1]-- == 0) PCol[1] = 15; break;
            case 3  : if (PCol[1]++ == 15) PCol[1] = 0; break;
            case 4  : if (PCol[2]-- == 0) PCol[2] = 15; break;
            case 5  : if (PCol[2]++ == 15) PCol[2] = 0; break;
            case 6  : if (PCol[3]-- == 0) PCol[3] = 15; break;
            case 7  : if (PCol[3]++ == 15) PCol[3] = 0; break;
            case 8 :
              for (j = 0; j < 4; j++)
                Game->Colors[j] = PCol[j];
              Game->SetColors();
            case 9 :
              MenuID = Menu_Main;
              Game->MapActive= true;
              break;
          }
}

void TMenu::MenuProcessMessage_Help(UINT msg_id, WPARAM, LPARAM)
{
        Game->MapActive = false;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_LBUTTONDBLCLK)
          return;

        for (int i = 0; i < 4; i++)
        if (InWindow(Game->GetMX(), Game->GetMY(), HelpWin[i]))
          switch (i) {
            case 0 : HelpID = Help_About; break;
            case 1 : HelpID = Help_Interface; break;
            case 2 : HelpID = Help_Rules; break;
            case 3 :
              MenuID = Menu_Main;
              Game->MapActive = true;
              break;
          }
}

void TMenu::DrawHelp_About()
{
        Game->GetBackBuffer()->SetTextJustify(TXT_CENTER, TXT_TOP);
        Game->GetBackBuffer()->SetTextAdd(16, 0);

        int taby[19];
        char* txt = "BATTLE FOR ATLANTIS";
        static TFontColors fc;

        for (int i = 0; i < 19; i++)
          taby[i] = sin(static_cast<double>(Time) / 200 + static_cast<double>(i) * 30 * M_PI / 180) * 20;

        int w = Game->GetBackBuffer()->GetTextWidth(HFont0, txt);
        int x = 400 - w / 2;

        for (int i = 0; i < 19; i++)
        {
          FillMemory(fc, sizeof fc, 200 + taby[i] / 2);
          x = Game->GetBackBuffer()->WriteChar(Font, x, 200 + taby[i], txt[i], &fc);
        }

        Game->GetBackBuffer()->SetTextAdd(4, 0);

        Game->GetBackBuffer()->WriteText(Font, 400, 300, "AUTHOR", &FC);
        Game->GetBackBuffer()->WriteText(Font, 400, 330, "PIOTR TURECKI", &FCSel);

		Game->GetBackBuffer()->WriteText(Font, 400, 500, "Based on \"Battle For Atlantis\" by William Soleau", &FC);
		Game->GetBackBuffer()->WriteText(Font, 400, 530, "(One of the first strategy games I've played as a kid)", &FC);

        Game->GetBackBuffer()->SetTextAdd(2, 0);
}

void TMenu::DrawHelp_Interface()
{
        Game->GetBackBuffer()->SetTextJustify(TXT_LEFT, TXT_TOP);
        Game->GetBackBuffer()->SetTextAdd(2, 0);

		static char* txt[] = { "To attack: first select a source field then a neighbour field",
                               "you want to attack (and press left mouse button)",
                               "",
                               "To move army: select a source field then a neighbour field",
                               "and press left mouse button (you lose turn when you move army)",
                               "",
                               "Click left mouse button on a field to select it",
							   "",
                               "Click right mouse button to unselect a field",
                               "",
                               "",
                               "Informations in the right upper corner:",
                               "- the amount of owned fields",
                               "- new army per turn",
                               "- current player (white rectangle)",
                               "- game turn",
                               "- difficulty level" };


        /*static char* txt[] = { "Aby zaatakowa† pole najpierw wybierz pole, z kt¢rego chcesz atakowa†,",
                               "a nast©pnie pole, kt¢re chcesz zaatakowa†",
                               "",
                               "Aby przenie˜† wojsko najpierw wybierz pole, z kt¢rego chcesz przenie˜†",
                               "wojsko, a nast©pnie pole, na kt¢re chcesz przenie˜† to wojsko (przy",
                               "przenoszeniu wojska tracisz tur©)",
                               "",
                               "Aby zaznaczy† pole kliknij na nim lewym przyciskiem myszki (pole b©dzie",
                               "zaznaczone migaj¥cym prostok¥tem)",
                               "",
                               "Aby odznaczy† pole kliknij prawym przyciskiem myszki",
                               "",
                               "",
                               "W prawym g¢rnym rogu znajduj¥ sie r¢¾ne informacje:",
                               "- ilo˜† p¢l, jak¥ aktualnie posiadaj¥ zawodnicy",
                               "- nowe wojsko, ktore dostaj¥ co tur©",
                               "- aktualnie graj¥cy zawodnik (biaˆy gruby prostok¥t)",
                               "- tura gry",
                               "- poziom trudno˜ci" };
*/
        for (int i = 0; i < 17; i++)
          Game->GetBackBuffer()->WriteText(HFont0, 70, 100 + i * 25, txt[i], &FC);

        static char* txt_s[] = { "*",
                                 "",
                                 "",
                                 "*",
                                 "",
                                 "",
                                 "*",
								 "",
                                 "*",
                                 "",
                                 "",
                                 "*",
                                 "",
                                 "",
                                 "" };

        for (int i = 0; i < 15; i++)
          Game->GetBackBuffer()->WriteText(HFont0, 30, 100 + i * 25, txt_s[i], &FCSel);
}

void TMenu::DrawHelp_Rules()
{
        Game->GetBackBuffer()->SetTextJustify(TXT_LEFT, TXT_TOP);
        Game->GetBackBuffer()->SetTextAdd(2, 0);

		static char* txt[] = { "To win you need to take control over the whole map",
                               "",
                               "You can attack only a field that has smaller army (If you attack",
							   "having only two troops you can lose).",
                               "",
                               "Every turn you get a new army",
                               "Things taken into account when calculating its size:",
                               "- number of fully occupied islands (the greater island the better)",
                               "- number of occupied command centers (fields with white dots)",
                               "- number of occupied fields",
                               "",
                               "Between islands you can only travel along the lines" };

        /*static char* txt[] = { "Aby wygra† musisz zaj¥† wszystkie pola",
                               "",
                               "Atakowa† mo¾esz tylko pole, na kt¢rym jest mniej wojska (je˜li atakujesz",
                               "pole z wojskiem r¢wnym 1, maj¥c na polu wojsko r¢wne 2, to mo¾esz przegra†)",
                               "",
                               "Co tur© dostajesz nowe wojsko, kt¢re rozdzielasz po swoich polach",
                               "Na ilo˜† tego wojska ma wpˆyw:",
                               "- ilo˜† wysp zaj©tych w caˆo˜ci (im wi©ksza wyspa tym wi©cej wojska",
                               "  dostaniesz)",
                               "- ilo˜† zaj©tych pol z biaˆymi koˆami (centrum dowodzenia)",
                               "- ilo˜† p¢l jakie posiadasz",
                               "",
                               "Pomi©dzy wyspami mo¾esz porusza† si© tylko po trasach oznaczonych liniami" };*/

        for (int i = 0; i < 12; i++)
          Game->GetBackBuffer()->WriteText(HFont0, 70, 100 + i * 25, txt[i], &FC);

        static char* txt_s[] = { "*",
                                 "",
                                 "*",
                                 "",
                                 "",
                                 "*",
                                 "*",
                                 "",
                                 "",
                                 "",
                                 "",
                                 "*",
                                 "" };

        for (int i = 0; i < 12; i++)
          Game->GetBackBuffer()->WriteText(HFont0, 30, 100 + i * 25, txt_s[i], &FCSel);
}

void TMenu::_3d_Init()
{
        for (int i = 0; i < MAXP; i++)
        {
          _3d_p3[i][0] = random(2) ? -random(2000) : random(2000);
          _3d_p3[i][1] = random(2) ? -random(2000) : random(2000);
          _3d_p3[i][2] = random(1000) + 10;
        }

        _3d_v = 0;
}

void TMenu::_3d_Move()
{
        for (int i = 0; i < MAXP; i++)
        {
          if ((_3d_p3[i][2] -= _3d_v * _3d_t / 25) <= 0)
          {
            _3d_p3[i][0] = random(2) ? -random(2000) : random(2000);
            _3d_p3[i][1] = random(2) ? -random(2000) : random(2000);
            _3d_p3[i][2] = random(100) + 900;
          }
        }

        float angle = M_PI / 180 * 0.4 * _3d_t / 25;

        float cosa = cos(angle);
        float sina = sin(angle);

        float nx, ny;
        for (int i = 0; i < MAXP; i++)
        {
          nx = _3d_p3[i][0] * cosa - _3d_p3[i][1] * sina;
          ny = _3d_p3[i][0] * sina + _3d_p3[i][1] * cosa;
          _3d_p3[i][0] = nx;
          _3d_p3[i][1] = ny;
        }
}

void TMenu::_3d_To2D()
{
        float t;
        for (int i = 0; i < MAXP; i++)
        {
          t = _3d_d / (_3d_d + _3d_p3[i][2]);
          _3d_p2[i][0] = 400 + t * _3d_p3[i][0];
          _3d_p2[i][1] = 300 - t * _3d_p3[i][1];
          _3d_p2[i][2] = 225 - _3d_p3[i][2] * 0.007;
        }
}

void TMenu::_3d_Draw()
{
        for (int x, y, i = 0; i < MAXP; i++)
        {
          x = _3d_p2[i][0];
          y = _3d_p2[i][1];
          if (x > 0 && x < 799 && y > 0 && y < 599)
            Game->GetBackBuffer()->RectFill(x, y, x+1, y+1, _3d_p2[i][2]);
        }
}
