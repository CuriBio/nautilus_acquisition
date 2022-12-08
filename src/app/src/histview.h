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
