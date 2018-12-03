//
// Created by realxie on 2018/9/4.
//

#include "cxx_jni_interface.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "glm/mat4x4.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "renderer.h"

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES3/gl3.h>
#include <malloc.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GLES3/gl3ext.h>

#include "logger.h"

#define LOG_TAG "EglSample"

static GLuint sProgram = 0;
static GLuint sVertexArrayObject = 0;
static GLuint sVertexBuffer = 0;
static GLuint sIndiceBuffer = 0;

static glm::mat4 pMatrix ;
static glm::mat4 mvMatrix;

GLuint loadShader(GLenum shaderType,const char* sourceCode) {
    GLuint shader; GLint compiled;
    shader = glCreateShader(shaderType);
    if(shader == 0)
        return 0;
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);
    // Check the compile status
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if(!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            LOG_ERROR("compile shader error %s", infoLog);
            free(infoLog);
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}
//
//GLint createProgram()
//{
//    //const char * filename = "/storage/emulated/0/APKs/shader.bin";
//    std::string filename = getDocumentPath() + "/shader.bin";
//
////    AAssetDir* assetDir = AAssetManager_openDir(mgr, "");
////    const char* _filename = (const char*)NULL;
////    while ((_filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
////        AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_STREAMING);
////        char buf[BUFSIZ];
////        int nb_read = 0;
////        FILE* out = fopen(filename, "w");
////        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
////            fwrite(buf, nb_read, 1, out);
////        fclose(out);
////        AAsset_close(asset);
////    }
////    AAssetDir_close(assetDir);
//
//
////    {
////        GLint   binaryLength;
////        void*   binary;
////        GLint   success;
////        FILE*   infile;
////        GLenum binaryFormat;
////
////
////        //
////        //  Read the program binary
////        //
////        infile = fopen(filename.c_str(), "rb");
////        if (infile) {
////            fseek(infile, 0, SEEK_END);
////            binaryLength = (GLint) ftell(infile) - sizeof(binaryFormat);
////            binary = (void *) malloc(binaryLength + 1);
////            fseek(infile, 0, SEEK_SET);
////            fread(&binaryFormat, sizeof(binaryFormat), 1, infile);
////            fread(binary, binaryLength, 1, infile);
////            fclose(infile);
////
////            GLuint progObj = glCreateProgram();
////
////            //
////            //  Load the binary into the program object -- no need to link!
////            //
////            glProgramBinaryOES(progObj, binaryFormat, binary, binaryLength);
////            free(binary);
////
////            glGetProgramiv(progObj, GL_LINK_STATUS, &success);
////
////            if (GL_TRUE == success)
////            {
////                return progObj;
////            }
////        }
////
////    }
//
//    GLuint program = glCreateProgram();
//    GLuint _v = loadShader(GL_VERTEX_SHADER, verticesShader);
//    GLuint _f = loadShader(GL_FRAGMENT_SHADER, fragmentShader);
//    GLint linked;
//
//    glAttachShader(program, _v);
//    glAttachShader(program, _f);
//
//    glLinkProgram(program);
//
//    glGetProgramiv(program, GL_LINK_STATUS, &linked);
//    if(!linked)
//    {
//        GLint infoLen = 0;
//        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
//        if(infoLen > 1) {
//            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
//            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
//            LOG_ERROR("create program fail. %s", infoLog);
//            free(infoLog);
//        }
//        glDeleteProgram(program);
//        return 0;
//    }
//
////    if (linked)
////    {
////        GLuint   binaryLength;
////        void*   binary;
////        FILE*   outfile;
////        GLenum binaryFormat;
////
////        //
////        //  Retrieve the binary from the program object
////        //
////        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, (GLint*)&binaryLength);
////        binary = (void*)malloc(binaryLength);
////        glGetProgramBinaryOES(program, (GLsizei)binaryLength, NULL, &binaryFormat, binary);
////
////        //
////        //  Cache the program binary for future runs
////        //
////        outfile = fopen(filename.c_str(), "wb");
////        if (outfile) {
////            fwrite(&binaryFormat, sizeof(binaryFormat), 1, outfile);
////            fwrite(binary, binaryLength, 1, outfile);
////            fclose(outfile);
////            free(binary);
////        }
////    }
//    return program;
//}

