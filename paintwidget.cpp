#include "paintwidget.h"

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QColor>

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(280, 280); // Размер виджета остается 280x280
    image = QImage(28, 28, QImage::Format_RGB32); // Размер изображения теперь 28x28
    clear();
}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    QPainter canvasPainter(this);
    // Масштабируем изображение при рисовании
    canvasPainter.drawImage(QRect(0, 0, 280, 280), image);
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
{
    // Масштабируем координаты мыши
    lastPoint = event->pos() / 10;
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPainter imagePainter(&image);

    // Создаем радиальный градиент
    QRadialGradient gradient(event->pos() / 10, 2.5, event->pos() / 10);
    gradient.setColorAt(0, Qt::white);      // Центр градиента белый
    gradient.setColorAt(1, Qt::transparent); // Края прозрачные

    QPen pen(QBrush(gradient), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    imagePainter.setPen(pen);

    imagePainter.drawLine(lastPoint, event->pos() / 10);
    lastPoint = event->pos() / 10;

    update();
    emit imageUpdated();
}

QVector<double> PaintWidget::getPixelData() const
{
    QVector<double> pixelData(28 * 28);
    int index = 0;
    for (int y = 0; y < 28; ++y) {
        for (int x = 0; x < 28; ++x) {
            QColor color = image.pixelColor(x, y);
            pixelData[index++] = double(color.red());
        }
    }
    return pixelData;
}

void PaintWidget::clear()
{
    image.fill(Qt::black);
    update();
}
