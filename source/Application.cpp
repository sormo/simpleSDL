#include "Application.h"
#include <SDL.h>
#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif
#include <stdio.h>
#include <cassert>
#include "Common.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_gles2.h"
#include "OpenGL.h"

static int g_done = 0;
static SDL_GLContext g_context = nullptr;
// window is not static because it's used in common
SDL_Window* g_window = nullptr;
ResizeDispatcher Application::g_resizeDispatcher;

void Application::GuiInit()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontDefault();
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImGui_ImplSdlGLES2_Init(g_window);
}

void Application::GuiDeinit()
{
    ImGui_ImplSdlGLES2_Shutdown();
    ImGui::DestroyContext();
}

void Application::GuiRender()
{
    ImGui_ImplSdlGLES2_NewFrame(g_window);

    //static bool showDemoWindow = true;
    //if (showDemoWindow)
    //    ImGui::ShowDemoWindow(&showDemoWindow);

    Gui();

    ImGui::Render();
}

void Application::AppDeinit()
{
    Application::Deinit();

    GuiDeinit();

    if (g_context)
    {
        SDL_GL_DeleteContext(g_context);
        g_context = nullptr;
    }

    if (g_window)
    {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }

    SDL_Quit();
}

bool Application::AppInit()
{
    static const int width = 800;
    static const int height = 600;
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

#if defined(ANDROID)
    flags |= SDL_WINDOW_FULLSCREEN;
#endif

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        printf("SDL init failed: %s\n", SDL_GetError());
        SDL_ClearError();

        return false;
    }

    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // enable multisapling
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

#if defined(_DEBUG)
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG) != 0)
    {
        printf("Error setting debug flag to opengl context.\n");
    }
#endif

    g_window = SDL_CreateWindow("SimpleSDL", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
    if (!g_window)
    {
        printf("Creating window failed: %s\n", SDL_GetError());
        SDL_ClearError();
        SDL_Quit();

        return false;
    }

    g_context = SDL_GL_CreateContext(g_window);
    if (!g_context)
    {
        printf("Creating GL context failed: '%s'.\n", SDL_GetError());
        SDL_ClearError();
        SDL_DestroyWindow(g_window);
        SDL_Quit();

        return false;
    }

    GuiInit();

    if (!InitOpenGL())
    {
        printf("Error initializing OpenGl\n");
        return false;
    }

    if (!Init())
    {
        printf("Initializing application failed.\n");
        Deinit();

        return false;
    }

    printf("Initialized.\n");

    return 1;
}

void Application::RenderFrame()
{
    SDL_GL_MakeCurrent(g_window, g_context);

    if (!MainLoop())
        g_done = true;

    GuiRender();

    SDL_GL_SwapWindow(g_window);
}

void Application::ProcessFrame()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSdlGLES2_ProcessEvent(&event);

        if (ImGui::GetIO().WantCaptureMouse)
            continue;

        if (event.type == SDL_QUIT)
        {
            printf("Event: SDL_QUIT\n");
            g_done = true;
        }

        Dispatch(event);
    }

    RenderFrame();

    SDL_Delay(5);

    Common::Frame::Signal();
}

bool Application::Execute()
{
    if (!AppInit())
    {
        printf("Initialization failed.\n");
        return 1;
    }

#if defined(EMSCRIPTEN)
    emscripten_set_main_loop(mainLoop, 0, 0);
    return 0;
#else
    while (!g_done)
    {
        ProcessFrame();
    }

    AppDeinit();
    printf("All done.\n");

    return 0;
#endif
}
