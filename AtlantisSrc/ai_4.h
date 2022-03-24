// ************************************************************************************************
// AI *** DiffLevel == 4 (5) **********************************************************************
// ************************************************************************************************

TMove TPlayer::AI_4_GetMove()
{
        // jeœli M_NOMOVE to MoveNearEnemy
        TMove move;

        if (AI.SchNum > 0)
        {
          switch (AI.Sch[0]) {
            case SCH_CAPTUREISLAND : move = AI_4_aux_GetMove_CaptureIsland(); break;
            case SCH_ATTACKPLAYER : move = AI_4_aux_GetMove_AttackPlayer(); break;
            case SCH_DEFENDISLANDS : move = AI_4_aux_GetMove_DefendIslands(); break;
            case SCH_ATTACK : move = AI_4_aux_GetMove_Attack(); break;
            case SCH_CAPTURECC : move = AI_4_aux_GetMove_CaptureCC(); break;
            case SCH_DEFENDCC : move = AI_4_aux_GetMove_DefendCC(); break;
            case SCH_DEPRIVEISLAND : move = AI_4_aux_GetMove_DepriveIsland(); break;
           }

           if (move.Type != M_NOMOVE)
             return move;

           for (int i = 0; i < --AI.SchNum; i++)
           {
             AI.Sch[i] = AI.Sch[i + 1];
             AI.SchPar[i] = AI.SchPar[i + 1];
           }
        }

        move = AI_4_aux_GetMove_Attack();
        if (move.Type == M_NOMOVE)
          move = AI_4_aux_GetMove_MoveNearEnemy();
        if (move.Type == M_NOMOVE)
          RETURN_MOVE(-1, -1, -1, M_WAITTURN);
        return move;
}

// ************************************************************************************************

bool TPlayer::AI_4_ContinueMove(TMove* move)
{
        if (move->Type == M_ATTACK &&
            Map[move->SrcFld].Army == 2)
          return false;

        return true;
}

// ************************************************************************************************

