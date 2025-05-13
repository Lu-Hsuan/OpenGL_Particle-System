#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#undef GLAD_GL_IMPLEMENTATION
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
 
#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include "particle/Helper.h"
#include "particle/Particle_TF.h"

#include "wave/wave.h"
#include "skybox.h"
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

//#include "pipline.h"
#include "bitonicsort.h"
#define MODEL_DRAW 1
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void keyCallback(GLFWwindow* window, int key, int, int action, int);
void processInput(GLFWwindow* window);
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(-19.0f, 5.0f, 5.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mouseBinded = true;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int mode = 0;
float Island_Scale = 0.15;
extern float offset_y;
Wave* wave_model;

// Particle
ParticleSystem_TF* Particle;
ParticleSystem_TF* Particle2;
ParticleSystem_TF* Particle3;
ParticleSystem_TF* Particle4;
ParticleSystem_TF* Particle5;
ParticleSystem_TF* Particle6;
ParticleSystem_TF* Particle7;
ParticleSystem_TF* Particle8;
ParticleSystem_TF* Particle9;
std::vector<ParticleSystem_TF*> Particle_manger_tornado;
std::vector<ParticleSystem_TF*> Particle_manger_snow;
std::vector<ParticleSystem_TF*> Particle_manger_flake;
std::vector<ParticleSystem_TF*> Particle_manger_tail;
GLuint smoketex = 0;
GLuint gradienttex = 0;
GLuint countquery = 0;
bool prevbufferusable = false;
float fps;
int particle_count = 0;

void renderMainPanel();
void renderGUI();

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    char tmp[256];
    chdir("../");
    auto _ = getcwd(tmp, 256);

    cout << "Current working directory: " << tmp << endl;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, keyCallback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    std::cout << glGetString(GL_VERSION);
    // build and compile shaders
    // -------------------------
    Shader fireShader("src/3.model_loading/1.model_loading/fire.vs", "src/3.model_loading/1.model_loading/fire.fs");
    Shader ourShader("src/3.model_loading/1.model_loading/1.model_loading.vs", "src/3.model_loading/1.model_loading/1.model_loading.fs");
    // load models
    // -----------
#ifdef MODEL_DRAW
    Model ourModel(FileSystem::getPath("resources/objects/Small Tropical Island/Small Tropical Island.obj"));
#endif
    // Particle Setting
    Particle = new ParticleSystem_TF("resources/textures/13.png");
    Particle2 = new ParticleSystem_TF("resources/textures/pngwing.com.png");
    Particle3 = new ParticleSystem_TF("resources/textures/flare1.png");
    Particle4 = new ParticleSystem_TF("resources/textures/TorqueX_Particle_Effect_Guide_snow.png");
    Particle5 = new ParticleSystem_TF("resources/textures/pngtab_snowflake-symmetry-golden.png");
    Particle6 = new ParticleSystem_TF("resources/textures/TorqueX_Particle_Effect_Guide_snow.png");
    Particle7 = new ParticleSystem_TF("resources/textures/TorqueX_Particle_Effect_Guide_snow.png");
    Particle8 = new ParticleSystem_TF("resources/textures/1234.png");
    Particle9 = new ParticleSystem_TF("resources/textures/TorqueX_Particle_Effect_Guide_snow.png");
    // create pipelines
    Particle->bind_shader("fire");
    Particle2->bind_shader("fire");
    Particle3->bind_shader("tornado");
    Particle4->bind_shader("tornado");
    Particle5->bind_shader("snow");
    Particle6->bind_shader("snow");
    Particle7->bind_shader("flake_fire");
    Particle8->bind_shader("flake_fire");
    Particle9->bind_shader("tornado_tail");

    glm::vec3 tornado_vel = glm::vec3(0);
    Particle3->LIFE_SPAN = 100.0f;
    Particle3->EMIT_RATE = 0.5f;
    Particle4->LIFE_SPAN = 100.0f;
    Particle4->EMIT_RATE = 1.0f;
    Particle3->pos = glm::vec3(-26, 1.77, 23);
    Particle4->pos = glm::vec3(-26, 1.77, 23);
    tornado_vel.x = 0.01f * randomFloatBetween(-1, 1);
    tornado_vel.z = 0.01f * randomFloatBetween(-1, 1);
    float time_count = 0;

    Particle5->LIFE_SPAN = 5.0f;
    Particle5->EMIT_RATE = 5.0f;
    Particle6->LIFE_SPAN = 5.0f;
    Particle6->EMIT_RATE = 2.0f;

    Particle7->LIFE_SPAN = 6.0f;
    Particle7->EMIT_RATE = 10.0f;
    Particle7->pos = glm::vec3(10, 10, 10);
    Particle8->LIFE_SPAN = 6.0f;
    Particle8->EMIT_RATE = 10.0f;
    Particle8->pos = glm::vec3(10, 10, 10);

    Particle9->LIFE_SPAN = 400.0f;
    Particle9->EMIT_RATE = 5.0f;
    Particle9->number_emt = 0;
    Particle9->pos = glm::vec3(0, 10, 30);

    Particle_manger_tornado.push_back(Particle3);
    Particle_manger_tornado.push_back(Particle4);
    Particle_manger_snow.push_back(Particle5);
    Particle_manger_snow.push_back(Particle6);
    Particle_manger_flake.push_back(Particle7);
    Particle_manger_flake.push_back(Particle8);
    Particle_manger_tail.push_back(Particle9);
    // create query
    glGenQueries(1, &countquery);

    // Wave
    // Build and compile our shader program
    extern GLuint surfaceVAO;
    extern GLuint lightVAO;
    extern float time_;
    extern glm::vec3 lampPos;
    extern float modelScale;
    extern float heightMin;
    extern float heightMax;
    extern int indexSize;
    Shader lightingShader("src/3.model_loading/1.model_loading/surface.vert", "src/3.model_loading/1.model_loading/surface.frag");
    //Shader lightingShader("lighting.vs", "lighting.glsl");
    Shader lampShader("src/3.model_loading/1.model_loading/lamp.vert", "src/3.model_loading/1.model_loading/lamp.frag");
    lightingShader.use();
    wave_model = InitWave();

    // Skybox
    extern float skyboxVertices[108];
    Shader skyboxShader("src/3.model_loading/1.model_loading/skybox.vs", "src/3.model_loading/1.model_loading/skybox.fs");
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // -------------
    tmp[256];
    //chdir("../");
     _ = getcwd(tmp, 256);
    cout << "Current working directory: " << tmp << endl;
    vector<std::string> faces
    {
        FileSystem::getPath("resources/textures/skybox/right.jpg"),
        FileSystem::getPath("resources/textures/skybox/left.jpg"),
        FileSystem::getPath("resources/textures/skybox/top.jpg"),
        FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
        FileSystem::getPath("resources/textures/skybox/front.jpg"),
        FileSystem::getPath("resources/textures/skybox/back.jpg")
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fps = 1/deltaTime;
        //std ::cout << fps << std::endl;
        // input
        // -----
        processInput(window);
        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // Wave
        buildTessendorfWaveMesh(wave_model);
        lightingShader.use();

        GLint lightPosLoc = glGetUniformLocation(lightingShader.ID, "light.position");
        GLint viewPosLoc = glGetUniformLocation(lightingShader.ID, "viewPos");
        glUniform3f(lightPosLoc, lampPos.x, lampPos.y, lampPos.z);
        glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "heightMin"), heightMin * modelScale);
        glUniform1f(glGetUniformLocation(lightingShader.ID, "heightMax"), heightMax * modelScale);

        // Set lights properties
        glUniform3f(glGetUniformLocation(lightingShader.ID, "light.ambient"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "light.diffuse"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(lightingShader.ID, "light.specular"), 1.0f, 0.9f, 0.7f);
        // Set material properties
        glUniform1f(glGetUniformLocation(lightingShader.ID, "material.shininess"), 32.0f);

        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.ID, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.ID, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.ID, "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // ===== Draw Model =====
        glBindVertexArray(surfaceVAO);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(modelScale));	// Scale the surface
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        // ===== Draw Lamp =====
        lampShader.use();
        // Get location objects for the matrices on the lamp shader
        modelLoc = glGetUniformLocation(lampShader.ID, "model");
        viewLoc = glGetUniformLocation(lampShader.ID, "view");
        projLoc = glGetUniformLocation(lampShader.ID, "projection");
        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        model = glm::mat4(1.0f);
        model = glm::translate(model, lampPos);
        model = glm::scale(model, glm::vec3(1.0f,1.0f,1.0f)); // Make it a smaller cube
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        // Draw the light object
        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, 6 * 180 * 360, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Model
        ourShader.use();
        // view/projection transformations
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(Island_Scale));	
        ourShader.setMat4("model", model);
