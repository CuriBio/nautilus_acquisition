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
    vec2 iScreen;
    vec2 iLevels;
    vec2 iAuto;
};

uniform sampler2D u_image;
uniform sampler2D u_rois;

vec4 fragCoord = gl_FragCoord;

void main() {
    vec2 uv = vec2(fragCoord.x, iResolution.y - fragCoord.y) / iResolution.xy;
    uv.x *= iResolution.x / iResolution.y;

    //TODO This needs to handle flips in config settings
    //account for flipped y-axis viewport
    uv.y += (iScreen.y - iResolution.y) / iResolution.y;

    float px = clamp(iAuto.x * (texture(u_image, uv).r - iAuto.y), 0.0f, 1.0f);
    vec4 texColor = vec4(px, px, px, 1.0);

    if (px < iLevels.x) {
        texColor = vec4(0.0, 0.0, 1.0, 1.0);
    } else if (px >= iLevels.y) {
        texColor = vec4(1.0, 0.0, 0.0, 1.0);
    }

    //TODO need a uniform to control showing the rois
    fragColor = mix(texColor, vec4(0.0f, 1.0f, 0.0f, 1.0f), float(ceil(texture(u_rois, uv).r)));
})";

/*
 * @breif Constructs live view widget.
 *
 * @param parent QWidget pointer to parent widget.
 */
LiveView::LiveView(QWidget* parent, uint32_t width, uint32_t height, bool vflip, bool hflip, ImageFormat fmt) : QOpenGLWidget(parent) {
    m_width = width;
    m_height = height;
    // default to img width/height, this will get overwritten in resizeGL when the viewport width/height are known
    m_viewportWidth = width;
    m_viewportHeight = height;

    m_imageInFmt = fmt;

    m_vflip = vflip;
    m_hflip = hflip;

    m_texData = new uint8_t[m_width * m_height * CHANNEL_COUNT];
    memset(m_texData, 255, m_width * m_height * CHANNEL_COUNT);

    float aspect = float(m_width) / float(m_height);
    int min = std::min(this->size().height(), this->size().width());

    m_roisTex = new uint8_t[m_width * m_height];
    memset(m_roisTex, 0x00, m_width * m_height);

    m_backgroundImage = new uint16_t[m_width*m_height];
    for (size_t i = 0; i < m_width*m_height; i++) {
        m_backgroundImage[i] = 0x6000;
    }

    m_shader_uniforms.resolution[0] = m_width;
    m_shader_uniforms.resolution[1] = m_height;

    SetImageFormat(m_imageInFmt);
    setAutoFillBackground(false);
}


/*
 * @breif Deconstructs a live view widget.
 */
LiveView::~LiveView() {
}

void LiveView::UpdateRois(Rois::RoiCfg* cfg, std::vector<std::tuple<uint32_t, uint32_t>> roiOffsets) {
    spdlog::info("Updating roi offsets");
    m_roiOffsets = roiOffsets;
    createRoiTex(cfg, roiOffsets);
}

void LiveView::createRoiTex(Rois::RoiCfg* cfg, std::vector<std::tuple<uint32_t, uint32_t>> roiOffsets) {
    //reset texture
    m_roisTex = new uint8_t[m_viewportWidth * m_viewportHeight];
    memset(m_roisTex, 0x00, m_viewportWidth * m_viewportHeight);

    // TODO not sure if viewport width/height will be the same, probably need to use the min here instead
    auto scaledW = cfg->width / cfg->scale * (m_width / m_viewportWidth);
    auto scaledH = cfg->height / cfg->scale * (m_height / m_viewportHeight);

    for (auto roiStart : m_roiOffsets) {
        drawROI(roiStart, scaledW, scaledH, 1);
    }

    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_viewportWidth, m_viewportHeight, 0, GL_RED, GL_UNSIGNED_BYTE, (GLvoid*)m_roisTex);

    this->update();
}

