
#include "atlantis.h"


TGame* Game;

bool TGame::Initialized = false;

HWND            TGame::Window                   = NULL;
bool            TGame::IsRunning                = true;
DWORD           TGame::Time                     = 0;
TBackBuffer*    TGame::BackBuffer               = NULL;
HDC             TGame::DeviceContext            = 0;
TMenu*          TGame::Menu                     = NULL;
int             TGame::MouseX                   = 0;
int             TGame::MouseY                   = 0;
bool            TGame::IsStarted                = false;
int             TGame::SelAnim                  = 0;
TFontColors     TGame::FCText                   = { { 0 } };
TFont*          TGame::FontText                 = NULL;
TImage*         TGame::PInfoImg                 = NULL;
bool            TGame::IsGetting_HowManyArmy    = false;
bool            TGame::IsGetting_AfterAttack    = false;
int             TGame::RandomEvent              = EV_NoEvent;
int             TGame::RandomTurnTime           = 0;
bool            TGame::RandomEventRange[48]     = { false };
int             TGame::RandomEventPar           = -1;
int             TGame::EndGameTime              = 0;
int             TGame::EndGameAnim              = 0;

int             TGame::DiffLevel                = 4;
int             TGame::Colors[4]                = { 0, 4, 5, 11 };
bool            TGame::MapActive                = true;
bool            TGame::MenuActive               = true;
int             TGame::Field1                   = -1;
int             TGame::Field2                   = -1;
int             TGame::Par                      = 0;
int             TGame::Turn                     = 1;
bool            TGame::IsPlacingArmy            = false;
int             TGame::NewArmy                  = 0;
TPlayer*        TGame::Player                   = NULL;
bool            TGame::EndGame                  = false;


BYTE ColorsDat[17][4] = { { 0, 210, 204, 210 },
                          { 0, 108, 150, 180 },
                          { 0,  20,  63,  29 },
                          { 0,  58,  29, 137 },
                          { 0,  22,  23,  52 },
                          { 0,  20,  20,  14 },
                          { 0,  78,  78, 116 },
                          { 0,  86,  86,  78 },
                          { 0,  86,  86,  48 },
                          { 0,  86,  87,  48 },
                          { 0, 204, 162, 204 },
                          { 0,  81, 123, 118 },
                          { 0, 122, 120, 128 },
                          { 0, 192, 151, 157 },
                          { 0, 202, 203, 160 },
                          { 0, 132, 133,  90 },
                          { 0, 235, 235, 235 } };       // zaznaczenie

TField Map[48];

int HMAWin[8][4] = { { 600, 350, 660, 385 },
                     { 730, 350, 790, 385 },
                     { 600, 390, 660, 415 },
                     { 730, 390, 790, 415 },
                     { 600, 420, 660, 445 },
                     { 730, 420, 790, 445 },
                     { 600, 455, 660, 490 },
                     { 670, 455, 790, 490 } };


