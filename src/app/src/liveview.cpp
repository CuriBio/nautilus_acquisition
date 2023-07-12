/*
 * MIT License
 *
 * Copyright (c) 2022 Curi Bio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


/*********************************************************************
 * @file  liveview.cpp
 *
 * @brief Implemetation of the liveview widget.
 *********************************************************************/
#include <spdlog/spdlog.h>

#include "liveview.h"
#include "qpainter.h"

std::string vertShader = R"(
#version 460
layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 inTexCoord;
out vec2 texCoord;

void main() {
    gl_Position = vec4(vertex.xy, 0, 1);
    texCoord = inTexCoord;
})";

std::string fragShader = R"(
#version 460

layout (location = 0) out vec4 fragColor;
layout (location = 1) in vec2 texCoord;
layout (binding = 0) uniform R { 
    vec2 iResolution;
    vec2 iLevels;
};
uniform sampler2D u_image;
vec2 fragCoord = gl_FragCoord.xy;

void main() {
    vec2 uv = fragCoord / iResolution.xy;

    float px = texture(u_image, texCoord).r;
    if (px < iLevels.x) {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0);
    } else if (px > iLevels.y) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        fragColor = vec4(px, px, px, 1.0);
    }
})";

/*
 * @breif Constructs live view widget.
 *
 * @param parent QWidget pointer to parent widget.
 */
LiveView::LiveView(QWidget* parent) : QOpenGLWidget(parent) {
}


/*
 * @breif Deconstructs a live view widget.
 */
LiveView::~LiveView() {
}


/*
 * @breif Initialize live view widget.
 *
 * Initializes LiveView widget with image width, height and pixel format.
 *
 * @param width Width of the image.
 * @param height Height of the image.
 * @param fmt ImageFormat type.
 */
void LiveView::Init(uint32_t width, uint32_t height, bool vflip, bool hflip, ImageFormat fmt) {
    m_width = width;
    m_height = height;
    m_imageInFmt = fmt;
    m_vflip = vflip;
    m_hflip = hflip;
    m_texData = new uint8_t[m_width * m_height * CHANNEL_COUNT];
    memset(m_texData, 255, m_width * m_height * CHANNEL_COUNT);

    SetImageFormat(m_imageInFmt);
    setAutoFillBackground(false);
}


/*
 * @breif Set live view image format.
 *
 * Sets the LiveView image format after the widget is created.
 *
 * @param fmt The ImageFormat for pixel data being displayed.
 */
void LiveView::SetImageFormat(ImageFormat fmt) {
    m_imageInFmt = fmt;
    switch (m_imageInFmt) {
        case ImageFormat::Mono16:       //16bit mono, 2 bytes per pixel.
            m_imageOutFmt = QImage::Format::Format_Grayscale16;
            m_totalPx = 2*m_width * m_height;
            m_imageData = new uint8_t[m_totalPx];
            memset(m_imageData, 256, m_totalPx);
            break;
        case ImageFormat::Mono8:       //8bit mono
            m_imageOutFmt = QImage::Format::Format_Grayscale8;
            m_totalPx = m_width * m_height;
            m_imageData = new uint8_t[m_totalPx];
            memset(m_imageData, 256, m_totalPx);
            break;
        default:
            spdlog::warn("Image format conversion not implemented, defaulting to Grayscale8");
            m_imageOutFmt = QImage::Format::Format_Grayscale8;
            break;
    }
}


/*
 * @breif Clears the live view display.
 *
 * Resets the LiveView memory to 50% gray.
 */
void LiveView::Clear() {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        memset(m_imageData, 256, m_totalPx);
        this->update();
    }
}


/*
 * @breif Update live view image.
 *
 * Updates the displayed image in LiveView.
 *
 * @param data The raw pixel data to display.
 */
void LiveView::UpdateImage(uint16_t* data) {
    std::unique_lock<std::mutex> lock(m_lock);
    m_uniforms[0] = float(this->size().width());
    m_uniforms[1] = float(this->size().height());
    m_uniforms[2] = 0.0f;
    m_uniforms[3] = float(m_level) / 4096.0f;

    if (m_imageData) {
        auto s = std::max(this->size().width(), this->size().height());
        m_target = QRectF(0.0, 0.0, s, s);

        m_imageData = (uint8_t*)(data);
        //m_image = QImage((uchar*)m_imageData, m_width, m_height, m_imageOutFmt).scaled(s, s, Qt::KeepAspectRatio);
        //m_image.mirror(m_hflip, m_vflip);
        //m_texture->setData(m_image);
        this->update();
    }
}


/*
 * @breif Initalizes openGL for live view.
 */
