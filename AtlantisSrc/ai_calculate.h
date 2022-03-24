void TPlayer::AI_Calculate()
{
        // *** Fields To Attack
        // *** Fields To Attack Num

        AI.FieldsToAttackNum = 0;

        for (int n, i = 0; i < 48; i++)
        if (Map[i].Player != Player)
          for (int j = 0; j < 8; j++)
          {
            n = Map[i].Neighbours[j];
            if (n != -1 &&
                Map[n].Player == Player &&
                Map[n].Army > Map[i].Army)
            {
              AI.FieldsToAttack[AI.FieldsToAttackNum++] = i;
              break;
            }
          }

        // posortuj wg si³y armi na polu
        
        for (int tmp, i = 0; i < AI.FieldsToAttackNum; i++)
          for (int j = i + 1; j < AI.FieldsToAttackNum; j++)
          if (Map[AI.FieldsToAttack[j]].Army > Map[AI.FieldsToAttack[i]].Army)
            tmp = AI.FieldsToAttack[j],
            AI.FieldsToAttack[j] = AI.FieldsToAttack[i],
            AI.FieldsToAttack[i] = tmp;

        // *** I Have Most Fields

        AI.IHaveMostFields = AI_aux_HasMostFields(Player);

        // *** I Have Strongest Army

        AI.IHaveStrongestArmy = AI_aux_HasStrongestArmy(Player);

        // ***************
        // *** Islands ***
        // ***************

        for (int isl = 0; isl < 8; isl++)
        {
          // *** Island

          AI.Islands[isl].Island = isl;

          // *** Player

          AI.Islands[isl].Player = GetPlayerOnIsland(isl);

          // *** Size

          AI.Islands[isl].Size = IslandsSize[isl];

          // *** Enemy Fields

          AI.Islands[isl].EnemyFields = AI_icalc_aux_GetEnemyFields(Player, isl);

          // *** Enemy Army

          AI.Islands[isl].EnemyArmy = AI_icalc_aux_GetEnemyArmy(Player, isl);

          // *** My Fields

          AI.Islands[isl].MyFields = AI_icalc_aux_GetMyFields(Player, isl);

          // *** My Army

          AI.Islands[isl].MyArmy = AI_icalc_aux_GetMyArmy(Player, isl);

          // *** My Army Percent

          AI.Islands[isl].MyArmyPercent = 100 * AI.Islands[isl].MyArmy /
                                          (AI.Islands[isl].MyArmy + AI.Islands[isl].EnemyArmy);

          // *** My Fields Percent

          AI.Islands[isl].MyFieldsPercent = 100 * AI.Islands[isl].MyFields /
                                            (AI.Islands[isl].MyFields + AI.Islands[isl].EnemyFields);

          // *** Neighbour With Enemy

          AI.Islands[isl].NeighbourWithEnemy = AI_icalc_aux_GetNeighbourWithEnemy(Player, isl);

          // *** Secure Island

          AI.Islands[isl].SecureIsland = AI_icalc_aux_GetSecureIsland(Player, isl);
        }

        // ****************
        // *** MyFields ***
        // ****************

        // *** My Fields .Field
        // *** My Fields Num

        AI.MyFieldsNum = 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Player == Player)
          AI.MyFields[AI.MyFieldsNum++].Field = i;

        // reszta

        for (int map_fld, fld = 0; fld < AI.MyFieldsNum; fld++)
        {
          map_fld = AI.MyFields[fld].Field;

          // *** Enemy Fields Around

          AI.MyFields[fld].EnemyFieldsAround =
                AI_fcalc_aux_GetEnemyFieldsAround(Player, map_fld);

          // *** Enemy Army Around

          AI.MyFields[fld].EnemyArmyAround =
                AI_fcalc_aux_GetEnemyArmyAround(Player, map_fld);

          // *** Enemy Strongest Army

          AI.MyFields[fld].EnemyStrongestArmy =
                AI_fcalc_aux_GetEnemyStrongestArmy(Player, map_fld);

          // *** Enemy Weakest Army

          AI.MyFields[fld].EnemyWeakestArmy =
                AI_fcalc_aux_GetEnemyWeakestArmy(Player, map_fld);

          // *** Enemy Strongest Army _SFld

          AI.MyFields[fld].EnemyStrongestArmy_SFld =
                AI_fcalc_aux_GetEnemyStrongestArmy_SFld(Player, map_fld);

          // *** Enemy Strongest Army _WFld

          AI.MyFields[fld].EnemyStrongestArmy_WFld =
                AI_fcalc_aux_GetEnemyStrongestArmy_WFld(Player, map_fld);

          // *** Enemy Weakest Army _SFld

          AI.MyFields[fld].EnemyWeakestArmy_SFld =
                AI_fcalc_aux_GetEnemyWeakestArmy_SFld(Player, map_fld);

          // *** Enemy Weakest Army _WFld

          AI.MyFields[fld].EnemyWeakestArmy_WFld =
                AI_fcalc_aux_GetEnemyWeakestArmy_WFld(Player, map_fld);

          // *** Enemy Weakest Army Sum

          AI.MyFields[fld].EnemyWeakestArmySum =
                AI.MyFields[fld].EnemyWeakestArmy + AI.MyFields[fld].EnemyWeakestArmy_WFld;

          // *** Fields To Enemy

          AI.MyFields[fld].FieldsToEnemy = AI_fcalc_aux_GetFieldsToEnemy(Player, map_fld);

          // *** Neighbour With Player [4]

          for (int i = 0; i < 4; i++)
            AI.MyFields[fld].NeighbourWithPlayer[i] =
                AI_fcalc_aux_GetNeighbourWithPlayer(Player, map_fld, i);

          // *** Neighbour With Enemy

          AI.MyFields[fld].NeighbourWithEnemy =
                AI_fcalc_aux_GetNeighbourWithEnemy(Player, map_fld);

          // *** Neighbour With Strongest Player

          AI.MyFields[fld].NeighbourWithStrongestPlayer =
                AI_fcalc_aux_GetNeighbourWithStrongestPlayer(Player, map_fld);

          // *** Neighbour With Weakest Player

          AI.MyFields[fld].NeighbourWithWeakestPlayer =
                AI_fcalc_aux_GetNeighbourWithWeakestPlayer(Player, map_fld);

          // *** Neighbour With Human Player

          AI.MyFields[fld].NeighbourWithHumanPlayer =
                AI_fcalc_aux_GetNeighbourWithHumanPlayer(Player, map_fld);

          // *** Neighbour Neighbour With Enemy

          AI.MyFields[fld].NeighbourNeighbourWithEnemy =
                AI_fcalc_aux_GetNeighbourNeighbourWithEnemy(Player, map_fld);

          // *** Neighbour With Command Centre

          AI.MyFields[fld].NeighbourWithCommandCentre =
                AI_fcalc_aux_GetNeighbourWithCommandCentre(Player, map_fld);

          // *** Neighbour With Enemy Whole Island

          AI.MyFields[fld].NeighbourWithEnemyWholeIsland =
                AI_fcalc_aux_GetNeighbourWithEnemyWholeIsland(Player, map_fld);

          // *** On Whole Island

          AI.MyFields[fld].OnWholeIsland =
                AI_fcalc_aux_GetOnWholeIsland(Player, map_fld);

          // *** Only Field On Island

          AI.MyFields[fld].OnlyFieldOnIsland =
                AI_fcalc_aux_GetOnlyFieldOnIsland(Player, map_fld);

          // *** Only Field On Whole Island

          AI.MyFields[fld].OnlyFieldOnWholeIsland =
                AI_fcalc_aux_GetOnlyFieldOnWholeIsland(Player, map_fld);

          // *** Secure Field

          AI.MyFields[fld].SecureField =
                AI_fcalc_aux_GetSecureField(Player, map_fld);

          // *** Neighbour With This Island Edge

          AI.MyFields[fld].NeighbourWithThisIslandEdge =
                AI_fcalc_aux_GetNeighbourWithThisIslandEdge(Player, map_fld);

          // *** Neighbour With Another Island Edge

          AI.MyFields[fld].NeighbourWithAnotherIslandEdge =
                Map[map_fld].IslandEdge;

          // *** Neighbour With Enemy On This Island

          AI.MyFields[fld].NeighbourWithEnemyOnThisIsland =
                AI_fcalc_aux_GetNeighbourWithEnemyOnThisIsland(Player, map_fld);

          // *** Command Centre

          AI.MyFields[fld].CommandCentre =
                Map[map_fld].CommandCentre;

          // *** Island Edge
          AI.MyFields[fld].IslandEdge =
                Map[map_fld].IslandEdge;
        }

        // *** My Fields On Map

        for (int i = 0; i < 48; i++)
        {
          AI.MyFieldsOnMap[i] = -1;

          for (int j = 0; j < AI.MyFieldsNum; j++)
          if (AI.MyFields[j].Field == i)
            AI.MyFieldsOnMap[i] = j;
        }

        // *** MyFields . Min Neighbour Fields To Enemy
        
        for (int map_fld, fld = 0; fld < AI.MyFieldsNum; fld++)
        {
          map_fld = AI.MyFields[fld].Field;
          
          int minf = MAXINT;

          for (int num, i = 0; i < 8; i++)
          {
            num = Map[map_fld].Neighbours[i];
            if (num != -1 &&
                Map[num].Player == Player)
              minf = min(minf, AI.MyFields[AI.MyFieldsOnMap[num]].FieldsToEnemy);
          }

          AI.MyFields[fld].MinNeighbourFieldsToEnemy = minf;
        }  

        // *******************************
        // *** Posortuj SortedMyFields ***
        // *******************************

        for (int key = 0; key < 5; key++)
          for (int i = 0; i < AI.MyFieldsNum; i++)
            AI.SortedMyFields[key][i] = /*AI.MyFields[i].Field;*/i;

        // wg MF_EnFieldsAround

        for (int tmp, i = 0; i < AI.MyFieldsNum; i++)
          for (int j = i + 1; j < AI.MyFieldsNum; j++)
          if (AI.MyFields[AI.SortedMyFields[MF_EnFieldsAround][j]].EnemyFieldsAround >
              AI.MyFields[AI.SortedMyFields[MF_EnFieldsAround][i]].EnemyFieldsAround)
            tmp = AI.SortedMyFields[MF_EnFieldsAround][j],
            AI.SortedMyFields[MF_EnFieldsAround][j] = AI.SortedMyFields[MF_EnFieldsAround][i],
            AI.SortedMyFields[MF_EnFieldsAround][i] = tmp;

        // wg MF_EnArmyAround

        for (int tmp, i = 0; i < AI.MyFieldsNum; i++)
          for (int j = i + 1; j < AI.MyFieldsNum; j++)
          if (AI.MyFields[AI.SortedMyFields[MF_EnArmyAround][j]].EnemyArmyAround >
              AI.MyFields[AI.SortedMyFields[MF_EnArmyAround][i]].EnemyArmyAround)
            tmp = AI.SortedMyFields[MF_EnArmyAround][j],
            AI.SortedMyFields[MF_EnArmyAround][j] = AI.SortedMyFields[MF_EnArmyAround][i],
            AI.SortedMyFields[MF_EnArmyAround][i] = tmp;

        // wg MF_EnStrongestArmy

        for (int tmp, i = 0; i < AI.MyFieldsNum; i++)
          for (int j = i + 1; j < AI.MyFieldsNum; j++)
          if (AI.MyFields[AI.SortedMyFields[MF_EnStrongestArmy][j]].EnemyStrongestArmy >
              AI.MyFields[AI.SortedMyFields[MF_EnStrongestArmy][i]].EnemyStrongestArmy)
            tmp = AI.SortedMyFields[MF_EnStrongestArmy][j],
            AI.SortedMyFields[MF_EnStrongestArmy][j] = AI.SortedMyFields[MF_EnStrongestArmy][i],
            AI.SortedMyFields[MF_EnStrongestArmy][i] = tmp;

        // wg MF_EnWeakestArmy

        for (int tmp, i = 0; i < AI.MyFieldsNum; i++)
          for (int j = i + 1; j < AI.MyFieldsNum; j++)
          if (AI.MyFields[AI.SortedMyFields[MF_EnWeakestArmy][j]].EnemyWeakestArmy >
              AI.MyFields[AI.SortedMyFields[MF_EnWeakestArmy][i]].EnemyWeakestArmy)
            tmp = AI.SortedMyFields[MF_EnWeakestArmy][j],
            AI.SortedMyFields[MF_EnWeakestArmy][j] = AI.SortedMyFields[MF_EnWeakestArmy][i],
            AI.SortedMyFields[MF_EnWeakestArmy][i] = tmp;

        // wg MF_EnWeakestArmySum
        
        for (int tmp, i = 0; i < AI.MyFieldsNum; i++)
          for (int j = i + 1; j < AI.MyFieldsNum; j++)
          if (AI.MyFields[AI.SortedMyFields[MF_EnWeakestArmySum][j]].EnemyWeakestArmySum >
              AI.MyFields[AI.SortedMyFields[MF_EnWeakestArmySum][i]].EnemyWeakestArmySum)
            tmp = AI.SortedMyFields[MF_EnWeakestArmySum][j],
            AI.SortedMyFields[MF_EnWeakestArmySum][j] = AI.SortedMyFields[MF_EnWeakestArmySum][i],
            AI.SortedMyFields[MF_EnWeakestArmySum][i] = tmp;

        // ******************************
        // *** Posortuj SortedIslands ***
        // ******************************

        for (int isl = 0; isl < 2; isl++)
          for (int i = 0; i < 8; i++)
            AI.SortedIslands[isl][i] = AI.Islands[i].Island;

        // wg I_MyFieldsPercent

        for (int tmp, i = 0; i < 8; i++)
          for (int j = i + 1; j < 8; j++)
          if (AI.Islands[AI.SortedIslands[I_MyFieldsPercent][j]].MyFieldsPercent >
              AI.Islands[AI.SortedIslands[I_MyFieldsPercent][i]].MyFieldsPercent)
            tmp = AI.SortedIslands[I_MyFieldsPercent][j],
            AI.SortedIslands[I_MyFieldsPercent][j] = AI.SortedIslands[I_MyFieldsPercent][i],
            AI.SortedIslands[I_MyFieldsPercent][i] = tmp;

        // wg I_MyArmyPercent

        for (int tmp, i = 0; i < 8; i++)
          for (int j = i + 1; j < 8; j++)
          if (AI.Islands[AI.SortedIslands[I_MyArmyPercent][j]].MyArmyPercent >
              AI.Islands[AI.SortedIslands[I_MyArmyPercent][i]].MyArmyPercent)
            tmp = AI.SortedIslands[I_MyArmyPercent][j],
            AI.SortedIslands[I_MyArmyPercent][j] = AI.SortedIslands[I_MyArmyPercent][i],
            AI.SortedIslands[I_MyArmyPercent][i] = tmp;

#ifdef DEBUG_MODE
        DEBUG_write_AI();
#endif
}
