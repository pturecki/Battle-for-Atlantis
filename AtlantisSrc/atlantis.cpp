
#include "atlantis.h"

#ifdef DEBUG_MODE
bool DEBUG_0 = true;
#endif


LARGE_INTEGER   OldTime;
float           TimeScale;
bool            DemoMode = false;

HCURSOR         Cursor1;
HCURSOR         Cursor2;


bool InitTimer()
{
        LARGE_INTEGER var;
        QueryPerformanceFrequency(&var);

        if (var.u.LowPart == 0 && var.u.HighPart == 0)
          return false;

        DWORD freq;
        if (var.u.HighPart > 0)
          freq = 0xffffffff;
        else
          freq = var.u.LowPart;

        TimeScale = 1000.0 / freq;

        QueryPerformanceCounter(&OldTime);

        return true;
}

DWORD GetTimerDelta()
{
        LARGE_INTEGER newtime;
        QueryPerformanceCounter(&newtime);

        DWORD delta;
        if (newtime.u.LowPart < OldTime.u.LowPart)
          delta = newtime.u.LowPart + ~OldTime.u.LowPart;
        else
          delta = newtime.u.LowPart - OldTime.u.LowPart;

        OldTime = newtime;

        return static_cast<float>(delta) * TimeScale;
}


LRESULT CALLBACK MainWndProc(HWND window, UINT msg_id, WPARAM wparam, LPARAM lparam)
{
        return Game->ProcessMessage(window, msg_id, wparam, lparam);
}


int PASCAL WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int)
{
        static char class_name[] = "MainWndClass";

        //Cursor1 = LoadCursor(inst, MAKEINTRESOURCE(IDC_CURSOR1));
        //Cursor2 = LoadCursor(inst, MAKEINTRESOURCE(IDC_CURSOR2));
        Cursor1 = LoadCursor(inst, IDC_ARROW);
        Cursor2 = LoadCursor(inst, IDC_ARROW);

// zarejestruj klasê okna
        WNDCLASS wnd_class;
        wnd_class.style         = CS_DBLCLKS;
        wnd_class.lpfnWndProc   = MainWndProc;
        wnd_class.cbClsExtra    = 0;
        wnd_class.cbWndExtra    = 0;
        wnd_class.hInstance     = inst;
        wnd_class.hIcon         = NULL;
        wnd_class.hCursor       = LoadCursor(NULL, IDC_ARROW);
//        wnd_class.hCursor       = NULL;
        wnd_class.hbrBackground = NULL;
        wnd_class.lpszMenuName  = NULL;
        wnd_class.lpszClassName = class_name;

        if (!RegisterClass(&wnd_class))
          return 0;

// stwórz okno
        DWORD style = WS_CAPTION | WS_VISIBLE;

        RECT ClientRect = { 0, 0, 800, 600 };
        AdjustWindowRect( &ClientRect, style, TRUE  );

        int scrx = GetSystemMetrics(SM_CXSCREEN);
        int scry = GetSystemMetrics(SM_CYSCREEN);
        int w    = 800 + 4 * GetSystemMetrics(SM_CXFIXEDFRAME);
        int h    = 600 + 4 * GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION);

        w = ClientRect.right - ClientRect.left;
        h = ClientRect.bottom - ClientRect.top;

        HWND window = CreateWindow(
                class_name,
                "Battle for Atlantis",
                style,
                (scrx-w) >> 1, (scry-h) >> 1, w, h,
                NULL, NULL, inst, NULL);

        if (window == NULL)
          return 0;

        // co za gowno
        GetClientRect( window, &ClientRect );
        int clw = ClientRect.right - ClientRect.left;
        int clh = ClientRect.bottom - ClientRect.top;
        int neww = w + ( 800 - clw );
        int newh = h + ( 600 - clh );

        SetWindowPos( window, NULL, ( scrx - neww ) >> 1, ( scry - newh ) >> 1, neww, newh, 0 );

        InitTimer();

        Game = new TGame(window);

// pêtla komunikatów
        MSG msg;
        DWORD time_delta;
        while (Game->Running())
        {
          time_delta = GetTimerDelta();
          if (time_delta < 1)
		  {
			Sleep(1);
			time_delta = 1;
		  }
          
          Game->Tick(time_delta);

          if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
          {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		  }
		  else
		  {
			Sleep(1);
		  }
        }

        delete Game;

        return 1;
}

void DeleteComponent(int* arr, int comp, int size)
{
        if (comp == size - 1)
          return;

        for (int i = comp; i < size - 1; i++)
          arr[i] = arr[i + 1];  
}