GLfloat positions[] = {
        // Front face
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,

        // Back face
        -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0,

        // Top face
        -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,

        // Bottom face
        -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, -1.0, 1.0, -1.0, -1.0, 1.0,

        // Right face
        1.0, -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0,

        // Left face
        -1.0, -1.0, -1.0, -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0, -1.0
    };

GLushort cubeVertexIndices[] = {
        // front
        0, 1, 2, 0, 2, 3,
        // back
        4, 5, 6, 4, 6, 7,
        // top
        8, 9, 10, 8, 10, 11,
        // bottom
        12, 13, 14, 12, 14, 15,
        // right
        16, 17, 18, 16, 18, 19,
        // left
        20, 21, 22, 20, 22, 23
    };

//static float tvertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,0.5f,  -0.5f };
//
//static GLint vertices[][3] = {
//        { -0x10000, -0x10000, -0x10000 },
//        {  0x10000, -0x10000, -0x10000 },
//        {  0x10000,  0x10000, -0x10000 },
//        { -0x10000,  0x10000, -0x10000 },
//        { -0x10000, -0x10000,  0x10000 },
//        {  0x10000, -0x10000,  0x10000 },
//        {  0x10000,  0x10000,  0x10000 },
//        { -0x10000,  0x10000,  0x10000 }
//};
//
//static GLint colors[][4] = {
//        { 0x00000, 0x00000, 0x00000, 0x10000 },
//        { 0x10000, 0x00000, 0x00000, 0x10000 },
//        { 0x10000, 0x10000, 0x00000, 0x10000 },
//        { 0x00000, 0x10000, 0x00000, 0x10000 },
//        { 0x00000, 0x00000, 0x10000, 0x10000 },
//        { 0x10000, 0x00000, 0x10000, 0x10000 },
//        { 0x10000, 0x10000, 0x10000, 0x10000 },
//        { 0x00000, 0x10000, 0x10000, 0x10000 }
//};
//
//GLubyte indices[] = {
//        0, 4, 5,    0, 5, 1,
//        1, 5, 6,    1, 6, 2,
//        2, 6, 7,    2, 7, 3,
//        3, 7, 4,    3, 4, 0,
//        4, 7, 6,    4, 6, 5,
//        3, 0, 1,    3, 1, 2
//};


Renderer::Renderer()
        : _msg(MSG_NONE), _display(0), _surface(0), _context(0), _angle(0)
{
    LOG_INFO("Renderer instance created");
    pthread_mutex_init(&_mutex, 0);
    return;
}

Renderer::~Renderer()
{
    LOG_INFO("Renderer instance destroyed");
    pthread_mutex_destroy(&_mutex);
    return;
}

void Renderer::start()
{
    LOG_INFO("Creating renderer thread");
    pthread_create(&_threadId, 0, threadStartCallback, this);
    return;
}

void Renderer::stop()
{
    LOG_INFO("Stopping renderer thread");

    // send message to render thread to stop rendering
    pthread_mutex_lock(&_mutex);
    _msg = MSG_RENDER_LOOP_EXIT;
    pthread_mutex_unlock(&_mutex);

    pthread_join(_threadId, 0);
    LOG_INFO("Renderer thread stopped");

    return;
}

void Renderer::setWindow(ANativeWindow *window)
{
    // notify render thread that window has changed
    pthread_mutex_lock(&_mutex);
    _msg = MSG_WINDOW_SET;
    _window = window;
    pthread_mutex_unlock(&_mutex);

    return;
}

