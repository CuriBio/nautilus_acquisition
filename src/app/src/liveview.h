#ifndef LIVEVIEW_H
#define LIVEVIEW_H

#include <mutex>
#include <QWidget>

class LiveView : public QWidget {
    Q_OBJECT

    public:
        LiveView(QWidget* parent = nullptr);
        virtual ~LiveView();

        void Init(uint32_t width, uint32_t height);
        void clear();
        void updateImage(uint8_t* data);
        void paintEvent(QPaintEvent* event);

    private:
        uint8_t* m_imageData{nullptr};
        std::mutex m_lock;

        uint32_t m_width{0};
        uint32_t m_height{0};
        uint32_t m_totalPx{0};
};

#endif //LIVEVIEW_H
