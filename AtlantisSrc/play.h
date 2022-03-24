
enum TMoveType {
        M_NOMOVE        = 0,
        M_MOVEARMY      = 1,
        M_ATTACK        = 2,
        M_WAITTURN      = 3 };

typedef         struct {
                  int           SrcFld;
                  int           DestFld;
                  int           Par;            // taki sobie parametr, np. ile wojska przenie��
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
        SCH_ATTACKPLAYER        = 3, // SchPar = gracz, kt�rego atakowa�
        SCH_DEFENDISLANDS       = 4, // bro� swoich wysp
        SCH_ATTACK              = 5, // atakuj najs�absze pola
        SCH_CAPTURECC           = 6, // atakuj cc
        SCH_DEFENDCC            = 7, // bro� cc
        SCH_DEPRIVEISLAND       = 8 }; // pozbaw kogo� wyspy, SchPar = nr wyspy

typedef         struct {
                  int           Field;                          // nr pola
                  int           EnemyFieldsAround;              // ile wrogich p�l jest woko�o
                  int           EnemyArmyAround;                // jaka wroga armia jest woko�o (suma armi z poszczeg�lnych p�l)
                  int           EnemyStrongestArmy;             // najsilniejsza armia s�siaduj�ca z polem
                  int           EnemyWeakestArmy;               // najs�absza ----
                  int           EnemyStrongestArmy_SFld;        // najsilniejsza armia s�siaduj�ca z polem, kt�re ma najsilniejsz� armi�
                  int           EnemyStrongestArmy_WFld;        // ----, kt�re ma najs�absz� armi�
                  int           EnemyWeakestArmy_SFld;          // najs�absza armia s�siaduj�ca z polem, kt�re ma najsilniejsz� armi�
                  int           EnemyWeakestArmy_WFld;          // ----, kt�re ma najs�absz� armi�
                  int           EnemyWeakestArmySum;            // EnemyWeakestArmy + EnemyWeakestArmy_W;
                  int           FieldsToEnemy;                  // odleg�o�� od najbli�szego wroga
                  int           MinNeighbourFieldsToEnemy;      // najmniejsza odleg�o�� dowolnego s�siedniego pola od wroga
                  bool          NeighbourWithPlayer[4];

                  unsigned int  NeighbourWithEnemy              : 1; // pole s�siaduje z wrogiem
                  unsigned int  NeighbourWithStrongestPlayer    : 1; // pole s�siaduje z najsilniejszym graczem
                  unsigned int  NeighbourWithWeakestPlayer      : 1; // ---- z najs�abszym ----
                  unsigned int  NeighbourWithHumanPlayer        : 1; // ---- z polem cz�owieka
                  unsigned int  NeighbourNeighbourWithEnemy     : 1; // jakie� pole s�siaduj�ce s�siaduje z wrogiem
                  unsigned int  NeighbourWithCommandCentre      : 1; // s�siaduje z centr. dow.
                  unsigned int  NeighbourWithEnemyWholeIsland   : 1; // s�siaduje z polem, kt�re nale�y do wyspy zaj�tej w ca�o�ci przez wroga
                  unsigned int  OnWholeIsland                   : 1; // pole le�y na wyspie, kt�ra jest ca�a zaj�ta
                  unsigned int  OnlyFieldOnIsland               : 1; // jedyne pole na wyspie
                  unsigned int  OnlyFieldOnWholeIsland          : 1; // jedyne pole na wyspie, kt�rej wszystkie inne pola s� zaj�te przez jednego gracza
                  unsigned int  SecureField                     : 1; // pola nie trzeba broni�, pola wok� maj� Army >= 2, najbli�sze wrogie pole nie bli�ej ni� w odleg�o�ci 3 p�l (Pole - P - P - Wr�g)

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
                  int           Size;                   // wielko�� wyspy
                  int           EnemyFields;            // ilo�� wrogich p�l na wyspie
                  int           EnemyArmy;              // ilo�� wrogich armii na wyspie (suma z p�l)
                  int           MyFields;               // ilo�c w�asnych p�l na wyspie
                  int           MyArmy;                 // ilo�� w�asnej armii na wyspie (suma z p�l)

                  int           MyArmyPercent;          // procent w�asnej armii na wyspie
                  int           MyFieldsPercent;        // procent w�asnych p�l na wyspie

                  unsigned int  NeighbourWithEnemy      : 1; // je�li ca�a wyspa jest opanowana to czy s�siaduje z polem wroga
                  unsigned int  SecureIsland            : 1; // wyspa zabezpieczona, nie s�siaduje z wrogiem, pola zaraz na zewn�trz i wewn�trz wyspy s� dobrze bronione je�li s�siaduj� z silnym wrogiem
                } TIslandInf;

#define I_MyFieldsPercent       0
#define I_MyArmyPercent         1

#define SCHMAX                  20

typedef         struct {
                  int           FieldsToAttack[48];     // pola, kt�re mo�na zaatakowa�; od najsilniejszego
                  int           FieldsToAttackNum;      // ile ich jest
                  bool          IHaveMostFields;        // aktualny gracz ma najwi�cej p�l
                  bool          IHaveStrongestArmy;     // ---- ma najsilniejsz� armi�
                  TScheme       Sch[SCHMAX];            // plan dzia�ania - wybierany przy rozdawaniu armii - losowo ( / logicznie) zmieniany ?
                  int           SchPar[SCHMAX];         // taki sobie parametr
                  int           SchNum;

                  TMyFieldInf   MyFields[48];           // informacje o w�asnych polach, posortowane wg si�y armii
                  int           MyFieldsNum;            // ile ich jest
                  int           SortedMyFields[5][48];  // posortowane numery w MyFields wg danego klucza (MF_xxx)
                  TIslandInf    Islands[8];             // pouk�adane numerami wysp
                  int           SortedIslands[2][8];    // wg klucza (I_xxx)

                  int           MyFieldsOnMap[48];      // indeksy w MyFields; mamy nr. pola -> dostajemy nr. w MyFields
                } TArtificialIntelligenceInf;

// zmienna TMove move musi by� dost�pna                
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

        static void Calculate(); // wywo�uj po ka�dym ruchu

        static void MakeMove(int fld1, int fld2, int par, bool wait_turn);

        static void Play();
        static TMove GetMove();
        static bool ContinueMove(TMove* move);
        static void ExecuteMove(TMove* move);
        static void Next();

        static void DrawWaitFields();

protected:
        static int CalcIncome(int player);

        static int GetPlayerFieldsOnIsland(int island, int player); // ile p�l danej wyspy ma gracz
        static int GetPlayerOnIsland(int island); // kt�ry gracz ma wszystkie pola na danej wyspie; -1 == �aden

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

        static bool AI_aux_IsEnemyInRange(int p, int f, int r); // czy na najbli�szych r polach jest wr�g; r == 1 -> s�siedzi

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

