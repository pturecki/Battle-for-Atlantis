#include "atlantis.h"

// *****************************************************************************************

#ifdef DEBUG_MODE

#define addpar(par)     wsprintf(buf, "%i\n", (int)(par)),                      \
                        WriteFile(f, buf, strlen(buf), &bw, NULL)

#define add(par)        wsprintf(buf, "%s%s%i\n", #par, " == ", (int)(par)),    \
                        WriteFile(f, buf, strlen(buf), &bw, NULL)

#define addhead(txt)    wsprintf(buf, "%s%s%s\n\n", "*** ", txt, " ***"),       \
                        WriteFile(f, buf, strlen(buf), &bw, NULL)

#define addline         strcpy(buf, "\n"),                                      \
                        WriteFile(f, buf, strlen(buf), &bw, NULL)

void TPlayer::DEBUG_write_AI()
{
        static char buf[1000];

        HANDLE f = CreateFile("d:\\ai.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;

        DWORD bw;

        addhead("Army Ranking");
        for (int i = 0; i < 4; i++)
        {
          add(ArmyRanking[i].Player);
          add(ArmyRanking[i].Army);
          addline;
        }

        add(Player);
        addline;

        add(AI.Sch[0]);
        add(AI.SchPar[0]);
        add(AI.SchNum);
        addline;

        addhead("FieldsToAttack");
        add(AI.FieldsToAttackNum);
        for (int i = 0; i < AI.FieldsToAttackNum; i++)
          addpar(AI.FieldsToAttack[i]);
        addline;

        add(AI.IHaveMostFields);

        add(AI.IHaveStrongestArmy);

        TMyFieldInf fld;

        addline;
        addhead("MyFields");
        add(AI.MyFieldsNum);
        for (int i = 0; i < AI.MyFieldsNum; i++)
        {
          fld = AI.MyFields[i];

          addline;

          addhead("FLD");
          
          add(fld.Field);
          add(fld.EnemyFieldsAround);
          add(fld.EnemyArmyAround);
          add(fld.EnemyStrongestArmy);
          add(fld.EnemyWeakestArmy);
          add(fld.EnemyStrongestArmy_SFld);
          add(fld.EnemyStrongestArmy_WFld);
          add(fld.EnemyWeakestArmy_SFld);
          add(fld.EnemyWeakestArmy_WFld);
          add(fld.EnemyWeakestArmySum);
          add(fld.FieldsToEnemy);
          add(fld.MinNeighbourFieldsToEnemy);

          addline;

          add(fld.NeighbourWithEnemy);
          add(fld.NeighbourWithStrongestPlayer);
          add(fld.NeighbourWithWeakestPlayer);
          add(fld.NeighbourWithHumanPlayer);
          add(fld.NeighbourNeighbourWithEnemy);
          add(fld.NeighbourWithCommandCentre);
          add(fld.NeighbourWithEnemyWholeIsland);
          add(fld.OnWholeIsland);
          add(fld.OnlyFieldOnIsland);
          add(fld.OnlyFieldOnWholeIsland);
          add(fld.SecureField);
          add(fld.NeighbourWithEnemyOnThisIsland);

          addline;

          add(fld.CommandCentre);
          add(fld.IslandEdge);
        }


        CloseHandle(f);
}

void TPlayer::DEBUG_write_move(TMove* m)
{
        static char buf[1000];

        HANDLE f = CreateFile("d:\\aimove.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;

        DWORD bw;

        add(Player);
        addline;

        add(AI.Sch[0]);
        add(AI.SchPar[0]);
        add(AI.SchNum);
        addline;

        add(m->SrcFld);
        add(m->DestFld);
        add(m->Par);
        add(m->Type);
        addline;

        add(Map[m->DestFld].Player);

        CloseHandle(f);
}

#endif // DEBUG_MOVE

// *****************************************************************************************

extern int AI_4_ArmyAdd[48];

#ifdef DEBUG_MODE

void TPlayer::DEBUG_write_a(char* fun, int army, int p)
{
        static char buf[1000];

        HANDLE f = CreateFile("d:\\a.txt", GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f == INVALID_HANDLE_VALUE)
          return;

        SetFilePointer(f, 0, NULL, FILE_END);

        DWORD bw;

        int sum = 0;
        for (int i = 0; i < 48; i++)
          sum += AI_4_ArmyAdd[i];

        wsprintf(buf, "%s: %d\t\t\t%s(%d / %d)\n", fun, sum, strlen(fun) > 12 ? "" : "\t",
                 army, Income[p]);
        WriteFile(f, buf, strlen(buf), &bw, NULL);

        CloseHandle(f);
}

#endif // DEBUG_MODE


int             TPlayer::Player = 0;
int             TPlayer::Army[4] = { 0 };
int             TPlayer::Fields[4] = { 0 };
int             TPlayer::WholeIslands[4] = { 0 };
int             TPlayer::WholeIslandsFields[4] = { 0 };
int             TPlayer::IslandsSize[8] = { 3, 11, 5, 7, 5, 9, 3, 5 };
TMove           TPlayer::UserMove = { -1, -1, -1, M_NOMOVE };
TMove           TPlayer::Move = { -1, -1, -1, M_NOMOVE };
TMove           TPlayer::RejectedMove = { -1, -1, -1, M_NOMOVE };
DWORD           TPlayer::WaitTime = 0;
bool            TPlayer::WaitFields[48] = { false };
bool            TPlayer::RequireNext = false;
TArtificialIntelligenceInf TPlayer::AI = { };

TRanking TPlayer::ArmyRanking[4] = { { 0, 0 } };
TRanking TPlayer::FieldsRanking[4] = { { 0, 0 } };
int TPlayer::Income[4] = { 0 };
bool TPlayer::IsExecutingMove = false;


TPlayer::TPlayer()
{
}

TPlayer::~TPlayer()
{
}

void TPlayer::Init()
{
        Player = 0;

        WaitTime = 0;

        Calculate();
//        AI_Calculate(); // jeœli cz³owiek zawsze zaczyna to tej linii nie musi byæ

        IsExecutingMove = false;
}

void TPlayer::Tick(DWORD time)
{
#ifdef DEBUG_MODE
        if (!DEBUG_0)
#endif
        if (time > WaitTime)
          WaitTime = 0;
        else
          WaitTime -= time;

        if (WaitTime == 0)
        {
          for (int i = 0; i < 48; i++)
            WaitFields[i] = false;
          if (RequireNext)
            Next();
        }
}

void TPlayer::Calculate()
{
        for (int i = 0; i < 4; i++)
          Army[i] = Fields[i] = WholeIslands[i] = WholeIslandsFields[i] = 0;

        // Army, Fields
        for (int p, i = 0; i < 48; i++)
        {
          p = Map[i].Player;
          Army[p] += Map[i].Army;
          Fields[p]++;
        }

        // WholeIslands, WholeIslandsFields
        for (int num, i = 0; i < 8; i++)
        if ((num = GetPlayerOnIsland(i)) != -1)
        {
          WholeIslands[num]++;
          WholeIslandsFields[num] += IslandsSize[i];
        }

        // Raninkgi
        for (int i = 0; i < 4; i++)
        {
          ArmyRanking[i].Army = Army[i];
          FieldsRanking[i].Fields = Fields[i];
          ArmyRanking[i].Player = FieldsRanking[i].Player = i;
        }

        TRanking tmp;

        // ArmyRanking
        for (int i = 0; i < 4; i++)
          for (int j = i + 1; j < 4; j++)
          if (ArmyRanking[j].Army > ArmyRanking[i].Army)
            tmp = ArmyRanking[j], ArmyRanking[j] = ArmyRanking[i], ArmyRanking[i] = tmp;

        // FieldsRanking
        for (int i = 0; i < 4; i++)
          for (int j = i + 1; j < 4; j++)
          if (FieldsRanking[j].Fields > FieldsRanking[i].Fields)
            tmp = FieldsRanking[j], FieldsRanking[j] = FieldsRanking[i], FieldsRanking[i] = tmp;

        // Income
        for (int i = 0; i < 4; i++)
          Income[i] = CalcIncome(i);
}

void TPlayer::MakeMove(int fld1, int fld2, int par, bool wait_turn)
{
        if (wait_turn)
        {
          UserMove.Type = M_WAITTURN;
          return;
        }
        
        UserMove.SrcFld = fld1;
        UserMove.DestFld = fld2;
        UserMove.Par = par;
        if (fld1 == -1)
          UserMove.Type = M_NOMOVE;
        else
          if (fld1 != -1 && fld2 != -1 && Map[fld1].Player == Map[fld2].Player)
            UserMove.Type = M_MOVEARMY;
          else
            if (fld1 != -1 && fld2 != -1 && Map[fld1].Player != Map[fld2].Player)
              UserMove.Type = M_ATTACK;
}

void TPlayer::Play()
{
        if (WaitTime > 0)
          return;

        if (!IsExecutingMove)
          Move = GetMove();

#ifdef DEBUG_MODE
        DEBUG_write_move(&Move);
#endif

        if (ContinueMove(&Move))
          ExecuteMove(&Move);
}

TMove TPlayer::GetMove()
{
        if (Player == 0 && !DemoMode)
        {
          TMove m = UserMove;
          UserMove.Type = M_NOMOVE;
          UserMove.SrcFld = UserMove.DestFld = UserMove.Par = -1;
          return m;
        }
        else
          return Computer_GetMove();
}

bool TPlayer::ContinueMove(TMove* move)
{
        if (Player == 0 && !DemoMode)
          return true;
        else
        {
          if (memcmp(move, &RejectedMove, sizeof(TMove)) == 0)
            return true;

          IsExecutingMove = Computer_ContinueMove(move);

          if (!IsExecutingMove)
            RejectedMove = *move;

          return IsExecutingMove;
        }
}

void TPlayer::ExecuteMove(TMove* move)
{
        if (Player == 0 && !DemoMode)
        switch (move->Type) {

          case M_NOMOVE :
            return;

          case M_ATTACK :
            Attack(move);
            if (Map[move->SrcFld].Army == 0)
            {
              Map[move->SrcFld].Army = 1;
              Map[move->DestFld].Army--;
              Map[move->SrcFld].Player = Map[move->DestFld].Player;
              Game->GetBackBuffer()->UpdateMap(&Map[move->SrcFld]);
              Game->Field1 = Game->Field2 = -1;
              Next();
            }
            if (Map[move->DestFld].Army == 0)
            {
              Map[move->DestFld].Player = 0;
              Game->GetBackBuffer()->UpdateMap(&Map[move->DestFld]);
              Game->MoveArmyAfterAttack(move);
            }
            else
            if (Map[move->SrcFld].Army <= Map[move->DestFld].Army)
              Game->CancelAttack();
            break;

          case M_MOVEARMY :
            Map[move->DestFld].Army += move->Par;
            Map[move->SrcFld].Army -= move->Par;
            Next();
            break;

          case M_WAITTURN :
            Next();
            break;

        }
        else
          Computer_ExecuteMove(move);

        Calculate();
        if (Player > 0 || DemoMode)
          AI_Calculate();
}

void TPlayer::Next()
{
        IsExecutingMove = false;

        if (WaitTime > 0)
        {
          RequireNext = true;
          return;
        }
        RequireNext = false;

        do {
          Player = (Player + 1) % 4;
          if (Player == 0)
            Game->Turn++;
        } while (Fields[Player] == 0);

        if (Player == 0 && !DemoMode)
        {
          Calculate();
          Game->PlaceArmy();
          Calculate();
        }

        if (Player > 0 || DemoMode)
        {
          Calculate();
          AI_Calculate();
          Computer_PlaceArmy();
          Calculate();
          AI_Calculate();
        }
}

void TPlayer::DrawWaitFields()
{
        for (int i = 0; i < 48; i++)
        if (WaitFields[i])
          Game->GetBackBuffer()->DrawWaitField(&Map[i]);
}

// ***********************************************************************************************

int TPlayer::CalcIncome(int player)
{
        if (Fields[player] == 0)
          return 0;

        int income_if = 0;
        for (int i = 0; i < 8; i++)
        if (GetPlayerOnIsland(i) == player)
          income_if += (IslandsSize[i] + 1) / 2;

        if (income_if > 0 &&
            income_if < 2)
          income_if = 2;

        for (int i = 0; i < 8; i++)
        if (GetPlayerFieldsOnIsland(i, player) > IslandsSize[i] * 0.5 &&
            GetPlayerOnIsland(i) != player)
          income_if++;

        int fields = Fields[player]/* - WholeIslandsFields[player]*/;

        int income_f = (fields + 3) / 4;
        if (income_f < 3)
          income_f = 3;

        int income_cc = 0;
        for (int i = 0; i < 48; i++)
        if (Map[i].CommandCentre &&
            Map[i].Player == player)
          income_cc++;

        int income = income_f + income_if + income_cc;

        return income < 3 ? 3 : income;
}

int TPlayer::GetPlayerFieldsOnIsland(int island, int player)
{
        int num = 0;
        for (int i = 0; i < 48; i++)
        if (Map[i].Island == island && Map[i].Player == player)
          num++;

        return num;
}

int TPlayer::GetPlayerOnIsland(int island)
{
        int num[4] = { 0, 0, 0, 0 };
        for (int i = 0; i < 48; i++)
        if (Map[i].Island == island)
          num[Map[i].Player]++;

        for (int i = 0; i < 4; i++)
        if (num[i] == IslandsSize[island])
          return i;

        return -1;
}

void TPlayer::Attack(TMove* move)
{
        int a1 = Map[move->SrcFld].Army;
        int a2 = Map[move->DestFld].Army;

        if (a1 == 2)
        {
          if (IS_PERCENT(50))
          {
            Map[move->SrcFld].Army = 0;
            Map[move->DestFld].Army = 2;
          }
          else
          {
            Map[move->SrcFld].Army = 2;
            Map[move->DestFld].Army = 0;
          }
          return;
        }

        int diff = abs(a1 - a2);

        if (diff > a1) diff = a1;
        if (diff > a2) diff = a2;

        if (diff < 2)
          diff = 2;     // ok diff

        int m1 = a1 / diff;
        int m2 = a2 / diff;

        int add = 0;

        if (m1 > 10 || m2 > 10)
          add = 2 * diff;

        int amin = min(a1, a2);

        if (diff + add > amin)
          add = amin - diff;

        if (add < 0)
          add = 0;      // ok add

        a1 -= random(diff + 1) + add;
        a2 -= random(diff + 1) + add;

        if (a1 == Map[move->SrcFld].Army)
          a1--;
        if (a2 == Map[move->DestFld].Army)
          a2--;

        if (a1 < 1 && a2 < 1)
        {
          Map[move->SrcFld].Army = 2;
          Map[move->DestFld].Army = 0;
          return;
        }

        if (a1 < 1 && a2 >= 1)
        {
          Map[move->SrcFld].Army = 0;
          if (a2 < 2)
            a2 = 2;
          Map[move->DestFld].Army = a2;
          return;
        }

        if (a1 >= 1 && a2 < 1)
        {
          if (a1 < 2)
            a1 = 2;
          Map[move->SrcFld].Army = a1;
          Map[move->DestFld].Army = 0;
          return;
        }

        Map[move->SrcFld].Army = a1;
        Map[move->DestFld].Army = a2;
}

TMove TPlayer::Computer_GetMove()
{
        IsExecutingMove = true;

#ifdef DEBUG_MOVE
        DEBUG_0 = true;
#endif

        switch (Game->DiffLevel) {
          case 0 : return AI_0_GetMove();
          case 1 : return AI_1_GetMove();
          case 2 : return AI_2_GetMove();
          case 3 : return AI_3_GetMove();
          case 4 : return AI_4_GetMove();
          case 5 : return AI_5_GetMove();
          case 6 : return AI_6_GetMove();
          case 7 : return AI_7_GetMove();
          case 8 : return AI_8_GetMove();
          case 9 : return AI_9_GetMove();
        }
}

bool TPlayer::Computer_ContinueMove(TMove* move)
{
        switch (Game->DiffLevel) {
          case 0 : return AI_0_ContinueMove(move);
          case 1 : return AI_1_ContinueMove(move);
          case 2 : return AI_2_ContinueMove(move);
          case 3 : return AI_3_ContinueMove(move);
          case 4 : return AI_4_ContinueMove(move);
          case 5 : return AI_5_ContinueMove(move);
          case 6 : return AI_6_ContinueMove(move);
          case 7 : return AI_7_ContinueMove(move);
          case 8 : return AI_8_ContinueMove(move);
          case 9 : return AI_9_ContinueMove(move);
        }
}

void TPlayer::Computer_ExecuteMove(TMove* move)
{
#ifdef DEBUG_MOVE
        if (DEBUG_0)
        {
          if (move->SrcFld != -1)
            InitWaitField(move->SrcFld);
          if (move->DestFld != -1)
            InitWaitField(move->DestFld);
          return;
        }
#endif

        switch (move->Type) {

          case M_NOMOVE :       // to nie powinno siê pojawiæ
            return;

          case M_MOVEARMY :
            Map[move->SrcFld].Army -= move->Par;
            Map[move->DestFld].Army += move->Par;
            InitWaitField(move->SrcFld);
            InitWaitField(move->DestFld);
            Next();
            break;

          case M_ATTACK :
            Attack(move);
            InitWaitField(move->SrcFld);
            InitWaitField(move->DestFld);
            if (Map[move->SrcFld].Army == 0)
            {
              Map[move->SrcFld].Army = 1;
              Map[move->DestFld].Army--;
              Map[move->SrcFld].Player = Map[move->DestFld].Player;
              Game->GetBackBuffer()->UpdateMap(&Map[move->SrcFld]);
              Next();
              break;
            }
            if (Map[move->DestFld].Army == 0)
              Computer_MoveArmy(move);
            if (Game->DiffLevel <= 3)
              if (Map[move->DestFld].Army > Map[move->SrcFld].Army)
                IsExecutingMove = false;
            break;

          case M_WAITTURN :
            Next();
        }
        if (Game->DiffLevel >= 4)
          IsExecutingMove = false;
}

void TPlayer::Computer_MoveArmy(TMove* move)
{
        Map[move->DestFld].Player = Player;
        Game->GetBackBuffer()->UpdateMap(&Map[move->DestFld]);
        
        if (Map[move->SrcFld].Army == 2)
        {
          Map[move->SrcFld].Army = Map[move->DestFld].Army = 1;
          AI_Calculate();
          IsExecutingMove = false;
          return;
        }

        AI_Calculate();

        switch (Game->DiffLevel) {
          case 0 : AI_0_MoveArmy(move); break;
          case 1 : AI_1_MoveArmy(move); break;
          case 2 : AI_2_MoveArmy(move); break;
          case 3 : AI_3_MoveArmy(move); break;
          case 4 : AI_4_MoveArmy(move); break;
          case 5 : AI_5_MoveArmy(move); break;
          case 6 : AI_6_MoveArmy(move); break;
          case 7 : AI_7_MoveArmy(move); break;
          case 8 : AI_8_MoveArmy(move); break;
          case 9 : AI_9_MoveArmy(move); break;
        }
        IsExecutingMove = false;
}

void TPlayer::Computer_PlaceArmy()
{
#ifdef DEBUG_MOVE
        DEBUG_0 = true;
#endif

        switch (Game->DiffLevel) {
          case 0 : AI_0_PlaceArmy(); break;
          case 1 : AI_1_PlaceArmy(); break;
          case 2 : AI_2_PlaceArmy(); break;
          case 3 : AI_3_PlaceArmy(); break;
          case 4 : AI_4_PlaceArmy(); break;
          case 5 : AI_5_PlaceArmy(); break;
          case 6 : AI_6_PlaceArmy(); break;
          case 7 : AI_7_PlaceArmy(); break;
          case 8 : AI_8_PlaceArmy(); break;
          case 9 : AI_9_PlaceArmy(); break;
        }
}

// AI *********************************************************************************************

#include "ai_calculate.h"

#include "ai_0.h"
#include "ai_1.h"
#include "ai_2.h"
#include "ai_3.h"
#include "ai_4.h"

// reszta tak jak AI == 4

// ************************************************************************************************
// AI *** DiffLevel == 5 (6) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_5_GetMove() { return AI_3_GetMove(); }
bool TPlayer::AI_5_ContinueMove(TMove* move) { return AI_4_ContinueMove(move); }
void TPlayer::AI_5_MoveArmy(TMove* move) { AI_4_MoveArmy(move); }
void TPlayer::AI_5_PlaceArmy() { AI_4_PlaceArmy(); }

// ************************************************************************************************
// AI *** DiffLevel == 6 (7) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_6_GetMove() { return AI_3_GetMove(); }
bool TPlayer::AI_6_ContinueMove(TMove* move) { return AI_4_ContinueMove(move); }
void TPlayer::AI_6_MoveArmy(TMove* move) { AI_4_MoveArmy(move); }
void TPlayer::AI_6_PlaceArmy() { AI_4_PlaceArmy(); }

// ************************************************************************************************
// AI *** DiffLevel == 7 (8) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_7_GetMove() { return AI_3_GetMove(); }
bool TPlayer::AI_7_ContinueMove(TMove* move) { return AI_4_ContinueMove(move); }
void TPlayer::AI_7_MoveArmy(TMove* move) { AI_4_MoveArmy(move); }
void TPlayer::AI_7_PlaceArmy() { AI_4_PlaceArmy(); }

// ************************************************************************************************
// AI *** DiffLevel == 8 (9) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_8_GetMove() { return AI_3_GetMove(); }
bool TPlayer::AI_8_ContinueMove(TMove* move) { return AI_4_ContinueMove(move); }
void TPlayer::AI_8_MoveArmy(TMove* move) { AI_4_MoveArmy(move); }
void TPlayer::AI_8_PlaceArmy() { AI_4_PlaceArmy(); }

// ************************************************************************************************
// AI *** DiffLevel == 9 (10) *********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_9_GetMove() { return AI_3_GetMove(); }
bool TPlayer::AI_9_ContinueMove(TMove* move) { return AI_4_ContinueMove(move); }
void TPlayer::AI_9_MoveArmy(TMove* move) { AI_4_MoveArmy(move); }
void TPlayer::AI_9_PlaceArmy() { AI_4_PlaceArmy(); }

// ************************************************************************************************

#include "ai_aux.h"

void TPlayer::AddSch(TScheme sch, int par, bool always_add)
{
        if (!always_add &&
            (AI.SchNum == 0 ? SCH_NOSCHEME : AI.Sch[AI.SchNum - 1]) == sch)
          return;

        if (AI.SchNum < SCHMAX)
        {
          AI.Sch[AI.SchNum] = sch;
          AI.SchPar[AI.SchNum++] = par;
        }
}

