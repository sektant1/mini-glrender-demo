#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

#include <GL/glew.h>

#include "GLFW/glfw3.h"

// ── Math helpers ────────────────────────────────────────────────────────────────

struct Vec3
{
    float x, y, z;
};

struct Mat4
{
    float m[16]; // column-major

    static Mat4 identity()
    {
        Mat4 r{};
        std::memset(r.m, 0, sizeof(r.m));
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    static Mat4 translate(float tx, float ty)
    {
        Mat4 r = identity();
        r.m[12] = tx;
        r.m[13] = ty;
        return r;
    }

    static Mat4 rotate(float radians)
    {
        Mat4 r = identity();
        float c = std::cos(radians);
        float s = std::sin(radians);
        r.m[0] = c;
        r.m[1] = s;
        r.m[4] = -s;
        r.m[5] = c;
        return r;
    }

    static Mat4 scale(float sx, float sy)
    {
        Mat4 r = identity();
        r.m[0] = sx;
        r.m[5] = sy;
        return r;
    }

    Mat4 operator*(const Mat4 &b) const
    {
        Mat4 r{};
        std::memset(r.m, 0, sizeof(r.m));
        for (int col = 0; col < 4; ++col)
            for (int row = 0; row < 4; ++row)
                for (int k = 0; k < 4; ++k)
                    r.m[col * 4 + row] += m[k * 4 + row] * b.m[col * 4 + k];
        return r;
    }
};

// ── Transform ───────────────────────────────────────────────────────────────────

struct Transform
{
    Vec3 position{0.0f, 0.0f, 0.0f};
    float rotation{0.0f}; // radians
    Vec3 scaleVal{1.0f, 1.0f, 1.0f};

    Mat4 matrix() const
    {
        return Mat4::translate(position.x, position.y) * Mat4::rotate(rotation) *
               Mat4::scale(scaleVal.x, scaleVal.y);
    }
};

// ── Shader helpers ──────────────────────────────────────────────────────────────

static GLuint compileShader(GLenum type, const char *source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error:\n" << log << "\n";
    }
    return shader;
}

static GLuint createProgram(const char *vertSrc, const char *fragSrc)
{
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragSrc);

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::cerr << "Program link error:\n" << log << "\n";
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

// ── Mesh: owns a VAO/VBO pair ──────────────────────────────────────────────────

struct Mesh
{
    GLuint vao{0};
    GLuint vbo{0};
    GLsizei vertexCount{0};

    void init(const std::vector<float> &vertices)
    {
        vertexCount = static_cast<GLsizei>(vertices.size()) / 2; // 2 floats per vertex (x,y)

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(vertices.size() * sizeof(float)),
                     vertices.data(),
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(GLenum mode = GL_TRIANGLE_FAN) const
    {
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, vertexCount);
        glBindVertexArray(0);
    }

    void destroy()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }
};

// ── Render object: mesh + transform + color ─────────────────────────────────────

struct RenderObject
{
    Mesh mesh;
    Transform transform;
    Vec3 color{1.0f, 1.0f, 1.0f};
    GLenum drawMode{GL_TRIANGLE_FAN};

    void draw(GLuint program, GLint uTransform, GLint uColor) const
    {
        Mat4 mat = transform.matrix();
        glUniformMatrix4fv(uTransform, 1, GL_FALSE, mat.m);
        glUniform3f(uColor, color.x, color.y, color.z);
        mesh.draw(drawMode);
    }

    void destroy() { mesh.destroy(); }
};

// ── Geometry generators ─────────────────────────────────────────────────────────

static std::vector<float> makeRegularPolygon(int sides, float radius)
{
    std::vector<float> verts;
    verts.reserve(static_cast<size_t>((sides + 2) * 2));
    // center
    verts.push_back(0.0f);
    verts.push_back(0.0f);
    for (int i = 0; i <= sides; ++i) {
        float angle = 2.0f * 3.14159265f * static_cast<float>(i) / static_cast<float>(sides);
        verts.push_back(radius * std::cos(angle));
        verts.push_back(radius * std::sin(angle));
    }
    return verts;
}

static std::vector<float> makeTriangle(float size)
{
    float h = size * 0.866f; // sqrt(3)/2
    return {
        0.0f,        h * 0.667f,   // top
        -size * 0.5f, -h * 0.333f,  // bottom-left
        size * 0.5f, -h * 0.333f,  // bottom-right
    };
}

static std::vector<float> makeStar(int points, float outerR, float innerR)
{
    std::vector<float> verts;
    verts.push_back(0.0f);
    verts.push_back(0.0f);
    int totalVerts = points * 2;
    for (int i = 0; i <= totalVerts; ++i) {
        float angle = 3.14159265f / 2.0f + 2.0f * 3.14159265f * static_cast<float>(i) / static_cast<float>(totalVerts);
        float r = (i % 2 == 0) ? outerR : innerR;
        verts.push_back(r * std::cos(angle));
        verts.push_back(r * std::sin(angle));
    }
    return verts;
}

// ── Input state ─────────────────────────────────────────────────────────────────

static bool keyW = false, keyA = false, keyS = false, keyD = false;

