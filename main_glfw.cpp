// Dear ImGui: standalone example application for Emscripten, using SDL2 + OpenGL3
// (Emscripten is a C++-to-javascript compiler, used to publish executables for the web. See https://emscripten.org/)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// This is mostly the same code as the SDL2 + OpenGL3 example, simply with the modifications needed to run on Emscripten.
// It is possible to combine both code into a single source file that will compile properly on Desktop and using Emscripten.
// See https://github.com/ocornut/imgui/pull/2492 as an example on how to do just that.

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include <stdio.h>
#include <emscripten.h>
#include <SDL.h>
#include <GLFW/glfw3.h>
#include "SimpleLogger.h"

#include <array>

GLFWwindow* window;

// For clarity, our main loop code is declared at the end.
static void main_loop();

ExampleAppLog imguiLog;

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOther(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // This function call won't return, and will engage in an infinite loop, processing events from the browser, and dispatching them.
    emscripten_set_main_loop(main_loop, 0, 1);
}

struct JoyStickState
{
    int axes_count;
    float axes[16];
    int button_count;
    unsigned char buttons[16];
} last_joystick_state[16] = { 0 };

bool update_joystick(int glfw_joystick_id, JoyStickState& prevState)
{
    if (!glfwJoystickPresent(glfw_joystick_id))
        return false;

    const char* name = glfwGetJoystickName(glfw_joystick_id);

    int axes_count = 0;
    const float *axes = glfwGetJoystickAxes(glfw_joystick_id, &axes_count);

    int button_count = 0;
    const unsigned char *buttons = glfwGetJoystickButtons(glfw_joystick_id, &button_count);

    prevState.axes_count = axes_count;
    for (int i = 0; i < axes_count; ++i) {
        if (i == 8) continue;

        if (prevState.axes[i] != axes[i] &&
            i != 0 && i != 1)
            imguiLog.AddLog("(%d %s) axis %d = %f\n", glfw_joystick_id, name, i, axes[i]);

        prevState.axes[i] = axes[i];
    }

    prevState.button_count = button_count;
    for (int i = 0; i < button_count; ++i)
    {
        if (prevState.buttons[i] != buttons[i])
            imguiLog.AddLog("(%d %s) button %d = %d\n", glfw_joystick_id, name, i, buttons[i]);

        prevState.buttons[i] = buttons[i];
    }

    return true;
}

void draw_plot(float x, float y)
{
    if (ImPlot::BeginPlot("Scatter Plot", "X", "Y"))
    {
        ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Square, 6, ImVec4(0, 1, 0, 0.5f), IMPLOT_AUTO,
                                                                                          ImVec4(0, 1, 0, 1));
        ImPlot::PlotScatter("Main axis", &x, &y, 1);
        ImPlot::PopStyleVar();
    }
    ImPlot::EndPlot();
}


static void main_loop()
{
    ImGuiIO& io = ImGui::GetIO();

    // Our state (make them static = more or less global) as a convenience to keep the example terse.
    static bool show_demo_window = true;
    static bool show_plot_demo_window = true;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    if (show_plot_demo_window)
        ImPlot::ShowDemoWindow(&show_plot_demo_window);

    ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Joystick widget");

    if (update_joystick(GLFW_JOYSTICK_1, last_joystick_state[0]))
        draw_plot(last_joystick_state[0].axes[0], last_joystick_state[0].axes[1]);

    ImGui::End();
    imguiLog.Draw("Joystick widget");

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}
