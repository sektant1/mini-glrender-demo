# C++ — Referencia Rapida para Game Dev

## O Basico

### Tipos fundamentais

```cpp
int x = 10;              // inteiro (32 bits geralmente)
float y = 3.14f;         // ponto flutuante (o 'f' importa!)
double z = 3.14;         // ponto flutuante dupla precisao
bool ativo = true;
char letra = 'A';
unsigned int id = 42;    // so positivos (OpenGL usa muito)
```

### Ponteiros e Referencias

```cpp
int x = 10;

int* ptr = &x;   // ponteiro: guarda o ENDERECO de x
*ptr = 20;       // dereferencia: muda o valor de x pra 20

int& ref = x;    // referencia: apelido pra x
ref = 30;        // muda x pra 30
```

**Quando usar:**
- Ponteiros: quando pode ser NULL, quando precisa reatribuir
- Referencias: quando sempre aponta pra algo valido (parametros de funcao)

```cpp
// Passagem por referencia (evita copiar objetos grandes)
void atualizar(Jogador& jogador) {
    jogador.vida -= 10;
}

// Passagem por const reference (leitura, sem copia)
float distancia(const glm::vec3& a, const glm::vec3& b) {
    return glm::length(a - b);
}
```

### nullptr (nunca use NULL em C++ moderno)

```cpp
int* ptr = nullptr;
if (ptr != nullptr) { /* seguro */ }
// ou simplesmente:
if (ptr) { /* seguro */ }
```

---

## Arrays e Vetores

### Array C-style (tamanho fixo, voce gerencia)

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};
// sizeof(vertices) = tamanho total em bytes
// sizeof(vertices) / sizeof(float) = numero de floats
```

OpenGL trabalha com arrays C-style porque precisa de ponteiros pra memoria raw.

### std::vector (tamanho dinamico, seguro)

```cpp
#include <vector>

std::vector<float> vertices;
vertices.push_back(1.0f);          // adiciona no final
vertices.size();                    // quantidade de elementos
vertices.data();                    // ponteiro pro array interno (pra OpenGL)
vertices[0];                        // acesso direto (sem checagem)
vertices.at(0);                     // acesso com checagem de bounds

// Inicializacao direta
std::vector<int> indices = {0, 1, 2, 2, 3, 0};

// Loop
for (int i = 0; i < vertices.size(); i++) { }
for (float v : vertices) { }              // range-based (copia)
for (const float& v : vertices) { }       // range-based (sem copia)
```

---

## Strings

```cpp
#include <string>
#include <iostream>

std::string nome = "Quake";
std::string completo = nome + " Clone";   // concatenacao
std::cout << completo << std::endl;        // print

// C-string (pra APIs C como OpenGL/GLFW)
const char* cstr = nome.c_str();
```

---

## Structs e Classes

### Struct (dados agrupados, tudo publico por padrao)

```cpp
struct Vertice {
    glm::vec3 posicao;
    glm::vec3 cor;
    glm::vec2 texCoord;
};

Vertice v;
v.posicao = glm::vec3(1.0f, 0.0f, 0.0f);
```

### Classe (dados + comportamento, tudo privado por padrao)

```cpp
class Camera {
public:
    Camera(glm::vec3 pos) : posicao(pos), yaw(-90.0f), pitch(0.0f) {
        atualizarVetores();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(posicao, posicao + frente, cima);
    }

    void processarMouse(float xoffset, float yoffset) {
        yaw += xoffset;
        pitch += yoffset;
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;
        atualizarVetores();
    }

private:
    glm::vec3 posicao;
    glm::vec3 frente;
    glm::vec3 cima;
    float yaw;
    float pitch;

    void atualizarVetores() {
        glm::vec3 direcao;
        direcao.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direcao.y = sin(glm::radians(pitch));
        direcao.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        frente = glm::normalize(direcao);
    }
};
```

**Diferenca real entre struct e class:** so o padrao de visibilidade.
Convencao: struct pra dados simples, class quando tem logica.

---

## Construtores e Inicializacao

```cpp
class Entidade {
public:
    // Construtor padrao
    Entidade() : vida(100), posicao(0.0f) {}

    // Construtor com parametros
    Entidade(int vida, glm::vec3 pos) : vida(vida), posicao(pos) {}

    // Destrutor (chamado quando o objeto morre)
    ~Entidade() {
        // liberar recursos se necessario
    }

private:
    int vida;
    glm::vec3 posicao;
};

