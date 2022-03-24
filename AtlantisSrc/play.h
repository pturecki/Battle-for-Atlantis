
enum TMoveType {
        M_NOMOVE        = 0,
        M_MOVEARMY      = 1,
        M_ATTACK        = 2,
        M_WAITTURN      = 3 };

typedef         struct {
                  int           SrcFld;
                  int           DestFld;
                  int           Par;            // taki sobie parametr, np. ile wojska przenieœæ
                  TMoveType     Type;
                } TMove;

//#pragma anon_struct on
typedef         struct {
                  union {
                    int           Army;
                    int           Fields;
                  };
                  int           Player;
                } TRanking;
//#pragma anon_struct off

enum TScheme {
        SCH_NOSCHEME            = 0,
        SCH_CAPTUREISLAND       = 1, // SchPar = nr wyspy
        SCH_DEFENDISLAND        = 2, // SchPar = nr wyspy
        SCH_ATTACKPLAYER        = 3, // SchPar = gracz, którego atakowaæ
        SCH_DEFENDISLANDS       = 4, // broñ swoich wysp
        SCH_ATTACK              = 5, // atakuj najs³absze pola
        SCH_CAPTURECC           = 6, // atakuj cc
        SCH_DEFENDCC            = 7, // broñ cc
        SCH_DEPRIVEISLAND       = 8 }; // pozbaw kogoœ wyspy, SchPar = nr wyspy

typedef         struct {
                  int           Field;                          // nr pola
                  int           EnemyFieldsAround;              // ile wrogich pól jest woko³o
                  int           EnemyArmyAround;                // jaka wroga armia jest woko³o (suma armi z poszczególnych pól)
                  int           EnemyStrongestArmy;             // najsilniejsza armia s¹siaduj¹ca z polem
                  int           EnemyWeakestArmy;               // najs³absza ----
                  int           EnemyStrongestArmy_SFld;        // najsilniejsza armia s¹siaduj¹ca z polem, które ma najsilniejsz¹ armiê
                  int           EnemyStrongestArmy_WFld;        // ----, które ma najs³absz¹ armiê
                  int           EnemyWeakestArmy_SFld;          // najs³absza armia s¹siaduj¹ca z polem, które ma najsilniejsz¹ armiê
                  int           EnemyWeakestArmy_WFld;          // ----, które ma najs³absz¹ armiê
                  int           EnemyWeakestArmySum;            // EnemyWeakestArmy + EnemyWeakestArmy_W;
                  int           FieldsToEnemy;                  // odleg³oœæ od najbli¿szego wroga
                  int           MinNeighbourFieldsToEnemy;      // najmniejsza odleg³oœæ dowolnego s¹siedniego pola od wroga
                  bool          NeighbourWithPlayer[4];

                  unsigned int  NeighbourWithEnemy              : 1; // pole s¹siaduje z wrogiem
                  unsigned int  NeighbourWithStrongestPlayer    : 1; // pole s¹siaduje z najsilniejszym graczem
                  unsigned int  NeighbourWithWeakestPlayer      : 1; // ---- z najs³abszym ----
                  unsigned int  NeighbourWithHumanPlayer        : 1; // ---- z polem cz³owieka
                  unsigned int  NeighbourNeighbourWithEnemy     : 1; // jakieœ pole s¹siaduj¹ce s¹siaduje z wrogiem
                  unsigned int  NeighbourWithCommandCentre      : 1; // s¹siaduje z centr. dow.
                  unsigned int  NeighbourWithEnemyWholeIsland   : 1; // s¹siaduje z polem, które nale¿y do wyspy zajêtej w ca³oœci przez wroga
                  unsigned int  OnWholeIsland                   : 1; // pole le¿y na wyspie, która jest ca³a zajêta
                  unsigned int  OnlyFieldOnIsland               : 1; // jedyne pole na wyspie
                  unsigned int  OnlyFieldOnWholeIsland          : 1; // jedyne pole na wyspie, której wszystkie inne pola s¹ zajête przez jednego gracza
                  unsigned int  SecureField                     : 1; // pola nie trzeba broniæ, pola wokó³ maj¹ Army >= 2, najbli¿sze wrogie pole nie bli¿ej ni¿ w odleg³oœci 3 pól (Pole - P - P - Wróg)

                  unsigned int  NeighbourWithThisIslandEdge     : 1;
                  unsigned int  NeighbourWithAnotherIslandEdge  : 1;
                  unsigned int  NeighbourWithEnemyOnThisIsland  : 1;


                  unsigned int  CommandCentre                   : 1;
                  unsigned int  IslandEdge                      : 1;
                } TMyFieldInf;