void TPlayer::AI_4_MoveArmy(TMove* move)
{
        // w GetMove plan zmienia siê dopiero kiedy f. pomocnicza zwróci M_NOMOVE,
        // wtedy plan zmienia siê na nastêpny
        // aktualny plan: Sch[0], SchPar[0]

        int army = Map[move->SrcFld].Army;
        int mysrc = AI.MyFieldsOnMap[move->SrcFld];
        int mydest = AI.MyFieldsOnMap[move->DestFld];

        if (AI.MyFields[mysrc].EnemyFieldsAround == 1)
        for (int enemy, i = 0; i < 8; i++)
        {
          enemy = Map[move->SrcFld].Neighbours[i];
          if (enemy != -1 &&
              Map[enemy].Player != Player)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[move->DestFld].Neighbours[j];
            if (num != -1 &&
                num == enemy)
            {
              Map[move->SrcFld].Army = 1;
              Map[move->DestFld].Army = army - 1;
              return;
            }
          }
        }
        if (AI.MyFields[mydest].EnemyFieldsAround == 1)
        for (int enemy, i = 0; i < 8; i++)
        {
          enemy = Map[move->DestFld].Neighbours[i];
          if (enemy != -1 &&
              Map[enemy].Player != Player)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[move->SrcFld].Neighbours[j];
            if (num != -1 &&
                num == enemy)
            {
              Map[move->DestFld].Army = 1;
              Map[move->SrcFld].Army = army - 1;
              return;
            }
          }
        }

        if (AI.Sch[0] == SCH_ATTACKPLAYER)
        {
          int p = AI.SchPar[0];

          int src_earmy = 0;
          for (int num, i = 0; i < 8; i++)
          {
            num = Map[move->SrcFld].Neighbours[i];
            if (num != -1 &&
                Map[num].Player == p)
              src_earmy += Map[num].Army;
          }

          int dest_earmy = 0;
          for (int num, i = 0; i < 8; i++)
          {
            num = Map[move->DestFld].Neighbours[i];
            if (num != -1 &&
                Map[num].Player == p)
              dest_earmy += Map[num].Army;
          }

          if (src_earmy ||
              dest_earmy)
          {
            int src_army = army * src_earmy / (src_earmy + dest_earmy);
            if (src_army < 1)
              src_army = 1;
            if (src_army > army - 1)
              src_army = army - 1;

            Map[move->SrcFld].Army = src_army;
            Map[move->DestFld].Army = army - src_army;
            return;
          }
        }

        if (AI.Sch[0] == SCH_CAPTUREISLAND)
        {
          int isl = AI.SchPar[0];

          if (AI.Islands[isl].Size == 3)
          {
            if (AI.MyFields[mysrc].NeighbourWithEnemy)
            {
              Map[move->SrcFld].Army = army - 1;
              Map[move->DestFld].Army = 1;
            }
            else
            {
              Map[move->DestFld].Army = army - 1;
              Map[move->SrcFld].Army = 1;
            }
            return;
          }

          // inne wyspy tak jak na koñcu
        }

        if (AI.Sch[0] == SCH_DEPRIVEISLAND ||
            AI.Sch[0] == SCH_CAPTURECC)
        {
          Map[move->SrcFld].Army = 1;
          Map[move->DestFld].Army = army - 1;
          return;
        }

        // domyœlnie

        if (AI.MyFields[mysrc].NeighbourWithEnemy &&
            AI.MyFields[mydest].NeighbourWithEnemy)
        {
          int src_army;
          if (AI_aux_CommonEnemy(move->SrcFld, move->DestFld))
            src_army = army - 1;
          else
            src_army = army / 2;
          Map[move->SrcFld].Army = src_army;
          Map[move->DestFld].Army = army - src_army;
          return;
        }

        if (AI.MyFields[mysrc].NeighbourWithEnemy &&
            !AI.MyFields[mydest].NeighbourWithEnemy)
        {
          Map[move->SrcFld].Army = army - 1;
          Map[move->DestFld].Army = 1;
          return;
        }

        if (!AI.MyFields[mysrc].NeighbourWithEnemy &&
            AI.MyFields[mydest].NeighbourWithEnemy)
        {
          Map[move->SrcFld].Army = 1;
          Map[move->DestFld].Army = army - 1;
          return;
        }

        if (!AI.MyFields[mysrc].NeighbourWithEnemy &&
            !AI.MyFields[mydest].NeighbourWithEnemy)
        {
          int src_army;
          if (AI.MyFields[mysrc].FieldsToEnemy > AI.MyFields[mydest].FieldsToEnemy)
            src_army = 1;
          else
            src_army = army - 1;
          Map[move->SrcFld].Army = src_army;
          Map[move->DestFld].Army = army - src_army;
          return;
        }
}

// ************************************************************************************************

static int AI_4_ArmyAdd[48];

