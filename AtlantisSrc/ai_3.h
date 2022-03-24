// ************************************************************************************************
// AI *** DiffLevel == 3 (4) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_3_GetMove()
{
        TMove move;

        int src, dest;

        // ****************************************************************************************
        // *** sprawdŸ czy mo¿na zaatakowaæ pole na tej samej wyspie

        for (int i = 0; i < AI.FieldsToAttackNum; i++)
        {
          dest = AI.FieldsToAttack[i];
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[dest].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player &&
                Map[num].Army > Map[dest].Army &&
                Map[num].Army > 3 &&
                Map[num].Island == Map[dest].Island)
            {
              src = num;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ****************************************************************************************
        // *** sprawdŸ czy mo¿na zaatakowaæ brzeg wyspy

        for (int i = 0; i < AI.FieldsToAttackNum; i++)
        {
          dest = AI.FieldsToAttack[i];
          if (Map[dest].IslandEdge)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[dest].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player &&
                Map[num].Army > Map[dest].Army &&
                Map[num].Army > 2)
            {
              src = num;
              if (IS_PERCENT(80))
                if (Map[dest].Island == Map[src].Island)
                  continue;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ****************************************************************************************
        // *** sprawdŸ czy mo¿na zaatakowaæ kogoœ 3x s³abszego

        for (int i = 0; i < AI.FieldsToAttackNum; i++)
        {
          dest = AI.FieldsToAttack[i];
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[dest].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player &&
                Map[num].Army >= 3 * Map[dest].Army &&
                Map[num].Army >= 6)
            {
              src = num;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ****************************************************************************************
        // *** sprawdŸ czy mo¿na zaatakowaæ centr. dow.

        for (int i = 0; i < AI.FieldsToAttackNum; i++)
        {
          dest = AI.FieldsToAttack[i];
          if (Map[dest].CommandCentre)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[dest].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player &&
                Map[num].Army > Map[dest].Army &&
                Map[num].Army > 2)
            {
              src = num;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ***************************************************************************************
        // *** jeœli mo¿na to przenieœ wojsko bli¿ej frontu

        if (IS_PERCENT(70) ||
            AI.FieldsToAttackNum == 0)
        for (int fld, i = 0; i < AI.MyFieldsNum; i++)
        if (!AI.MyFields[i].NeighbourWithEnemy)
        {
          fld = AI.MyFields[i].Field;
          if (Map[fld].Army > 2)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[fld].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player)
              if (AI.MyFields[i].MinNeighbourFieldsToEnemy ==
                          AI.MyFields[AI.MyFieldsOnMap[num]].FieldsToEnemy)
                RETURN_MOVE(fld,
                            num,
                            Map[fld].Army - ((IS_PERCENT(50) ? 1 : 2)),
                            M_MOVEARMY);
          }
        }

        // ****************************************************************************************
        // *** nic nie rób jeœli nie mo¿esz atakowaæ

        if (AI.FieldsToAttackNum == 0)
          RETURN_MOVE(-1, -1, -1, M_WAITTURN)

        // ****************************************************************************************
        // *** zaatakuj losowe pole

        for (int i = 0; i < 10; i++)
        {
          dest = AI.FieldsToAttack[random(AI.FieldsToAttackNum)];
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[dest].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player &&
                Map[num].Army > Map[dest].Army &&
                Map[num].Army > 3)
            {
              src = num;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ****************************************************************************************
        // *** !!!

        RETURN_MOVE(-1, -1, -1, M_WAITTURN)
}

bool TPlayer::AI_3_ContinueMove(TMove* move)
{
        if (move->Type == M_MOVEARMY ||
            move->Type == M_WAITTURN)
          return true;

        if (move->Type == M_ATTACK)
        {
          if (Map[move->SrcFld].Army == 2)
            return false;

          if (Map[move->SrcFld].Army <= 6 &&
              !AI.MyFields[AI.MyFieldsOnMap[move->SrcFld]].SecureField &&
              Map[move->SrcFld].CommandCentre)
            return false;
        }
        
        return true;    
}

void TPlayer::AI_3_MoveArmy(TMove* move)
{
        int army = Map[move->SrcFld].Army;

        int mysrc = AI.MyFieldsOnMap[move->SrcFld];
        int mydest = AI.MyFieldsOnMap[move->DestFld];

        int asrc, adest;

        if (AI.MyFields[mysrc].NeighbourWithEnemy &&
            !AI.MyFields[mydest].NeighbourWithEnemy &&
            !AI.MyFields[mydest].IslandEdge)
        {
          asrc = army - 1;
          adest = army - asrc;
          Map[move->SrcFld].Army = asrc;
          Map[move->DestFld].Army = adest;
          return;
        }

        if (AI.MyFields[mysrc].NeighbourWithEnemy &&
            AI.MyFields[mydest].NeighbourWithEnemy)
        {
          asrc = army / 2;
          adest = army - asrc;
          Map[move->SrcFld].Army = asrc;
          Map[move->DestFld].Army = adest;
          return;
        }

        if (AI.MyFields[mysrc].CommandCentre)
          if (AI.MyFields[mysrc].NeighbourNeighbourWithEnemy)
          {
            if (army > 12)
              asrc = army / 4;
            else
              asrc = army > 4 ? army - 4 : army - 1;
            adest = army - asrc;
            Map[move->SrcFld].Army = asrc;
            Map[move->DestFld].Army = adest;
            return;
          }
          else
          {
            asrc = army > 4 ? army - 4 : army - 1;
            adest = army - adest;
            Map[move->SrcFld].Army = asrc;
            Map[move->DestFld].Army = adest;
            return;
          }

        asrc = 1;
        adest = army - asrc;

        Map[move->SrcFld].Army = asrc;
        Map[move->DestFld].Army = adest;
}

void TPlayer::AI_3_PlaceArmy()
{
        int myfld, fld, numa, max, army = Income[Player];

        // ****************************************************************************************
        // *** jeœli wróg ma tylko jedno pole to stawiaj ca³¹ armiê obok tego pola na swoim
        // *** najlepszym polu

        if (Fields[Player] == 47)
        {
          int a = 0, fld = -1;

          for (int i = 0; i < AI.MyFieldsNum; i++)
          if (AI.MyFields[i].NeighbourWithEnemy)
            if (a < Map[AI.MyFields[i].Field].Army)
            {
              fld = AI.MyFields[i].Field;
              a = Map[AI.MyFields[i].Field].Army;
            }

          Map[fld].Army += army;
          InitWaitField(fld);
          return;
        }

        // ****************************************************************************************
        // *** na 40% ca³¹ armiê postaw obok wyspy zajêtej w ca³oœci przez wroga

        if (IS_PERCENT(40))
        for (int i = 0; i < AI.MyFieldsNum; i++)
          if (AI.MyFields[i].NeighbourWithEnemyWholeIsland)
          {
            fld = AI.MyFields[i].Field;
            Map[fld].Army += army;
            InitWaitField(fld);
            return;
          }

        // ****************************************************************************************
        // *** na 80% postaw ca³¹ armiê na polu na ma³ej wyspie

        if (IS_PERCENT(80))
        for (int isl, i = 0; i < AI.MyFieldsNum; i++)
        {
          fld = AI.MyFields[i].Field;
          isl = Map[fld].Island;

          if (IslandsSize[isl] <= 5 &&
              AI.MyFields[i].NeighbourWithEnemy)
          {
            Map[fld].Army += army;
            InitWaitField(fld);
            return;
          }
        }

        // ****************************************************************************************
        // *** rozmieœæ armiê

        while (army)
        {
          for (int i = 0; i < 40; i++)
          {
            myfld = random(AI.MyFieldsNum);
            fld = AI.MyFields[myfld].Field;

            if (AI.MyFields[myfld].NeighbourWithEnemy)
              if (i <= AI.MyFieldsNum / 3)
                break;

            if (AI.MyFields[myfld].IslandEdge &&
                !AI.IHaveMostFields &&
                !AI.MyFields[myfld].SecureField)
              break;

            if (AI.MyFields[myfld].CommandCentre &&
                !AI.MyFields[myfld].NeighbourNeighbourWithEnemy &&
                !AI.IHaveMostFields &&
                IS_PERCENT(70))
              break;
          }

          max = army - 5;
          if (max < 1)
            max = 1;

          numa = random(max) + 3;
          if (numa > max)
            numa = max;

          Map[fld].Army += numa;
          army -= numa;

          InitWaitField(fld);
        }
}