void Renderer::getAssetContent(const std::string& filename, std::string& content)
{
    getAssetContent(filename.c_str(), content);
}

void Renderer::getAssetContent(const char* filename, std::string& content)
{
    AAsset* asset = AAssetManager_open(_assetManager, filename, AASSET_MODE_BUFFER);
    if (asset){
        content.clear();
        size_t length = AAsset_getLength(asset);
        content.resize(length, '\0');
        AAsset_read(asset, &content[0], BUFSIZ);
        AAsset_close(asset);
    }
    else {
        content.clear();
        LOG_ERROR("getAssetContent filename=%s doesn't exist.", filename);
    }
}

GLuint Renderer::createProgramFromFiles(const std::string& vsFilename, const std::string& fsFilename)
{
    std::string vsSourceCode;
    std::string fsSourceCode;

    this->getAssetContent(vsFilename, vsSourceCode);
    this->getAssetContent(fsFilename, fsSourceCode);

    GLuint program = glCreateProgram();
    GLuint _v = loadShader(GL_VERTEX_SHADER, vsSourceCode.c_str());
    GLuint _f = loadShader(GL_FRAGMENT_SHADER, fsSourceCode.c_str());

    glAttachShader(program, _v);
    glAttachShader(program, _f);

    glLinkProgram(program);
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);

    if(!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            std::string infoLog(std::max(infoLen, 1), '\0');
            glGetProgramInfoLog(program, infoLen, NULL, &infoLog[0]);
            LOG_ERROR("create program fail. %s", infoLog.c_str());
        }
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

void Renderer::renderLoop()
{
    std::string shader;
    getAssetContent("shaders/shader_1.vs", shader);

    bool renderingEnabled = true;

    LOG_INFO("renderLoop()");

    while (renderingEnabled) {

        pthread_mutex_lock(&_mutex);

        // process incoming messages
        switch (_msg) {

            case MSG_WINDOW_SET:
                initialize();
                break;

            case MSG_RENDER_LOOP_EXIT:
                renderingEnabled = false;
                destroy();
                break;

            default:
                break;
        }
        _msg = MSG_NONE;

        if (_display) {
            drawFrame();
            if (!eglSwapBuffers(_display, _surface)) {
                LOG_ERROR("eglSwapBuffers() returned error %d", eglGetError());
            }
        }

        pthread_mutex_unlock(&_mutex);
    }

    LOG_INFO("Render loop exits");

    return;
}

bool Renderer::initialize()
{
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLDisplay display;
    EGLConfig config;
    EGLint numConfigs;
    EGLint format;
    EGLSurface surface;
    EGLContext context;
    EGLint width;
    EGLint height;
    GLfloat ratio;

    LOG_INFO("Initializing context");

    if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY) {
        LOG_ERROR("eglGetDisplay() returned error %d", eglGetError());
        return false;
    }
    if (!eglInitialize(display, 0, 0)) {
        LOG_ERROR("eglInitialize() returned error %d", eglGetError());
        return false;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs)) {
        LOG_ERROR("eglChooseConfig() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format)) {
        LOG_ERROR("eglGetConfigAttrib() returned error %d", eglGetError());
        destroy();
        return false;
    }

    ANativeWindow_setBuffersGeometry(_window, 0, 0, format);

    if (!(surface = eglCreateWindowSurface(display, config, _window, 0))) {
        LOG_ERROR("eglCreateWindowSurface() returned error %d", eglGetError());
        destroy();
        return false;
    }

    EGLint contextAtt[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE, EGL_NONE };

    if (!(context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAtt))) {
        LOG_ERROR("eglCreateContext() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        LOG_ERROR("eglMakeCurrent() returned error %d", eglGetError());
        destroy();
        return false;
    }

    if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) ||
        !eglQuerySurface(display, surface, EGL_HEIGHT, &height)) {
        LOG_ERROR("eglQuerySurface() returned error %d", eglGetError());
        destroy();
        return false;
    }

    _display = display;
    _surface = surface;
    _context = context;

    glViewport(0, 0, width, height);

    return true;
}

