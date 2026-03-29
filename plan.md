# Plano — Clone Quake 3 (Espadas & Magias)

## Visao Geral

Arena 3D em primeira pessoa com combate melee/magia, estilo Quake 3.
Motor proprio em C++ com OpenGL 3.3 Core.

---

## Fase 1 — Janela + Triangulo

**Objetivo:** ver algo na tela

- [ ] Inicializar GLFW e criar janela
- [ ] Carregar OpenGL com GLAD
- [ ] Game loop basico (clear, swap, poll)
- [ ] Fechar com ESC
- [ ] Criar VAO/VBO com vertices de um triangulo
- [ ] Escrever vertex shader (posicao)
- [ ] Escrever fragment shader (cor)
- [ ] Compilar e linkar shader program
- [ ] Desenhar o triangulo com glDrawArrays

**Resultado:** triangulo colorido na tela, janela fecha com ESC

---

## Fase 2 — Texturas e Retangulos

**Objetivo:** entender como imagens vao parar na GPU

- [ ] Adicionar coordenadas UV aos vertices
- [ ] Carregar textura com stb_image
- [ ] Criar texture object no OpenGL
- [ ] Usar EBO (Element Buffer Object) para desenhar retangulo com 2 triangulos
- [ ] Passar textura para o fragment shader via uniform sampler2D

**Resultado:** retangulo texturizado na tela

---

## Fase 3 — 3D: Camera e Transformacoes

**Objetivo:** entrar no mundo 3D

- [ ] Entender Model/View/Projection
- [ ] Passar matrizes MVP como uniforms
- [ ] Desenhar cubo 3D com perspectiva
- [ ] Implementar camera FPS (posicao, frente, cima)
- [ ] Mouse controla direcao (yaw/pitch)
- [ ] WASD move a camera
- [ ] Delta time para movimento consistente

**Resultado:** voce "anda" num mundo 3D com cubos

---

## Fase 4 — Arena (Mapa)

**Objetivo:** ter um lugar pra lutar

- [ ] Carregar modelo .obj simples (parser basico)
- [ ] Ou: construir arena proceduralmente com cubos/planos
- [ ] Iluminacao basica (Phong: ambient + diffuse + specular)
- [ ] Normal vectors nos vertices
- [ ] Chao, paredes, teto
- [ ] Skybox (cubemap)

**Resultado:** arena iluminada onde voce pode andar

---

## Fase 5 — Fisica e Colisao

**Objetivo:** nao atravessar paredes

- [ ] AABB (Axis-Aligned Bounding Box) para colisao
- [ ] Gravidade e chao
- [ ] Sliding contra paredes (nao travar, deslizar)
- [ ] Pulo
- [ ] Movimento estilo Quake: aceleracao, strafe jumping (opcional)

**Resultado:** personagem com fisica solida na arena

---

## Fase 6 — Combate: Espadas

**Objetivo:** atacar coisas

- [ ] Sistema de entidades basico (jogador, inimigo)
- [ ] Modelo/sprite da espada na mao (view model)
- [ ] Animacao de ataque (swing)
- [ ] Hitbox do ataque (raycast ou caixa na frente)
- [ ] Dano e vida (HP)
- [ ] HUD: barra de vida
- [ ] Inimigo burro que fica parado (teste)

**Resultado:** voce bate com espada e causa dano

---

## Fase 7 — Magia

**Objetivo:** projeteis e efeitos

- [ ] Projetil basico (esfera que anda em linha reta)
- [ ] Colisao projetil vs mundo e vs entidade
- [ ] Particulas: fogo, gelo, impacto
- [ ] Billboards (particulas sempre viradas pra camera)
- [ ] Cooldown de magia
- [ ] Diferentes tipos: fireball (dano area), ice bolt (dano direto)
- [ ] Sons (OpenAL ou miniaudio)

**Resultado:** combate com espada + magia funcionando

---

## Fase 8 — IA e Gameplay

**Objetivo:** ter contra quem lutar

- [ ] Inimigo que persegue o jogador
- [ ] Pathfinding simples (ou direto ao jogador)
- [ ] Inimigo ataca quando perto
- [ ] Spawn de inimigos
- [ ] Sistema de waves ou arena deathmatch
- [ ] Tela de game over / restart
- [ ] Score

**Resultado:** jogo jogavel

---

## Fase 9 — Polimento (opcional)

- [ ] Sombras (shadow mapping)
- [ ] Bloom/glow nas magias
- [ ] Multiplayer local (split screen) ou rede
- [ ] Menu principal
- [ ] Diferentes espadas com stats
- [ ] Diferentes classes de magia