void LiveView::initializeGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    QOpenGLExtraFunctions *fx = QOpenGLContext::currentContext()->extraFunctions();

    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
    f->glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT);

    f->glGenTextures(1, &m_texColor);
    f->glBindTexture(GL_TEXTURE_2D, m_texColor);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    f->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RED, GL_UNSIGNED_SHORT, (GLvoid*)0);
    f->glBindTexture(GL_TEXTURE_2D, 0);

    //PBOs
    f->glGenBuffers(2, m_pbo);
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[0]);
    f->glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*CHANNEL_COUNT, 0, GL_STREAM_DRAW);
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[1]);
    f->glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*CHANNEL_COUNT, 0, GL_STREAM_DRAW);
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    GLint status;
    //vertex shader
    const char* vs = vertShader.c_str();
    m_vertexShader = f->glCreateShader(GL_VERTEX_SHADER);
    f->glShaderSource(m_vertexShader, 1, &vs, NULL);
    f->glCompileShader(m_vertexShader);
    f->glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        spdlog::error("Failed to compile vertex shader");
    }

    // fragment shader
    const char* fs = fragShader.c_str();
    m_fragmentShader = f->glCreateShader(GL_FRAGMENT_SHADER);
    f->glShaderSource(m_fragmentShader, 1, &fs, NULL);
    f->glCompileShader(m_fragmentShader);
    f->glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        spdlog::error("Failed to compile fragment shader");
    }

    // link shaders
    m_shaderProgram = f->glCreateProgram();
    f->glAttachShader(m_shaderProgram, m_vertexShader);
    f->glAttachShader(m_shaderProgram, m_fragmentShader);
    f->glLinkProgram(m_shaderProgram);
    // check for linking errors

    // get texture uniform location
    m_texLoc = f->glGetUniformLocation(m_shaderProgram, "u_image");

    // generate and bind the vao
    fx->glGenVertexArrays(1, &m_vao);
    fx->glBindVertexArray(m_vao);
    
    // generate and bind the vertex buffer object
    f->glGenBuffers(1, &m_vbo);
    f->glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            
    // data for a fullscreen quad (this time with texture coords)
    GLfloat vertexData[] = {
    //  X     Y     Z           U     V     
       1.0f, 1.0f, 0.0f,       1.0f, 1.0f, // vertex 0
      -1.0f, 1.0f, 0.0f,       0.0f, 1.0f, // vertex 1
       1.0f,-1.0f, 0.0f,       1.0f, 0.0f, // vertex 2
      -1.0f,-1.0f, 0.0f,       0.0f, 0.0f, // vertex 3
    }; // 4 vertices with 5 components (floats) each

    // fill with data
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*5, vertexData, GL_STATIC_DRAW);
           
    // set up generic attrib pointers
    f->glEnableVertexAttribArray(0);
    f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 0*sizeof(GLfloat));
 
    f->glEnableVertexAttribArray(1);
    f->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (char*)0 + 3*sizeof(GLfloat));
    
    // generate and bind the index buffer object
    f->glGenBuffers(1, &m_ibo);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
            
    GLuint indexData[] = {
        0,1,2, // first triangle
        2,1,3, // second triangle
    };

    // fill with data
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*2*3, indexData, GL_STATIC_DRAW);
    // "unbind" vao
    fx->glBindVertexArray(0);

    f->glDeleteShader(m_vertexShader);
    f->glDeleteShader(m_fragmentShader);
    f->glUseProgram(m_shaderProgram);

    m_blockIndex = fx->glGetUniformBlockIndex(m_shaderProgram, "R");
    f->glGenBuffers(1, &m_R);
    f->glBindBuffer(GL_UNIFORM_BUFFER, m_R);
    fx->glGetActiveUniformBlockiv(m_shaderProgram, m_blockIndex, GL_UNIFORM_BLOCK_BINDING, &m_binding);

    f->glBufferData(GL_UNIFORM_BUFFER, sizeof(m_uniforms), m_uniforms, GL_DYNAMIC_DRAW);
    fx->glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, m_R);
}


/*
 * @breif Draws the pixel data to the screen.
 */
void LiveView::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    QOpenGLExtraFunctions *fx = QOpenGLContext::currentContext()->extraFunctions();
    f->glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    f->glClear(GL_COLOR_BUFFER_BIT);

    f->glUseProgram(m_shaderProgram);

    m_uniforms[0] = float(m_width);
    m_uniforms[1] = float(m_height);
    m_uniforms[2] = 0.0f;
    m_uniforms[3] = float(m_level) / 4096.0f;

    float height = 0.75 * this->size().width();
    f->glViewport(0, this->size().height() - height, this->size().width(), int(height));
    f->glBufferData(GL_UNIFORM_BUFFER, sizeof(m_uniforms), m_uniforms, GL_DYNAMIC_DRAW);
    fx->glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, m_R);

    // bind the texture and PBO
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[m_pboIndex]);
    // copy pixels from PBO to texture object
    // Use offset instead of ponter.
    f->glBindTexture(GL_TEXTURE_2D, m_texColor);
    f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_RED, GL_UNSIGNED_SHORT, 0);

    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[1 - m_pboIndex]);
    f->glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*CHANNEL_COUNT, 0, GL_STREAM_DRAW);
    GLubyte* ptr = (GLubyte*)fx->glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_width*m_height*CHANNEL_COUNT, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
    if(ptr) {
        if (m_imageData) { memcpy(ptr, m_imageData, m_width*m_height*CHANNEL_COUNT); }
        fx->glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
    }
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    f->glBindTexture(GL_TEXTURE_2D, m_texColor);
    
    // bind the vao
    fx->glBindVertexArray(m_vao);
    f->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // unbind texture
    f->glBindTexture(GL_TEXTURE_2D, 0);
    m_pboIndex = 1 - m_pboIndex;
    f->glViewport(0, 0, this->size().width(), this->size().height());
}