TGame::TGame(HWND window)
{
//        randomize();/////////////////////////////////////

        Window = window;
        IsRunning = true;
        Time = 0;

        DeviceContext = GetDC(Window);

        BackBuffer = new TBackBuffer(window);

        Menu = new TMenu();

        HANDLE f = CreateFile("map.dat", GENERIC_READ, 0, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;
        DWORD bread;
        ReadFile(f, &Map, sizeof Map, &bread, NULL);
        
        int rr = sizeof(Map);
        
        CloseHandle(f);

        char buf[10];
        for (int i = 0; i < 48; i++)
        {
          itoa(i, buf, 10);
          strcat(buf, ".fld");
          Map[i].Img = BackBuffer->CreateImageFile(buf, 0);
          Map[i].ColorsSel = ColorsDat[16];
        }

        Player = new TPlayer();

        FillMemory(&FCText, 1024, 215);
        FontText = BackBuffer->CreateFontFile("svga_f0.fnt", 0);
        PInfoImg = BackBuffer->CreateImageFile("pinfobg.dat", 0);

		Initialized = true;

        ShowWindow(Window, SW_SHOW);
        UpdateWindow(Window);
}

TGame::~TGame()
{
        BackBuffer->DeleteImage(PInfoImg);
        BackBuffer->DeleteFont(FontText);

        delete Player;

        for (int i = 0; i < 48; i++)
          BackBuffer->DeleteImage(Map[i].Img);

        delete BackBuffer;

        delete Menu;

        ReleaseDC(Window, DeviceContext);
}

LRESULT TGame::ProcessMessage(HWND window, UINT msg_id, WPARAM wp, LPARAM lp)
{
		if (!Initialized)
			return DefWindowProc(window, msg_id, wp, lp);

        if (MenuActive)
          Menu->MenuProcessMessage(msg_id, wp, lp);
        else
          if (!EndGame)
            if (IsGetting_HowManyArmy)
              Process_HowManyArmy(msg_id, wp, lp);
            else
              if (IsPlacingArmy)
                Process_PlacingArmy(msg_id, wp, lp);
              else
                Process_RandomEvent(msg_id, wp, lp);

        switch (msg_id) {

#ifdef DEBUG_MODE
          case WM_KEYDOWN :
            DEBUG_0 = false;
            break;
#endif

          case WM_PAINT :
            Draw();
            break;

          case WM_MOUSEMOVE :
            MouseX = LOWORD(lp);
            MouseY = HIWORD(lp);
            break;

          case WM_LBUTTONDOWN :
          case WM_LBUTTONDBLCLK :
            if (!IsPlacingArmy)
            {
              CalcFields();
              Run_HowManyArmy();
              if (Field1 != -1 && Field2 != -1 && Map[Field2].Player != 0 &&
                  Map[Field1].Army > Map[Field2].Army)
                Player->MakeMove(Field1, Field2, -1, false);
            }
            else
            {
              int num = GetField();
              if (num != -1 && Map[num].Player == 0)
                Field1 = num;
            }
            break;

          case WM_RBUTTONDOWN :
          case WM_RBUTTONDBLCLK :
            if (IsGetting_HowManyArmy || IsPlacingArmy)
              break;
            Field1 = Field2 = -1;
            break;
        }

        return DefWindowProc(window, msg_id, wp, lp);
}

void TGame::Draw()
{
        if (Menu->MenuID != Menu_Help)
        {
          BackBuffer->DrawMap();
          int f = GetField();
          if (f != -1 && IsStarted && !DemoMode)
            BackBuffer->DrawField(&Map[f], true);

          Draw_PlayersInfo();
          DrawEndGame();
        }

        Menu->DrawMenu();

        if (!EndGame && Menu->MenuID != Menu_Help)
        {
          Draw_HowManyArmy();
          Draw_PlacingArmy();
          Draw_RandomEvent();
          Player->DrawWaitFields();
        }

        BackBuffer->Show(DeviceContext);
}

void TGame::NewGame()
{
        randomize();

        Turn = 1;
        RandomTurnTime = 0;

        EndGame = false;
        MapActive = MenuActive = true;

        IsStarted = true;

        Field1 = Field2 = -1;

        Time = 0;
        IsGetting_HowManyArmy = IsGetting_AfterAttack = false;
        RandomEvent = EV_NoEvent;
        IsPlacingArmy = false;

        switch (DiffLevel) {
          case 0 : InitMap_0(); break;
          case 1 : InitMap_1(); break;
          case 2 : InitMap_2(); break;
          case 3 : InitMap_3(); break;
          case 4 : InitMap_4(); break;
          case 5 : InitMap_5(); break;
          case 6 : InitMap_6(); break;
          case 7 : InitMap_7(); break;
          case 8 : InitMap_8(); break;
          case 9 : InitMap_9(); break;
        }

        // polosuj kolory
        for (int p1, p2, i = 0; i < 10; i++)
        {
          p1 = DemoMode ? random(4) : random(3) + 1;
          while ((p2 = DemoMode ? random(4) : random(3) + 1) == p1)
            ;
          for (int j = 0; j < 48; j++)
          if (Map[j].Player == p1)
            Map[j].Player = p2;
          else
            if (Map[j].Player == p2)
              Map[j].Player = p1;
        }

        for (int i = 0; i < 48; i++)
        {
          Map[i].Colors = ColorsDat[Colors[Map[i].Player]];
          Map[i].ColorsSel = ColorsDat[16];
          BackBuffer->UpdateMap(&Map[i]);
        }

        Player->Init();

        if (!DemoMode)
          PlaceArmy();
}

void TGame::SetColors()
{
        for (int i = 0; i < 48; i++)
        {
          Map[i].Colors = ColorsDat[Colors[Map[i].Player]];
          Map[i].ColorsSel = ColorsDat[16];
          BackBuffer->UpdateMap(&Map[i]);
        }
}

void TGame::EndTurn()
{
        Field1 = Field2 = -1;
        Player->MakeMove(-1, -1, -1, M_WAITTURN);
}

void TGame::PlaceArmy()
{
        IsPlacingArmy = true;
        NewArmy = Player->Income[0];
        Par = 1;
        MenuActive = false;
}

void TGame::Save(char* fn)
{
        static char buf[255];
        strcpy(buf, "zapisy\\");
        strcat(buf, fn);
        strcat(buf, ".dat");

        HANDLE f = CreateFile(buf, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;
        DWORD bw;
        for (int i = 0; i < 48; i++)
        {
          WriteFile(f, &Map[i].Player, sizeof Map[i].Player, &bw, NULL);
          WriteFile(f, &Map[i].Army, sizeof Map[i].Army, &bw, NULL);
        }
        for (int i = 0; i < 4; i++)
          WriteFile(f, &Colors[i], sizeof Colors[i], &bw, NULL);
        WriteFile(f, &RandomTurnTime, sizeof RandomTurnTime, &bw, NULL);
        WriteFile(f, &Turn, sizeof Turn, &bw, NULL);
        WriteFile(f, &DiffLevel, sizeof DiffLevel, &bw, NULL);
        CloseHandle(f);
}

void TGame::Load(char* fn)
{
        static char buf[255];
        strcpy(buf, "zapisy\\");
        strcat(buf, fn);
        strcat(buf, ".dat");

        HANDLE f = CreateFile(buf, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;
        DWORD br;
        for (int i = 0; i < 48; i++)
        {
          ReadFile(f, &Map[i].Player, sizeof Map[i].Player, &br, NULL);
          ReadFile(f, &Map[i].Army, sizeof Map[i].Army, &br, NULL);
        }
        for (int i = 0; i < 4; i++)
          ReadFile(f, &Colors[i], sizeof Colors[i], &br, NULL);
        ReadFile(f, &RandomTurnTime, sizeof RandomTurnTime, &br, NULL);
        ReadFile(f, &Turn, sizeof Turn, &br, NULL);
        ReadFile(f, &DiffLevel, sizeof DiffLevel, &br, NULL);
        CloseHandle(f);

        for (int i = 0; i < 48; i++)
          Map[i].Colors = ColorsDat[Colors[Map[i].Player]];

        randomize();
        
        EndGame = false;
        MapActive = MenuActive = true;

        IsStarted = true;

        Field1 = Field2 = -1;

        Time = 0;
        IsGetting_HowManyArmy = IsGetting_AfterAttack = false;
        RandomEvent = EV_NoEvent;
        IsPlacingArmy = false;

        for (int i = 0; i < 48; i++)
          BackBuffer->UpdateMap(&Map[i]);
        Draw_PlayersInfo();

        DemoMode = false;  

        Player->Init();
}

void TGame::Tick(DWORD time)
{
        Time += time;
        if (EndGame)
        {
          EndGameTime += time;
          EndGameAnim += time;
          if (EndGameAnim > 100)
          {
            for (int i = 0; i < 48; i++)
              RandomEventRange[i] = IS_PERCENT(25);
            EndGameAnim = 0;
          }
        }

        SelAnim = Time % 200 <= 100 ? (225 + (Time % 200) / 10) : (245 - (Time % 200) / 10);
        for (int i = 0; i < 3; i++)
          ColorsDat[16][i + 1] = static_cast<BYTE>(SelAnim);

        Menu->Tick(time);

        CheckEndGame();

        if (!EndGame && IsStarted && Menu->MenuID != Menu_Help)
        {
          Player->Tick(time);
          Player->Play();
        }

        //SetCursor(Menu->MenuID == Menu_Help ? Cursor2 : Cursor1);
        InvalidateRect(Window, NULL, false);
}

int TGame::GetField()
{
        if (!MapActive || Player->GetPlayer() > 0 || EndGame)
          return -1;

        for (int i = 0; i < 48; i++)
        if (InWindow(MouseX, MouseY, Map[i].Win))
          if (BackBuffer->GetImagePixel(Map[i].Img, MouseX - Map[i].Win[0], MouseY - Map[i].Win[1]))
            return i;
        return -1;
}

void TGame::MoveArmyAfterAttack(TMove* move)
{
        Field1 = move->SrcFld;
        Field2 = move->DestFld;

        if (Map[Field1].Army == 2)
        {
          Map[Field1].Army = Map[Field2].Army = 1;
          Field1 = Field2 = Par = -1;
          return;
        }

        IsGetting_HowManyArmy = IsGetting_AfterAttack = true;
        Par = 1;

        MapActive = false;
        MenuActive = false;

        HMAWin[6][2] = 790;
}

void TGame::CancelAttack()
{
        Field1 = Field2 = Par = -1;
}

// ************************************************************************************************

void TGame::Draw_PlayersInfo()
{
        if (!IsStarted)
          return;

        unsigned int rssav = rand();
        srand(0);

        BackBuffer->PutImage(680, 10, PInfoImg, IMG_COPY);

        for (int p = 0; p < 4; p++)
        {
          if (Player->GetFields(p) == 0)
            continue;

          for (int i = 1; i < 20; i++)
            for (int j = 1; j < 20; j++)
              BackBuffer->PutPixel(690 + i, 20 + p * 30 + j, ColorsDat[Colors[p]][random(3) + 1]);
          BackBuffer->SetAColor(215);
          BackBuffer->Rect(690, 20 + p * 30, 710, 40 + p * 30);
        }

        int p = Player->GetPlayer();
        BackBuffer->SetAColor(215);
        for (int i = 2; i < 4; i++)
          BackBuffer->Rect(690 - i, 20 + p * 30 - i, 710 + i, 40 + p * 30 + i);

        char TxtBuf[10];

        BackBuffer->SetTextJustify(TXT_RIGHT, TXT_TOP);
        BackBuffer->SetAColor(210);
        for (int i = 0; i < 4; i++)
        {
          if (Player->GetFields(i) == 0)
            continue;

          BackBuffer->WriteText(FontText, 735, 22 + i * 30, itoa(Player->GetFields(i), TxtBuf, 10),
                                &FCText);
          BackBuffer->WriteText(FontText, 780, 22 + i * 30, itoa(Player->Income[i], TxtBuf, 10),
                                &FCText);
          BackBuffer->LineX(750, 26 + i * 30, 32 + i * 30);
          BackBuffer->LineY(i * 30 + 29, 747, 753);
        }

        BackBuffer->SetTextJustify(TXT_LEFT, TXT_TOP);
        BackBuffer->WriteText(Menu->Font, 690, 145, "turn:",
                              reinterpret_cast<TFontColors*>(Menu->FC));
        BackBuffer->SetTextJustify(TXT_CENTER, TXT_TOP);
        BackBuffer->WriteText(Menu->Font, 755, 145, itoa(Turn, TxtBuf, 10), &FCText);

        BackBuffer->SetTextJustify(TXT_LEFT, TXT_TOP);
        BackBuffer->WriteText(Menu->Font, 690, 170, "diff:",
                              reinterpret_cast<TFontColors*>(Menu->FC));
        BackBuffer->SetTextJustify(TXT_CENTER, TXT_TOP);
        BackBuffer->WriteText(Menu->Font, 755, 170, itoa(DiffLevel + 1, TxtBuf, 10),
                              reinterpret_cast<TFontColors*>(Menu->FCGrey));

        srand(rssav);
}

void TGame::CalcFields()
{
        int fld = GetField();
        if (fld == -1)
          return;

        if (Field1 == -1)
        {
          if (Map[fld].Player == 0 && Map[fld].Army > 1)
            Field1 = fld;
        }
        else
          Field2 = fld;

        if (Field1 != -1 && Field2 != -1)
        {
          for (int i = 0; i < 8; i++)
            if (Map[Field1].Neighbours[i] == Field2)
              return;
          if (Map[Field2].Player == 0 && Map[Field2].Army > 1)
            Field1 = Field2;
          else
            Field1 = -1;
          Field2 = -1;
        }
}

void TGame::Run_HowManyArmy()
{
        if (IsGetting_HowManyArmy || IsPlacingArmy)
          return;

        if (Field1 != -1 && Field2 != -1 && Map[Field1].Player == Map[Field2].Player)
        {
          IsGetting_HowManyArmy = true;
          IsGetting_AfterAttack = false;
          Par = 1;

          MapActive = false;
          MenuActive = false;

          HMAWin[6][2] = 660;
        }
}

void TGame::Draw_HowManyArmy()
{
        if (!IsGetting_HowManyArmy)
          return;

        BackBuffer->DrawField(&Map[Field2], true);

        static char* txt[8] = { "<", ">",
                                "<<", ">>",
                                "<<<<", ">>>>",
                                "OK", "CANCEL" };

        for (int i = 0; i < (IsGetting_AfterAttack ? 7 : 8); i++)
        if (InWindow(MouseX, MouseY, HMAWin[i]))
          Menu->DrawButtonSel(HMAWin[i][0], HMAWin[i][1], HMAWin[i][2], HMAWin[i][3], txt[i]);
        else
          Menu->DrawButton(HMAWin[i][0], HMAWin[i][1], HMAWin[i][2], HMAWin[i][3], txt[i]);

        char TxtBuf[10];

        BackBuffer->RectFill(670, 350, 720, 385, 13);
        BackBuffer->SetAColor(230);
        BackBuffer->Rect(670, 350, 720, 385);
        BackBuffer->SetTextJustify(TXT_CENTER, TXT_TOP);
        BackBuffer->WriteText(Menu->Font, 695, 359, itoa(Par, TxtBuf, 10),
                              &*reinterpret_cast<TFontColors*>(Menu->FCText));

        BackBuffer->RectFill(600, 320, 790, 340, 15);
        BackBuffer->SetAColor(230);
        BackBuffer->Rect(600, 320, 790, 340);
        BackBuffer->WriteText(Menu->Font, 695, 319, "army to move",
                              &*reinterpret_cast<TFontColors*>(Menu->FCText));
}

void TGame::Process_HowManyArmy(UINT msg_id, WPARAM, LPARAM)
{
        if (!IsGetting_HowManyArmy)
          return;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_LBUTTONDBLCLK)
          return;

        for (int i = 0; i < (IsGetting_AfterAttack ? 7 : 8); i++)
        if (InWindow(MouseX, MouseY, HMAWin[i]))
          switch (i) {
            case 0 : if (Par > 1) Par--; break;
            case 1 : if (Par < Map[Field1].Army - 1) Par++; break;
            case 2 : if ((Par -= 10) < 1) Par = 1; break;
            case 3 : if ((Par += 10) > Map[Field1].Army - 1) Par = Map[Field1].Army - 1; break;
            case 4 : Par = 1; break;
            case 5 : Par = Map[Field1].Army - 1; break;
            case 6 :
              if (!IsGetting_AfterAttack)
                Player->MakeMove(Field1, Field2, Par, false);
              else
              {
                Map[Field1].Army -= Par;
                Map[Field2].Army += Par;
              }
            case 7 :
              IsGetting_HowManyArmy = false;
              Field1 = Field2 = -1;
              MapActive = MenuActive = true;
              break;
          }
}

static int PAWin[7][4] = { { 600, 400, 660, 435 },
                           { 730, 400, 790, 435 },
                           { 600, 440, 660, 465 },
                           { 730, 440, 790, 465 },
                           { 600, 470, 660, 495 },
                           { 730, 470, 790, 495 }, 
                           //////////////////////////////////////////////////////////
                           { 600, 510, 790, 545 } };

void TGame::Draw_PlacingArmy()
{
        if (!IsPlacingArmy)
          return;

        static char* txt[7] = { "<", ">",
                                "<<", ">>",
                                "<<<<", ">>>>",
                                "OK" };

        char TxtBuf[10];

        if (Field1 != -1)
        {
          for (int i = 0; i < 7; i++)
          if (InWindow(MouseX, MouseY, PAWin[i]))
            Menu->DrawButtonSel(PAWin[i][0], PAWin[i][1], PAWin[i][2], PAWin[i][3], txt[i]);
          else
            Menu->DrawButton(PAWin[i][0], PAWin[i][1], PAWin[i][2], PAWin[i][3], txt[i]);

          BackBuffer->RectFill(670, 400, 720, 435, 13);
          BackBuffer->SetAColor(230);
          BackBuffer->Rect(670, 400, 720, 435);
          BackBuffer->SetTextJustify(TXT_CENTER, TXT_TOP);
          BackBuffer->WriteText(Menu->Font, 695, 409, itoa(Par, TxtBuf, 10),
                                reinterpret_cast<TFontColors*>(Menu->FCText));

          BackBuffer->RectFill(600, 370, 790, 390, 15);
          BackBuffer->SetAColor(230);
          BackBuffer->Rect(600, 370, 790, 390);
          BackBuffer->WriteText(Menu->Font, 695, 369, "army to move",
                                reinterpret_cast<TFontColors*>(Menu->FCText));
        }
        else
        {
          BackBuffer->RectFill(600, 370, 790, 390, 15);
          BackBuffer->SetAColor(230);
          BackBuffer->Rect(600, 370, 790, 390);
          BackBuffer->WriteText(Menu->Font, 695, 369, "select your field",
                                reinterpret_cast<TFontColors*>(Menu->FCText));
        }

        BackBuffer->RectFill(600, 295, 790, 315, 15);
        BackBuffer->SetAColor(230);
        BackBuffer->Rect(600, 295, 790, 315);
        BackBuffer->WriteText(Menu->Font, 695, 294, "new army",
                              reinterpret_cast<TFontColors*>(Menu->FCText));

        BackBuffer->RectFill(670, 325, 720, 360, 13);
        BackBuffer->SetAColor(230);
        BackBuffer->Rect(670, 325, 720, 360);
        BackBuffer->SetTextJustify(TXT_CENTER, TXT_TOP);
        BackBuffer->WriteText(Menu->Font, 695, 334, itoa(NewArmy, TxtBuf, 10),
                              reinterpret_cast<TFontColors*>(Menu->FCText));
}

void TGame::Process_PlacingArmy(UINT msg_id, WPARAM, LPARAM)
{
        if (!IsPlacingArmy)
          return;

        if (msg_id != WM_LBUTTONDOWN && msg_id != WM_LBUTTONDBLCLK)
          return;

        for (int i = 0; i < 7; i++)
        if (InWindow(MouseX, MouseY, PAWin[i]))
          switch (i) {
            case 0 : if (Par > 1) Par--; break;
            case 1 : if (Par < NewArmy) Par++; break;
            case 2 : if ((Par -= 10) < 1) Par = 1; break;
            case 3 : if ((Par += 10) > NewArmy) Par = NewArmy; break;
            case 4 : Par = 1; break;
            case 5 : Par = NewArmy; break;
            case 6 :
              Map[Field1].Army += Par;
              NewArmy -= Par;
              Par = 1;
              if (NewArmy == 0)
              {
                IsPlacingArmy = false;
                MenuActive = true;
                Field1 = Field2 = Par = -1;
                RandomEvent_Fun();
              }
              return;
          }
}

void TGame::RandomEvent_Fun()
{
        if (++RandomTurnTime < 6)
          return;

        if (random(100) < 30 + (RandomTurnTime - 6) * 15)
        {
          RandomTurnTime = 0;
          RandomEvent = random(6) + 1;

          for (int i = 0; i < 48; i++)
            RandomEventRange[i] = false;

          int i, j, fld, num;
          switch (RandomEvent) {

            case EV_Earthquake : // -50% 4 pola
              num = 1;
              RandomEventRange[random(48)] = true;
              while (num < 4)
                for (i = 0; i < 48; i++)
                if (RandomEventRange[i])
                  for (j = 0; j < 8; j++)
                  {
                    fld = Map[i].Neighbours[j];
                    if (fld != -1 &&
                        !RandomEventRange[fld])
                    {
                      RandomEventRange[fld] = true;
                      if (++num == 4)
                        goto EV_Earthquake_Ok;
                    }
                  }
              EV_Earthquake_Ok:
              break;

            case EV_Flood : // - 70% 6 pól
              num = 1;
              RandomEventRange[random(48)] = true;
              while (num < 6)
                for (i = 0; i < 48; i++)
                if (RandomEventRange[i])
                  for (j = 0; j < 8; j++)
                  {
                    fld = Map[i].Neighbours[j];
                    if (fld != -1 &&
                        !RandomEventRange[fld])
                    {
                      RandomEventRange[fld] = true;
                      if (++num == 6)
                        goto EV_Flood_Ok;
                    }
                  }
              EV_Flood_Ok:
              break;

            case EV_PeopleJoinUp : // +5..10 5 pól
              num = 0;
              while (num < 5)
              {
                fld = random(48);
                if (!RandomEventRange[fld])
                {
                  RandomEventRange[fld] = true;
                  num++;
                }
              }
              break;

            case EV_Rebellion : // 4 pola
              num = 0;
              while (num < 4)
              {
                fld = random(48);
                if (!RandomEventRange[fld] &&
                    Player->GetFields(Map[fld].Player) > 4)
                {
                  RandomEventRange[fld] = true;
                  num++;
                }
              }
              break;

            case EV_PlusCC : // CC + 100%
            case EV_MinusCC : // CC - 3..6
              for (i = 0; i < 48; i++)
              if (Map[i].CommandCentre)
                RandomEventRange[i] = true;
              break;
          }

          MapActive = false;
          MenuActive = false;
        }
}

void TGame::Draw_RandomEvent()
{
        if (RandomEvent == EV_NoEvent)
          return;

        for (int i = 0; i < 48; i++)
        if (RandomEventRange[i])
          if (Time % 200 <= 100)
            BackBuffer->DrawWaitField(&Map[i]);
          else
            BackBuffer->DrawField(&Map[i], false);

        static int bwin[4] = { 600, 460, 790, 495 };

        if (InWindow(MouseX, MouseY, bwin))
          Menu->DrawButtonSel(bwin[0], bwin[1], bwin[2], bwin[3], "OK");
        else
          Menu->DrawButton(bwin[0], bwin[1], bwin[2], bwin[3], "OK");

        BackBuffer->RectFill(600, 300, 790, 450, 15);
        BackBuffer->SetAColor(230);
        BackBuffer->Rect(600, 300, 790, 450);

        static char* txt[7][5] = { { NULL },
                                   { "Earthquake", "50% of troops", "have been killed.", NULL },
                                   { "Flood", "70% of troops", "have been killed.", NULL },
                                   { "Partisans have joined", "to the troops.", NULL },
                                   { "Rebellion", "Troops have rebelled", NULL },
                                   { "Command Centers", "Troops have been",  "doubled", NULL },
                                   { "Command Centers", "Accident. Troops", "have been killed", NULL } };

        BackBuffer->SetTextJustify(TXT_LEFT, TXT_TOP);

        for (int i = 0; i < 5; i++)
        {
          if (txt[RandomEvent][i] == NULL)
            break;
          BackBuffer->WriteText(Menu->Font, 605, 305 + i * 25, txt[RandomEvent][i],
                                reinterpret_cast<TFontColors*>(Menu->FCText));
        }
}


void TGame::Process_RandomEvent(UINT msg_id, WPARAM, LPARAM)
{
        if (RandomEvent == EV_NoEvent)
          return;

        static int bwin[4] = { 600, 460, 790, 495 };

        if (msg_id == WM_LBUTTONDOWN &&
            InWindow(MouseX, MouseY, bwin))
        {
          int i, tmp;

          switch (RandomEvent) {

            case EV_Earthquake :
              for (i = 0; i < 48; i++)
              if (RandomEventRange[i])
              {
                tmp = Map[i].Army * 0.5;
                if (tmp < 1)
                  tmp = 1;
                Map[i].Army = tmp;
              }
              break;

            case EV_Flood :
              for (i = 0; i < 48; i++)
              if (RandomEventRange[i])
              {
                tmp = Map[i].Army * 0.3;
                if (tmp < 1)
                  tmp = 1;
                Map[i].Army = tmp;
              }
              break;

            case EV_PeopleJoinUp :
              for (i = 0; i < 48; i++)
              if (RandomEventRange[i])
                Map[i].Army += random(6) + min(Turn / 4 + 1, 4);
              break;

            case EV_Rebellion : 
              for (i = 0; i < 48; i++)
              if (RandomEventRange[i])
              {
                while ((tmp = random(4)) == Map[i].Player || Player->GetArmy(tmp) == 0)
                  ;
                Map[i].Player = tmp;
                Map[i].Army *= 0.6;
                if (Map[i].Army < 1)
                  Map[i].Army = 1;
                if (Map[i].Army < 2)
                  Map[i].Army = random(3) + 3;  
                BackBuffer->UpdateMap(&Map[i]);  
              }      
              break;

            case EV_PlusCC :
              for (i = 0; i < 48; i++)
              if (RandomEventRange[i])
                Map[i].Army *= 2;
              break;

            case EV_MinusCC :
              for (i = 0; i < 48; i++)
              if (RandomEventRange[i])
              {
                tmp = Map[i].Army;
                tmp -= random(4) + 3;
                if (tmp < 1)
                  tmp = 1;
                Map[i].Army = tmp;
              }
              break;

          }
          
          RandomEvent = EV_NoEvent;
          MapActive = MenuActive = true;
          Player->Calculate();
        }
}

void TGame::CheckEndGame()
{
        if (EndGame)
          return;

        int num = 0;

        for (int i = 0; i < 4; i++)
        if (Player->GetFields(i) != 0)
          num++;

        if (num != 1)
          return;

        EndGame = true;
        MenuActive = MapActive = true;
        Field1 = Field2 = -1;
        EndGameTime = 0;
        EndGameAnim = 0;
}


void TGame::DrawEndGame()
{
        if (!EndGame)
          return;
          
        if (EndGameTime < 500)
        {
          if (EndGameTime % 200 < 100)
          for (int i = 0; i < 48; i++)
            BackBuffer->DrawWaitField(&Map[i]);
        }
        else
        {
          for (int i = 0; i < 48; i++)
          if (RandomEventRange[i])
            BackBuffer->DrawWaitField(&Map[i]);
        }
}

static int remain_tab[48]; 

void TGame::InitMap_0()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 0 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 9, 12, 11, 11 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;
        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + random(2);
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_1()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 0 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 9, 14, 12, 8 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;
        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + random(2);
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_2()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 0 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 9, 16, 10, 8 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;
        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + random(2);
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_3()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 8, 17, 10, 8 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;
        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + random(2);
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_4()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 12, 16, 8, 7 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;

        int isl;
        switch (random(3)) {
          case 0 : isl = 2; break;
          case 1 : isl = 4; break;
          case 2 : isl = 7; break;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == isl)
        {
          Map[remain_tab[i]].Player = 0;
          Map[remain_tab[i]].Army = 1 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[0]--;
          i = -1;
        }

        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + random(3);
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_5()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = Map[remain_tab[i]].Island == 3 ? 1 : cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 10, 17, 9, 7 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;

        int isl;
        switch (random(3)) {
          case 0 : isl = 2; break;
          case 1 : isl = 4; break;
          case 2 : isl = 7; break;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == isl)
        {
          Map[remain_tab[i]].Player = 0;
          Map[remain_tab[i]].Army = 1 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[0]--;
          i = -1;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == 3)
        {
          Map[remain_tab[i]].Player = 1;
          Map[remain_tab[i]].Army = random(3) + (Map[remain_tab[i]].IslandEdge ? 5 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[1]--;
          i = -1;
        }

        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + (p ? 1 + random(3) : random(2));
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_6()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = Map[remain_tab[i]].Island == 3 ? 1 : cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 11, 16, 16, 0 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;

        int isl;
        switch (random(3)) {
          case 0 : isl = 2; break;
          case 1 : isl = 4; break;
          case 2 : isl = 7; break;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == isl)
        {
          Map[remain_tab[i]].Player = 0;
          Map[remain_tab[i]].Army = 1 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[0]--;
          i = -1;
        }

        isl = random(2) ? 0 : 6; 
        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == 3 ||
            Map[remain_tab[i]].Island == isl)
        {
          Map[remain_tab[i]].Player = 1;
          Map[remain_tab[i]].Army = random(3) + (Map[remain_tab[i]].IslandEdge ? 3 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[1]--;
          i = -1;
        }

        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + (p ? 1 + random(4) : random(2));
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_7()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player = Map[remain_tab[i]].Island == 3 ? 1 : cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(3);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 8, 15, 10, 9 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;

        int isl;
        switch (random(3)) {
          case 0 : isl = 2; break;
          case 1 : isl = 4; break;
          case 2 : isl = 7; break;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == isl)
        {
          Map[remain_tab[i]].Player = 0;
          Map[remain_tab[i]].Army = random(2) + (Map[remain_tab[i]].IslandEdge ? 2 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[0]--;
          i = -1;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == 3 ||
            Map[remain_tab[i]].Island == 0 ||
            Map[remain_tab[i]].Island == 6)
        {
          Map[remain_tab[i]].Player = 1;
          Map[remain_tab[i]].Army = random(3) + (Map[remain_tab[i]].IslandEdge ? 4 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[1]--;
          i = -1;
        }

        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + (p ? 1 + random(4) : random(2));
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_8()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 1, 1, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player =
                (Map[remain_tab[i]].Island == 1 ||
                 Map[remain_tab[i]].Island == 3) ? 1 : cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 15, 17, 6, 5 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;

        int isl;
        switch (random(3)) {
          case 0 : isl = 2; break;
          case 1 : isl = 4; break;
          case 2 : isl = 7; break;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == isl ||
            Map[remain_tab[i]].Island == 6)
        {
          Map[remain_tab[i]].Player = 0;
          Map[remain_tab[i]].Army = random(3) + (Map[remain_tab[i]].IslandEdge ? 3 : 1); 
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[0]--;
          i = -1;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == 1 ||
            Map[remain_tab[i]].Island == 3)
        {
          Map[remain_tab[i]].Player = 1;
          Map[remain_tab[i]].Army = random(2) + (Map[remain_tab[i]].IslandEdge ? 4 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[1]--;
          i = -1;
        }

        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + (p ? 1 + random(4) : random(2));
            DeleteComponent(remain_tab, fld, remain_num--);
          }
}

void TGame::InitMap_9()
{
        for (int i = 0; i < 48; i++)
          remain_tab[i] = i;

        static int cc[5] = { 0, 1, 2, 3, 1 };
        for (int tmp, a, b, i = 0; i < 10; i++)
        {
          a = random(4);
          while ((b = random(4)) == a)
            ;
          tmp = cc[a], cc[a] = cc[b], cc[b] = tmp;
        }

        int remain_num = 48;

        for (int cnum = 0, i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].CommandCentre)
        {
          Map[remain_tab[i]].Player =
                (Map[remain_tab[i]].Island == 1 ||
                 Map[remain_tab[i]].Island == 5) ? 1 : cc[cnum++];
          Map[remain_tab[i]].Army = 3 + random(2);
          DeleteComponent(remain_tab, i, remain_num--);
          i = -1;
        }

        int cnt[4] = { 11, 20, 9, 3 };
        int rnd = random(2);
        cnt[0] -= rnd;
        cnt[1] += rnd;
        rnd = random(2);
        cnt[2] -= rnd;
        cnt[3] += rnd;

        int isl;
        switch (random(3)) {
          case 0 : isl = 2; break;
          case 1 : isl = 4; break;
          case 2 : isl = 7; break;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == isl ||
            Map[remain_tab[i]].Island == 6 ||
            Map[remain_tab[i]].Island == 0)
        {
          Map[remain_tab[i]].Player = 0;
          Map[remain_tab[i]].Army = random(3) + (Map[remain_tab[i]].IslandEdge ? 2 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[0]--;
          i = -1;
        }

        for (int i = 0; i < remain_num; i++)
        if (Map[remain_tab[i]].Island == 1 ||
            Map[remain_tab[i]].Island == 5)
        {
          Map[remain_tab[i]].Player = 1;
          Map[remain_tab[i]].Army = random(2) + (Map[remain_tab[i]].IslandEdge ? 4 : 1);
          DeleteComponent(remain_tab, i, remain_num--);
          cnt[1]--;
          i = -1;
        }

        for (int p = 0; p < 4; p++)
          for (int fld, i = 0; i < cnt[p]; i++)
          {
            fld = random(remain_num);
            Map[remain_tab[fld]].Player = p;
            Map[remain_tab[fld]].Army = 1 + (p ? 1 + random(4) : random(2));
            DeleteComponent(remain_tab, fld, remain_num--);
          }

        for (int i = 0; i < 48; i++)
        if (Map[i].Player == 3 &&
            IS_PERCENT(70))
          Map[i].Player = 0;
}

