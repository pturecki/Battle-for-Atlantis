#ifndef __INCLUDE_MENU__
#define __INCLUDE_MENU__


enum TMenuID {
        Menu_Main,
        Menu_New,
        Menu_Save,
        Menu_Load,
//        Menu_EndTurn,
        Menu_Colors,
        Menu_Help };
//        Menu_Exit };        

enum THelpID {
        Help_About      = 0,
        Help_Interface  = 1,
        Help_Rules      = 2 };

#define MAXP            600        


class TMenu {
public:
        TMenu();
        ~TMenu();

        static void MenuProcessMessage(UINT msg_id, WPARAM wparam, LPARAM lparam);

        static void Tick(DWORD time);

        static void DrawMenu();

        static void DrawButton(int x1, int y1, int x2, int y2, char* txt);
        static void DrawButtonSel(int x1, int y1, int x2, int y2, char* txt);
        static void DrawButtonGrey(int x1, int y1, int x2, int y2, char* txt);

protected:
        static void DrawMenu_Main();
        static void DrawMenu_New();
        static void DrawMenu_Save();
        static void DrawMenu_Load();
//        static void DrawMenu_EndTurn();
        static void DrawMenu_Colors();
        static void DrawMenu_Help();
//        static void DrawMenu_Exit();

        static void MenuProcessMessage_Main(UINT msg_id, WPARAM wparam, LPARAM lparam);
        static void MenuProcessMessage_New(UINT msg_id, WPARAM wparam, LPARAM lparam);
        static void MenuProcessMessage_Save(UINT msg_id, WPARAM wparam, LPARAM lparam);
        static void MenuProcessMessage_Load(UINT msg_id, WPARAM wparam, LPARAM lparam);
//        static void MenuProcessMessage_EndTurn(UINT msg_id, WPARAM wparam, LPARAM lparam);
        static void MenuProcessMessage_Colors(UINT msg_id, WPARAM wparam, LPARAM lparam);
        static void MenuProcessMessage_Help(UINT msg_id, WPARAM wparam, LPARAM lparam);
//        static void MenuProcessMessage_Exit(UINT msg_id, WPARAM wparam, LPARAM lparam);

        static void DrawHelp_About();
        static void DrawHelp_Interface();
        static void DrawHelp_Rules();

        static void _3d_Init();
        static void _3d_Move();
        static void _3d_To2D();
        static void _3d_Draw();

public:
        static TFont*           Font;
        static TFontColors      FC;
        static TFontColors      FCSel;
        static TFontColors      FCGrey;
        static TFontColors      FCText;

        static TMenuID          MenuID;

protected:
        static char             TxtBuf[20];
        static DWORD            Time;
        static int              Anim;
        static int              PCol[4];
        static int              DLevel;
        static bool             FileError;
        static THelpID          HelpID;
        static TFont*           HFont0;

        static float            _3d_d;
        static float            _3d_v;
        static float            _3d_p3[MAXP][3];
        static float            _3d_p2[MAXP][3]; // 2 == kolor
        static int              _3d_t;
};


#endif