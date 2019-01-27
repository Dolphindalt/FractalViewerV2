#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <limits.h>
#include <cassert>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <string>

#define MAJOR_VERSION 3
#define MINOR_VERSION 1

int width = 1000, height = 1000;
int running = 1;
double cx = 0.0, cy = 0.0, zoom = 1.0;
#define ITR_STEP 10
int itr = 100;
int selection = 0;
float orbit_x, orbit_y;
float mod_x = 1.0, mod_y = 1.0, mod_z = 1.0;
float kda = 1.0, kdb = 1.5, kdc = 1.5, kdd = -1.0;
float startx = 0.0, starty = 0.0;
float uf_min_param = 16.0;

char *file_to_string(const char *file_name);
GLuint build_shader();
void compile_shader(const char *str, GLuint id);
void input();

int main(int argc, char *argv[])
{

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MINOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = NULL;

    window = SDL_CreateWindow(
        "Mandelbrot", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL
    );

    assert(window != NULL);

    SDL_GLContext glcon = SDL_GL_CreateContext(window);
    assert(glcon);

    GLenum err = glewInit();
    assert(err == GLEW_OK);

    SDL_GL_SetSwapInterval(1);

    GLuint shaders = 0;
    shaders = build_shader();

    float vertex_data[] =
    {
        -1.0f,  1.0f,  0.0f,
	    -1.0f,  -1.0f,  0.0f,
	    1.0f,  -1.0f,  0.0f,

	    -1.0f,  1.0f,  0.0f,
	    1.0f,  -1.0f,  0.0f,
        1.0f, 1.0f, 0.0f,
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), vertex_data, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUseProgram(shaders);

    glBindVertexArray(vao);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplSDL2_InitForOpenGL(window, glcon);
    ImGui_ImplOpenGL3_Init("#version 400");

    ImGui::StyleColorsLight();

    Uint64 now;
    Uint64 last_time = SDL_GetPerformanceCounter();
    double delta = 0;
    double ns = 1000000000.0 / 60.0;
    while(running)
    {
        now = SDL_GetPerformanceCounter();
        delta += (now - last_time) / ns;
        while(delta >= 1.0)
        {
            input();
            delta--;
        }
        
        glUniform1i(glGetUniformLocation(shaders, "selection"), selection);
        glUniform2d(glGetUniformLocation(shaders, "screen_size"), (double)width, (double)height);
        glUniform1d(glGetUniformLocation(shaders, "screen_ratio"), (double)width/(double)height);
        glUniform2d(glGetUniformLocation(shaders, "center"), cx, cy);
        glUniform1d(glGetUniformLocation(shaders, "zoom"), zoom);
        glUniform1i(glGetUniformLocation(shaders, "iterations"), itr);
        glUniform2d(glGetUniformLocation(shaders, "orbit_trap"), (double)orbit_x, (double)orbit_y);
        glUniform3f(glGetUniformLocation(shaders, "color_mod"), mod_x, mod_y, mod_z);

        glUniform1f(glGetUniformLocation(shaders, "kda"), kda);
        glUniform1f(glGetUniformLocation(shaders, "kdb"), kdb);
        glUniform1f(glGetUniformLocation(shaders, "kdc"), kdc);
        glUniform1f(glGetUniformLocation(shaders, "kdd"), kdd);
        glUniform1f(glGetUniformLocation(shaders, "startx"), startx);
        glUniform1f(glGetUniformLocation(shaders, "starty"), starty);

        glUniform1f(glGetUniformLocation(shaders, "uf_min"), uf_min_param);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.0, 1.0, 1.0, 1.0);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(window);
        ImGui::NewFrame();

        ImGui::Begin("Mandelbrot Boy V2");
        ImGui::Text("Itr: %d Zoom: %lf CX: %.5lf CY: %.5lfi", itr, zoom, cx, cy);
        ImGui::DragInt("Fractal Type", &selection, 1.0F, 0, 5);
        if(selection == 0 || selection == 2)
        {
            ImGui::Text("Orbit Trapping");
            ImGui::SliderFloat("Orbit X", &orbit_x, -10.0f, 10.0f);
            ImGui::SliderFloat("Orbit Y", &orbit_y, -10.0f, 10.0f);
        }
        if(selection == 3)
        {
            ImGui::Text("Color Modding");
            ImGui::SliderFloat("Mod X", &mod_x, 0.1, 3.0);
            ImGui::SliderFloat("Mod Y", &mod_y, 0.1, 3.0);
            ImGui::SliderFloat("Mod Z", &mod_z, 0.1, 3.0);
        }
        if(selection == 4)
        {
            ImGui::Text("Kings Dream");
            ImGui::SliderFloat("startx", &startx, -2.0, 2.0);
            ImGui::SliderFloat("starty", &starty, -2.0, 2.0);
            ImGui::SliderFloat("a", &kda, -3.0, 3.0);
            ImGui::SliderFloat("b", &kdb, -3.0, 3.0);
            ImGui::SliderFloat("c", &kdc, -0.5, 1.5);
            ImGui::SliderFloat("d", &kdd, -0.5, 1.5);
        }
        if(selection == 5)
        {
            ImGui::Text("Unknown Fractal?");
            ImGui::SliderFloat("Min Param", &uf_min_param, 1.0, 100.0);
        }
        ImGui::End();

        ImGui::Render();

        SDL_GL_MakeCurrent(window, glcon);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    printf("Iterations: %d, Zoom: %lf at %.5lf + %.5lfi\n", itr, zoom, cx, cy);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(glcon);
    SDL_DestroyWindow(window);
    SDL_Quit();
    glDeleteProgram(shaders);
    glDeleteVertexArrays(1, &vao);
    return 0;
}

