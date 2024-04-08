/*
 *
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
 * @file  liveview.h
 *
 * @brief Definition of the liveview widget class.
 *********************************************************************/
#ifndef LIVEVIEW_H
#define LIVEVIEW_H

#include <mutex>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <BitmapFormat.h>

#define MAX_ROIS 4
const int CHANNEL_COUNT = 2;

#pragma pack(1)
struct ShaderUniforms {
    float resolution[2];
    float screen[2];
    float levels[2];
    float autoCon[2];
};

/*
 *  LiveView display widget.
 *
 * LiveView display widget for painting raw pixel data to screen
 * using openGL.
 */
class LiveView : public QOpenGLWidget {
    Q_OBJECT

    public:
        LiveView(QWidget* parent, uint32_t width, uint32_t height, bool vflip, bool hflip, ImageFormat fmt);
        virtual ~LiveView();

        void Clear();
        void UpdateImage(uint16_t* data, float scale, float min);
        void SetImageFormat(ImageFormat fmt);
        void SetLevel(int level) { m_level = level; };

        //QT Overrides
        void initializeGL();
        void paintGL();

    private:
        uint8_t* m_imageData{nullptr};
	uint8_t* m_roisTex{nullptr};

        uint16_t* m_backgroundImage;
        std::mutex m_lock;

        uint32_t m_width{0};
        uint32_t m_height{0};
        uint32_t m_totalPx{0};
        int m_level{4095};

        bool m_vflip{false};
        bool m_hflip{false};

        ImageFormat m_imageInFmt;
        QImage m_image;
        QRectF m_target;
        QImage::Format m_imageOutFmt;

        ShaderUniforms m_shader_uniforms = {
            .resolution = {0.0f},
            .screen = {0.0f},
            .levels = {0.0f, 1.0f},
            .autoCon = {0.0f},
        };

        GLuint m_vao, m_vbo, m_ibo;
        unsigned int m_vertexShader, m_fragmentShader, m_shaderProgram;
        uint8_t* m_texData = nullptr;

	GLuint m_textures[2];
        GLuint m_blockIndex, m_R;
        GLint m_binding, m_texLoc, m_roisLoc;

        GLuint m_pbo[2];           // IDs of PBOs
        int m_pboIndex{0};

        void updatePixels(GLubyte* ptr, size_t len);
        void drawFOV_ROIs(uint32_t roi_size, uint32_t well_spacing, uint8_t scaled_px, uint16_t rows, uint16_t cols);
        void drawROI(int32_t x, int32_t y, uint16_t size, uint8_t border);
};

#endif //LIVEVIEW_H
