#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>

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

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        // Overview VkInstance(the Vulkan API Context) -> VkPhysicalDevice(a reference to a GPU) -> VkDevice(a reference to the GPU Driver)
        createInstance();
        //VkPhysicalDevice vulkPhysDevice = ;
        //VkDevice vulkDevice = ;

        // A swapchain is a series of buffers/images you can draw to then later display to the screen.
        // NOTE: swapchains are created for a given size if the screen is resized then you need to recreate the swapchain.

    }

    void createInstance() {
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
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        createInfo.enabledLayerCount = 0;

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