# OpenGL 3.3 Core — Referencia Rapida

## O que e OpenGL

Uma API que fala com a GPU. Voce manda dados (vertices, texturas) pra GPU
e diz como desenhar. O OpenGL e uma **maquina de estados** — voce configura
o estado atual e os comandos seguintes usam esse estado.

---

## O Pipeline Grafico

```
Vertices (CPU)
    |
    v
[Vertex Shader]       ← voce programa (GLSL)
    |                    transforma cada vertice (posicao, cor, UV)
    v
[Rasterizacao]        ← automatico
    |                    converte triangulos em fragmentos (pixels candidatos)
    v
[Fragment Shader]     ← voce programa (GLSL)
    |                    define a cor final de cada fragmento
    v
[Testes/Blending]     ← automatico (depth test, alpha blend)
    |
    v
Framebuffer (tela)
```

---

## Objetos Fundamentais

### VAO (Vertex Array Object)

Guarda a **configuracao** de como ler os dados dos VBOs.
Pense nele como um "perfil" que lembra quais buffers usar e como interpretar.

```cpp
unsigned int VAO;
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
// ... configura VBO e atributos aqui ...
glBindVertexArray(0); // desvincula

// Na hora de desenhar:
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, numVertices);
```

### VBO (Vertex Buffer Object)

Bloco de memoria na GPU que guarda os vertices.

```cpp
unsigned int VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

`GL_STATIC_DRAW` = dados definidos uma vez, usados muitas vezes.
`GL_DYNAMIC_DRAW` = dados mudam frequentemente.

### EBO (Element Buffer Object)

Permite reusar vertices. Em vez de repetir vertices compartilhados,
voce define indices.

```cpp
// Retangulo: 4 vertices, 6 indices (2 triangulos)
unsigned int indices[] = {
    0, 1, 2,   // primeiro triangulo
    2, 3, 0    // segundo triangulo
};

unsigned int EBO;
glGenBuffers(1, &EBO);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// Desenhar:
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

---

## Vertex Attributes

Diz ao OpenGL como interpretar os bytes no VBO.

```cpp
// Atributo 0: posicao (3 floats)
glVertexAttribPointer(
    0,                  // indice do atributo (layout location no shader)
    3,                  // quantidade de componentes (x, y, z)
    GL_FLOAT,           // tipo
    GL_FALSE,           // normalizar?
    stride,             // bytes entre cada vertice (0 = apertado)
    (void*)offset       // offset em bytes dentro do vertice
);
glEnableVertexAttribArray(0);
```

Exemplo com posicao + cor no mesmo VBO:

```
Cada vertice: [x, y, z, r, g, b]
              |       |  |       |
              pos(0)     cor(1)

stride = 6 * sizeof(float)
posicao: offset = 0
cor:     offset = 3 * sizeof(float)
```

---

## Shaders (GLSL)

Programas que rodam na GPU. Escritos em GLSL (parece C).

### Vertex Shader minimo

```glsl
#version 330 core

layout (location = 0) in vec3 aPos;    // recebe posicao do VBO
layout (location = 1) in vec3 aColor;  // recebe cor do VBO

out vec3 vertexColor;  // passa pro fragment shader

uniform mat4 model;      // transformacao do objeto
uniform mat4 view;       // transformacao da camera
uniform mat4 projection; // perspectiva

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
```

### Fragment Shader minimo

```glsl
#version 330 core

in vec3 vertexColor;  // vem do vertex shader (interpolado)
out vec4 FragColor;   // cor final do pixel

void main() {
    FragColor = vec4(vertexColor, 1.0);
}
```

### Compilar e linkar shaders em C++

```cpp
// 1. Criar e compilar vertex shader
unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
glCompileShader(vertexShader);
// checar erros com glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success)

// 2. Criar e compilar fragment shader (mesmo processo)
unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
glCompileShader(fragmentShader);

// 3. Linkar num program
unsigned int shaderProgram = glCreateProgram();
glAttachShader(shaderProgram, vertexShader);
glAttachShader(shaderProgram, fragmentShader);
glLinkProgram(shaderProgram);
// checar erros com glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success)

// 4. Limpar (ja foram copiados pro program)
glDeleteShader(vertexShader);
glDeleteShader(fragmentShader);

// 5. Usar
glUseProgram(shaderProgram);
```

---

## Uniforms

Variaveis que voce manda da CPU pro shader. Globais por draw call.

```cpp
glUseProgram(shaderProgram); // precisa estar ativo
int loc = glGetUniformLocation(shaderProgram, "model");
glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
```

Tipos comuns:
- `glUniform1f(loc, valor)` — float
- `glUniform3f(loc, x, y, z)` — vec3
- `glUniform1i(loc, valor)` — int (usado pra texture unit)
- `glUniformMatrix4fv(loc, 1, GL_FALSE, ptr)` — mat4

---

## Texturas

```cpp
unsigned int texture;
glGenTextures(1, &texture);
glBindTexture(GL_TEXTURE_2D, texture);

// Configurar wrapping e filtering
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// Enviar pixels pra GPU
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```

No shader:
```glsl
uniform sampler2D ourTexture;
// ...
FragColor = texture(ourTexture, texCoord);
```

---

## Depth Test

Sem isso, triangulos atras aparecem na frente.

```cpp
glEnable(GL_DEPTH_TEST);

// No game loop, limpar junto com a cor:
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

---

## Funcoes Comuns Rapidas

| Funcao | O que faz |
|--------|-----------|
| `glClearColor(r,g,b,a)` | Define cor de fundo |
| `glClear(bits)` | Limpa o framebuffer |
| `glViewport(x,y,w,h)` | Area de desenho na janela |
| `glEnable(cap)` | Liga funcionalidade (depth, blend) |
| `glDrawArrays(mode,first,count)` | Desenha vertices sequenciais |
| `glDrawElements(mode,count,type,offset)` | Desenha com indices |
| `glBindVertexArray(vao)` | Ativa um VAO |
| `glUseProgram(program)` | Ativa um shader program |

---

## Matrizes MVP (com GLM)

```cpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model: posiciona o objeto no mundo
glm::mat4 model = glm::mat4(1.0f); // identidade
model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
model = glm::rotate(model, angulo, glm::vec3(0.0f, 1.0f, 0.0f));
model = glm::scale(model, glm::vec3(1.0f));

// View: posiciona a camera
glm::mat4 view = glm::lookAt(
    cameraPos,               // onde a camera esta
    cameraPos + cameraFront, // pra onde olha
    cameraUp                 // direcao "cima"
);

// Projection: perspectiva
glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),     // FOV
    (float)width / height,   // aspect ratio
    0.1f,                    // near plane
    100.0f                   // far plane
);
```

---

## Erros e Debug

```cpp
// Checar erro de compilacao de shader
int success;
char infoLog[512];
glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "SHADER ERROR: " << infoLog << std::endl;
}

// Checar erro de link do program
glGetProgramiv(program, GL_LINK_STATUS, &success);
if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    std::cout << "LINK ERROR: " << infoLog << std::endl;
}
```