void input()
{
    const double delta = 0.1 / zoom;
    SDL_Event e;
    while(SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);
        switch(e.type)
        {
            case SDL_QUIT: running = 0; break;
            case SDL_KEYDOWN:
                switch(e.key.keysym.sym)
                {
                    case SDLK_j: cx -= delta; break;
                    case SDLK_l: cx += delta; break;
                    case SDLK_i: cy += delta; break;
                    case SDLK_k: cy -= delta; break;
                    case SDLK_u:
                        itr -= ITR_STEP;
                        if(itr < 0)
                            itr = 0;
                        break;
                    case SDLK_o:
                        if(itr > INT_MAX - ITR_STEP)
                            itr = INT_MAX;
                        else
                            itr += ITR_STEP;
                        break;
                    case SDLK_ESCAPE: running = 0; break;
                    case SDLK_EQUALS:
                        zoom *= 1.1;
                        break;
                    case SDLK_MINUS:
                        zoom *= 0.9;
                        break;
                    default: break;
                }
            default: break;
        }
    }
}

void compile_shader(const char *str, GLuint id)
{
    assert(id);
    GLint result = GL_FALSE;
    int error_length;
    const char *ptr = str;
    glShaderSource(id, 1, &ptr, NULL);
    glCompileShader(id);
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &error_length);
    if(error_length > 0 && result == GL_FALSE)
    {
        char msg[error_length + 1];
        glGetShaderInfoLog(id, error_length, NULL, &msg[0]);
        printf("%s\n", msg);
        exit(1);
    }
}

GLuint build_shader()
{
    GLuint vs_id = 0;
    GLuint fs_id = 0;

    vs_id = glCreateShader(GL_VERTEX_SHADER);
    fs_id = glCreateShader(GL_FRAGMENT_SHADER);

    assert(vs_id);
    assert(fs_id);

    const char *vs_str = "#version 400\nin vec3 vp; void main () {  gl_Position = vec4 (vp, 1.0); }";
    char *fs_str = file_to_string("fragment.glsl");

    assert(fs_str);

    compile_shader(vs_str, vs_id);
    compile_shader(fs_str, fs_id);

    GLuint shader_program_id = glCreateProgram();
    glAttachShader(shader_program_id, vs_id);
    glAttachShader(shader_program_id, fs_id);
    glLinkProgram(shader_program_id);

    GLint result = GL_FALSE;
    int error_length;
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &error_length);
    if(error_length > 0 && result == GL_FALSE)
    {
        char msg[error_length + 1];
        glGetProgramInfoLog(shader_program_id, error_length, NULL, &msg[0]);
        printf("%s\n", msg);
        exit(1);
    }

    glDetachShader(shader_program_id, vs_id);
    glDetachShader(shader_program_id, fs_id);
    glDeleteShader(fs_id);
    glDeleteShader(vs_id);

    assert(shader_program_id > 0);

    free(fs_str);

    return shader_program_id;
}

char *file_to_string(const char *file_name)
{
    char *buffer = 0;
    unsigned long len;
    FILE *fp = fopen(file_name, "r");
    assert(fp);

    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (char *)malloc(len + 1);
    assert(buffer);

    fread(buffer, 1, len, fp);
    fclose(fp);
    buffer[len] = '\0';

    return buffer;
}