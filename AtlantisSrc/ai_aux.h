int TPlayer::AI_fcalc_aux_GetEnemyFieldsAround(int p, int f)
{
        int fields = 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
            fields++;
        }    

        return fields;
}

int TPlayer::AI_fcalc_aux_GetEnemyArmyAround(int p, int f)
{
        int army = 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
            army += Map[num].Army;
        }    

        return army;
}

int TPlayer::AI_fcalc_aux_GetEnemyStrongestArmy(int p, int f)
{
        int army = 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
          army = max(army, Map[num].Army);
        }  

        return army;
}

int TPlayer::AI_fcalc_aux_GetEnemyWeakestArmy(int p, int f)
{
        int army = MAXINT;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
          army = min(army, Map[num].Army);
        }  

        return army == MAXINT ? 0 : army;
}

int TPlayer::AI_fcalc_aux_GetEnemyStrongestArmy_SFld(int p, int f)
{
        int army = AI_fcalc_aux_GetEnemyStrongestArmy(p, f);

        if (army == 0)
          return 0;

        int army_sfld = 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p &&
              Map[num].Army == army)
            for (int num2, j = 0; j < 8; j++)
            {
              num2 = Map[num].Neighbours[j];
              if (num2 != -1 &&
                  Map[num2].Player != p)
                army_sfld = max(army_sfld, Map[num2].Army);
            }
        }

        return army_sfld;
}

int TPlayer::AI_fcalc_aux_GetEnemyStrongestArmy_WFld(int p, int f)
{
        int army = AI_fcalc_aux_GetEnemyWeakestArmy(p, f);

        if (army == 0)
          return 0;

        int army_sfld = 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p &&
              Map[num].Army == army)
            for (int num2, j = 0; j < 8; j++)
            {
              num2 = Map[num].Neighbours[j];
              if (num2 != -1 &&
                  Map[num2].Player != p)
                army_sfld = max(army_sfld, Map[num2].Army);
            }
        }

        return army_sfld == MAXINT ? 0 : army_sfld;
}

int TPlayer::AI_fcalc_aux_GetEnemyWeakestArmy_SFld(int p, int f)
{
        int army = AI_fcalc_aux_GetEnemyStrongestArmy(p, f);

        if (army == 0)
          return 0;

        int army_wfld = MAXINT;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p &&
              Map[num].Army == army)
            for (int num2, j = 0; j < 8; j++)
            {
              num2 = Map[num].Neighbours[j];
              if (num2 != -1 &&
                  Map[num2].Player != p)
                army_wfld = min(army_wfld, Map[num2].Army);
            }
        }

        return army_wfld == MAXINT ? 0 : army_wfld;
}

int TPlayer::AI_fcalc_aux_GetEnemyWeakestArmy_WFld(int p, int f)
{
        int army = AI_fcalc_aux_GetEnemyWeakestArmy(p, f);

        if (army == 0)
          return 0;

        int army_wfld = MAXINT;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p &&
              Map[num].Army == army)
            for (int num2, j = 0; j < 8; j++)
            {
              num2 = Map[num].Neighbours[j];
              if (num2 != -1 &&
                  Map[num2].Player != p)
                army_wfld = min(army_wfld, Map[num2].Army);
            }
        }

        return army_wfld == MAXINT ? 0 : army_wfld;
}

int TPlayer::AI_fcalc_aux_GetFieldsToEnemy(int p, int f)
{
        int arr[48];

        if (f == 0)
          arr[f] = 0;

        for (int i = 0; i < 48; i++)
          arr[i] = 0;
        arr[f] = 1;

        for (int fields = 1; ; fields++)
        {
          for (int i = 0; i < 48; i++)
          if (arr[i] == 1)
            for (int num, j = 0; j < 8; j++)
            {
              num = Map[i].Neighbours[j];
              if (num != -1 &&
                  arr[num] != 1)
                arr[num] = 2;
            }  

          for (int i = 0; i < 48; i++)
          if (arr[i] == 2)
            arr[i] = 1;

          for (int i = 0; i < 48; i++)
          if (arr[i] == 1 &&
              Map[i].Player != p)
            return fields;

          // sprawdŸ czy nie sprawdzi³eœ ju¿ ca³ej mapy
          for (int i = 0; i < 49; i++) // nie 48 !!!
          {
            if (i == 48)
              return 0;
            if (arr[i] == 0)
              break;
          }
        }
}