void TPlayer::AI_4_PlaceArmy()
{
        int army = Income[Player];

        for (int i = 0; i < 48; i++)
          AI_4_ArmyAdd[i] = 0;

        AI.SchNum = 0;

        if ((FieldsRanking[0].Army >= 16 &&
             FieldsRanking[0].Player != Player) ||
            (FieldsRanking[1].Army >= 16 &&
             FieldsRanking[1].Player != Player)) 
        {
#ifdef DEBUG_MODE        
          if (IS_PERCENT(70))
            AI_4_aux_PlaceArmy_DepriveIsland(army), DEBUG_write_a("DepriveIsland", army, Player);
          AI_4_aux_PlaceArmy_AttackStrongestPlayer(army), DEBUG_write_a("AttackStrPl", army, Player);
          AI_4_aux_PlaceArmy_CaptureSmallIsland(army) ,DEBUG_write_a("CaptSmallIsl", army, Player);
#else
          if (IS_PERCENT(70))
            AI_4_aux_PlaceArmy_DepriveIsland(army);
          AI_4_aux_PlaceArmy_AttackStrongestPlayer(army);
          AI_4_aux_PlaceArmy_CaptureSmallIsland(army);
#endif
        }
        else
        {
#ifdef DEBUG_MODE        
          AI_4_aux_PlaceArmy_CaptureSmallIsland(army), DEBUG_write_a("CaptSmallIsl", army, Player);
          AI_4_aux_PlaceArmy_AttackStrongestPlayer(army), DEBUG_write_a("AttackStrPl", army, Player);
#else
          AI_4_aux_PlaceArmy_CaptureSmallIsland(army);
          AI_4_aux_PlaceArmy_AttackStrongestPlayer(army);
#endif          
        }

        int num = 0;
        while (army > 0 && num < 10)
        {
          int percent = random(100);

#ifdef DEBUG_MODE
          if (IN_RANGE(percent, 0, 49))
            AI_4_aux_PlaceArmy_CaptureIsland(army), DEBUG_write_a("CaptIsl", army, Player);
          else

          if (IN_RANGE(percent, 50, 59))
            AI_4_aux_PlaceArmy_Attack(army), DEBUG_write_a("Attack", army, Player);
          else

          if (IN_RANGE(percent, 60, 89))
            AI_4_aux_PlaceArmy_DepriveIsland(army), DEBUG_write_a("DepriveIsland", army, Player);
          else

          if (IN_RANGE(percent, 90, 94))
            AI_4_aux_PlaceArmy_DefendIslands(army), DEBUG_write_a("DefendIsland", army, Player);
          else

          AI_4_aux_PlaceArmy_CheckCC(army), DEBUG_write_a("CheckCC", army, Player);
#else
          if (IN_RANGE(percent, 0, 49))
            AI_4_aux_PlaceArmy_CaptureIsland(army);
          else

          if (IN_RANGE(percent, 50, 59))
            AI_4_aux_PlaceArmy_Attack(army);
          else

          if (IN_RANGE(percent, 60, 89))
            AI_4_aux_PlaceArmy_DepriveIsland(army);
          else

          if (IN_RANGE(percent, 90, 94))
            AI_4_aux_PlaceArmy_DefendIslands(army);
          else

          AI_4_aux_PlaceArmy_CheckCC(army);
#endif          

          num++;
        }

        if (num == 10)
        for (int i = 0; i < AI.MyFieldsNum; i++)
          if (AI.MyFields[i].NeighbourWithEnemy)
          {
            AI_4_ArmyAdd[AI.MyFields[i].Field] += army;
            break;
          }

        for (int i = 0; i < 48; i++)
        if (AI_4_ArmyAdd[i] > 0)
        {
          InitWaitField(i);
          Map[i].Army += AI_4_ArmyAdd[i];
        }
}

// ************************************************************************************************
// ************************************************************************************************
// ************************************************************************************************

void TPlayer::AI_4_aux_PlaceArmy_CaptureSmallIsland(int& army)
{
        if (army == 0)
          return;

        int rnd = random(2) + 2;

        for (int i = 0; i < 8; i++)
        if (AI.Islands[i].Size == 3 &&
            AI.Islands[i].EnemyFields > 0 &&
            AI.Islands[i].MyArmy + army >= AI.Islands[i].EnemyArmy + AI.Islands[i].EnemyFields + rnd &&
            AI.Islands[i].MyFields > 0)
        {
          // na którym polu postawiæ wojsko
          struct { int f, a; } fld = { -1, 0 };
          for (int j = 0; j < 48; j++)
          if (Map[j].Island == i &&
              Map[j].Player == Player &&
              Map[j].Army > fld.a)
            fld.f = j, fld.a = Map[j].Army;

          int army_needed = AI.Islands[i].EnemyArmy + AI.Islands[i].EnemyFields + rnd;
          if (army_needed > army ||
              army_needed < 1)
            continue;

          AI_4_ArmyAdd[fld.f] += army_needed;
          army -= army_needed;

          AddSch(SCH_CAPTUREISLAND, i, true);
        }
}

