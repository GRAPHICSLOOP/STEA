#include "UIPass.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include "function/global/RuntimeGlobalContext.h"

UIPass::~UIPass()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIPass::initialize(const UIPassConfigParam& config)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    mConfig = config;
    initializeUIRenderBackend();
}

void UIPass::initializeUIRenderBackend()
{
    ImGui_ImplGlfw_InitForVulkan(gRuntimeGlobalContext.getWindowSystem()->getWindow(), true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = gRuntimeGlobalContext.getRHI()->mInstance;
    init_info.PhysicalDevice = gRuntimeGlobalContext.getRHI()->mPhyDevice;
    init_info.Device = gRuntimeGlobalContext.getRHI()->mDevice;
    init_info.QueueFamily = gRuntimeGlobalContext.getRHI()->mQueueFamilyIndices.graphicsIndices.value();
    init_info.Queue = gRuntimeGlobalContext.getRHI()->mGraphicsQueue;
    init_info.DescriptorPool = gRuntimeGlobalContext.getRHI()->mDescriptorPool;
    init_info.Subpass = 1; //init_info.Subpass = _main_camera_subpass_ui;
    

    // may be different from the real swapchain image count
    // see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
    init_info.MinImageCount = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mImageCount;
    init_info.ImageCount = gRuntimeGlobalContext.getRHI()->mSwapchainSupportDetails.mImageCount;
    ImGui_ImplVulkan_Init(&init_info, mConfig.mRenderPass);

    uploadFonts();
}

void UIPass::uploadFonts()
{
    vk::CommandPool command_pool = gRuntimeGlobalContext.getRHI()->mCommandPool;
    vk::CommandBuffer command_buffer = gRuntimeGlobalContext.getRHI()->mCommandBuffer;

    gRuntimeGlobalContext.getRHI()->mDevice.resetCommandPool(command_pool);

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    command_buffer.begin(beginInfo);

    ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

    vk::SubmitInfo endInfo;
    endInfo.commandBufferCount = 1;
    endInfo.pCommandBuffers = &command_buffer;
    command_buffer.end();
    gRuntimeGlobalContext.getRHI()->mGraphicsQueue.submit(endInfo, VK_NULL_HANDLE);

    gRuntimeGlobalContext.getRHI()->mDevice.waitIdle();
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void UIPass::drawPass()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // begine windowrender
    // Our state
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
    // end

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), gRuntimeGlobalContext.getRHI()->mCommandBuffer);

}
