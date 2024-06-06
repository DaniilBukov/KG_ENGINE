
#include "imgui.h"


#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <vector>

#include <iostream>

void addModels(std::string str);

void drawGui();

#pragma warning(disable : 4996)

void renderModels(Shader* shader);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void renderQuad();
void renderBody();

// Константы
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

float heightScale = 0.1;

// Камера
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// Тайминги
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool isInViewMode = false;
int ViewModeTimeout = 100;
int ViewTimeoutCounter = 0;
bool objFileOpenState = false;

vector<Model> models;
vector<glm::vec3> modelPositions;
vector<glm::vec3> modelScales;
vector<glm::vec3> modelRotations;
vector<string> modelPaths;


int main()
{
    // glfw: инициализация и конфигурирование
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "4LABA)))))MB_KURSACH", NULL, NULL);
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

    
    

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Конфигурирование глобального состояния OpenGL
    glEnable(GL_DEPTH_TEST);
    bool isLoaded1;
    // Компилирование нашей шейдерной программы
    Shader shader("../vertex.glsl", "../fragment.glsl");

    //Model ourModel("../resources/objects/shrek/shrek_obj.obj", &isLoaded1);
    glm::vec3 modelPos = glm::vec3(1.0f, -3.0f, 2.0f);

    // Загрузка текстур
    
	unsigned int diffuseMap = loadTexture("../resources/textures/wing_albedo.jpg");
    unsigned int normalMap = loadTexture("../resources/textures/wing_normalmap.jpg");
    unsigned int heightMap = loadTexture("../resources/textures/wing_height.jpg");
    unsigned int alphaMap = loadTexture("../resources/textures/wing_alpha.jpg");

    unsigned int diffuseMapBody = loadTexture("../resources/textures/BODY_ALBEDO.jpg");
    unsigned int normalMapBody = loadTexture("../resources/textures/BODY_NORMAL.png");
    unsigned int heightMapBody = loadTexture("../resources/textures/BODY_ALPHA1.png");
    unsigned int alphaMapBody = loadTexture("../resources/textures/BODY_ALPHA1.jpg");

    unsigned int diffuseMapBlank = loadTexture("../resources/textures/BLANK.png");
    unsigned int normalMapBlank = loadTexture("../resources/textures/BLANK.png");
    unsigned int heightMapBlank = loadTexture("../resources/textures/BLANK.png");
    unsigned int alphaMapBlank = loadTexture("../resources/textures/BLANK.png");
	
    /*unsigned int diffuseMap = loadTexture("../resources/textures/toy_box_diffuse.png");
    unsigned int normalMap = loadTexture("../resources/textures/toy_box_normal.png");
    unsigned int heightMap = loadTexture("../resources/textures/toy_box_disp.png");*/

    // Конфигурация шейдеров
    shader.use();
    shader.setInt("diffuseMap", 0);
    shader.setInt("normalMap", 1);
    shader.setInt("depthMap", 2);
    shader.setInt("alphaMap", 3);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    // Параметры освещения
    glm::vec3 lightPos(0.5f, 5.0f, 0.3f);

    // Цикл рендеринга
    while (!glfwWindowShouldClose(window))
    {
        // Сообщаем GLFW, чтобы он захватил наш курсор
        if (isInViewMode)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        shader.setInt("isDepth", 1);
        // Обработка ввода
        processInput(window);

        // Рендер
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Imgui Initial Update
        ImGuiIO& io = ImGui::GetIO();
        int _width;
        int _height;
        glfwGetWindowSize(window, &_width, &_height);
        io.DisplaySize.x = _width;
        io.DisplaySize.y = _height;
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        

        // Конфигурирование матриц вида/проекции
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
		
        // Рендеринг прямоугольника с параллакс-эффектом
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec3 playerPos = camera.Position + glm::vec3(camera.Front.x * 0.5, camera.Front.y * 0.5, camera.Front.z * 0.5);
        renderQuad();
        model = glm::translate(model, playerPos);
        model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
        //model = glm::rotate(model, -30 * glm::radians((float)3.14), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
        //model = glm::rotate(model, -30 * glm::radians((float)3.14), glm::normalize(glm::vec3(1.0, 0.0, 0.0)));
        model = glm::rotate(model, -3.14f / 2, glm::normalize(glm::vec3(0.0, 0.0, 1.0)));
        model = glm::rotate(model, -3.14f / 2, glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
        model = glm::rotate(model, 3.1f/4*sin((float)glfwGetTime() * 10.0f)+3.14f/4, glm::normalize(glm::vec3(1.0, 0.0, 0.0))); // поворачиваем прямоугольник для демонстрации работы параллакс отображения со всех сторон
        model = glm::translate(model, glm::vec3(0.0, 1.01, 0.0));
        shader.setMat4("model", model);
        shader.setVec3("viewPos", camera.Position);
        shader.setVec3("lightPos", lightPos);
        shader.setFloat("heightScale", heightScale); // изменение значений кнопками Q и E 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, heightMap);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, alphaMap);

        renderModels(&shader);










      
        shader.setInt("isDepth", 0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPos);
        model = glm::scale(model, glm::vec3(0.001f));
        shader.setMat4("model", model);
        //ourModel.Draw(shader);

        // Рендеринг источника света (для простоты используется уменьшенная плоскость, размещенная на месте источника света)
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.01f));
        shader.setMat4("model", model);


       



        ImGui::Begin("models");
        ImGui::SliderFloat3("Light position", glm::value_ptr(lightPos), -30, 30);


        for (int i = 0; i < modelPaths.size(); i++)

        {
            ImGui::Text(modelPaths[i].c_str());
            if (modelPaths[i].c_str())
            {
                float* scale = glm::value_ptr(modelScales[i]);
                float* rotate = glm::value_ptr(modelRotations[i]);
                float* position=glm::value_ptr(modelPositions[i]);;

                string sliderNames[3] = { "scale", "rotate", "position" };

                ImGui::SliderFloat3((sliderNames[0]+to_string(i)).c_str(), scale, 0.f, .1f);
                ImGui::SliderFloat3((sliderNames[1] + to_string(i)).c_str(), rotate, -360.f, 360.f);
                ImGui::SliderFloat3((sliderNames[2] + to_string(i)).c_str(), position, -1500.f, 1500.f);


            }
        }






        static char str1[128] = "";
        ImGui::InputTextWithHint("input path to .obj file", "enter path here", str1, IM_ARRAYSIZE(str1));
        bool addModel = ImGui::Button("add obj model");

        if (addModel == true)
        {
            
            addModels(str1);
        }

        addModel = false;
        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
        ViewTimeoutCounter++;
    }



    glfwTerminate();
    return 0;
}

