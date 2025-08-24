//Nombre: Flores Torres Ángel Joel
//Práctica 2
//Fecha de entrega: 22/08/2025
//No. de cuenta: 318312857

#include <iostream>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"

void resize(GLFWwindow* window, int width, int height);

const GLint WIDTH = 800, HEIGHT = 600;

// Estructura de vértice (pos + color)
struct Vertex {
    float x, y, z;
    float r, g, b;
};

// Colores básicos
struct Color { float r,g,b; };
const Color C_BLACK {0.0f,0.0f,0.0f};
const Color C_RED   {1.0f,0.0f,0.0f};

// Mostrar o no la cuadrícula
static const bool SHOW_GRID = true;

// Patrón del hongo:
// '.' = fondo (no se dibuja), 'B' = negro, 'R' = rojo
static const std::vector<std::string> pattern = {
    "..................",
    "......BBBBBB......",
    "....BBRRRR..BB....",
    "...B..RRRR....B...",
    "..B..RRRRRR....B..",
    "..B.RR....RR...B..",
    ".BRRR......RRRRRB.",
    ".BRRR......RR..RB.",
    ".B.RR......R....B.",
    ".B..RR....RR....B.",
    ".B..RRRRRRRRR..RB.",
    ".B.RRBBBBBBBBRRRB.",
    "..BBB..B..B..BBB..",
    "...B...B..B...B...",
    "...B..........B...",
    "....B........B....",
    ".....BBBBBBBB.....",
    "..................",
};

// Genera vértices de los píxeles (ignora '.')
void buildGeometry(std::vector<Vertex>& vertices) {
    vertices.clear();
    int rows = (int)pattern.size();
    int cols = 0;
    for (auto& s : pattern) cols = std::max(cols, (int)s.size());

    // Tamaño de cada cuadrito
    float cellW = 2.0f / cols;
    float cellH = 2.0f / rows;

    // Recorre cada fila y columna
    for (int r = 0; r < rows; ++r) {
        const std::string& line = pattern[r];
        for (int c = 0; c < cols; ++c) {
            char ch = (c < (int)line.size()) ? line[c] : '.';
            if (ch == '.') continue;              // Fondo → no dibujar

            Color col = (ch == 'B') ? C_BLACK : C_RED;

            // Coordenadas del cuadrito en pantalla (NDC)
            float x0 = -1.0f + c * cellW;
            float y0 =  1.0f - (r + 1) * cellH;   // Y hacia abajo
            float x1 = x0 + cellW;
            float y1 = y0 + cellH;

            // 2 triángulos (6 vértices)
            vertices.push_back({x0,y0,0.0f,col.r,col.g,col.b});
            vertices.push_back({x1,y0,0.0f,col.r,col.g,col.b});
            vertices.push_back({x1,y1,0.0f,col.r,col.g,col.b});

            vertices.push_back({x0,y0,0.0f,col.r,col.g,col.b});
            vertices.push_back({x1,y1,0.0f,col.r,col.g,col.b});
            vertices.push_back({x0,y1,0.0f,col.r,col.g,col.b});
        }
    }
}

// Construye las líneas de la cuadrícula
void buildGrid(std::vector<Vertex>& gridVerts) {
    gridVerts.clear();
    int rows = (int)pattern.size();
    int cols = 0;
    for (auto& s : pattern) cols = std::max(cols, (int)s.size());

    float cellW = 2.0f / cols;
    float cellH = 2.0f / rows;

    // Líneas verticales
    for (int c = 0; c <= cols; ++c) {
        float x = -1.0f + c * cellW;
        gridVerts.push_back({x,-1.0f,0.0f, C_BLACK.r,C_BLACK.g,C_BLACK.b});
        gridVerts.push_back({x, 1.0f,0.0f, C_BLACK.r,C_BLACK.g,C_BLACK.b});
    }
    // Líneas horizontales
    for (int r = 0; r <= rows; ++r) {
        float y = 1.0f - r * cellH;
        gridVerts.push_back({-1.0f,y,0.0f, C_BLACK.r,C_BLACK.g,C_BLACK.b});
        gridVerts.push_back({ 1.0f,y,0.0f, C_BLACK.r,C_BLACK.g,C_BLACK.b});
    }
}

int main() {
    // Inicia GLFW
    if (!glfwInit()) {
        std::cout << "Error iniciando GLFW\n";
        return EXIT_FAILURE;
    }

    // Crea ventana
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Practica 2 - Angel Flores", nullptr, nullptr);
    glfwSetFramebufferSizeCallback(window, resize);
    if (!window) {
        std::cout << "No se pudo crear ventana\n";
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);

    // Inicia GLEW
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        std::cout << "Error iniciando GLEW\n";
        return EXIT_FAILURE;
    }

    // Info de la GPU (solo para ver)
    std::cout << "> Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "> Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "> Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "> SL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Carga shader (archivos externos)
    Shader ourShader("Shader/core.vs", "Shader/core.frag");

    // Geometría del hongo
    std::vector<Vertex> pixels;
    buildGeometry(pixels);

    // Geometría de la cuadrícula
    std::vector<Vertex> grid;
    if (SHOW_GRID) buildGrid(grid);

    // VAO/VBO del sprite
    GLuint VAO=0, VBO=0;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pixels.size()*sizeof(Vertex), pixels.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // VAO/VBO de la cuadrícula
    GLuint gridVAO=0, gridVBO=0;
    if (SHOW_GRID) {
        glGenVertexArrays(1,&gridVAO);
        glGenBuffers(1,&gridVBO);
        glBindVertexArray(gridVAO);
        glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
        glBufferData(GL_ARRAY_BUFFER, grid.size()*sizeof(Vertex), grid.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    // Bucle principal
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Fondo blanco
        glClearColor(1.0f,1.0f,1.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.Use();

        // Dibuja sprite
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)pixels.size());

        // Dibuja cuadrícula encima
        if (SHOW_GRID) {
            glBindVertexArray(gridVAO);
            glLineWidth(1.0f);
            glDrawArrays(GL_LINES, 0, (GLsizei)grid.size());
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    // Limpia memoria GPU
    glDeleteBuffers(1,&VBO);
    glDeleteVertexArrays(1,&VAO);
    if (SHOW_GRID) {
        glDeleteBuffers(1,&gridVBO);
        glDeleteVertexArrays(1,&gridVAO);
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

// Ajusta el área de dibujo al cambiar el tamaño de la ventana
void resize(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}