#define _WIN32_WINNT 0x0502 //Needed for GetConsoleWindow()
#define WM_WSAASYNC (WM_USER +5) //for Async windows messages
#include <iostream>
#include <windows.h>
#include <gl/gl.h>
#include <fcntl.h> //for console
#include <stdio.h>
#include <cstdio>
#include <ctime>
#include <winsock2.h>

#include "definitions.h"
#include "game.h"
#include "networkCom.h"

/*Include following libraries
lobSOIL.a
OpenAL32.lib
XInput32.lib
XInput64.lib
opengl32
glu32
gdi32
ogg.lib
vorbis.lib
vorbisfile.lib
*/

int*  g_pWindow_size;
bool* g_pKeys_real;
bool* g_pKeys_translated;
int*  g_pMouse_pos;
bool* g_pMouse_but;
int   g_window_border_size=28;
game  g_game;

networkCom g_NetCom;

using namespace std;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);
bool MP_setup(HWND hwnd);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    string command_line=lpCmdLine;
    bool debug_mode=true;
    if(command_line=="debug") debug_mode=true;

    //init and reset keys
    g_pWindow_size=new int[2];
    g_pWindow_size[0]=800;
    g_pWindow_size[1]=800;
    g_pKeys_real=new bool[256];
    g_pKeys_translated=new bool[256];
    for(int i=0;i<256;i++)
    {
        g_pKeys_real[i]=false;
        g_pKeys_translated[i]=false;
    }
    g_pMouse_pos=new int[2];
    g_pMouse_pos[0]=0; g_pMouse_pos[1]=0;
    g_pMouse_but=new bool[4];
    g_pMouse_but[0]=g_pMouse_but[1]=g_pMouse_but[2]=g_pMouse_but[3]=false;

    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "Rapid Chess";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    if(!debug_mode && false)
    {
        //Detect screen resolution
        RECT desktop;
        // Get a handle to the desktop window
        const HWND hDesktop = GetDesktopWindow();
        // Get the size of screen to the variable desktop
        GetWindowRect(hDesktop, &desktop);
        // The top left corner will have coordinates (0,0)
        // and the bottom right corner will have coordinates
        // (horizontal, vertical)
        g_pWindow_size[0] = desktop.right;
        g_pWindow_size[1] = desktop.bottom;
    }

    //if debug mode start console
    HWND hwnd_console;
    if(debug_mode)
    {
        //Only output console
        AllocConsole() ;
        AttachConsole( GetCurrentProcessId() ) ;
        freopen( "CON", "w", stdout ) ;

        //Set console title
        SetConsoleTitle("Debug Console");
        hwnd_console=GetConsoleWindow();
        MoveWindow(hwnd_console,g_pWindow_size[0],0,680,510,TRUE);

        cout<<"*-------------*\n*-Rapid-Chess-*\n*-------------*\n\n";
        cout<<"Version: "<<_version<<endl<<endl;
    }
    else
    {
        //ShowCursor(FALSE);//hide cursor
        //ShowWindow(GetConsoleWindow(),SW_HIDE);//hide console
    }

    hwnd = CreateWindowEx(0,
                          "Rapid Chess",
                          "Rapid Chess",
                          //WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, //WS_OVERLAPPEDWINDOW for window
                          WS_POPUP | (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME),
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          g_pWindow_size[0],
                          g_pWindow_size[1]+g_window_border_size,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    EnableOpenGL(hwnd, &hDC, &hRC);

    //CloseWindow(hwnd_console);

    //start the game
    if( !g_game.init(&g_pWindow_size[0],&g_pKeys_real[0],&g_pKeys_translated[0],
                   &g_pMouse_pos[0],&g_pMouse_but[0]) )
    {
        cout<<"ERROR: Could not initaialize the game\n";
        if(debug_mode) system("PAUSE");
        return 1;
    }

    //MP status
    MP_setup(hwnd);

    clock_t time_now=clock();
    clock_t time_last=time_now;
    //clock_t time_sum=0;
    clock_t time_step=_time_step*1000.0;//10.0;//0.010 ms -> 100 updates per sec
    bool update_screen=true;

    bool quit=false;
    while(!quit)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message==WM_QUIT)
            {
                quit=true;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            //quick quit test
            if(g_pKeys_real[VK_ESCAPE]) quit=true;

            time_now=clock();
            while( time_last+time_step <= time_now )
            {
                time_last+=time_step;

                bool quit_game=false;
                if( !g_game.update(quit_game) )//static update
                {
                    //require reset of game
                    if( !g_game.init(&g_pWindow_size[0],&g_pKeys_real[0],&g_pKeys_translated[0],
                                   &g_pMouse_pos[0],&g_pMouse_but[0],false) )
                    {
                        cout<<"ERROR: Game could not reinitialize\n";
                        if(debug_mode) system("PAUSE");
                        return 1;
                    }

                    //MP reset
                    WSACleanup();
                    MP_setup(hwnd);
                }
                if(quit_game) quit=true;

                update_screen=true;
            }
            //draw, if anything updated
            if(update_screen)
            {
                update_screen=false;

                g_game.draw();
                SwapBuffers(hDC);
            }
        }
    }

    //clean up
    WSACleanup();
    delete[] g_pWindow_size;
    delete[] g_pKeys_real;
    delete[] g_pKeys_translated;
    delete[] g_pMouse_pos;
    delete[] g_pMouse_but;

    DisableOpenGL(hwnd, hDC, hRC);

    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        }
        break;

        case WM_DESTROY:
        {
            return 0;
        }
        break;

        case WM_MOUSEMOVE:
        {
             g_pMouse_pos[0]=LOWORD(lParam);
             g_pMouse_pos[1]=HIWORD(lParam);//+g_window_border_size;
        }
        break;

        case WM_LBUTTONDOWN:
        {
             g_pMouse_but[0]=true;
        }
        break;

        case WM_LBUTTONUP:
        {
             g_pMouse_but[0]=false;
        }
        break;

        case WM_RBUTTONDOWN:
        {
             g_pMouse_but[1]=true;

             cout<<"x: "<<g_pMouse_pos[0]<<", y: "<<g_pMouse_pos[1]<<endl; //temp
        }
        break;

        case WM_RBUTTONUP:
        {
             g_pMouse_but[1]=false;
        }
        break;

        case WM_MOUSEWHEEL:
        {
            if(HIWORD(wParam)>5000) {g_pMouse_but[2]=true;}
            if(HIWORD(wParam)>100&&HIWORD(wParam)<5000) {g_pMouse_but[3]=true;}
        }
        break;

		case WM_KEYDOWN:
		{
			g_pKeys_real[wParam]=true;

			cout<<"Pressed: "<<wParam<<endl;
		}
		break;

		case WM_KEYUP:
		{
			g_pKeys_real[wParam]=false;
		}
		break;

		case WM_WSAASYNC:
        {
            // what word?
            switch(WSAGETSELECTEVENT(lParam))
            {
                case FD_READ: //incomming data from SOCKET wParam
                {
                    cout<<"FD_READ\n";

                    g_game.recv_data(wParam);

                } break;

                case FD_WRITE: //only used if sending large files
                {
                    cout<<"FD_WRITE\n";

                } break;

                case FD_ACCEPT: // client wants to join
                {
                    cout<<"FD_ACCEPT\n";
                    if(g_NetCom.add_client(wParam))
                    {
                        //start game counter
                        g_game.start_game();

                        cout<<"New Client Joined\n";
                    }
                    else
                    {
                        cout<<"Bad Client tried to join\n";
                    }

                    return(0);

                } break;

                case FD_CONNECT: //Client is now connected to server
                {
                    cout<<"FD_CONNECT\n";

                    //Test Connection
                    if(g_NetCom.test_connection())
                    {
                        //start game counter
                        g_game.start_game();

                        cout<<"You are now connected to the server\n";
                    }
                    else//not connected
                    {
                        cout<<"Could not connect to server\n";
                        break;
                    }
                } break;

                case FD_CLOSE: //lost connection to server
                {
                    cout<<"FD_CLOSE\n";

                    if(g_NetCom.m_net_status==net_client)
                      g_NetCom.lost_connection();

                    if(g_NetCom.m_net_status==net_server)
                    {
                        if(g_NetCom.remove_client(wParam)) cout<<"Client Removed\n";
                    }

                    g_game.m_pause_game=true;

                } break;

            }
        }

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    //set 2D mode

    glClearColor(0.0,0.0,0.0,0.0);  //Set the cleared screen colour to black
    glViewport(0,0,g_pWindow_size[0],g_pWindow_size[1]);   //This sets up the viewport so that the coordinates (0, 0) are at the top left of the window

    //Set up the orthographic projection so that coordinates (0, 0) are in the top left
    //and the minimum and maximum depth is -10 and 10. To enable depth just put in
    //glEnable(GL_DEPTH_TEST)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,g_pWindow_size[0],g_pWindow_size[1],0,-1,1);

    //Back to the modelview so we can draw stuff
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Enable antialiasing
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearStencil( 0 );


}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

