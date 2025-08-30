#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkInstance vulkInstance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
    VkQueue graphicQueue;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // We don't need the openGL stuff
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Important for swapchain shenanigans

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        // Overview VkInstance(the Vulkan API Context) -> VkPhysicalDevice(a reference to a GPU) -> VkDevice(a reference to the GPU Driver)
        createInstance();
        pickPhysicalDevice();
        createLogicalDevice();

        // A swapchain is a series of buffers/images you can draw to then later display to the screen.
        // NOTE: swapchains are created for a given size if the screen is resized then you need to recreate the swapchain.

    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        // Specify the queues to be created
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // Specify the device features to use
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Create the logical device
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device.");
        }
    }

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete() {
            return graphicsFamily.has_value();
        }
    };

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vulkInstance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("Failed to find any GPUs with Vulkan Support.");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vulkInstance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("Failed to find GPU.");
        }
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        //VkPhysicalDeviceProperties deviceProperties;
        //vkGetPhysicalDeviceProperties(device, &deviceProperties);

        //VkPhysicalDeviceFeatures deviceFeatures;
        //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = findQueueFamilies(device);


        // TODO: give score to available GPUs
        return indices.isComplete();
    }

    void createInstance() {
        // Setup Vulkan validation layers
        const VkBool32 verboseValue = true;
        const VkLayerSettingEXT layerSettings = {"VK_LAYER_KHRONOS_validation", "printf_verbose", VK_LAYER_SETTING_TYPE_BOOL32_EXT, 1, &verboseValue};
        VkLayerSettingsCreateInfoEXT layerSettingsCreateInfo = { VK_STRUCTURE_TYPE_LAYER_SETTINGS_CREATE_INFO_EXT, nullptr, 1, &layerSettings };

        // Specify the application info for Vulkan
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Space Invaders";
        appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0); // This application's version
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3; // The version of the vulkan api to use

        // Setup the create info with the application info and get the required extension to run in a glfw context
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount; // NOTE: these are ignored in modern implementation of vulkan
        createInfo.ppEnabledExtensionNames = glfwExtensions; // NOTE: setting them up to be compatbile with older implementations
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = &layerSettingsCreateInfo;

        //VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkInstance);
        // This is also possible on most calls for error checking and handling
        if (vkCreateInstance(&createInfo, nullptr, &vulkInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        // Cleanup vulkan
        vkDestroyInstance(vulkInstance, nullptr);
        vkDestroyDevice(device, nullptr);

        // Cleanup glfw
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}