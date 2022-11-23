#include <spdlog/spdlog.h>

#include "liveview.h"
#include "qpainter.h"

LiveView::LiveView(QWidget* parent) : QWidget(parent) {
}

LiveView::~LiveView() {
}

void LiveView::updateImage(uint8_t* data) {
    spdlog::info("Called updateImage");
    m_imageData = data;

    this->update();
}
void LiveView::paintEvent(QPaintEvent* event) {
    spdlog::info("Called paintEvent");
    if (m_imageData) {
        QRectF target(0.0, 0.0, 650.0, 650.0);
        //QRectF source(0.0, 0.0, 3200.0, 2200.0);
        QImage image = QImage((uchar*)m_imageData,3200,2200, QImage::Format_Grayscale8).scaled(650, 650, Qt::KeepAspectRatio);

        QPainter painter(this);
        painter.drawImage(target, image, target, Qt::MonoOnly);
    }
}