bool MP_setup(HWND hwnd)
{
    string ip;
    switch(g_game.get_MP_settings(ip))
    {
        case 0://SP
        {
            g_game.m_player_id=ps_none;
            g_game.start_game();
        }break;

        case 1://host
        {
            cout<<"SERVER\n";
            if(!g_NetCom.init("server")) cout<<"ERROR: Init\n";
            if(!g_NetCom.set_port_and_bind(5001)) cout<<"ERROR: Set port and bind\n";
            WSAAsyncSelect( g_NetCom.get_server_socket() , hwnd, WM_WSAASYNC, FD_READ | FD_WRITE | FD_ACCEPT | FD_CLOSE);
            int err=WSAGetLastError();
            if(err!=0) cout<<"WSAAsync err: "<<err<<endl;
            if(!g_NetCom.start_to_listen(10)) cout<<"ERROR: Listen\n";
            g_game.m_pNetCom=&g_NetCom;
            g_game.m_player_id=ps_hoster;
        }break;

        case 2://join
        {
            cout<<"CLIENT\n";
            cout<<"IP: "<<ip<<endl;
            if(!g_NetCom.init("client")) cout<<"ERROR: Init\n";
            WSAAsyncSelect( g_NetCom.get_server_socket() , hwnd, WM_WSAASYNC, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);
            int err=WSAGetLastError();
            if(err!=0) cout<<"WSAAsync err: "<<err<<endl;
            if(!g_NetCom.connect_to_server(ip,5001)) cout<<"ERROR: Connect to server\n";
            g_game.m_pNetCom=&g_NetCom;
            g_game.m_player_id=ps_joiner;
        }break;
    }

    return true;
}