void TPlayer::AI_4_aux_PlaceArmy_AttackStrongestPlayer(int& army)
{
        int best_income = 0;
        for (int i = 0; i < 4; i++)
        if (i != Player)
          best_income = max(best_income, Income[i]);
        if (best_income < /*12*/random(5) + 10)
          best_income = MAXINT; //

        int st_army = 0;
        for (int i = 0; i < 4; i++)
        if (i != Player)
          if (Fields[i] > /*5*/random(3) + 4)
            st_army = max(st_army, Army[i]);
        if (st_army < /*30*/25 + random(25))
          st_army = MAXINT;

        int p = -1;

        for (int i = 0; i < 4; i++)
        if (i != Player)
          if (Fields[i] >= /*15*/random(5) + 12 ||        // mo¿e uwzglêdniæ czy to cz³owiek i wtedy mniej
              Army[i] == st_army ||
              Income[i] == best_income ||
              WholeIslandsFields[i] >= 5 + 3 + 9 - Game->DiffLevel)
          {
            p = i;
            break;
          }

        if (p == -1)
        for (int i = 0; i < 4; i++)
        if (i != Player &&
            WholeIslandsFields[i] >= 5 + 3)
        {
          p = i;
          break;
        }

        if (p == -1)
          return;

        static int per[10] = { -1,
                               -1,
                               -1,
                               -1,
                               50,
                               40,
                               30,
                               20,
                               10,
                               10 };

/*        if (IS_PERCENT(per[Game->DiffLevel])) // bez oszustw
          return;*/

        if (p == 0 && // z oszustwami
            IS_PERCENT(per[Game->DiffLevel]))
          return;

        if (p != 0 &&
            IS_PERCENT((Game->DiffLevel == 9) ? 100 : 100 - per[Game->DiffLevel]))
          return;

        // najpierw rozmieœæ wojsko tak ¿eby pozbawiæ wyspy - !!! w GetMove najpierw atakuj pola na ca³ych wyspach

        for (int i = 0; i < 8; i++)
        if (AI.Islands[i].Player == p)
        {
          // na którym polu postawiæ
          for (int j = 0; j < 48; j++)
          if (Map[j].Island == i &&
              Map[j].IslandEdge)
            for (int num, k = 0; k < 8; k++)
            {
              num = Map[j].Neighbours[k];
              if (num != -1 &&
                  Map[num].Player == Player &&
                  Map[num].Army + army > Map[j].Army)
              {
                AI_4_ArmyAdd[num] += army;
                army = 0;
                AddSch(SCH_ATTACKPLAYER, p);
                return;
              }
            }
        }

        // rozmieœæ wojsko obok wyspy najs³abiej bronionej
        int mina = MAXINT, minf = -1;
        for (int i = 0; i < 8; i++)
        if (AI.Islands[i].Player == p)
        {
          for (int j = 0; j < 48; j++)
          if (Map[j].Island == i &&
              Map[j].IslandEdge)
            for (int num, k = 0; k < 8; k++)
            {
              num = Map[j].Neighbours[k];
              if (num != -1 &&
                  Map[num].Player == Player)
                {
                  mina = min(mina, Map[j].Army);
                  minf = num;
                }
            }
        }

        if (minf != -1)
        {
          AI_4_ArmyAdd[minf] += army;
          army = 0;
          AddSch(SCH_ATTACKPLAYER, p);
          return;
        }

        // rozmieœæ wojsko tak aby pozbawiæ cc - !!! - w GetMove - najpierw atakuj cc potem inne pola (przy ATTACKPLAYER)

        for (int i = 0; i < 48 && army > 0; i++) // ??? - b³¹d - ok 
        if (Map[i].CommandCentre &&
            Map[i].Player == p)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[i].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player)
            {
              int army_needed = Map[i].Army - Map[num].Army + random(2) + 1;
              if (Map[num].Army + army_needed == 2)
                army_needed++; //
              if (army_needed < 1 ||
                  army_needed > army)
                continue;
              AI_4_ArmyAdd[num] += army_needed;
              army -= army_needed;
              AddSch(SCH_ATTACKPLAYER, p);
              break;
            }
          }

        // rozmieœæ wojsko obok najs³abszych pól wroga

        struct { int f, a, m, n; } fields[48], tmp;     // f = pole, a = wojsko, m = najmn. wojsko, n = ile pól woko³o jest zawodnika p
        int fnum = 0;

        for (int i = 0; i < 48; i++)
        if (Map[i].Player == Player &&
            AI.MyFields[AI.MyFieldsOnMap[i]].NeighbourWithPlayer[p])
        {
          fields[fnum].f = i;
          fields[fnum].a = 0;
          fields[fnum].m = MAXINT;
          fields[fnum].n = 0;
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[i].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == p)
            {
              fields[fnum].a += Map[num].Army;
              fields[fnum].m = min(Map[num].Army, fields[fnum].m);
              fields[fnum].n++;
            }
          }
          fnum++;
        }

        if (fnum == 0)
          return;

        for (int i = 0; i < fnum; i++)
          for (int j = i + 1; j < fnum; j++)
          if (fields[j].a > fields[i].a)
            tmp = fields[j], fields[j] = fields[i], fields[i] = tmp;

        for (int i = 0; i < fnum; i++)
        {
          int army_needed = fields[i].a - Map[fields[i].f].Army + fields[i].n + random(2);
          if (army_needed < 1)
            army_needed = fields[i].m - Map[fields[i].f].Army + /*1*/2 + random(2);
          if (Map[fields[i].f].Army + army_needed == 2)
            army_needed++;

          if (army_needed < 1 ||
              army_needed > army)
            continue;

          AI_4_ArmyAdd[fields[i].f] += army_needed;
          army -= army_needed;

          AddSch(SCH_ATTACKPLAYER, p);
        }
        
        // rozmieœæ ca³e wojsko obok najs³abszego pola wroga
        
        if (fnum > 0)
        {
          AI_4_ArmyAdd[fields[0].f] = army;
          army = 0;

          AddSch(SCH_ATTACKPLAYER, p);
        }
}