void Renderer::destroy() {
    LOG_INFO("Destroying context");

    eglMakeCurrent(_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    eglTerminate(_display);

    _display = EGL_NO_DISPLAY;
    _surface = EGL_NO_SURFACE;
    _context = EGL_NO_CONTEXT;

    return;
}

void Renderer::drawFrame()
{
    if (sProgram == 0)
    {
        sProgram = createProgramFromFiles("shaders/shader_1.vs", "shaders/shader_1.fs");
        glGenVertexArrays(1, &sVertexArrayObject);
        GL_CHECK_ERROR();

        GLuint buffers[] = {sVertexBuffer, sIndiceBuffer};
        glGenBuffers(sizeof(buffers)/sizeof(GLuint), buffers);
        GL_CHECK_ERROR();

        sVertexBuffer = buffers[0];
        glBindBuffer(GL_ARRAY_BUFFER, sVertexBuffer);
        GL_CHECK_ERROR();
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
        GL_CHECK_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GL_CHECK_ERROR();

        sIndiceBuffer = buffers[1];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sIndiceBuffer);
        GL_CHECK_ERROR();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeVertexIndices), cubeVertexIndices, GL_STATIC_DRAW);
        GL_CHECK_ERROR();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        GL_CHECK_ERROR();

        glBindVertexArray(sVertexArrayObject);
        GL_CHECK_ERROR();

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, sVertexBuffer);
        GL_CHECK_ERROR();
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        GL_CHECK_ERROR();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        GL_CHECK_ERROR();

        glBindVertexArray(0);
        GL_CHECK_ERROR();

        pMatrix = glm::perspective(0.785f, 1.0f, 1.0f, 1000.0f);
        mvMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 使用shader程序
    glUseProgram(sProgram);
    GL_CHECK_ERROR();

    auto uniMatMV = glGetUniformLocation(sProgram, "mvMatrix");
    auto uniMatP = glGetUniformLocation(sProgram, "pMatrix");

    static float orientation[] = {0.0020, 0.0010, 0.0005};
    mvMatrix = glm::rotate(mvMatrix, orientation[0], glm::vec3(1.0f, 0.0f, 0.0f));
    mvMatrix = glm::rotate(mvMatrix, orientation[1], glm::vec3(0.0f, 1.0f, 0.0f));
    mvMatrix = glm::rotate(mvMatrix, orientation[2], glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(uniMatMV, 1, false, glm::value_ptr(mvMatrix));
    GL_CHECK_ERROR();
    glUniformMatrix4fv(uniMatP, 1, false, glm::value_ptr(pMatrix));
    GL_CHECK_ERROR();

//    GLint m_vPosition = glGetAttribLocation(sProgram, "vPosition");
//    GLint m_uColor = glGetUniformLocation(sProgram, "uColor");
//
//    // 为画笔指定顶点位置数据(vPosition)
//    glVertexAttribPointer(m_vPosition, 2, GL_FLOAT, false, 0, tvertices);
//    // 允许顶点位置数据数组
//    glEnableVertexAttribArray(m_vPosition);
//    // 设置属性uColor(颜色 索引,R,G,B,A)
//    glUniform4f(m_uColor, 0.0f, 1.0f, 0.0f, 1.0f);

    glBindVertexArray(sVertexArrayObject);
    GL_CHECK_ERROR();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sIndiceBuffer);
    GL_CHECK_ERROR();

    // 绘制
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
    GL_CHECK_ERROR();

    glBindVertexArray(0);

    _angle += 1.2f;
}

void* Renderer::threadStartCallback(void *myself)
{
    Renderer *renderer = (Renderer*)myself;

    renderer->renderLoop();
    pthread_exit(0);

    return 0;
}
