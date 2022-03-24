#define DECLARE_AI_FUNCTIONS(diff)                              \
        static TMove AI_##diff##_GetMove();                     \
        static bool AI_##diff##_ContinueMove(TMove* move);      \
        static void AI_##diff##_MoveArmy(TMove* move);          \
        static void AI_##diff##_PlaceArmy()

#define IN_RANGE(p, min, max) ( ((p) >= (min)) && ((p) <= (max)) )

        DECLARE_AI_FUNCTIONS(0);
        DECLARE_AI_FUNCTIONS(1);
        DECLARE_AI_FUNCTIONS(2);
        DECLARE_AI_FUNCTIONS(3);
        DECLARE_AI_FUNCTIONS(4);
        DECLARE_AI_FUNCTIONS(5);
        DECLARE_AI_FUNCTIONS(6);
        DECLARE_AI_FUNCTIONS(7);
        DECLARE_AI_FUNCTIONS(8);
        DECLARE_AI_FUNCTIONS(9);

        static void AddSch(TScheme sch, int par, bool always_add = false);

        // DiffLevel == 4

        static void AI_4_aux_PlaceArmy_CaptureSmallIsland(int& army);
        static void AI_4_aux_PlaceArmy_AttackStrongestPlayer(int& army);
        static void AI_4_aux_PlaceArmy_CaptureIsland(int& army);
        static void AI_4_aux_PlaceArmy_Attack(int& army);
        static void AI_4_aux_PlaceArmy_DepriveIsland(int& army);
        static void AI_4_aux_PlaceArmy_DefendIslands(int& army);
        static void AI_4_aux_PlaceArmy_CheckCC(int& army);

        static TMove AI_4_aux_GetMove_AttackPlayer();
        static TMove AI_4_aux_GetMove_CaptureIsland();
        static TMove AI_4_aux_GetMove_Attack();
        static TMove AI_4_aux_GetMove_DepriveIsland();
        static TMove AI_4_aux_GetMove_DefendIslands();
        static TMove AI_4_aux_GetMove_CaptureCC();
        static TMove AI_4_aux_GetMove_DefendCC();

        static TMove AI_4_aux_GetMove_MoveNearEnemy();