bool TPlayer::AI_fcalc_aux_GetNeighbourWithPlayer(int, int f, int enemy)
{
        for (int fld, i = 0; i < 8; i++)
        {
          fld = Map[f].Neighbours[i];
          if (fld != -1 &&
              Map[fld].Player == enemy)
            return true;
        }
        
        return false;    
}            

int TPlayer::AI_fcalc_aux_GetNeighbourWithEnemy(int p, int f)
{
        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
            return 1;
        }

        return 0;
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithStrongestPlayer(int p, int f)
{
        int sp = ArmyRanking[0].Player;

        if (!AI_aux_HasStrongestArmy(sp) ||
            sp == p)
          return 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player == sp)
            return 1;
        }

        return 0;
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithWeakestPlayer(int p, int f)
{
        int wp = ArmyRanking[3].Player;

        if (!AI_aux_HasWeakestArmy(wp) ||
            wp == p)
          return 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player == wp)
            return 1;
        }

        return 0;
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithHumanPlayer(int, int f)
{
        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player == 0)
            return 1;
        }

        return 0;
}

int TPlayer::AI_fcalc_aux_GetNeighbourNeighbourWithEnemy(int p, int f)
{
        return AI_aux_IsEnemyInRange(p, f, 2);
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithCommandCentre(int, int f)
{
        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].CommandCentre)
            return 1;
        }

        return 0;
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithEnemyWholeIsland(int p, int f)
{
        if (Map[f].IslandEdge)
        for (int num, pisl, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p &&
              Map[num].Island != Map[f].Island)
          {
            pisl = GetPlayerOnIsland(Map[num].Island);
            if (pisl != -1 &&
                pisl != p)
              return 1;
          }
        }

        return 0;
}

int TPlayer::AI_fcalc_aux_GetOnWholeIsland(int p, int f)
{
        return GetPlayerOnIsland(Map[f].Island) == p ? 1 : 0;
}

int TPlayer::AI_fcalc_aux_GetOnlyFieldOnIsland(int p, int f)
{
        return GetPlayerFieldsOnIsland(Map[f].Island, p) == 1 ? 1 : 0;
}        

int TPlayer::AI_fcalc_aux_GetOnlyFieldOnWholeIsland(int p, int f)
{
        int island = Map[f].Island;

        if (GetPlayerFieldsOnIsland(island, p) != 1)
          return 0;

        int pisl = -1;  

        for (int i = 0; i < 48; i++)
        if (Map[i].Island == island)
          if (pisl == -1)
          {
            if (Map[i].Player != p)
              pisl = Map[i].Player;
          }
          else
            if (pisl != Map[i].Player)
              return 0;

        return 1;
}

int TPlayer::AI_fcalc_aux_GetSecureField(int p, int f)
{
        if (Map[f].Army == 1)
          return 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Army == 1)
            return 0;
        }

        return !AI_aux_IsEnemyInRange(p, f, 3);        
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithThisIslandEdge(int, int f)
{
        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].IslandEdge &&
              Map[num].Island == Map[f].Island)
            return 1;
        }

        return 0;
}

int TPlayer::AI_icalc_aux_GetEnemyFields(int p, int isl)
{
        int fields = 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Island == isl &&
            Map[i].Player != p)
          fields++;

        return fields;
}

int TPlayer::AI_icalc_aux_GetEnemyArmy(int p, int isl)
{
        int army = 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Island == isl &&
            Map[i].Player != p)
          army += Map[i].Army;

        return army;
}

int TPlayer::AI_icalc_aux_GetMyFields(int p, int isl)
{
        return IslandsSize[isl] - AI_icalc_aux_GetEnemyFields(p, isl);
}

int TPlayer::AI_icalc_aux_GetMyArmy(int p, int isl)
{
        int army = 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Island == isl &&
            Map[i].Player == p)
          army += Map[i].Army;

        return army;
}

int TPlayer::AI_icalc_aux_GetNeighbourWithEnemy(int p, int isl)
{
        if (GetPlayerOnIsland(isl) != p)
          return 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Island == isl &&
            Map[i].IslandEdge)
          if (AI_aux_IsEnemyInRange(p, i, 1))
            return 1;

        return 0;
}