void TPlayer::AI_4_aux_PlaceArmy_CaptureIsland(int& army)
{
        int isl = AI.SortedIslands[I_MyArmyPercent][0];

        bool capt_isl = false;

        for (int i = 0; i < 48; i++)
        if (Map[i].Player != Player &&
            Map[i].Island == isl &&
            army > 0)
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[i].Neighbours[j];
            if (num != -1 &&
                Map[num].Player == Player)
            {
              int army_needed = Map[i].Army - Map[num].Army + 1 + random(2);
              if (Map[num].Army + army_needed == 2)
                army_needed++;
              if (army_needed > army &&
                  IS_PERCENT(15))
                army_needed = army;
              if (army_needed > army ||
                  army_needed < 1)
                continue;
              AI_4_ArmyAdd[num] += army_needed;
              army -= army_needed;
              capt_isl = true;
              break;
            }
          }

        if (capt_isl)
          AddSch(SCH_CAPTUREISLAND, isl, true);
}

void TPlayer::AI_4_aux_PlaceArmy_Attack(int& army) // atakuj najs³absze pola
{
        bool ef[48];
        for (int i = 0; i < 48; i++)
          ef[i] = false;
        for (int i = AI.MyFieldsNum -1; i >= 0; i--)
        {
          int myfld = AI.SortedMyFields[MF_EnArmyAround][i];
          int fld = AI.MyFields[myfld].Field;
          if (AI.MyFields[myfld].EnemyArmyAround > 0)
          {
            int army_needed = AI.MyFields[myfld].EnemyArmyAround - Map[fld].Army +
                              AI.MyFields[myfld].EnemyFieldsAround + 2 + random(2);
            if (army_needed > army ||
                army_needed < 1)
              continue;

            int army_around = AI.MyFields[myfld].EnemyArmyAround;
            for (int num, j = 0; j < 8; j++)
            {
              num = Map[fld].Neighbours[j];
              if (num != -1 &&
                  Map[num].Player != Player &&
                  ef[num])
                army_around -= Map[num].Army;
            }

            if (army_around == 0)
              continue;

            for (int num, j = 0; j < 8; j++)
            {
              num = Map[fld].Neighbours[j];
              if (num != -1)
                ef[num] = true;
            }

            AI_4_ArmyAdd[fld] += army_needed;
            army -= army_needed;

            AddSch(SCH_ATTACK, -1);
          }
        }
}

void TPlayer::AI_4_aux_PlaceArmy_DepriveIsland(int& army)
{
        struct { int f, a, l; } fields[48], tmp;        // l = wyspa
        int fnum = 0;

        for (int fld, i = 0; i < AI.MyFieldsNum; i++)
        if (AI.MyFields[i].NeighbourWithEnemyWholeIsland)
        {
          fld = AI.MyFields[i].Field;
          fields[fnum].f = fld;
          fields[fnum].a = MAXINT;
          for (int num, isl, j = 0; j < 8; j++)
          {
            num = Map[fld].Neighbours[j];
            isl = Map[fld].Island;
            if (num != -1 &&
                Map[num].Player != Player &&
                AI.Islands[isl].Player != Player &&
                AI.Islands[isl].Player != -1 &&
                Map[num].Army < fields[fnum].a)
            {
              fields[fnum].l = isl;
              fields[fnum++].a = Map[num].Army;
            }
          }
        }

        if (fnum == 0)
          return;

        for (int i = 0; i < fnum; i++)
          for (int j = i + 1; j < fnum; j++)
          if (fields[j].a < fields[i].a)
            tmp = fields[j], fields[j] = fields[i], fields[i] = tmp;

        for (int i = 0; i < fnum; i++)
        {
          int army_needed = AI.Islands[fields[i].l].EnemyArmy +
                            AI.Islands[fields[i].l].Size * 2 + random(3);
          if (army_needed > army)
            army_needed = army;

          army -= army_needed;
          AI_4_ArmyAdd[fields[i].f] += army_needed;

          AddSch(SCH_DEPRIVEISLAND, fields[i].l, true);
        }
}

