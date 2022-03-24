// ************************************************************************************************
// AI *** DiffLevel == 2 (3) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_2_GetMove()   
{
        TMove move;
        
        // ****************************************************************************************
        // *** sprawd� czy mo�na zaatakowa� brzeg wyspy

        int src, dest;

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
                Map[num].Army > 3)
            {
              src = num;
              if (IS_PERCENT(80))
                if (Map[dest].Island != Map[src].Island)
                  continue;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ****************************************************************************************
        // *** sprawd� czy mo�na przenie�� wojsko z bronionego pola na brzeg wyspy

        for (int i = 0; i < AI.MyFieldsNum; i++)
        {
          src = AI.MyFields[i].Field;
          if (Map[src].Army > 4 &&
              AI.MyFields[i].SecureField &&
              AI.MyFields[i].NeighbourWithThisIslandEdge)
          for (int j = 0; j < 8; j++)
          {
            dest = Map[src].Neighbours[j];
            if (dest != -1 &&
                Map[dest].Island == Map[src].Island &&
                Map[dest].Player == Player &&
                Map[dest].IslandEdge)
              RETURN_MOVE(src, dest, Map[src].Army - 1, M_MOVEARMY);
          }
        }

        // ****************************************************************************************
        // *** sprawd� czy mo�na zaatakowa� kogo� 3x s�abszego

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
        // *** sprawd� czy mo�na zaatakowa� centr. dow.

        for (int i = 0; i < AI.FieldsToAttackNum; i++)
        {
          dest = AI.FieldsToAttack[i];
          if (Map[dest].CommandCentre)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[dest].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player &&
                Map[num].Army > Map[dest].Army)
            {
              src = num;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ***************************************************************************************
        // *** je�li mo�na to przenie� wojsko bli�ej frontu

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
        // *** nic nie r�b je�li nie mo�esz atakowa�

        if (AI.FieldsToAttackNum == 0)
          RETURN_MOVE(-1, -1, -1, M_WAITTURN)

        // ****************************************************************************************
        // *** na 10% nic nie r�b

        if (IS_PERCENT(10))
          RETURN_MOVE(-1, -1, -1, M_WAITTURN);

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
                Map[num].Army > Map[dest].Army)
              if (Map[num].Army > 3 ||
                  IS_PERCENT(10))
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

bool TPlayer::AI_2_ContinueMove(TMove* move)
{
        if (move->Type == M_MOVEARMY ||
            move->Type == M_WAITTURN)
          return true;

        if (move->Type == M_ATTACK)
        {
          if (Map[move->SrcFld].Army == 2 &&
              IS_PERCENT(80))
            return false;

          if (Map[move->SrcFld].Army <= 6 &&
              !AI.MyFields[AI.MyFieldsOnMap[move->SrcFld]].SecureField &&
              Map[move->SrcFld].CommandCentre &&
              IS_PERCENT(10))
            return false;
        }
        
        return true;    
}

void TPlayer::AI_2_MoveArmy(TMove* move)
{
        int army = Map[move->SrcFld].Army;

        int asrc = army / 5;
        if (asrc == 0)
          asrc = 1;

        if (Map[move->DestFld].IslandEdge &&
            Map[move->SrcFld].IslandEdge)
          if (!AI.MyFields[AI.MyFieldsOnMap[move->SrcFld]].NeighbourNeighbourWithEnemy)
            asrc = army / 12 ? army / 12 : 1;
          else
            asrc = army / 4 ? army / 4 : 1;

        if (Map[move->SrcFld].CommandCentre &&
            !AI.MyFields[AI.MyFieldsOnMap[move->SrcFld]].SecureField)
          asrc = army - 1 > 12 ? 12 : army - 1;

        int adest = army - asrc;

        Map[move->SrcFld].Army = asrc;
        Map[move->DestFld].Army = adest;
}

void TPlayer::AI_2_PlaceArmy()
{
//        AI.Sch = SCH_NOSCHEME;

        int myfld, fld, numa, max, army = Income[Player];

        // ****************************************************************************************
        // *** je�li wr�g ma tylko jedno pole to stawiaj ca�� armi� obok tego pola na swoim
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
        // *** na 20% ca�� armi� postaw obok wyspy zaj�tej w ca�o�ci przez wroga

        if (IS_PERCENT(20))
        for (int i = 0; i < AI.MyFieldsNum; i++)
          if (AI.MyFields[i].NeighbourWithEnemyWholeIsland)
          {
            fld = AI.MyFields[i].Field;
            Map[fld].Army += army;
            InitWaitField(fld);
            return;
          }

        // ****************************************************************************************
        // *** rozmie�� armi�

        while (army)
        {
          for (int i = 0; i < 20; i++)
          {
            myfld = random(AI.MyFieldsNum);
            fld = AI.MyFields[myfld].Field;

            if (AI.MyFields[myfld].NeighbourWithEnemy)
              if (i <= Fields[Player] / 5)
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

          max = army / 2;
          if (max < 1)
            max = 1;

          numa = random(max) + 1;

          Map[fld].Army += numa;
          army -= numa;

          InitWaitField(fld);
        }
}