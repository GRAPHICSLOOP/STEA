#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define DESCRIPTORMAXCOUNT_SAMPLE 32
#define DESCRIPTORMAXCOUNT_BUFFER 4


struct SwapchainSupportDetails
{
	vk::SurfaceCapabilitiesKHR mCapabilities;
	vk::SurfaceFormatKHR mFormat;
	vk::PresentModeKHR mPresentMode;
	vk::Extent2D mExtent2D;
	uint32_t mImageCount;
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsIndices; // 图形处理队列
	std::optional<uint32_t> presentIndices; // 图形显示队列

	bool isComplete()
	{
		return graphicsIndices.has_value() && presentIndices.has_value();
	}
};

class VulkanRHI
{
public:
	~VulkanRHI();
	void initialize();
	vk::CommandBuffer beginSingleTimeBuffer();
	void endSingleTimeBuffer(vk::CommandBuffer commandBuffer);
	void prepareBeforePass();
	void submitRendering();
	const uint32_t getNextImageIndex() const;

public:
	vk::Instance mInstance;
	vk::PhysicalDevice mPhyDevice;
	vk::Device mDevice;
	vk::DescriptorPool mDescriptorPool;
	std::vector<vk::Image> mSwapchainImages;
	std::vector<vk::ImageView> mSwapchainImageViews;

	vk::CommandPool mCommandPool;
	vk::CommandBuffer mCommandBuffer;

	vk::Semaphore mImageAvailableSemaphore;
	vk::Semaphore mRenderFinishedSemaphore;
	vk::Fence mFence;
	vk::Queue mGraphicsQueue;
	vk::Queue mPresentQueue;

	SwapchainSupportDetails mSwapchainSupportDetails;
	QueueFamilyIndices mQueueFamilyIndices;

private:
	std::array<const char*, 1> mEnableLayerNames = { "VK_LAYER_KHRONOS_validation" };
	uint32_t nextImageIndex;

private:
	VkSurfaceKHR mSurfaceKHR;
	vk::SwapchainKHR mSwapchain;


private:
	void createInstance();
	void createWindowSurface();
	void initializePhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();
	void createCommandBuffers();
	void createDescriptorPool();
	void createSyncPrimitives();
	void createSwapchain();
	void createSwapchainImageViews();
	void createFramebufferImageAndView();

private:
	void findQueueFamilyIndeices();
	void findSwapchainSupport();

	vk::SurfaceFormatKHR chooseSwapchainSurfaceFormatFromDetails(std::vector<vk::SurfaceFormatKHR> format);
	vk::PresentModeKHR chooseSwapchainPresentModeFromDetails(std::vector<vk::PresentModeKHR> mode);
	vk::Extent2D chooseSwapchainExtentFromDetails(vk::SurfaceCapabilitiesKHR cap);
};


