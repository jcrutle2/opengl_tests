//
// Created by Caleb Rutledge on 1/4/24.
//

#include "Renderer.h"

void sdlFatalError(const std::string &errorString) {
    std::cout << errorString << std::endl;
    std::cout << "Press any key to quit...";
    int tmp;
    std::cin >> tmp;
    SDL_Quit();
}

Renderer::Renderer(int screenWidth, int screenHeight) {
    _window = nullptr;
    _context = nullptr;
    _screenWidth = screenWidth;
    _screenHeight = screenHeight;

    initSystems();
}

Renderer::~Renderer() = default;

void Renderer::initSystems() {
    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    // Set GL Attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);

    // create window
    _window = SDL_CreateWindow("OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _screenWidth, _screenHeight, SDL_WINDOW_OPENGL);

    if (_window == nullptr) {
        sdlFatalError("Error: SDL Window could not be created.");
    }

    _context = SDL_GL_CreateContext(_window);

    if (_context == nullptr) {
        sdlFatalError("Error: GL Context could not be created.");
    }

    glewExperimental = GL_TRUE;

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        sdlFatalError("Error: Failed to initialize GLEW!");
    }

    glViewport(0, 0, _screenWidth, _screenHeight);

    // initialize default shader
    shader = Shader("Assets/Shaders/exVertex.vert", "Assets/Shaders/exFragment.frag");
    tileShader = Shader("Assets/Shaders/tiles.vert", "Assets/Shaders/tiles.frag");

    // initialize VAO
    glGenVertexArrays(1, &VAO);

    // initialize buffer
    glGenBuffers(1, &VBO);

    // initialize EBO
    glGenBuffers(1, &EBO);

    // enable depth testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    // enable anti-aliasing
    glEnable(GL_MULTISAMPLE);

    // trap mouse
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // uncomment to test in wireframe mode
    _renderMode = GL_FILL;
    glPolygonMode( GL_FRONT_AND_BACK, _renderMode );

    // Load Textures
    TextureGlobals::loadAllTextures();

    // open IMGUI
    UI::Initalize(_window, _context);
}

void Renderer::toggleRenderMode() {
    _renderMode = (_renderMode == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode( GL_FRONT_AND_BACK, _renderMode );
}

void Renderer::stopSystems() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(_context);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}

void Renderer::update() {
    SDL_GL_SwapWindow(_window);
    logFrames();
}

void Renderer::draw(Scene &scene, Camera &camera) {

    // clear depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // draw boxes
    tileShader.use();
    tileShader.setMats(glm::mat4(1.0f), camera.getView(), camera.getPerspective());
    tileShader.setDirectionLight(scene.dirLight);
    tileShader.setPointLights(scene.pointLights, scene.entities);
    tileShader.setCamera(camera);
    for (const auto &b : scene.boxes)
        b.draw(tileShader);

    // draw models
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMats(model, camera.getView(), camera.getPerspective());
    shader.setDirectionLight(scene.dirLight);
    shader.setPointLights(scene.pointLights, scene.entities);
    shader.setCamera(camera);
    for (auto &m : scene.entities) {
        m.Draw(shader);
    }

    // draw skybox
    if (scene.skybox.active)
        scene.skybox.draw(camera.getView(), camera.getPerspective());

}

void Renderer::logFrames() {
    _currentFrame = SDL_GetPerformanceCounter();
    _deltaTime = (float) (_currentFrame - _lastFrame) / (float) SDL_GetPerformanceFrequency();
    _elapsedMS = _deltaTime * 1000;
    SDL_Delay(floor(16.666f - _elapsedMS));
    _elapsedMS = std::max(_elapsedMS, 16.666f);
    if (_frameInt >= 60) {
        _frameRate = std::to_string(1.0f / (_elapsedMS / 1000.0f));
        _frameInt = 0;
    }
    else _frameInt++;
    _lastFrame = SDL_GetPerformanceCounter();
}

float Renderer::getElapsedMS() const {
    return _elapsedMS;
}