//
// Created by realxie on 2018/9/4.
//

#include "cxx_jni_interface.h"

#include "renderer.h"

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES2/gl2.h>
#include <malloc.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GLES2/gl2ext.h>

#include "logger.h"

#define LOG_TAG "EglSample"

// 顶点着色器的脚本
const char* verticesShader = "attribute vec2 vPosition;            \n"
        "void main(){                         \n"
        "   gl_Position = vec4(vPosition,0,1);\n"
        "}";

// 片元着色器的脚本
const char* fragmentShader = "precision mediump float;         \n"
        "uniform vec4 uColor;             \n"
        "void main(){                     \n"
        "   gl_FragColor = uColor;        \n"
        "}";

static GLint sProgram = 0;

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

GLint createProgram()
{
    //const char * filename = "/storage/emulated/0/APKs/shader.bin";
    std::string filename = getDocumentPath() + "/shader.bin";


    {
        GLint   binaryLength;
        void*   binary;
        GLint   success;
        FILE*   infile;
        GLenum binaryFormat;


        //
        //  Read the program binary
        //
        infile = fopen(filename.c_str(), "rb");
        if (infile) {
            fseek(infile, 0, SEEK_END);
            binaryLength = (GLint) ftell(infile) - sizeof(binaryFormat);
            binary = (void *) malloc(binaryLength + 1);
            fseek(infile, 0, SEEK_SET);
            fread(&binaryFormat, sizeof(binaryFormat), 1, infile);
            fread(binary, binaryLength, 1, infile);
            fclose(infile);

            GLuint progObj = glCreateProgram();

            //
            //  Load the binary into the program object -- no need to link!
            //
            glProgramBinaryOES(progObj, binaryFormat, binary, binaryLength);
            free(binary);

            glGetProgramiv(progObj, GL_LINK_STATUS, &success);

            if (GL_TRUE == success)
            {
                return progObj;
            }
        }

    }

    GLuint program = glCreateProgram();
    GLuint _v = loadShader(GL_VERTEX_SHADER, verticesShader);
    GLuint _f = loadShader(GL_FRAGMENT_SHADER, fragmentShader);
    GLint linked;

    glAttachShader(program, _v);
    glAttachShader(program, _f);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if(!linked)
    {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if(infoLen > 1) {
            char *infoLog = (char *) malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(program, infoLen, NULL, infoLog);
            LOG_ERROR("create program fail. %s", infoLog);
            free(infoLog);
        }
        glDeleteProgram(program);
        return 0;
    }

    if (linked)
    {
        GLuint   binaryLength;
        void*   binary;
        FILE*   outfile;
        GLenum binaryFormat;

        //
        //  Retrieve the binary from the program object
        //
        glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, (GLint*)&binaryLength);
        binary = (void*)malloc(binaryLength);
        glGetProgramBinaryOES(program, (GLsizei)binaryLength, NULL, &binaryFormat, binary);

        //
        //  Cache the program binary for future runs
        //
        outfile = fopen(filename.c_str(), "wb");
        if (outfile) {
            fwrite(&binaryFormat, sizeof(binaryFormat), 1, outfile);
            fwrite(binary, binaryLength, 1, outfile);
            fclose(outfile);
            free(binary);
        }
    }
    return program;
}

static float tvertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,0.5f,  -0.5f };

static GLint vertices[][3] = {
        { -0x10000, -0x10000, -0x10000 },
        {  0x10000, -0x10000, -0x10000 },
        {  0x10000,  0x10000, -0x10000 },
        { -0x10000,  0x10000, -0x10000 },
        { -0x10000, -0x10000,  0x10000 },
        {  0x10000, -0x10000,  0x10000 },
        {  0x10000,  0x10000,  0x10000 },
        { -0x10000,  0x10000,  0x10000 }
};

static GLint colors[][4] = {
        { 0x00000, 0x00000, 0x00000, 0x10000 },
        { 0x10000, 0x00000, 0x00000, 0x10000 },
        { 0x10000, 0x10000, 0x00000, 0x10000 },
        { 0x00000, 0x10000, 0x00000, 0x10000 },
        { 0x00000, 0x00000, 0x10000, 0x10000 },
        { 0x10000, 0x00000, 0x10000, 0x10000 },
        { 0x10000, 0x10000, 0x10000, 0x10000 },
        { 0x00000, 0x10000, 0x10000, 0x10000 }
};

GLubyte indices[] = {
        0, 4, 5,    0, 5, 1,
        1, 5, 6,    1, 6, 2,
        2, 6, 7,    2, 7, 3,
        3, 7, 4,    3, 4, 0,
        4, 7, 6,    4, 6, 5,
        3, 0, 1,    3, 1, 2
};


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



void Renderer::renderLoop()
{
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

//    glDisable(GL_DITHER);
//    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
//    glClearColor(0, 0, 0, 0);
//    glEnable(GL_CULL_FACE);
//    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    ratio = (GLfloat) width / height;
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    glFrustumf(-ratio, ratio, -1, 1, 1, 10);

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
        sProgram = createProgram();
    }

    // 设置clear color颜色RGBA(这里仅仅是设置清屏时GLES20.glClear()用的颜色值而不是执行清屏)
    glClearColor(1.0f, 0, 0, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // 使用某套shader程序
    glUseProgram(sProgram);

    GLint m_vPosition = glGetAttribLocation(sProgram, "vPosition");
    GLint m_uColor = glGetUniformLocation(sProgram, "uColor");

    // 为画笔指定顶点位置数据(vPosition)
    glVertexAttribPointer(m_vPosition, 2, GL_FLOAT, false, 0, tvertices);
    // 允许顶点位置数据数组
    glEnableVertexAttribArray(m_vPosition);
    // 设置属性uColor(颜色 索引,R,G,B,A)
    glUniform4f(m_uColor, 0.0f, 1.0f, 0.0f, 1.0f);
    // 绘制
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);

    _angle += 1.2f;
}

void* Renderer::threadStartCallback(void *myself)
{
    Renderer *renderer = (Renderer*)myself;

    renderer->renderLoop();
    pthread_exit(0);

    return 0;
}
