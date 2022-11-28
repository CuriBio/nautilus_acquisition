#include <spdlog/spdlog.h>

#include "liveview.h"
#include "qpainter.h"

LiveView::LiveView(QWidget* parent) : QWidget(parent) {
    m_imageData = new uint8_t[3200*2200];
    memset(m_imageData, 128, 3200*2200);
}

LiveView::~LiveView() {
}

void LiveView::clear() {
    memset(m_imageData, 128, 3200*2200);
    this->update();
}

void LiveView::updateImage(uint8_t* data) {
    std::unique_lock<std::mutex> lock(m_lock);
    memcpy(m_imageData, data, 3200*2200);
    //m_imageData = data;
    this->update();
}

void LiveView::paintEvent(QPaintEvent* event) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        spdlog::info("Width: {}, Height: {}", this->size().width(), this->size().height());
        auto width = this->size().width();
        auto height = this->size().height();
        auto s = std::max(width, height);
        QRectF target(0.0, 0.0, s, s);

        //QRectF source(0.0, 0.0, 3200.0, 2200.0);
        QImage image = QImage((uchar*)m_imageData,3200,2200, QImage::Format_Grayscale8).scaled(s, s, Qt::KeepAspectRatio);

        QPainter painter(this);
        painter.drawImage(target, image, target, Qt::MonoOnly);
    }
}
