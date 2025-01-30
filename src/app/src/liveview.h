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
#include <tuple>
#include <vector>

#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include <BitmapFormat.h>
#include <Rois.h>

#define MAX_ROIS 4
const int CHANNEL_COUNT = 2;
const uint32_t ROIS_TEX_MAX_SIDE_LEN = 5000;

#pragma pack(1)
struct ShaderUniforms {
    float resolution[2];
    float screen[2];
    float levels[2];
    float autoCon[2];
    uint32_t displayRois; // there seem to be memory layout issues when using a bool here
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
        LiveView(QWidget* parent, uint32_t width, uint32_t height, bool vflip, bool hflip, bool displayRois);
        virtual ~LiveView();

        void Clear();
        void UpdateImage(uint8_t* data, float scale, float min);
        void SetBitDepth(uint16_t bitDepth);
        void SetLevel(int level) { m_level = level; };
        void UpdateRois(Rois::RoiCfg cfg, std::vector<std::tuple<uint32_t, uint32_t>> roiOffsets);
        void UpdateDisplayRois(bool display);

        //QT Overrides
        void initializeGL();
        void paintGL();
        void resizeGL(int w, int h);


    private:
        uint8_t* m_imageData{nullptr};
        std::vector<std::tuple<uint32_t, uint32_t>> m_roiOffsets;
        Rois::RoiCfg m_roiCfg;

        uint16_t* m_backgroundImage;
        std::mutex m_lock;

        uint32_t m_width{0};
        uint32_t m_height{0};
        uint32_t m_totalPx{0};
        uint16_t m_bitDepth{12};
        uint16_t m_bytesPerPixel{2};
        float m_maxPixelIntensity{4095.0f};
        int m_level{4095};

        uint8_t* m_roisTex{nullptr};
        uint32_t m_roisTexCurrentSideLen{ROIS_TEX_MAX_SIDE_LEN};

        bool m_vflip{false};
        bool m_hflip{false};

        ImageFormat m_imageInFmt{ImageFormat::Mono16};
        QImage m_image;
        QRectF m_target;
        QImage::Format m_imageOutFmt;

        ShaderUniforms m_shader_uniforms = {
            .resolution = {0.0f, 0.0f},
            .screen = {0.0f, 0.0f},
            .levels = {0.0f, 1.0f},
            .autoCon = {0.0f, 0.0f},
            .displayRois = 0,
        };

        GLuint m_vao, m_vbo, m_ibo;
        unsigned int m_vertexShader, m_fragmentShader, m_shaderProgram;
        uint8_t* m_texData = nullptr;

        GLuint m_textures[2];
        GLuint m_blockIndex, m_R;
        GLint m_binding, m_texLoc, m_roisLoc;
        GLint m_internalformat{GL_R16};
        GLenum m_type{GL_UNSIGNED_SHORT};

        GLuint m_pbo[2];           // IDs of PBOs
        int m_pboIndex{0};

        void SetImageFormat(ImageFormat fmt);
        void drawROI(std::tuple<size_t, size_t> offset, size_t width, size_t height, uint8_t border);
        void createRoiTex();
};

#endif //LIVEVIEW_H
