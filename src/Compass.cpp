#include "Compass.h"
#include <QPainter>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Compass::Compass(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(200, 200);
}

void Compass::setHeading(float heading)
{
    m_heading_degrees = heading;
    update();
}

void Compass::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    drawCompass(painter);
}

void Compass::drawCompass(QPainter &painter)
{
    int side = qMin(width(), height());
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 200.0, side / 200.0);

    // Outer circle
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(0, 0), 98, 98);

    painter.save();
    // Rotate the painter to the current heading
    painter.rotate(m_heading_degrees);

    // Markings
    painter.setPen(Qt::white);
    for (int i = 0; i < 360; i += 10) {
        painter.save();
        painter.rotate(i);
        if (i % 30 == 0) {
            painter.drawLine(0, -98, 0, -88);
            QString text;
            if (i == 0) text = "N";
            else if (i == 90) text = "E";
            else if (i == 180) text = "S";
            else if (i == 270) text = "W";
            else text = QString::number(i);
            
            painter.save();
            painter.translate(0, -78);
            painter.rotate(-i); // counter-rotate to keep text upright
            painter.drawText(QRect(-15, -10, 30, 20), Qt::AlignCenter, text);
            painter.restore();

        } else {
            painter.drawLine(0, -98, 0, -94);
        }
        painter.restore();
    }
    painter.restore();
    
    // Fixed lubber line at the top
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(Qt::white);
    QPolygonF lubber;
    lubber << QPointF(0, -98) << QPointF(-5, -88) << QPointF(5, -88);
    painter.drawPolygon(lubber);

    // Fixed airplane icon in the center
    painter.setPen(QPen(Qt::yellow, 1));
    painter.setBrush(Qt::yellow);
    QPolygonF airplane;
    airplane << QPointF(0, -18);   // Nose
    airplane << QPointF(2, -10);
    airplane << QPointF(12, -5);  // Right wing tip
    airplane << QPointF(12, 0);
    airplane << QPointF(2, 0);
    airplane << QPointF(2, 12);
    airplane << QPointF(6, 17);   // Right tail tip
    airplane << QPointF(4, 17);
    airplane << QPointF(0, 15);   // Tail center
    airplane << QPointF(-4, 17);
    airplane << QPointF(-6, 17);  // Left tail tip
    airplane << QPointF(-2, 12);
    airplane << QPointF(-2, 0);
    airplane << QPointF(-12, 0);
    airplane << QPointF(-12, -5); // Left wing tip
    airplane << QPointF(-2, -10);
    painter.drawPolygon(airplane);
    
    // Display heading value in a professional-looking box above the airplane
    QRectF headingRect(-25, -45, 50, 20);
    painter.setPen(QPen(Qt::white, 1));
    painter.setBrush(QColor(20, 20, 20, 180)); // Semi-transparent dark gray
    painter.drawRoundedRect(headingRect, 5, 5);
    
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    QString headingText = QString::asprintf("%03.0f", fmod(m_heading_degrees, 360));
    painter.drawText(headingRect, Qt::AlignCenter, headingText);

    painter.restore();
} 