void TPlayer::AI_4_aux_PlaceArmy_DefendIslands(int& army)
{
        int fields[48], fnum = 0;
        bool bf[48];

        for (int i = 0; i < 48; i++)
          bf[i] = false;

        for (int isl, i = 0; i < 48; i++)
        {
          isl = Map[i].Island;
          if (Map[i].Player == Player &&
              Map[i].IslandEdge &&
              AI.Islands[isl].Player == Player &&
              !AI.Islands[isl].SecureIsland)
            if (AI.MyFields[AI.MyFieldsOnMap[i]].NeighbourWithEnemy)
              fields[fnum++] = i;
            else // sprawdŸ czy pola na zewn¹trz s¹ bronione
              for (int num, j = 0; j < 8; j++)
              {
                num = Map[i].Neighbours[j];
                if (num != -1 &&
                    !bf[num] &&
                    AI.MyFields[AI.MyFieldsOnMap[num]].NeighbourWithEnemy)  // ??? - trochê prymitywne
                {
                  fields[fnum++] = num; // poprawiony; b³¹d: pola mog¹ siê powtarzaæ
                  bf[num] = true;
                }
              }
        }

        if (fnum == 0)
          return;

        AddSch(SCH_DEFENDISLANDS, -1);

        int num = 0;
        while (army > 0)
        {
          AI_4_ArmyAdd[fields[num++]]++;
          if (num == fnum)
            num = 0;
          army--;  
        }
}

void TPlayer::AI_4_aux_PlaceArmy_CheckCC(int& army)
{
        // najpierw atakuj cc
        for (int fld, i = 0; i < AI.MyFieldsNum; i++)
        if (AI.MyFields[i].NeighbourWithCommandCentre)
        {
          fld = AI.MyFields[i].Field;
          for (int num, j = 0; j < 8; j++)
          {
            num = Map[fld].Neighbours[j];
            if (num != -1 &&
                Map[num].CommandCentre &&
                Map[num].Player != Player &&
                Map[num].Army < Map[fld].Army + army  &&
                Map[fld].Army + army > 2)
            {
              int army_needed = Map[num].Army - Map[fld].Army + 2;
              if (army_needed < 1 ||
                  army_needed > army)
                continue;
              AI_4_ArmyAdd[fld] += army_needed;
              army -= army_needed;

              AddSch(SCH_CAPTURECC, -1);
            }
          }
        }

        if (army == 0)
          return;

        // broñ cc
        int cc[5], ccnum = 0;

        for (int i = 0; i < AI.MyFieldsNum; i++)
        {
          if (AI.MyFields[i].CommandCentre &&
              AI.MyFields[i].NeighbourWithEnemy)
            cc[ccnum++] = AI.MyFields[i].Field;
        }

        if (ccnum == 0)
          return;

        int num = 0;
        while (army > 0)
        {
          AI_4_ArmyAdd[cc[num++]]++;
          if (num == ccnum)
            num = 0;
          army--;  

          AddSch(SCH_DEFENDCC, -1);  
        }
}

// ************************************************************************************************