#ifdef MODEL_DRAW
        ourModel.Draw(ourShader);
#endif
        // Skybox
        glDepthFunc(GL_LEQUAL);
        glFrontFace(GL_CW);
        skyboxShader.use();
        model = glm::scale(model, glm::vec3(1000.0f));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("model", model);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glFrontFace(GL_CCW);
        glDepthFunc(GL_LESS);

        // Particle
        Particle7->pos = glm::vec3(randomFloatBetween(-10, 10), 10, randomFloatBetween(-10, 10));
        Particle8->pos = Particle7->pos;
        Particle->update(time_, deltaTime, camera);
        Particle2->pos = Particle->pos;
        time_ += deltaTime;
        Particle2->update(time_, deltaTime + 0.005f, camera);
        time_count += deltaTime;
        if (int(time_count) / 4 >= 1) {
          tornado_vel.x = randomFloatBetween(0, 2 * deltaTime) * randomFloatBetween(-1, 1);
          tornado_vel.z = randomFloatBetween(0, 2 * deltaTime) * randomFloatBetween(-1, 1);
          time_count -= 4;
        }
        for (auto& pointer_ : Particle_manger_tornado) {
          pointer_->pos += tornado_vel;
          if (pointer_->pos.x <= -49.0) {
            pointer_->pos.x = -49.0f;
          }
          if (pointer_->pos.x >= -20.0) {
            pointer_->pos.x = -20.0f;
          }
          if (pointer_->pos.z >= 48.0) {
            pointer_->pos.z = 48.0f;
          }
          if (pointer_->pos.z <= 20.0) {
            pointer_->pos.z = 20.0f;
          }
          pointer_->update(time_, deltaTime, camera);
          deltaTime += 0.005f;
        }
        for (auto& pointer_ : Particle_manger_snow) {
          pointer_->update(time_, deltaTime, camera);
        }
        for (auto& pointer_ : Particle_manger_flake) {
          pointer_->update(time_, deltaTime, camera);
          // deltaTime += 0.005f;
        }
        for (auto& pointer_ : Particle_manger_tail) {
          pointer_->update(time_, deltaTime, camera);
        }

        Particle->draw(projection, view);
        Particle2->draw(projection, view);
        for (auto& pointer_ : Particle_manger_tornado) {
          pointer_->draw(projection, view);
        }
        for (auto& pointer_ : Particle_manger_snow) {
          pointer_->draw(projection, view);
        }
        for (auto& pointer_ : Particle_manger_flake) {
          pointer_->draw(projection, view);
        }
        for (auto& pointer_ : Particle_manger_tail) {
          pointer_->draw(projection, view);
        }
        
        // advance

        Particle->currentbuffer = 1 - Particle->currentbuffer;
        particle_count += Particle->count;
        Particle2->currentbuffer = 1 - Particle2->currentbuffer;
        particle_count += Particle2->count;
        for (auto& pointer_ : Particle_manger_tornado) {
          pointer_->currentbuffer = 1 - pointer_->currentbuffer;
          particle_count += pointer_->count;
        }
        for (auto& pointer_ : Particle_manger_snow) {
          pointer_->currentbuffer = 1 - pointer_->currentbuffer;
          particle_count += pointer_->count;
        }
        for (auto& pointer_ : Particle_manger_flake) {
          pointer_->currentbuffer = 1 - pointer_->currentbuffer;
          particle_count += pointer_->count;
        }
        for (auto& pointer_ : Particle_manger_tail) {
          pointer_->currentbuffer = 1 - pointer_->currentbuffer;
          particle_count += pointer_->count;
        }
        renderGUI();
        glfwSwapBuffers(window);
        //std::cout << "particle_number" << particle_count << std::endl;
        particle_count = 0;
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) 
        offset_y += 0.2;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) 
        offset_y -= 0.2;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    // There are three actions: press, release, hold
    extern float A;
    if (action != GLFW_PRESS) return;
    // Press ESC to close the window.
    else if (key == GLFW_KEY_UP) {
            Island_Scale += 0.05;
            std::cout << Island_Scale << std::endl;
    }
    else if (key == GLFW_KEY_DOWN) {
        Island_Scale -= 0.05;
        std::cout << Island_Scale << std::endl;
    }
    else if (key == GLFW_KEY_LEFT) {
        offset_y += 0.2;
        std::cout << offset_y << std::endl;
    }
    else if (key == GLFW_KEY_RIGHT) {
        offset_y -= 0.2;
        std::cout << offset_y << std::endl;
        std::cout << "X:" << camera.Position.x << "Y:" << camera.Position.y << "Z:" << camera.Position.z << std::endl;
    }
    else if (key == GLFW_KEY_Z) {
        A /= 2;
        wave_model = InitWave();
        std::cout << A << std::endl;
    }
    else if (key == GLFW_KEY_X) {
        A *= 2;
        wave_model = InitWave();
        std::cout << A << std::endl;
    }
    if (key == GLFW_KEY_F9) {
      // Disable / enable mouse cursor.
      if (mouseBinded)
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      else
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      mouseBinded = !mouseBinded;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{   
    if (mouseBinded) {
  
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void renderMainPanel() {
  static bool showall = false;
  static bool heightMapButton = false;
  ImGui::SetNextWindowSize(ImVec2(400, 0.0f), ImGuiCond_Once);
  ImGui::SetNextWindowCollapsed(0, ImGuiCond_Once);
  ImGui::SetNextWindowPos(ImVec2(10.0f,10.0f), ImGuiCond_Once);
  ImGui::SetNextWindowBgAlpha(0.2f);
  if (ImGui::Begin("Configs")) {
    ImGui::SetWindowSize(ImVec2(0.0f, 0.0f), 0);
    ImGui::Text("Current framerate: %.0f", ImGui::GetIO().Framerate);
    ImGui::Text("Current All Particle number: %.0d", particle_count);
  }
  if (ImGui::Button("Show all particle")) {
    showall = !showall;
  }
  if (showall) {
    ImGui::SetWindowSize(ImVec2(0.0f, 0.0f), 0);
    particle_count = 0;
    particle_count += Particle->count;
    particle_count += Particle2->count;
    ImGui::Text("Current Fire Particle number: %.0d", particle_count);
    particle_count = 0;
    for (auto& pointer_ : Particle_manger_tornado) {
      particle_count += pointer_->count;
    }
    ImGui::Text("Current Tornado Particle number: %.0d", particle_count);
    particle_count = 0;
    for (auto& pointer_ : Particle_manger_snow) {
      particle_count += pointer_->count;
    }
    ImGui::Text("Current Snow Particle number: %.0d", particle_count);
    particle_count = 0;
    for (auto& pointer_ : Particle_manger_flake) {
      particle_count += pointer_->count;
    }
    ImGui::Text("Current Flake Particle number: %.0d", particle_count);
    particle_count = 0;
    for (auto& pointer_ : Particle_manger_tail) {
      particle_count += pointer_->count;
    }
    ImGui::Text("Current Tail Particle number: %.0d", particle_count);
    particle_count = 0;
  }
  ImGui::End();
  ImGui::SetNextWindowSize(ImVec2(400.0f, 100.0f), 0);
}

void renderGUI() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  renderMainPanel();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}