#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "../Global.h"
#include "../Sprite.h"
class Console {


    struct LogItem
    {
        std::string text;
        ImVec4 color;
    };

    std::vector<LogItem> items;
    char inputBuf[256]{};

    bool autoScroll = true;
    bool scrollToBottom = false;
public:
    void Clear()
    {
        items.clear();
    }

    void AddLog(const char* text, ImVec4 color = ImVec4(1, 1, 1, 1))
    {
        items.push_back({ text, color });
        scrollToBottom = true;
    }
    void Draw(const char* title, bool* p_open = nullptr)
    {
        ImGui::Begin(title, p_open);

        // Clear button
        if (ImGui::Button("Clear"))
        {
            Clear();
        }

        ImGui::Separator();

        // Log area
        ImGui::BeginChild("ScrollingRegion",
            ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
            false,
            ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto& item : items)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, item.color);
            ImGui::TextUnformatted(item.text.c_str());
            ImGui::PopStyleColor();
        }

        if (scrollToBottom ||
            (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
        {
            ImGui::SetScrollHereY(1.0f);
        }

        scrollToBottom = false;

        ImGui::EndChild();

        ImGui::Separator();

        // Input
        if (ImGui::InputText("Input", inputBuf, sizeof(inputBuf),
            ImGuiInputTextFlags_EnterReturnsTrue))
        {
            ExecCommand(inputBuf);
            inputBuf[0] = '\0';
        }

        ImGui::End();
    }
    void ExecCommand(const char* command)
    {
        AddLog(command, ImVec4(0, 1, 0, 1));

        if (strcmp(command, "clear") == 0)
        {
            Clear();
        }
        else if (strcmp(command, "help") == 0)
        {
            AddLog("Commands:", ImVec4(1, 1, 0, 1));
            AddLog(" help", ImVec4(1, 1, 1, 1));
            AddLog(" clear", ImVec4(1, 1, 1, 1));
        }
        else
        {
            AddLog("Unknown command", ImVec4(1, 0, 0, 1));
        }
    }

};
class Console;
extern Sprite* spt;
class WindowGUI
{

public:
    static void check_vk_result(VkResult err)
    {
        if (err == VK_SUCCESS)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            assert(0);
    }
    static ImGuiIO* pio;
    static Console* console;
	
	static void Draw(VkCommandBuffer commandBuffer) {

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            static float f = 0.0f;
            static int counter = 0;
            static bool check = false;
            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &check);                 // Edit bools storing our window open/close state
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / pio->Framerate, pio->Framerate);
            ImGui::End();
        }
        {
            ImGui::Begin("Camera");                                 // Create a window called "Hello, world!" and append into it.
            ImGui::SliderFloat("x",&g_Cam2d.position.x,0.0f,500.0f);     
            ImGui::SliderFloat("y", &g_Cam2d.position.y, 0.0f, 500.0f);

            ImGui::SliderFloat("left", &g_Cam2d.left, 0.0f, 1000.0f);
            ImGui::SliderFloat("right", &g_Cam2d.right, 0.0f, 1000.0f);
            ImGui::SliderFloat("bottom", &g_Cam2d.bottom, 0.0f, 1000.0f);
            ImGui::SliderFloat("top", &g_Cam2d.top, 0.0f, 1000.0f);
            ImGui::SliderFloat("near", &g_Cam2d.near, -20.0f, 20.0f);
            ImGui::SliderFloat("far", &g_Cam2d.far, -20.0f, 20.0f);

            ImGui::SliderFloat("zoom", &g_Cam2d.zoom, 0.1f, 10.0f);
            
            ImGui::End();                                           // SDsssds
        }
        {

            ImGui::Begin("Sprite");                                 // Create a window called "Hello, world!" and append into it.
            ImGui::SliderFloat("x", &spt->trans2d.position.x, 0.0f, 500.0f);
            ImGui::SliderFloat("y", &spt->trans2d.position.y, 0.0f, 500.0f);
            ImGui::SliderFloat("rotation", &spt->trans2d.rotation, 0.0f, 360.0f);
            ImGui::SliderFloat("scale-x", &spt->trans2d.scale.x, 0.1f, 2.0f);
            ImGui::SliderFloat("scale-y", &spt->trans2d.scale.y, 0.1f, 2.0f);
            ImGui::SliderFloat("pivot-x", &spt->trans2d.pivot.x, 0.0f, 500.0f);
            ImGui::SliderFloat("pivot-y", &spt->trans2d.pivot.y, 0.0f, 500.0f);
            ImGui::End();

        }
       // console->Draw("console");
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        ImGui_ImplVulkan_RenderDrawData(draw_data, commandBuffer);

	}
	static void clean()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

	}
};

