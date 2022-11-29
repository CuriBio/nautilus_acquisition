#include <spdlog/spdlog.h>

#include "liveview.h"
#include "qpainter.h"

LiveView::LiveView(QWidget* parent) : QWidget(parent) {
}

LiveView::~LiveView() {
    delete m_imageData;
}

void LiveView::Init(uint32_t width, uint32_t height) {
    m_width = width;
    m_height = height;
    m_totalPx = width*height;

    m_imageData = new uint8_t[m_totalPx];
    memset(m_imageData, 128, m_totalPx);
}

void LiveView::clear() {
    if (m_imageData) {
        memset(m_imageData, 128, m_totalPx);
        this->update();
    }
}

void LiveView::updateImage(uint8_t* data) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        memcpy(m_imageData, data, m_totalPx);
        //m_imageData = data;
        this->update();
    }
}

void LiveView::paintEvent(QPaintEvent* event) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        auto s = std::max(this->size().width(), this->size().height());
        QRectF target(0.0, 0.0, s, s);

        //QRectF source(0.0, 0.0, 3200.0, 2200.0);
        QImage image = QImage((uchar*)m_imageData,m_width,m_height, QImage::Format_Grayscale8).scaled(s, s, Qt::KeepAspectRatio);

        QPainter painter(this);
        painter.drawImage(target, image, target, Qt::MonoOnly);
    }
}