TMove TPlayer::AI_4_aux_GetMove_AttackPlayer()
{
        TMove move;

        int p = AI.SchPar[0];

        for (int src, i = 0; i < AI.MyFieldsNum; i++)
        if (AI.MyFields[i].NeighbourWithPlayer[p])
        {
          src = AI.MyFields[i].Field;
          for (int dest, j = 0; j < 8; j++)
          {
            dest = Map[src].Neighbours[j];
            if (dest != -1 &&
                Map[dest].Player == p &&
                Map[src].Army > Map[dest].Army &&
                Map[src].Army > 2 &&
                AI_aux_IsStrongestArmyToAttack(src, dest))
              RETURN_MOVE(src, dest, -1, M_ATTACK);
          }
        }

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_CaptureIsland()
{
        TMove move;

        int isl = AI.SchPar[0];

        for (int src, i = 0; i < AI.MyFieldsNum; i++)
        {
          src = AI.MyFields[i].Field;
          if (Map[src].Island == isl)
          for (int dest, j = 0; j < 8; j++)
          {
            dest = Map[src].Neighbours[j];
            if (dest != -1 &&
                Map[dest].Player != Player &&
                Map[src].Army > Map[dest].Army &&
                Map[src].Army > 2 &&
                Map[dest].Island == isl &&
                AI_aux_IsStrongestArmyToAttack(src, dest))
              RETURN_MOVE(src, dest, -1, M_ATTACK);
          }
        }

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_Attack()
{
        TMove move;

        if (AI.FieldsToAttackNum == 0)
          RETURN_MOVE(-1, -1, -1, M_NOMOVE);

        for (int dest, i = AI.FieldsToAttackNum - 1; i >= 0; i--)
        {
          dest = AI.FieldsToAttack[i];
          for (int src, j = 0; j < 8; j++)
          {
            src = Map[dest].Neighbours[j];
            if (src != -1 &&
                Map[src].Player == Player &&
                Map[src].Army > Map[dest].Army &&
                Map[src].Army > 2 &&
                AI_aux_IsStrongestArmyToAttack(src, dest))
              RETURN_MOVE(src, dest, -1, M_ATTACK);
          }
        }

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_DepriveIsland()
{
        TMove move;

        int isl = AI.SchPar[0];

        for (int dest, i = AI.FieldsToAttackNum - 1; i >= 0; i--)
        {
          dest = AI.FieldsToAttack[i];
          if (Map[dest].Island == isl)
          for (int src, j = 0; j < 8; j++)
          {
            src = Map[dest].Neighbours[j];
            if (src != -1 &&
                Map[src].Player == Player &&
                Map[src].Army > Map[dest].Army &&
                Map[src].Army > 2 &&
                AI_aux_IsStrongestArmyToAttack(src, dest))
              RETURN_MOVE(src, dest, -1, M_ATTACK);
          }
        }

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_DefendIslands()
{
        TMove move;

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_CaptureCC()
{
        TMove move;

        for (int dest, i = AI.FieldsToAttackNum - 1; i >= 0; i--)
        {
          dest = AI.FieldsToAttack[i];
          if (Map[dest].CommandCentre)
          for (int src, j = 0; j < 8; j++)
          {
            src = Map[dest].Neighbours[j];
            if (src != -1 &&
                Map[src].Player == Player &&
                Map[src].Army > Map[dest].Army &&
                Map[src].Army > 2)
              RETURN_MOVE(src, dest, -1, M_ATTACK);
          }
        }

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_DefendCC() // ??? - przenieœæ armiê do cc
{
        TMove move;

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}

TMove TPlayer::AI_4_aux_GetMove_MoveNearEnemy()
{
        TMove move;

        struct { int f, a; } fields[48], tmp;
        int fnum = 0;

        for (int fld, i = 0; i < AI.MyFieldsNum; i++)
        {
          fld = AI.MyFields[i].Field;
          if (!AI.MyFields[i].NeighbourWithEnemy &&
              Map[fld].Army > 1)
          {
            fields[fnum].f = fld;
            fields[fnum++].a = Map[fld].Army;
          }
        }

        if (fnum == 0)
          RETURN_MOVE(-1, -1, -1, M_NOMOVE);

        for (int i = 0; i < fnum; i++)
          for (int j = i + 1; j < fnum; j++)
          if (fields[j].a > fields[i].a)
            tmp = fields[j], fields[j] = fields[i], fields[i] = tmp;

        int fld = fields[0].f;

        for (int dest, i = 0; i < 8; i++)
        {
          dest = Map[fld].Neighbours[i];
          if (dest != -1 &&
              Map[dest].Player == Player &&
              AI.MyFields[AI.MyFieldsOnMap[fld]].MinNeighbourFieldsToEnemy ==
                        AI.MyFields[AI.MyFieldsOnMap[dest]].FieldsToEnemy)
            RETURN_MOVE(fld, dest, Map[fld].Army - 1, M_MOVEARMY);
        }

        RETURN_MOVE(-1, -1, -1, M_NOMOVE);
}
