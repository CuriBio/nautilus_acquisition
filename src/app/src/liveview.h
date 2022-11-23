#ifndef LIVEVIEW_H
#define LIVEVIEW_H

#include <QWidget>

class LiveView : public QWidget {
    Q_OBJECT

    public:
        LiveView(QWidget* parent = nullptr);
        virtual ~LiveView();

        void updateImage(uint8_t* data);
        void paintEvent(QPaintEvent* event);

    private:
        uint8_t* m_imageData{nullptr};
};

#endif //LIVEVIEW_H
