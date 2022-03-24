// ************************************************************************************************
// AI *** DiffLevel == 0 (1) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_0_GetMove()
{
        TMove move;

        // ****************************************************************************************
        // *** przenie� wojsko bli�ej frontu

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
        // *** je�li nie mo�na nikogo zaatakowa� to czekaj tur�

        if (AI.FieldsToAttackNum == 0)
          RETURN_MOVE(-1, -1, -1, M_WAITTURN)

        // ****************************************************************************************
        // *** najpierw sprawd� czy mo�na zaatakowa� kogo� 3x s�abszego

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
              RETURN_MOVE(src, dest, -1, M_ATTACK)
            }
          }
        }

        // ****************************************************************************************
        // *** na 30% nic nie r�b

        if (IS_PERCENT(30))
          RETURN_MOVE(-1, -1, -1, M_WAITTURN);

        // ****************************************************************************************
        // *** wybierz losowe pole do zaatakowania

        dest = AI.FieldsToAttack[random(AI.FieldsToAttackNum)];

        for (int num, i = 0; i < 8; i++)
        {
          num = Map[dest].Neighbours[i];
          if (num != -1 &&
            Map[num].Player == Player &&
            Map[num].Army > Map[dest].Army)
          {
            src = Map[dest].Neighbours[i];
            RETURN_MOVE(src, dest, -1, M_ATTACK)
          }
        }

        // ****************************************************************************************
        // *** !!! do t�d program nie powinien doj��

        RETURN_MOVE(-1, -1, -1, M_WAITTURN)
}


bool TPlayer::AI_0_ContinueMove(TMove*)
{
        return true;
}

void TPlayer::AI_0_MoveArmy(TMove* move)
{
        int army = Map[move->SrcFld].Army;

        // ****************************************************************************************
        // *** przenie� 90% armi

        int asrc = army / 10;
        if (asrc == 0)
          asrc = 1;

        int adest = army - asrc;

        Map[move->SrcFld].Army = asrc;
        Map[move->DestFld].Army = adest;
}

void TPlayer::AI_0_PlaceArmy()
{
//        AI.Sch = SCH_NOSCHEME;

        int myfld, fld, numa, max, army = Income[Player];

        while (army)
        {

          // **************************************************************************************
          // *** wybierz jakie� pole - staraj si� �eby s�siadowa�o z wrogiem

          for (int i = 0; i < 10; i++)
          {
            myfld = random(AI.MyFieldsNum);
            fld = AI.MyFields[myfld].Field;
            if (AI.MyFields[myfld].NeighbourWithEnemy)
              break;
          }

          // **************************************************************************************
          // *** ile mo�na po�o�y� maksymalnie

          if ((max = army / 4) == 0)
            max = 1;

          // **************************************************************************************
          // *** wylosuj ile po�o�y� armi i po�� j�

          numa = random(max) + 1;

          Map[fld].Army += numa;
          army -= numa;

          InitWaitField(fld);
        }
}