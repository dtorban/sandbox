#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <cstdlib>
#include <map>
#include <vector>
#include <optional>
#include <set>
#include <fstream>
#include <string.h>

class HelloTriangleApplication {
public:
    void run() {
	    initWindow();
	    initVulkan();
	    mainLoop();
	    cleanup();
	}

private:
    void initWindow() {

    	std::cout << VK_KHR_SWAPCHAIN_EXTENSION_NAME << std::endl;
	    glfwInit();

	    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

    void initVulkan() {
    	std::cout << "createInstance" << std::endl;
	    createInstance();
		setupDebugMessenger();
    	std::cout << "createSurface" << std::endl;
	    createSurface();
    	std::cout << "pickPhysicalDevice" << std::endl;
	    pickPhysicalDevice();
	    checkDeviceExtensionSupport(physicalDevice);
    	std::cout << "createLogicalDevice" << std::endl;
	    createLogicalDevice();  
    	std::cout << "createSwapChain" << std::endl;  
	    createSwapChain();
    	std::cout << "createImageViews" << std::endl;
	    createImageViews();  
    	std::cout << "createGraphicsPipeline" << std::endl; 
	    createGraphicsPipeline();
	}

	void setupDebugMessenger() {
	    if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		    throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	    if (func != nullptr) {
	        func(instance, debugMessenger, pAllocator);
	    }
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	    if (func != nullptr) {
	        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	    } else {
	        return VK_ERROR_EXTENSION_NOT_PRESENT;
	    }
	}

	bool checkValidationLayerSupport() {
	    uint32_t layerCount;
	    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	    std::vector<VkLayerProperties> availableLayers(layerCount);
	    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	    std::cout << layerCount << std::endl;

	    for (const char* layerName : validationLayers) {
		    bool layerFound = false;

		    for (const auto& layerProperties : availableLayers) {
		    	std::cout << layerProperties.layerName << std::endl;
		        if (strcmp(layerName, layerProperties.layerName) == 0) {
		            layerFound = true;
		            break;
		        }
		    }

		    if (!layerFound) {
		        return false;
		    }
		}

		return true;
	}


	void createGraphicsPipeline() {
	    auto vertShaderCode = readFile("shaders/vert.spv");
	    auto fragShaderCode = readFile("shaders/frag.spv");
	}

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS) {
			    throw std::runtime_error("failed to create image views!");
			}
		}
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
	    uint32_t extensionCount;
	    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	    for (const auto& extension : availableExtensions) {
	    	std::cout << extension.extensionName << std::endl;
	        requiredExtensions.erase(extension.extensionName);
	    }

	    return requiredExtensions.empty();
	}

	void createSwapChain() {

		std::cout << "start" << std::endl;
				bool swapChainAdequate = false;
				std::cout << "start-z" << std::endl;

		//if(extensionsSupported) {
		    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
		    swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		//}
		std::cout << "adequate " << swapChainAdequate << std::endl;

	    //SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
		//std::cout << "start1" << std::endl;

	    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		std::cout << "start2" << std::endl;
	    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		std::cout << "start3" << std::endl;
	    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		std::cout << "starta" << std::endl;
	    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		    imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		std::cout << "startb" << std::endl;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		// try removing imageExtent for validation
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


		std::cout << "startc" << std::endl;
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};


		std::cout << "startd" << std::endl;
		if (indices.graphicsFamily != indices.presentFamily) {
			std::cout << "startd2" << std::endl;
		    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		    createInfo.queueFamilyIndexCount = 2;
		    createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
		    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			std::cout << "startd3" << std::endl;
		    createInfo.queueFamilyIndexCount = 0; // Optional
		    createInfo.pQueueFamilyIndices = nullptr; // Optional
		}
		std::cout << "starte" << std::endl;

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		std::cout << "startf " << device << " " << swapChain << " " << surface << std::endl;
		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		    
			std::cout << "starth" << std::endl;
		    throw std::runtime_error("failed to create swap chain!");
		}


		std::cout << "startg" << std::endl;

		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}


	void createSurface() {
	    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
	        throw std::runtime_error("failed to create window surface!");
	    }
	}

	struct QueueFamilyIndices {
	    std::optional<uint32_t> graphicsFamily;
    	std::optional<uint32_t> presentFamily;

	    bool isComplete() {
	        return graphicsFamily.has_value() && presentFamily.has_value();
	    }
	};

	struct SwapChainSupportDetails {
	    VkSurfaceCapabilitiesKHR capabilities;
	    std::vector<VkSurfaceFormatKHR> formats;
	    std::vector<VkPresentModeKHR> presentModes;
	};

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
	    SwapChainSupportDetails details;

	    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	    uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
		    details.formats.resize(formatCount);
		    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
		    details.presentModes.resize(presentModeCount);
		    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

	    return details;

	    // is suitable: return indices.isComplete() && extensionsSupported && swapChainAdequate;

	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	    std::cout << "Avaliable formats " << availableFormats.size() << std::endl;
	    for (const auto& availableFormat : availableFormats) {
	    	std::cout << "Format " <<  availableFormat.format << std::endl;
	        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
	            return availableFormat;
	        }
	    }

	    return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	    for (const auto& availablePresentMode : availablePresentModes) {
	        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
	            return availablePresentMode;
	        }
	    }

	    return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
	        return capabilities.currentExtent;
	    } else {
	        VkExtent2D actualExtent = {WIDTH, HEIGHT};

	        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
	        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

	        return actualExtent;
	    }
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
	    QueueFamilyIndices indices;

	    uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {

		    if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
		        indices.graphicsFamily = i;
		    }

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
			    indices.presentFamily = i;
			}

		    if (indices.isComplete()) {
		        break;
		    }


		    i++;
		}


	    return indices;
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
		    VkDeviceQueueCreateInfo queueCreateInfo = {};
		    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		    queueCreateInfo.queueFamilyIndex = queueFamily;
		    queueCreateInfo.queueCount = 1;
		    queueCreateInfo.pQueuePriorities = &queuePriority;
		    queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		//createInfo.enabledExtensionCount = 0;

		if (enableValidationLayers) {
		    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		    createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
		    createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		    throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

    void mainLoop() {
	    while (!glfwWindowShouldClose(window)) {
	        glfwPollEvents();
	    }
	}

    void cleanup() {
    	for (auto imageView : swapChainImageViews) {
	        vkDestroyImageView(device, imageView, nullptr);
	    }

    	vkDestroySwapchainKHR(device, swapChain, nullptr);
    	vkDestroyDevice(device, nullptr);
    	vkDestroySurfaceKHR(instance, surface, nullptr);

    	if (enableValidationLayers) {
	        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	    }

	    vkDestroyInstance(instance, nullptr);

	    glfwDestroyWindow(window);

	    glfwTerminate();
	}

	void createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
	        throw std::runtime_error("validation layers requested, but not available!");
	    }

	    VkApplicationInfo appInfo = {};
	    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	    appInfo.pApplicationName = "Hello Triangle";
	    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	    appInfo.pEngineName = "No Engine";
	    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	    appInfo.apiVersion = VK_API_VERSION_1_0;

	    VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::cout << "count " << glfwExtensionCount << std::endl;
		for (int f = 0; f < glfwExtensionCount; f++) {
			std::cout << glfwExtensions[f] << std::endl;
		}

		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if (enableValidationLayers) {
		    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		    createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
		    createInfo.enabledLayerCount = 0;
		}

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		    throw std::runtime_error("failed to create instance!");
		}
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	    VkDebugUtilsMessageTypeFlagsEXT messageType,
	    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	    void* pUserData) {

	    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	    return VK_FALSE;
	}

	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	    // Use an ordered map to automatically sort candidates by increasing score
	    std::multimap<int, VkPhysicalDevice> candidates;

	    for (const auto& device : devices) {
	        int score = rateDeviceSuitability(device);
	        candidates.insert(std::make_pair(score, device));
	    }

	    // Check if the best candidate is suitable at all
	    if (candidates.rbegin()->first > 0) {
	    	std::cout << "picked" << " " << physicalDevice << std::endl;
	        physicalDevice = candidates.rbegin()->second;

	    	std::cout << "picked" << " " << physicalDevice << std::endl;
	    } else {
	        throw std::runtime_error("failed to find a suitable GPU!");
	    }
	}

	int rateDeviceSuitability(VkPhysicalDevice device) {
		return 1;
	}

	std::vector<const char*> getRequiredExtensions() {
	    uint32_t glfwExtensionCount = 0;
	    const char** glfwExtensions;
	    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	    if (enableValidationLayers) {
	        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	    }

	    return extensions;
	}

	static std::vector<char> readFile(const std::string& filename) {
	    std::ifstream file(filename, std::ios::ate | std::ios::binary);

	    std::cout << filename << std::endl;
	    if (!file.is_open()) {
	        throw std::runtime_error("failed to open file!");
	    }

	    size_t fileSize = (size_t) file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	GLFWwindow* window;
	VkInstance instance;
	const uint32_t WIDTH = 800;
	const uint32_t HEIGHT = 600;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkDebugUtilsMessengerEXT debugMessenger;

	const std::vector<const char*> validationLayers = {
	    //"VK_LAYER_KHRONOS_validation"
	    "VK_LAYER_LUNARG_standard_validation"
	};

	#ifdef NDEBUG
	    const bool enableValidationLayers = false;
	#else
	    const bool enableValidationLayers = true;
	#endif

	const std::vector<const char*> deviceExtensions = {
	    VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}