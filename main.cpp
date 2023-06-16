#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"
#include <cstdio>
#include <GLFW/glfw3.h>
#include <string>
#include "functionHandling.h"

//TODO: Add pi handling
//TODO: Asymptotes
//TODO: Make it so that the program can be easily downloaded

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char**){
    //Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    #if defined(IMGUI_IMPL_OPENGL_ES2)
        // GL ES 2.0 + GLSL 100
        const char* glsl_version = "#version 100";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #elif defined(__APPLE__)
        // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
    #else
        // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Curve Viewer", nullptr, nullptr);
    if(window == nullptr){
        return 1;
    }
    glfwMaximizeWindow(window);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);


    //SETUP Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    //input & output
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    //theme: can be light, dark or classic
    ImGui::StyleColorsDark();

    //Set up the backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io.Fonts->AddFontFromFileTTF("../segoeui.ttf", 25.0f);

    while(!glfwWindowShouldClose(window)){
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();



        //Inputs must have unique ID- to not display label, use ##[id]

        static bool inputtingFunctions = true;
        static char xFun[128] = "";
        static char yFun[128] = "";
        static bool paused = false;


        //Putting in input section
        if(inputtingFunctions){

            ImGui::Begin("Input");
            ImGui::Text("X=");
            ImGui::SameLine();
            ImGui::InputText("##InputX", xFun, IM_ARRAYSIZE(xFun));
            ImGui::SameLine();
            ImGui::TextDisabled("(Example Input)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Do not write your functions with implicit notations, such as 2t, or sin^2(t).\n"
                                       "t^2\n"
                                       "x=(sin(t))^2\n"
                                       "x=2*t+5");

                ImGui::EndTooltip();
            }


            ImGui::Text("Y=");
            ImGui::SameLine();
            ImGui::InputText("##Input Y", yFun, IM_ARRAYSIZE(yFun));
            ImGui::SameLine();
            ImGui::TextDisabled("(Example Input)");
            if (ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::TextUnformatted("Do not write your functions with implicit notations, such as 2t, or sin^2(t).\n"
                                       "t^2\n"
                                       "x=(sin(t))^2\n"
                                       "x=2*t+5");
                ImGui::EndTooltip();
            }

            ImGui::Text("\n");

            //TODO:Implement isValidFunction function that checks if the input is valid
            ImGui::Text("More complex functions will take a while to load. ");
            if (ImGui::Button("Graph")) {
                inputtingFunctions = false;
            }
            ImGui::End();
        }

//FIXME: when slider is all the way to the right, the graph disappears
        if(!inputtingFunctions){


            ImGui::Begin("Control Panel");
            ImGui::SeparatorText("Range of T");

            static float range[2] = {-10,10};
            ImGui::InputFloat2("##rangeInput",range);


            ImGui::Text("\n");
            ImGui::SeparatorText("Control");
            static float t = range[0];
            static double xPoints[10000] , yPoints[10000], tValues[10000];
            static bool needToCalculate = true;
            for(needToCalculate; needToCalculate; needToCalculate = false){
                fillTValues(tValues, range, 10000);
                generatePoints(xPoints, yPoints, tValues, yFun, xFun, 10000);
            }



            if(!paused) {
                t += (range[1]-range[0])/450;
            }
            if(t>range[1] || t<range[0]){
                t = range[0];
            }

            ImGui::Text("t=");
            ImGui::SameLine();
            ImGui::SliderFloat("##tSlider", &t, range[0], range[1]);
            ImGui::SameLine();

            if(ImGui::Button("Pause/Play")){
                paused = !paused;
            }

            ImGui::Text("\n");

            if(ImGui::Button("Refresh")){
                needToCalculate = true;
                t = range[0];
            }
            ImGui::Text("More complex functions will take a while to load");

            ImGui::Text("\n");

            if(ImGui::Button("Graph Another Function")){
                inputtingFunctions=true;
                needToCalculate = true;
                t = range[0];
            }
            ImGui::End();


            ImGui::Begin("##GraphingWindow");
            ImGui::Text("Graph of: %s, %s", xFun, yFun);
            int indexToGraphTo = indexOfTVal(t, tValues);
            ImPlot::BeginPlot("##Graph", ImVec2(-1,-1),ImPlotFlags_Equal);
            ImPlot::SetupAxis(ImAxis_X1);
            ImPlot::SetupAxis(ImAxis_Y1);
            ImPlot::PlotLine("##plottedLine", xPoints, yPoints, indexToGraphTo);
            ImPlot::EndPlot();
            ImGui::End();

        }

//TODO: figure out how to change scaling of the data

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

