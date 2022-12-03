#ifndef LIVEVIEW_H
#define LIVEVIEW_H

#include <mutex>
#include <QWidget>

#include <BitmapFormat.h>

class LiveView : public QWidget {
    Q_OBJECT

    public:
        LiveView(QWidget* parent = nullptr);
        virtual ~LiveView();

        void Init(uint32_t width, uint32_t height, ImageFormat fmt);
        void Clear();
        void UpdateImage(uint8_t* data);
        void SetImageFormat(ImageFormat fmt);

        void paintEvent(QPaintEvent* event);

    private:
        uint8_t* m_imageData{nullptr};
        std::mutex m_lock;

        uint32_t m_width{0};
        uint32_t m_height{0};
        uint32_t m_totalPx{0};
        ImageFormat m_imageInFmt;
        QImage::Format m_imageOutFmt;
};

#endif //LIVEVIEW_H
