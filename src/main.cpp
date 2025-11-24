#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> 

#include <stdio.h>
#include <string>       
#include <sstream>      
#include <iomanip> 

#include "backend.h"   

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const ImVec4 CLEAR_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

std::string formatBytes(unsigned long long bytes) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    if (bytes > (1024 * 1024 * 1024)) { // GB
        ss << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
    }
    else if (bytes > (1024 * 1024)) { // MB
        ss << (bytes / (1024.0 * 1024.0)) << " MB";
    }
    else if (bytes > 1024) { // KB
        ss << (bytes / 1024.0) << " KB";
    }
    else {
        ss << bytes << " B";
    }
    return ss.str();
}

void renderUI_ProcessTab(SystemMonitor& monitor, SystemInfo& info) {

    static unsigned long focusedPid = 0;
    static ProcessInfo focusedProcessInfo;
    bool foundFocusedProcess = false;

    ImGui::Columns(2, "ProcessSplitter", true);
    ImGui::SetColumnWidth(0, ImGui::GetWindowWidth() * 0.7f);

    ImGui::Text("Lista de Processos");
    if (ImGui::BeginChild("ProcessListChild", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_None)) {
        if (ImGui::BeginTable("processTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("PID");
            ImGui::TableSetupColumn("Nome");
            ImGui::TableSetupColumn("CPU %");
            ImGui::TableSetupColumn("Memoria");
            ImGui::TableSetupColumn("");
            ImGui::TableHeadersRow();

            for (const ProcessInfo& p : info.processes) {
                ImGui::TableNextRow();

                if (p.pid == focusedPid) {
                    focusedProcessInfo = p;
                    foundFocusedProcess = true;
                }

                ImGui::TableSetColumnIndex(0);
                char pidStr[32];
                sprintf(pidStr, "%lu", p.pid);

                if (ImGui::Selectable(pidStr, p.pid == focusedPid, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                    focusedPid = p.pid;
                    monitor.setFocusedProcess(p.pid);
                    focusedProcessInfo = p;
                    foundFocusedProcess = true;
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", p.name.c_str());

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%.1f %%", p.cpuUsagePercentage);

                ImGui::TableSetColumnIndex(3);
                std::string memStr = formatBytes(p.memoryUsedBytes);
                ImGui::Text("%s", memStr.c_str());

                ImGui::TableSetColumnIndex(4);
                ImGui::PushID(p.pid);
                if (ImGui::Button("Encerrar")) {
                    monitor.killProcess(p.pid);
                    if (p.pid == focusedPid) {
                        focusedPid = 0;
                        monitor.setFocusedProcess(0);
                    }
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();

    ImGui::NextColumn();

    ImGui::Text("Detalhes do Processo");
    ImGui::Separator();

    if (!foundFocusedProcess) {
        focusedPid = 0;
        monitor.setFocusedProcess(0);
    }

    if (ImGui::BeginChild("DetailsChild", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()))) {
        if (focusedPid == 0) {
            ImGui::Text("Clique no PID de um processo na tabela para ver os detalhes.");
        }
        else {
            ExtraProcessInfo extraInfo = monitor.getExtraProcessInfo(focusedPid);

            ImGui::Text("PID: %lu", focusedProcessInfo.pid);
            ImGui::Text("Nome: %s", focusedProcessInfo.name.c_str());
            ImGui::Separator();

            std::string memStr = formatBytes(focusedProcessInfo.memoryUsedBytes);
            ImGui::Text("Memoria: %s", memStr.c_str());

            ImGui::Text("CPU: %.1f %%", focusedProcessInfo.cpuUsagePercentage);
            ImGui::Separator();
            ImGui::Text("Contagem de Threads: %lu", extraInfo.threadCount);
            ImGui::Separator();

            std::string readStr = formatBytes(extraInfo.ioReadBytes);
            ImGui::Text("I/O Leitura: %s", readStr.c_str());


            std::string writeStr = formatBytes(extraInfo.ioWriteBytes);
            ImGui::Text("I/O Escrita: %s", writeStr.c_str());

            ImGui::Separator();
            ImGui::Text("Uso de CPU por Thread:");

            if (ImGui::BeginChild("ThreadListChild", ImVec2(0, 0), false, ImGuiWindowFlags_None)) {
                if (ImGui::BeginTable("ThreadTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
                    ImGui::TableSetupColumn("Thread ID (TID)");
                    ImGui::TableSetupColumn("CPU %");
                    ImGui::TableHeadersRow();
                    for (const ThreadInfo& t : info.focusedProcessThreads) {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%lu", t.tid);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%.1f %%", t.cpuUsagePercentage);
                    }
                    ImGui::EndTable();
                }
            }
            ImGui::EndChild();
        }
    }
    ImGui::EndChild();

    ImGui::Columns(1);
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
    SystemInfo currentInfo;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        ImGui::SetNextWindowSize(ImVec2(display_w, display_h));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

        ImGui::Begin("Monitor de Sistema Principal", NULL, window_flags);

        monitor.getLatestInfo(currentInfo);

        ImGui::Text("Uso de RAM:");
        std::stringstream ssRam;
        ssRam << std::fixed << std::setprecision(1)
            << (currentInfo.ramUsedBytes / (1024.0 * 1024.0 * 1024.0)) << " GB / "
            << (currentInfo.ramTotalBytes / (1024.0 * 1024.0 * 1024.0)) << " GB ("
            << (int)currentInfo.ramUsagePercentage << "%%)";
        ImGui::ProgressBar(currentInfo.ramUsagePercentage / 100.0, ImVec2(-1.0f, 0.0f), ssRam.str().c_str());
        ImGui::Spacing();
        ImGui::Text("Carga da CPU:");
        std::stringstream ssCpu;
        ssCpu << std::fixed << std::setprecision(1) << currentInfo.cpuLoadPercentage << " %%";
        ImGui::ProgressBar(currentInfo.cpuLoadPercentage / 100.0, ImVec2(-1.0f, 0.0f), ssCpu.str().c_str());
        ImGui::Separator();

        if (ImGui::BeginTabBar("MainTabBar")) {
            if (ImGui::BeginTabItem("Processos")) {
                renderUI_ProcessTab(monitor, currentInfo);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::Separator();
        ImGui::Text("Desempenho da UI: %.1f FPS (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);

        ImGui::End();

        ImGui::Render();
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