#define MF_EnFieldsAround       0
#define MF_EnArmyAround         1
#define MF_EnStrongestArmy      2
#define MF_EnWeakestArmy        3
#define MF_EnWeakestArmySum     4

typedef         struct {
                  int           Island;                 // nr wyspy
                  int           Player;                 // kogo jest wyspa
                  int           Size;                   // wielkoœæ wyspy
                  int           EnemyFields;            // iloœæ wrogich pól na wyspie
                  int           EnemyArmy;              // iloœæ wrogich armii na wyspie (suma z pól)
                  int           MyFields;               // iloœc w³asnych pól na wyspie
                  int           MyArmy;                 // iloœæ w³asnej armii na wyspie (suma z pól)

                  int           MyArmyPercent;          // procent w³asnej armii na wyspie
                  int           MyFieldsPercent;        // procent w³asnych pól na wyspie

                  unsigned int  NeighbourWithEnemy      : 1; // jeœli ca³a wyspa jest opanowana to czy s¹siaduje z polem wroga
                  unsigned int  SecureIsland            : 1; // wyspa zabezpieczona, nie s¹siaduje z wrogiem, pola zaraz na zewn¹trz i wewn¹trz wyspy s¹ dobrze bronione jeœli s¹siaduj¹ z silnym wrogiem
                } TIslandInf;

#define I_MyFieldsPercent       0
#define I_MyArmyPercent         1

#define SCHMAX                  20

typedef         struct {
                  int           FieldsToAttack[48];     // pola, które mo¿na zaatakowaæ; od najsilniejszego
                  int           FieldsToAttackNum;      // ile ich jest
                  bool          IHaveMostFields;        // aktualny gracz ma najwiêcej pól
                  bool          IHaveStrongestArmy;     // ---- ma najsilniejsz¹ armiê
                  TScheme       Sch[SCHMAX];            // plan dzia³ania - wybierany przy rozdawaniu armii - losowo ( / logicznie) zmieniany ?
                  int           SchPar[SCHMAX];         // taki sobie parametr
                  int           SchNum;

                  TMyFieldInf   MyFields[48];           // informacje o w³asnych polach, posortowane wg si³y armii
                  int           MyFieldsNum;            // ile ich jest
                  int           SortedMyFields[5][48];  // posortowane numery w MyFields wg danego klucza (MF_xxx)
                  TIslandInf    Islands[8];             // pouk³adane numerami wysp
                  int           SortedIslands[2][8];    // wg klucza (I_xxx)

                  int           MyFieldsOnMap[48];      // indeksy w MyFields; mamy nr. pola -> dostajemy nr. w MyFields
                } TArtificialIntelligenceInf;

// zmienna TMove move musi byæ dostêpna                
#define RETURN_MOVE(s, d, p, m) {                       \
                                  move.SrcFld = s;      \
                                  move.DestFld = d;     \
                                  move.Par = p;         \
                                  move.Type = m;        \
                                  return move;          \
                                }

#define IS_PERCENT(p) (random(100) < p)

class TPlayer {
public:
        TPlayer();
        ~TPlayer();

        static void Init();

        static void Tick(DWORD time);

        static int GetPlayer();
        static int GetArmy(int player);
        static int GetFields(int player);
        static int GetWholeIslands(int player);
        static int GetWholeIslandsFields(int player);

        static void Calculate(); // wywo³uj po ka¿dym ruchu

        static void MakeMove(int fld1, int fld2, int par, bool wait_turn);

        static void Play();
        static TMove GetMove();
        static bool ContinueMove(TMove* move);
        static void ExecuteMove(TMove* move);
        static void Next();

        static void DrawWaitFields();

protected:
        static int CalcIncome(int player);

        static int GetPlayerFieldsOnIsland(int island, int player); // ile pól danej wyspy ma gracz
        static int GetPlayerOnIsland(int island); // który gracz ma wszystkie pola na danej wyspie; -1 == ¿aden

        static void Attack(TMove* move);

