// ************************************************************************************************
// AI *** DiffLevel == 1 (2) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_1_GetMove()
{
        TMove move;
        
        // ***************************************************************************************
        // *** jeœli mo¿na to przenieœ wojsko bli¿ej frontu

        if (IS_PERCENT(40) ||
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
        // *** najpierw sprawdŸ czy mo¿na zaatakowaæ kogoœ 3x s³abszego

        int src, dest;

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
                Map[num].Army > Map[dest].Army)
            {
              src = num;
              RETURN_MOVE(src, dest, -1, M_ATTACK);
            }
          }
        }

        // ****************************************************************************************
        // *** na 20% nic nie rób

        if (IS_PERCENT(20))
          RETURN_MOVE(-1, -1, -1, M_WAITTURN);

        // ****************************************************************************************
        // *** zaatakuj losowe pole

        for (int i = 0; i < 5; i++)
        {
          dest = AI.FieldsToAttack[random(AI.FieldsToAttackNum)];
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

        // ****************************************************************************************
        // *** !!!

        RETURN_MOVE(-1, -1, -1, M_WAITTURN)
}

bool TPlayer::AI_1_ContinueMove(TMove* move)
{
        if (move->Type == M_MOVEARMY ||
            move->Type == M_WAITTURN)
          return true;

        if (move->Type == M_ATTACK)
        {
          if (Map[move->SrcFld].Army == 2 &&
              IS_PERCENT(10))
            return false;

          if (Map[move->SrcFld].Army <= 6 &&
              !AI.MyFields[AI.MyFieldsOnMap[move->SrcFld]].SecureField &&
              Map[move->SrcFld].CommandCentre)
            return false;
        }
        
        return true;    
}

void TPlayer::AI_1_MoveArmy(TMove* move)
{
        int army = Map[move->SrcFld].Army;

        int asrc = army / 5;
        if (asrc == 0)
          asrc = 1;

        if (Map[move->SrcFld].CommandCentre &&
            !AI.MyFields[AI.MyFieldsOnMap[move->SrcFld]].SecureField)
          asrc = army - 1 > 12 ? 12 : army - 1;  

        int adest = army - asrc;

        Map[move->SrcFld].Army = asrc;
        Map[move->DestFld].Army = adest;
}

void TPlayer::AI_1_PlaceArmy()
{
//        AI.Sch = SCH_NOSCHEME;

        int myfld, fld, numa, max, army = Income[Player];

        // ****************************************************************************************
        // *** jeœli wróg ma tylko jedno pole to stawiaj ca³¹ armiê obok tego pola

        if (Fields[Player] == 47)
        {
          for (int i = 0; i < AI.MyFieldsNum; i++)
          if (AI.MyFields[i].NeighbourWithEnemy)
          {
            fld = AI.MyFields[i].Field;
            break;
          }
          Map[fld].Army += army;
          InitWaitField(fld);
          return;
        }

        // ****************************************************************************************
        // *** rozmieœæ armiê

        while (army)
        {
          for (int i = 0; i < 20; i++)
          {
            myfld = random(AI.MyFieldsNum);
            fld = AI.MyFields[myfld].Field;

            if (AI.MyFields[myfld].NeighbourWithEnemy)
              break;

            if (AI.MyFields[myfld].CommandCentre &&
                !AI.MyFields[myfld].NeighbourNeighbourWithEnemy &&
                IS_PERCENT(70))
              break;
          }

          if ((max = army / 3) == 0)
            max = 1;

          numa = random(max) + 1;

          Map[fld].Army += numa;
          army -= numa;

          InitWaitField(fld);
        }
}