// Usar:
Entidade e1;                              // construtor padrao
Entidade e2(50, glm::vec3(1.0f));         // com parametros
Entidade e3 = Entidade(50, glm::vec3(1.0f)); // equivalente
```

A lista de inicializacao (`: vida(100), posicao(0.0f)`) e preferivel
a atribuir no corpo do construtor. E mais eficiente.

---

## Headers e Organizacao de Codigo

### header (.h / .hpp) — declaracao

```cpp
// camera.h
#pragma once   // evita incluir duas vezes

#include <glm/glm.hpp>

class Camera {
public:
    Camera(glm::vec3 pos);
    glm::mat4 getViewMatrix() const;
    void processarMouse(float xoffset, float yoffset);

private:
    glm::vec3 posicao;
    float yaw, pitch;
};
```

### source (.cpp) — implementacao

```cpp
// camera.cpp
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 pos) : posicao(pos), yaw(-90.0f), pitch(0.0f) {
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(posicao, posicao + frente, cima);
}
```

### Por que separar?

- Compilacao mais rapida: so recompila o .cpp que mudou
- Interface limpa: outros arquivos incluem o .h e sabem O QUE existe
  sem saber COMO funciona
- Evita dependencias circulares

---

## Memoria

### Stack vs Heap

```cpp
// Stack: automatico, rapido, tamanho fixo
{
    Camera cam(glm::vec3(0.0f));  // vive aqui dentro
}   // destrutor chamado automaticamente aqui

// Heap: manual, flexivel, mais lento
Camera* cam = new Camera(glm::vec3(0.0f));
// ... usar cam ...
delete cam;  // SE ESQUECER: memory leak
```

### Smart Pointers (prefira sobre new/delete)

```cpp
#include <memory>

// unique_ptr: um unico dono
auto cam = std::make_unique<Camera>(glm::vec3(0.0f));
cam->processarMouse(1.0f, 0.0f);  // usa com ->
// destrutor chamado automaticamente quando sai do escopo

// shared_ptr: varios donos (contagem de referencia)
auto mesh = std::make_shared<Mesh>(vertices);
auto ref = mesh;  // agora 2 donos
// destrutor chamado quando o ultimo dono morre
```

**Regra geral para o projeto:**
- Objetos pequenos/locais → stack
- Objetos grandes/com lifetime complexo → `unique_ptr`
- Recursos compartilhados (meshes, texturas) → `shared_ptr`

---

## Conversao de Tipos (Casting)

```cpp
// C++ style (prefira)
float f = static_cast<float>(inteiro);

// C style (funciona mas menos seguro)
float f = (float)inteiro;

// void* cast (necessario pra OpenGL)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
//                                                       ^^^^^^^^
// OpenGL e uma API C, entao espera void*. Aqui 0 e o offset em bytes.
// Esse cast e normal e esperado.
```

---

## Enum

```cpp
enum class TipoMagia {
    Fogo,
    Gelo,
    Raio
};

TipoMagia atual = TipoMagia::Fogo;

if (atual == TipoMagia::Fogo) {
    // lanca fireball
}
```

`enum class` e melhor que `enum` puro porque nao polui o namespace.

---

## Lambda (funcoes inline)

```cpp
// Usado com GLFW para callbacks
glfwSetFramebufferSizeCallback(window,
    [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
);

// Nota: GLFW callbacks nao aceitam lambdas com captura.
// Por isso, pra callbacks mais complexos, use funcoes normais.
```

---

## Delta Time

Fundamental pra movimento consistente independente do FPS:

```cpp
float deltaTime = 0.0f;
float ultimoFrame = 0.0f;

// No game loop:
float frameAtual = glfwGetTime();
deltaTime = frameAtual - ultimoFrame;
ultimoFrame = frameAtual;

// Usar:
float velocidade = 5.0f * deltaTime;  // 5 unidades por SEGUNDO, nao por frame
posicao += frente * velocidade;
```

---

## Leitura de Arquivos

```cpp
#include <fstream>
#include <sstream>
#include <string>

std::string lerArquivo(const std::string& caminho) {
    std::ifstream arquivo(caminho);
    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    return buffer.str();
}

// Util pra carregar shaders de arquivos .glsl em vez de hardcoded
```

---

## Dicas Gerais

1. **Compile com warnings:** `-Wall -Wextra` no CMake
2. **Use const** sempre que algo nao muda
3. **Prefira referencias** a ponteiros quando possivel
4. **Nao otimize cedo** — faca funcionar, depois faca rapido
5. **sizeof** e seu amigo com OpenGL — a GPU precisa saber tamanhos exatos
