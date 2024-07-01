#include <glad/glad.h> // OpenGL libs
#include <GLFW/glfw3.h>

#include <glm/vec2.hpp> // glm libs
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/ShaderProgram.hpp" // my libs
#include "Renderer/Texture2D.hpp"
#include "Renderer/Sprite.hpp"
#include "Resources/ResourceManager.hpp"
#include "Resources/TextureLoader.hpp"
#include "Resources/SpriteGroup.hpp"

#include "keys"

#include <iostream> // other libs
#include <string>
#include <cmath>
#include <vector>
#include <thread>

// #define debug

using namespace std;

glm::ivec2 Size(3840, 2160); // Screen size (resolution)

// Engine vars
GLenum gl_texture_mode = GL_LINEAR; // GL_LINEAR or GL_NEAREST
string gl_default_shader = "DefaultShader"; // Name of default shader
string gl_sprite_shader = "SpriteShader"; // Name of default sprite shader
vector <string> gl_default_shader_path_list = {"res/shaders/vertex.cfg", "res/shaders/fragment.cfg"}; // List of paths to default shader
vector <string> gl_sprite_shader_path_list = {"res/shaders/vSprite.cfg", "res/shaders/fSprite.cfg"}; // List of paths to sprite shader
int gl_sprite_size = 80; // Size of your sprites

vector <string> tx_path_list = {
    "res/textures/texture.png"
}; // List of paths to textures

float cam_x = 0; // Camera X pos
float cam_y = 0; // Camera Y pos
float cam_rot = 180.f; // Camera rotation

int sv_max_speed = 10; // Max player movement speed

ResourceManager rm_main; // Main Resource Manager

TexLoader tl_textures; // Main Texture Loader

SprGroup sg_sprites; // Your Sprite Groups here

void sizeHandler(GLFWwindow* win, int width, int height) {
    Size.x = width;
    Size.y = height;
    glViewport(0, 0, Size.x, Size.y);
}

// Controls
void keyHandler(GLFWwindow* win) {
    // Example
    if (glfwGetKey(win, KEY_SPACE) == GLFW_PRESS) {
        cout << "Hello, World!" << endl;
    }
}

// Detects when 2 sprite groups are colliding
bool sg_collision(SprGroup& sg1, SprGroup& sg2) {
    for (auto i : sg1.get_current_pos()) {
        for (auto j : sg2.get_current_pos()) {
            if (abs(i.x - j.x) <= gl_sprite_size && abs(i.y - j.y) <= gl_sprite_size) return true;
        }
    }
    return false;
}

/*
Your functions here
*/

int main(int argc, char const *argv[]) {
    if (!glfwInit()) {
        cerr << "glfwInit failed!" << endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(Size.x, Size.y, "Title here", glfwGetPrimaryMonitor(), nullptr); // Change glfwGetPrimaryMonitor() to nullptr if you wanna windowed mode
    if (!window) {
        cerr << "glfwCreateWindow failed!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowSizeCallback(window, sizeHandler);

    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        cerr << "Can't load GLAD!" << endl;
    }

    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

    glClearColor(0.19f, 0.61f, 0.61f, 1.f); // Background color (in GLSL vec4 format)

    {
        rm_main = ResourceManager(argv[0]);
        tl_textures = TexLoader(&rm_main);
        sg_sprites = SprGroup(&rm_main); // If you have more than one Sprite Groups type 'sg_your_group = SprGroup(&rm_main);' below

        auto defaultShaderProgram = rm_main.loadShaders(gl_default_shader, gl_default_shader_path_list[0], gl_default_shader_path_list[1]);

        if (!defaultShaderProgram) {
            cerr << "Can't create shader program!" << endl;
            return -1;
        }

        auto spriteShaderProgram = rm_main.loadShaders(gl_sprite_shader, gl_sprite_shader_path_list[0], gl_sprite_shader_path_list[1]);

        if (!spriteShaderProgram) {
            cerr << "Can't create SpriteShader" << endl;
            return -1;
        }

        tl_textures.add_texture("TextureName", tx_path_list[0]); // Add your textures here

        defaultShaderProgram->use();
        defaultShaderProgram->setInt("tex", 0);

        spriteShaderProgram->use();
        spriteShaderProgram->setInt("tex", 0);

        sg_sprites.add_sprite("SpriteName", "TextureName", gl_sprite_shader, gl_sprite_size, gl_sprite_size, 0.f, 0, 0); // Add your sprites here

        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            keyHandler(window);

            float projMat_right  = Size.x + cam_x; // Variables for Projection Matrix (you can change it if you want)
            float projMat_top    = Size.y + cam_y;
            float projMat_left   = - Size.x + cam_x;
            float projMat_bottom = - Size.y + cam_y;

            glm::mat4 projMat = glm::ortho(projMat_left, projMat_right, projMat_bottom, projMat_top, -100.f, 100.f); // Projection Matrix

            defaultShaderProgram->setMat4("projMat", projMat);
            spriteShaderProgram->setMat4("projMat", projMat);

            defaultShaderProgram->use();
            tl_textures.bind_all();

            /*
            Your update functions here
            */

            sg_sprites.render_all(); // Render your Sprite Groups here

            sleep(1);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        
        sg_sprites.delete_all(); // Delete your Sprite Groups here
    }


    glfwTerminate();
    return 0;
}