unsigned int bodyVAO = 0;
unsigned int bodyVBO;

void renderBody()
{
    if (bodyVAO == 0)
    {
        // Координаты
        glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3(1.0f, -1.0f, 0.0f);
        glm::vec3 pos4(1.0f, 1.0f, 0.0f);

        // Текстурные координаты
        glm::vec2 uv4(0.0f, 377.0f/512.0f);
        glm::vec2 uv1(0.0f, 0.0f);
        glm::vec2 uv2(377.0f/512.0f, 0.0f);
        glm::vec2 uv3(377.0f/512.0f, 377.0f / 512.0f);

        // Векторы нормалей
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // Вычисляем касательные/бикасательные векторы обоих треугольников
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;

        // Треугольник №1
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // Треугольник №2
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
            // координаты           // нормали        // текст. коорд.          // касательные          // бикасательные
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };

        // Конфигурируем VAO плоскости
        glGenVertexArrays(1, &bodyVAO);
        glGenBuffers(1, &bodyVBO);
        glBindVertexArray(bodyVAO);
        glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(bodyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}



// Рендерим 1x1-прямоугольник в NDC с вычисленными вручную касательными векторами
unsigned int quadVAO = 0;
unsigned int quadVBO;


void renderQuad()
{
    if (quadVAO == 0)
    {
        // Координаты
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
		
        // Текстурные координаты
        glm::vec2 uv4(0.0f, 1.0f);
        glm::vec2 uv1(0.0f, 0.0f);
        glm::vec2 uv2(1.0f, 0.0f);
        glm::vec2 uv3(1.0f, 1.0f);
		
        // Векторы нормалей
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // Вычисляем касательные/бикасательные векторы обоих треугольников
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
		
        // Треугольник №1
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent1 = glm::normalize(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent1 = glm::normalize(bitangent1);

        // Треугольник №2
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
        tangent2 = glm::normalize(tangent2);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        bitangent2 = glm::normalize(bitangent2);


        float quadVertices[] = {
            // координаты           // нормали        // текст. коорд.          // касательные          // бикасательные
            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

            pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
            pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
		
        // Конфигурируем VAO плоскости
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void renderSphere(glm::vec3 spherePos)
{
    
}

// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании кнопки мыши в данном кадре и соответствующая обработка данных событий
void processInput(GLFWwindow *window)
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

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && ViewTimeoutCounter>ViewModeTimeout)
    {
        isInViewMode = !isInViewMode;
        ViewTimeoutCounter = 0;
    }
    

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    }
    else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
    }
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Убеждаемся, что окно просмотра соответствует новым размерам окна.
    // Обратите внимание, ширина и высота будут значительно больше, чем указано, на Retina-дисплеях
    glViewport(0, 0, width, height);
}

// glfw: всякий раз, когда перемещается мышь, вызывается данная callback-функция
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевернуто, так как y-координаты идут снизу вверх

    lastX = xpos;
    lastY = ypos;
    if(isInViewMode)
    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: всякий раз, когда прокручивается колесико мыши, вызывается данная callback-функция
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// Вспомогательная функция загрузки 2D-текстур из файла
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void addModels(std::string str)
{
    bool isLoaded;
    models.push_back(Model(str, &isLoaded));
    if (isLoaded)
    {
        modelPositions.push_back(camera.Front);

        modelScales.push_back(glm::vec3(1, 1, 1));
        modelRotations.push_back(glm::vec3(0, 0, 0));

        modelPaths.push_back(str);

        std::cout << "added obj" << std::endl;


    }
    else models.pop_back();
}


void renderModels(Shader* shader)
{
    for (int i = 0; i < models.size(); i++)
    {
        shader->setInt("isDepth", 0);
        glm::mat4 modelMat = glm::mat4(1.0f);

        modelMat = glm::scale(modelMat, modelScales[i]);

        modelMat = glm::rotate(modelMat, glm::radians(modelRotations[i].x), glm::vec3(1, 0, 0));
        modelMat = glm::rotate(modelMat, glm::radians(modelRotations[i].y), glm::vec3(0, 1, 0));
        modelMat = glm::rotate(modelMat, glm::radians(modelRotations[i].z), glm::vec3(0, 0, 1));



        modelMat = glm::translate(modelMat, modelPositions[i]);
        
        shader->setMat4("model", modelMat);
        models[i].Draw(*shader);

    }
}

