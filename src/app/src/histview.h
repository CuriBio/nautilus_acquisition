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
 * @file  histview.h
 *
 * @brief Definition of the histogram widget class.
 *********************************************************************/
#ifndef HISTVIEW_H
#define HISTVIEW_H

#include <mutex>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QOpenGLFunctions>


class HistView : public QOpenGLWidget {
    Q_OBJECT

    private:
        uint32_t* m_hist{nullptr};
        size_t m_size{0};
        uint32_t m_max{0}, m_imin{0}, m_imax{0};

    public:
        HistView(QWidget* parent = nullptr);
        virtual ~HistView();

        void Init(uint32_t* hist, size_t size);
        void Update(uint32_t histMax, uint32_t imgMin, uint32_t imgMax);

    private:
        void drawHistogram(QPainter* p);
        void initializeGL();
        void paintGL();
};

#endif //HISTVIEW_H