void LiveView::drawROI(std::tuple<size_t, size_t> offsets, size_t width, size_t height, uint8_t border) {
    if (!m_roisTex) { return; }

    size_t x, y;
    std::tie(x, y) = offsets;

    auto from_xy = [&](int32_t x, int32_t y) {
        return Rois::roiToOffset(x, y, m_width);
    };

    for (size_t i = 0; i < height; i++) {
        if (i < border || i > height-border-1) {
            memset(m_roisTex+from_xy(x, i+y), 0xFF, width);
        } else {
            memset(m_roisTex+from_xy(x, i+y), 0xFF, border);
            memset(m_roisTex+from_xy(x + width - border, i+y), 0xFF, border);
        }
    }
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
            break;
        case ImageFormat::Mono8:       //8bit mono
            m_imageOutFmt = QImage::Format::Format_Grayscale8;
            m_totalPx = m_width * m_height;
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
void LiveView::UpdateImage(uint16_t* data, float scale, float min) {
    std::unique_lock<std::mutex> lock(m_lock);
    m_shader_uniforms.resolution[0] = m_width;
    m_shader_uniforms.resolution[1] = m_height;

    m_shader_uniforms.screen[0] = float(this->size().width());
    m_shader_uniforms.screen[1] = float(this->size().height());

    m_shader_uniforms.levels[0] = 0.0f;
    m_shader_uniforms.levels[1] = float(m_level) / 4095.0f; //TODO Assumes 12-bit images, will need changed to support 8-bit/16-bit images

    m_shader_uniforms.autoCon[0] = scale;
    m_shader_uniforms.autoCon[1] = min;

    m_imageData = (uint8_t*)(data);
    this->update();
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

    float aspect = float(m_width) / float(m_height);
    int min = std::min(this->size().height(), this->size().width());

    f->glViewport(0, aspect * (this->size().height() - min), min / aspect, min * aspect);

    int32_t width = (min / aspect);
    int32_t height = min * aspect;

    float borderColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    f->glGenTextures(2, m_textures);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    f->glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, m_width, m_height, 0, GL_RED, GL_UNSIGNED_SHORT, (GLvoid*)0);

    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, m_textures[1]);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, (GLvoid*)m_roisTex);

    spdlog::info("initializeGL - width: {}, height: {}", width, height);

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

    // get texture uniform location
    f->glUniform1i(f->glGetUniformLocation(m_shaderProgram, "u_image"), 0);
    f->glUniform1i(f->glGetUniformLocation(m_shaderProgram, "u_rois"), 1);

    m_blockIndex = fx->glGetUniformBlockIndex(m_shaderProgram, "R");
    f->glGenBuffers(1, &m_R);
    f->glBindBuffer(GL_UNIFORM_BUFFER, m_R);
    fx->glGetActiveUniformBlockiv(m_shaderProgram, m_blockIndex, GL_UNIFORM_BLOCK_BINDING, &m_binding);

    f->glBufferData(GL_UNIFORM_BUFFER, sizeof(m_shader_uniforms), (void*)&m_shader_uniforms, GL_DYNAMIC_DRAW);
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

    float aspect = float(m_width) / float(m_height);
    int min = std::min(this->size().height(), this->size().width());

    //TODO this needs to handle flip settings in config
    f->glViewport(0, aspect * (this->size().height() - min), min / aspect, min * aspect);

    m_shader_uniforms.resolution[0] = float(min / aspect);
    m_shader_uniforms.resolution[1] = float(min * aspect);

    m_shader_uniforms.screen[0] = float(this->size().width());
    m_shader_uniforms.screen[1] = float(this->size().height());


    //TODO Assumes 12-bit images, will need changed to support 8-bit/16-bit images
    m_shader_uniforms.levels[0] = 1.0f / 4095.0f;
    m_shader_uniforms.levels[1] = float(m_level) / 4095.0f;

    if (!m_imageData) {
        m_shader_uniforms.levels[1] = 1.0f;
        m_shader_uniforms.autoCon[0] = 1.0f;
        m_shader_uniforms.autoCon[1] = 0.0f;
    }


    f->glBufferData(GL_UNIFORM_BUFFER, sizeof(m_shader_uniforms), (void*)&m_shader_uniforms, GL_DYNAMIC_DRAW);
    fx->glBindBufferBase(GL_UNIFORM_BUFFER, m_binding, m_R);

    // bind the texture and PBO
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[m_pboIndex]);
    // copy pixels from PBO to texture object
    // Use offset instead of ponter.
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, m_textures[1]);

    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, m_textures[0]);
    f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_SHORT, 0);

    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo[1 - m_pboIndex]);
    f->glBufferData(GL_PIXEL_UNPACK_BUFFER, m_width*m_height*CHANNEL_COUNT, 0, GL_STREAM_DRAW);
    GLubyte* ptr = (GLubyte*)fx->glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, m_width*m_height*CHANNEL_COUNT, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

    if(ptr) {
        if (m_imageData) {
            memcpy(ptr, m_imageData, m_width*m_height*CHANNEL_COUNT);
        } else {
            memcpy(ptr, (uint8_t*)m_backgroundImage, m_width*m_height*CHANNEL_COUNT);
        }
        fx->glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);  // release pointer to mapping buffer
    }

    // unbind buffer
    f->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, m_textures[1]);

    // bind the vao
    fx->glBindVertexArray(m_vao);
    f->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //unbind texture
    f->glBindTexture(GL_TEXTURE_2D, 0);

    m_pboIndex = 1 - m_pboIndex;
}



/*
 * @breif Resize live view window
 */
void LiveView::resizeGL(int w, int h) {
    m_viewportWidth = w;
    m_viewportHeight = h;
    // TODO createRoiTex() ?
}