void keyCallback(GLFWwindow *window, int key, int /*scanCode*/, int action, int /*mods*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    bool released = (action == GLFW_RELEASE);

    auto update = [&](int target, bool &state) {
        if (key == target) {
            if (pressed) state = true;
            if (released) state = false;
        }
    };

    update(GLFW_KEY_W, keyW);
    update(GLFW_KEY_A, keyA);
    update(GLFW_KEY_S, keyS);
    update(GLFW_KEY_D, keyD);
}

// ── Main ────────────────────────────────────────────────────────────────────────

int main()
{
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    if (!glfwInit()) {
        std::cerr << "Failed initializing GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "Mini Render Lab Demo", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Error creating window\n";
        glfwTerminate();
        return -1;
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed initializing GLEW\n";
        glfwTerminate();
        return -1;
    }
    glGetError(); // flush GL error from glewExperimental

    // ── Shaders ─────────────────────────────────────────────────────────────

    const char *vertSrc = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        uniform mat4 uTransform;
        void main() {
            gl_Position = uTransform * vec4(aPos, 0.0, 1.0);
        }
    )";

    const char *fragSrc = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 uColor;
        void main() {
            FragColor = vec4(uColor, 1.0);
        }
    )";

    // Pulsating shader — uses a time-based alpha for the animated object
    const char *fragPulseSrc = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 uColor;
        uniform float uTime;
        void main() {
            float brightness = 0.5 + 0.5 * sin(uTime * 3.0);
            FragColor = vec4(uColor * brightness, 1.0);
        }
    )";

    GLuint solidProgram = createProgram(vertSrc, fragSrc);
    GLuint pulseProgram = createProgram(vertSrc, fragPulseSrc);

    GLint solidUTransform = glGetUniformLocation(solidProgram, "uTransform");
    GLint solidUColor = glGetUniformLocation(solidProgram, "uColor");

    GLint pulseUTransform = glGetUniformLocation(pulseProgram, "uTransform");
    GLint pulseUColor = glGetUniformLocation(pulseProgram, "uColor");
    GLint pulseUTime = glGetUniformLocation(pulseProgram, "uTime");

    // ── 5 Objects ───────────────────────────────────────────────────────────

    // 1) Triangle — static, red
    RenderObject triangle;
    triangle.mesh.init(makeTriangle(0.2f));
    triangle.drawMode = GL_TRIANGLES;
    triangle.transform.position = {-0.6f, 0.6f, 0.0f};
    triangle.color = {0.9f, 0.2f, 0.15f};

    // 2) Square — keyboard controlled (WASD), blue
    RenderObject square;
    square.mesh.init(makeRegularPolygon(4, 0.15f));
    square.transform.position = {0.0f, 0.0f, 0.0f};
    square.transform.rotation = 3.14159265f / 4.0f; // 45° so it looks like a square
    square.color = {0.2f, 0.4f, 0.9f};

    // 3) Pentagon — animated (orbiting + rotating), green, uses pulse shader
    RenderObject pentagon;
    pentagon.mesh.init(makeRegularPolygon(5, 0.12f));
    pentagon.transform.position = {0.6f, 0.6f, 0.0f};
    pentagon.color = {0.1f, 0.85f, 0.3f};

    // 4) Star — static, yellow
    RenderObject star;
    star.mesh.init(makeStar(5, 0.18f, 0.08f));
    star.transform.position = {-0.6f, -0.5f, 0.0f};
    star.color = {0.95f, 0.85f, 0.1f};

    // 5) Octagon — static, magenta
    RenderObject octagon;
    octagon.mesh.init(makeRegularPolygon(8, 0.14f));
    octagon.transform.position = {0.6f, -0.5f, 0.0f};
    octagon.color = {0.85f, 0.15f, 0.8f};

    // ── Render loop ─────────────────────────────────────────────────────────

    float moveSpeed = 1.5f;
    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float now = static_cast<float>(glfwGetTime());
        float dt = now - lastTime;
        lastTime = now;

        // Input: move square with WASD
        if (keyW) square.transform.position.y += moveSpeed * dt;
        if (keyS) square.transform.position.y -= moveSpeed * dt;
        if (keyA) square.transform.position.x -= moveSpeed * dt;
        if (keyD) square.transform.position.x += moveSpeed * dt;

        // Animation: pentagon orbits center-right area and rotates + pulses
        pentagon.transform.position.x = 0.5f * std::cos(now * 0.8f);
        pentagon.transform.position.y = 0.3f * std::sin(now * 0.8f);
        pentagon.transform.rotation = now * 2.0f;

        // Render
        glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw solid objects (triangle, square, star, octagon)
        glUseProgram(solidProgram);
        triangle.draw(solidProgram, solidUTransform, solidUColor);
        square.draw(solidProgram, solidUTransform, solidUColor);
        star.draw(solidProgram, solidUTransform, solidUColor);
        octagon.draw(solidProgram, solidUTransform, solidUColor);

        // Draw pulsating pentagon with its own shader
        glUseProgram(pulseProgram);
        glUniform1f(pulseUTime, now);
        pentagon.draw(pulseProgram, pulseUTransform, pulseUColor);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ── Cleanup ─────────────────────────────────────────────────────────────

    triangle.destroy();
    square.destroy();
    pentagon.destroy();
    star.destroy();
    octagon.destroy();

    glDeleteProgram(solidProgram);
    glDeleteProgram(pulseProgram);

    glfwTerminate();
    return 0;
}
