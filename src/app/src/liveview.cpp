#include <spdlog/spdlog.h>

#include "liveview.h"
#include "qpainter.h"

LiveView::LiveView(QWidget* parent) : QWidget(parent) {
}

LiveView::~LiveView() {
    delete m_imageData;
}

void LiveView::Init(uint32_t width, uint32_t height, ImageFormat fmt) {
    m_width = width;
    m_height = height;
    m_imageInFmt = fmt;
    SetImageFormat(m_imageInFmt);

    memset(m_imageData, 128, m_totalPx);
}

void LiveView::SetImageFormat(ImageFormat fmt) {
    m_imageInFmt = fmt;
    switch (m_imageInFmt) {
        case ImageFormat::Mono16:       //16bit mono, 2 bytes per pixel.
            m_imageOutFmt = QImage::Format::Format_Grayscale16;
            m_totalPx = 2 * m_width * m_height;
            m_imageData = new uint8_t[m_totalPx];
            break;
        default:
            spdlog::warn("Image format conversion not implemented, defaulting to Grayscale16");
            m_imageOutFmt = QImage::Format::Format_Grayscale16;
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
        //memcpy(m_imageData, data, m_totalPx);
        m_imageData = data;
        this->update();
    }
}

void LiveView::paintEvent(QPaintEvent* event) {
    std::unique_lock<std::mutex> lock(m_lock);
    if (m_imageData) {
        auto s = std::max(this->size().width(), this->size().height());
        QRectF target(0.0, 0.0, s, s);

        //QRectF source(0.0, 0.0, 3200.0, 2200.0);
        QImage image = QImage((uchar*)m_imageData,m_width,m_height, m_imageOutFmt).scaled(s, s, Qt::KeepAspectRatio);

        QPainter painter(this);
        painter.drawImage(target, image, target, Qt::MonoOnly);
    }
}
