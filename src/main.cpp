#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include "GLFW/glfw3.h"

void keyCallback(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
    // Keybinds
}

int main()
{
    // This function initializes the GLFW library. Before most GLFW functions can be used, GLFW must be initialized, and
    // before an application terminates GLFW should be terminated in order to free any resources allocated during or
    // after initialization.
    //  If this function fails, it calls glfwTerminate before returning. If it succeeds, you should call glfwTerminate
    //  before the application exits.
    if (!glfwInit()) {
        std::cout << "Failed initializing glfw" << "\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Mini Render Lab Demo", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Error creating window" << "\n";
        glfwTerminate();
        return -1;
    }

    // This function sets the key callback of the specified window, which is called when a key is pressed, repeated or
    // released.
    // The key functions deal with physical keys, with layout independent key tokens named after their values in the
    // standard US keyboard layout. If you want to input text, use the character callback instead.
    glfwSetKeyCallback(window, keyCallback);

    glfwMakeContextCurrent(window);

    if (!glewInit() != GLEW_OK) {
        std::cout << "Failed initializing glew" << "\n";
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