        static TMove Computer_GetMove();
        static bool Computer_ContinueMove(TMove* move);
        static void Computer_ExecuteMove(TMove* move);
        static void Computer_MoveArmy(TMove* move);
        static void Computer_PlaceArmy();

        static void InitWaitField(int fld);

        static void AI_Calculate();

#include "ai_functions.h"

        static int AI_fcalc_aux_GetEnemyFieldsAround(int p, int f);
        static int AI_fcalc_aux_GetEnemyArmyAround(int p, int f);
        static int AI_fcalc_aux_GetEnemyStrongestArmy(int p, int f);
        static int AI_fcalc_aux_GetEnemyWeakestArmy(int p, int f);
        static int AI_fcalc_aux_GetEnemyStrongestArmy_SFld(int p, int f);
        static int AI_fcalc_aux_GetEnemyStrongestArmy_WFld(int p, int f);
        static int AI_fcalc_aux_GetEnemyWeakestArmy_SFld(int p, int f);
        static int AI_fcalc_aux_GetEnemyWeakestArmy_WFld(int p, int f);
        static int AI_fcalc_aux_GetFieldsToEnemy(int p, int f);
        static bool AI_fcalc_aux_GetNeighbourWithPlayer(int p, int f, int enemy);
        static int AI_fcalc_aux_GetNeighbourWithEnemy(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithStrongestPlayer(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithWeakestPlayer(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithHumanPlayer(int p, int f);
        static int AI_fcalc_aux_GetNeighbourNeighbourWithEnemy(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithCommandCentre(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithEnemyWholeIsland(int p, int f);
        static int AI_fcalc_aux_GetOnWholeIsland(int p, int f);
        static int AI_fcalc_aux_GetOnlyFieldOnIsland(int p, int f);
        static int AI_fcalc_aux_GetOnlyFieldOnWholeIsland(int p, int f);
        static int AI_fcalc_aux_GetSecureField(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithThisIslandEdge(int p, int f);
        static int AI_fcalc_aux_GetNeighbourWithEnemyOnThisIsland(int p, int f);

        static int AI_icalc_aux_GetEnemyFields(int p, int isl);
        static int AI_icalc_aux_GetEnemyArmy(int p, int isl);
        static int AI_icalc_aux_GetMyFields(int p, int isl);
        static int AI_icalc_aux_GetMyArmy(int p, int isl);
        static int AI_icalc_aux_GetNeighbourWithEnemy(int p, int isl);
        static int AI_icalc_aux_GetSecureIsland(int p, int isl);

        static bool AI_aux_HasStrongestArmy(int p);
        static bool AI_aux_HasWeakestArmy(int p);
        static bool AI_aux_HasMostFields(int p);
        static bool AI_aux_HasFewestFields(int p);

        static bool AI_aux_IsEnemyInRange(int p, int f, int r); // czy na najbli¿szych r polach jest wróg; r == 1 -> s¹siedzi

        static bool AI_aux_IsStrongestArmyToAttack(int src, int dest);

        static bool AI_aux_CommonEnemy(int src, int dest);

protected:
        static int              Player;
        static int              Army[4];
        static int              Fields[4];
        static int              WholeIslands[4];
        static int              WholeIslandsFields[4];
        static int              IslandsSize[8];
        static TMove            UserMove;
        static TMove            Move;
        static TMove            RejectedMove;
        static DWORD            WaitTime;
        static bool             WaitFields[48];
        static bool             RequireNext;
        static TArtificialIntelligenceInf AI;

public:
        static TRanking         ArmyRanking[4];
        static TRanking         FieldsRanking[4];
        static int              Income[4];
        static bool             IsExecutingMove;

#ifdef DEBUG_MODE
        static void             DEBUG_write_AI();
        static void             DEBUG_write_move(TMove* m);
        static void             DEBUG_write_a(char* fun, int army, int p);
#endif
};



inline int TPlayer::GetPlayer()
{
        return Player;
}

inline int TPlayer::GetArmy(int player)
{
        return Army[player];
}

inline int TPlayer::GetFields(int player)
{
        return Fields[player];
}

inline int TPlayer::GetWholeIslands(int player)
{
        return WholeIslands[player];
}

inline int TPlayer::GetWholeIslandsFields(int player)
{
        return WholeIslandsFields[player];
}

inline void TPlayer::InitWaitField(int fld)
{
        WaitFields[fld] = true;
        WaitTime = 100;
}

