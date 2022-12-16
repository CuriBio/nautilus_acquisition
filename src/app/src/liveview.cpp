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
#include <spdlog/spdlog.h>

#include "liveview.h"
#include "qpainter.h"


LiveView::LiveView(QWidget* parent) : QOpenGLWidget(parent) {
}

LiveView::~LiveView() {
    //delete m_imageData;
}

void LiveView::Init(uint32_t width, uint32_t height, ImageFormat fmt) {
    m_width = width;
    m_height = height;
    m_imageInFmt = fmt;

    SetImageFormat(m_imageInFmt);

    setAutoFillBackground(false);
    memset(m_imageData, 128, m_totalPx);
}

void LiveView::SetImageFormat(ImageFormat fmt) {
    m_imageInFmt = fmt;
    switch (m_imageInFmt) {
        case ImageFormat::Mono16:       //16bit mono, 2 bytes per pixel.
            m_imageOutFmt = QImage::Format::Format_Grayscale16;
            m_totalPx = 2*m_width * m_height;
            m_imageData = new uint8_t[m_totalPx];
            break;
        case ImageFormat::Mono8:       //8bit mono
            m_imageOutFmt = QImage::Format::Format_Grayscale8;
            m_totalPx = m_width * m_height;
            m_imageData = new uint8_t[m_totalPx];
            break;
        default:
            spdlog::warn("Image format conversion not implemented, defaulting to Grayscale8");
            m_imageOutFmt = QImage::Format::Format_Grayscale8;
            break;
    }
}

void LiveView::Clear() {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        memset(m_imageData, 128, m_totalPx);
        this->update();
    }
}

void LiveView::UpdateImage(uint8_t* data) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        auto s = std::max(this->size().width(), this->size().height());
        m_target = QRectF(0.0, 0.0, s, s);

        m_imageData = data;
        m_image = QImage((uchar*)m_imageData,m_width,m_height, m_imageOutFmt).scaled(s, s, Qt::KeepAspectRatio);
        this->update();
    }
}


void LiveView::initializeGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
}

void LiveView::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);

    QPainter painter(this);
    painter.drawImage(m_target, m_image, m_target, Qt::MonoOnly);
}
