/*
 * MIT License
 *test
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

#include <BitmapFormat.h>


/*
 *  LiveView display widget.
 *
 * LiveView display widget for painting raw pixel data to screen
 * using openGL.
 */
class LiveView : public QOpenGLWidget {
    Q_OBJECT

    public:
        LiveView(QWidget* parent = nullptr);
        virtual ~LiveView();

        void Init(uint32_t width, uint32_t height, ImageFormat fmt);
        void Clear();
        void UpdateImage(uint8_t* data);
        void SetImageFormat(ImageFormat fmt);

        //QT Overrides
        void initializeGL();
        void paintGL();

    private:
        uint8_t* m_imageData{nullptr};
        std::mutex m_lock;

        uint32_t m_width{0};
        uint32_t m_height{0};
        uint32_t m_totalPx{0};

        ImageFormat m_imageInFmt;
        QImage m_image;
        //QPixmap m_pixmap;
        QRectF m_target;
        QImage::Format m_imageOutFmt;
};

#endif //LIVEVIEW_H