int TPlayer::AI_icalc_aux_GetSecureIsland(int p, int isl)
{
        if (GetPlayerOnIsland(isl) != p)
          return 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Island == isl &&
            Map[i].IslandEdge)
        {
          if (Map[i].Army <= 2)
            return 0;

          for (int num, j = 0; j < 8; j++)
          {
            num = Map[i].Neighbours[j];
            if (num != -1 &&
                Map[num].IslandEdge &&
                Map[num].Island != isl)
              if (Map[num].Player != p) // jeœli wyspa s¹siaduje z wrogiem
              {
                if (Map[num].Army > Map[i].Army)
                  return 0;

                if (Map[i].Army <= 6 &&
                    Map[num].Army >= 3)
                  return 0;

                int enemy_income = Income[Map[num].Player];

                if (Map[i].Army > 6 &&
                    Map[i].Army < Map[num].Army + enemy_income / 2)
                  return 0;
              }
              else // jeœli nie s¹siaduje
              {
                if (!AI_fcalc_aux_GetNeighbourWithEnemy(p, num) &&
                    Map[num].Army < 4)
                  return 0;

                int strongest_army = AI_fcalc_aux_GetEnemyStrongestArmy(p, num);

                if (Map[num].Army < strongest_army + 1)
                  return 0;
              }
          }
        }

        return 1;
}

bool TPlayer::AI_aux_HasStrongestArmy(int p)
{
        int pnum = 0;
        for (int i = 0; i < 4; i++)
        if (Fields[i] > 0)
          pnum++;

        return Army[p] == ArmyRanking[0].Army &&
               Army[p] != ArmyRanking[pnum - 1].Army;
}

bool TPlayer::AI_aux_HasWeakestArmy(int p)
{
        int pnum = 0;
        for (int i = 0; i < 4; i++)
        if (Fields[i] > 0)
          pnum++;

        return Army[p] == ArmyRanking[pnum - 1].Army &&
               Army[p] != ArmyRanking[0].Army;
}

bool TPlayer::AI_aux_HasMostFields(int p)
{
        int pnum = 0;
        for (int i = 0; i < 4; i++)
        if (Fields[i] > 0)
          pnum++;

        return Fields[p] == FieldsRanking[0].Fields &&
               Fields[p] != FieldsRanking[pnum - 1].Fields;
}

bool TPlayer::AI_aux_HasFewestFields(int p)
{
        int pnum = 0;
        for (int i = 0; i < 4; i++)
        if (Fields[i] > 0)
          pnum++;

        return Fields[p] == FieldsRanking[pnum - 1].Fields &&
               Fields[p] != FieldsRanking[0].Fields;
}

bool TPlayer::AI_aux_IsEnemyInRange(int p, int f, int r) // popr !!!!!!!!!!!!
{
/*        bool arr[48];

        for (int i = 0; i < 48; i++)
          arr[i] = false;
        arr[f] = true;

        for (int i = 0; i < r; i++)
        if (arr[i])
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[i].Neighbours[j];
            if (num != -1)
              arr[num] = true;
          }

        for (int i = 0; i < 48; i++)
        if (arr[i] &&
            Map[i].Player != p)
          return false;

        return true;*/

        return AI_fcalc_aux_GetFieldsToEnemy(p, f) <= r;
}

int TPlayer::AI_fcalc_aux_GetNeighbourWithEnemyOnThisIsland(int p, int f)
{
        for (int num, i = 0; i < 8; i++)
        {
          num = Map[f].Neighbours[i];
          if (num != -1 &&
              Map[num].Island == Map[f].Island &&
              Map[num].Player != p)
            return 1;
        }

        return 0;
}

bool TPlayer::AI_aux_IsStrongestArmyToAttack(int src, int dest)
{
        int p = Map[src].Player;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[dest].Neighbours[i];
          if (num != -1 &&
              Map[num].Player == p &&
              Map[num].Army > Map[src].Army)
            return false;
        }

        return true;
}

bool TPlayer::AI_aux_CommonEnemy(int src, int dest)
{
        int p = Map[src].Player;
        int es = 0, ed = 0;

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[src].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
            es++;
          num = Map[dest].Neighbours[i];
          if (num != -1 &&
              Map[num].Player != p)
            ed++;
        }

        if (ed == 1 && es == 1)
        // sprawdŸ czy wrogie pole jest s¹siadem obu pól
        for (int enemy, i = 0; i < 8; i++)
        {
          enemy = Map[src].Neighbours[i];
          if (enemy != -1 &&
              Map[enemy].Player != p)
          for (int j = 0; j < 8; j++)
          if (Map[enemy].Neighbours[j] == dest)
            return true;
        }

        return false;
}
