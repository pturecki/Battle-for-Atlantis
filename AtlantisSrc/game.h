
#define EV_NoEvent              0
#define EV_Earthquake           1 // -50%
#define EV_Flood                2 // -70%
#define EV_PeopleJoinUp         3 // +5..10
#define EV_Rebellion            4 // kilka pól zmienia w³aœciciela
#define EV_PlusCC               5 // armia w c. dow. +100%
#define EV_MinusCC              6 // armia w c. dow. -3..6


class TGame {
public:
        TGame(HWND window);
        ~TGame();

        static LRESULT ProcessMessage(HWND window, UINT msg_id, WPARAM wp, LPARAM lp);

        static void Draw();
        static void Exit();
        static void NewGame();
        static void SetColors();
        static void EndTurn();
        static void PlaceArmy();
        static void Save(char* fn);
        static void Load(char* fn);

        static DWORD GetTime();
        static HWND GetWindow();
        static TBackBuffer* GetBackBuffer();
        static int GetMX();
        static int GetMY();
        static bool Started();

        static void Tick(DWORD time);
        static bool Running();
        static int GetField();
        static void MoveArmyAfterAttack(TMove* move);
        static void CancelAttack();

protected:
        static void Draw_PlayersInfo();
        static void CalcFields();

        static void Run_HowManyArmy();
        static void Draw_HowManyArmy();
        static void Process_HowManyArmy(UINT msg_id, WPARAM wp, LPARAM lp);

        static void Draw_PlacingArmy();
        static void Process_PlacingArmy(UINT msg_id, WPARAM wp, LPARAM lp);

public:
        static void RandomEvent_Fun();

protected:
        static void Draw_RandomEvent();
        static void Process_RandomEvent(UINT msg_id, WPARAM wp, LPARAM lp);
        static void CheckEndGame();
        static void DrawEndGame();

        static void InitMap_0();
        static void InitMap_1();
        static void InitMap_2();
        static void InitMap_3();
        static void InitMap_4();
        static void InitMap_5();
        static void InitMap_6();
        static void InitMap_7();
        static void InitMap_8();
        static void InitMap_9();

protected:
		static bool Initialized;

        static HWND             Window;
        static bool             IsRunning;
        static DWORD            Time;     // w ms
        static TBackBuffer*     BackBuffer;
        static HDC              DeviceContext;
        static TMenu*           Menu;
        static int              MouseX;
        static int              MouseY;
        static bool             IsStarted;
        static int              SelAnim;
        static TFontColors      FCText;
        static TFont*           FontText;
        static TImage*          PInfoImg;
        static bool             IsGetting_HowManyArmy;
        static bool             IsGetting_AfterAttack;
        static int              RandomEvent;
        static int              RandomTurnTime; // jeœli np. > 7 to mo¿e wyst¹piæ zd. losowe
        static bool             RandomEventRange[48];
        static int              RandomEventPar; // np. nr. graczam którego bêd¹ pola po rebelii
        static int              EndGameTime;
        static int              EndGameAnim;

public:
        static int              DiffLevel; // na ekranie 1..10, wewn¹trz 0..9
        static int              Colors[4];
        static bool             MapActive;
        static bool             MenuActive;
        static int              Field1; // 1 zaznaczone pole
        static int              Field2; // 2 ---- ; np gdzie atakowaæ
        static int              Par; // par
        static int              Turn;
        static bool             IsPlacingArmy;
        static int              NewArmy;
        static TPlayer*         Player;
        static bool             EndGame; // ktoœ wygra³
};


inline void TGame::Exit()
{
        IsRunning = false;
}

inline DWORD TGame::GetTime()
{
        return Time;
}

inline HWND TGame::GetWindow()
{
        return Window;
}

inline TBackBuffer* TGame::GetBackBuffer()
{
        return BackBuffer;
}

inline int TGame::GetMX()
{
        return MouseX;
}

inline int TGame::GetMY()
{
        return MouseY;
}

inline bool TGame::Started()
{
        return IsStarted;
}

inline bool TGame::Running()
{
        return IsRunning;
}
