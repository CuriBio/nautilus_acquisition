#include <spdlog/spdlog.h>

#include "histview.h"
#include "qpainter.h"

HistView::HistView(QWidget* parent) : QOpenGLWidget(parent) {
}

HistView::~HistView() {
}

void HistView::Init(uint32_t* hist, size_t size) {
    m_size = size;
    m_hist = hist;
}

void HistView::Update(uint32_t histMax, uint32_t imgMin, uint32_t imgMax) {
    m_max = histMax;
    m_imin = imgMin;
    m_imax = imgMax;
    this->update();
}


void HistView::initializeGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


void HistView::drawHistogram(QPainter* p) {
    auto w = this->size().width();
    auto h = this->size().height();

    //rescale to 256 bins
    uint32_t hmax = 0;
    uint32_t hist[257] = {0};
    double scalex = w / 256.0;

    double bscale = (m_imax - m_imin) / 256.0;
    double bwidth = (256.0 / static_cast<double>(m_imax - m_imin + 1));

    for (size_t i = m_imin; i <= m_imax; i++) {
        uint32_t index = static_cast<uint32_t>(bwidth * (i-m_imin));
        hist[index] += m_hist[i];

        if (hist[index] > hmax) { hmax = hist[index]; }
    }

    for(size_t i = 0; i < 256; i++) {
        int32_t x = static_cast<int32_t>(i * scalex);
        int32_t y = static_cast<int32_t>(h * hist[i] / double(hmax));

        y = std::clamp<int32_t>(y, 0, h);

        if (y > 0) {
            p->fillRect(x, h-y, scalex, y, QColor(45,45,45));
        }
    }
}


void HistView::paintGL() {
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT);

    QPainter painter(this);
    if (m_hist) {
        drawHistogram(&painter);
    }
}
