#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> 

#include <stdio.h>
#include <string>       
#include <sstream>      

#include "backend.h"   

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const ImVec4 CLEAR_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


void renderUI(SystemMonitor& monitor, SystemInfo& info, float fps) {
    ImGui::Begin("Meu Monitor de Sistema");

    
    ImGui::Text("Uso de RAM:");
    std::stringstream ssRam;
    double usedGB = (double)info.ramUsedBytes / (1024 * 1024 * 1024);
    double totalGB = (double)info.ramTotalBytes / (1024 * 1024 * 1024);

    ssRam.precision(1);
    ssRam << std::fixed << usedGB << " GB / " << totalGB << " GB (" << (int)info.ramUsagePercentage << "%)";
    std::string ramText = ssRam.str();
    ImGui::ProgressBar(info.ramUsagePercentage / 100.0, ImVec2(-1.0f, 0.0f), ramText.c_str());

    
    ImGui::Text("Carga da CPU:");
    std::stringstream ssCpu;
    ssCpu.precision(1);
    ssCpu << std::fixed << info.cpuLoadPercentage << "%";
    std::string cpuText = ssCpu.str();
    ImGui::ProgressBar(info.cpuLoadPercentage / 100.0, ImVec2(-1.0f, 0.0f), cpuText.c_str());

    ImGui::Separator();

    
    ImGui::Text("Lista de Processos (Ordenado por Memória)");

    
    if (ImGui::BeginTable("processTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable)) {
        
        ImGui::TableSetupColumn("PID");
        ImGui::TableSetupColumn("Nome");
        ImGui::TableSetupColumn("CPU %"); 
        ImGui::TableSetupColumn("Memória (RSS)");
        ImGui::TableSetupColumn("Ação");
        ImGui::TableHeadersRow();

        
        for (const ProcessInfo& p : info.processes) {
            ImGui::TableNextRow();

            
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%lu", p.pid);

            
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", p.name.c_str());

            
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%.1f %%", p.cpuUsagePercentage);
            
            ImGui::TableSetColumnIndex(3);
            double memoryMB = (double)p.memoryUsedBytes / (1024 * 1024);
            ImGui::Text("%.1f MB", memoryMB);

            ImGui::TableSetColumnIndex(4);
            ImGui::PushID(p.pid);
            if (ImGui::Button("Encerrar")) {
                monitor.killProcess(p.pid);
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }


    
    ImGui::Separator();
    ImGui::Text("Desempenho da UI: %.1f FPS (%.3f ms/frame)", fps, 1000.0f / fps);

    ImGui::End();
}


int main(int, char**) {
    if (!glfwInit()) {
        fprintf(stderr, "Falha ao inicializar GLFW\n");
        return 1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Meu Monitor de Sistema", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Falha ao criar janela GLFW\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    SystemMonitor monitor;
    monitor.start();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        SystemInfo currentInfo = monitor.getLatestInfo();
        renderUI(monitor, currentInfo, io.Framerate);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}