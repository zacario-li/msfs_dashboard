#include "AttitudeIndicator.h"
#include <QPainter>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

AttitudeIndicator::AttitudeIndicator(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(200, 200);
}

void AttitudeIndicator::setRoll(float roll)
{
    m_roll_degrees = roll;
    update();
}

void AttitudeIndicator::setPitch(float pitch)
{
    m_pitch_degrees = pitch;
    update();
}

void AttitudeIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawPitchAndRoll(painter);
    drawFixedSymbol(painter);
}

void AttitudeIndicator::drawBackground(QPainter &painter)
{
    int side = qMin(width(), height());
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 200.0, side / 200.0);

    // Outer circle
    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::black);
    painter.drawEllipse(QPointF(0, 0), 98, 98);

    // Bank angle markings
    painter.setPen(Qt::white);
    for (int i = -180; i < 180; i += 10) {
        painter.save();
        painter.rotate(i);

        if (i == 0 || abs(i) == 90 || abs(i) == 180 ) {
             // Major markings (e.g., 0, 90, 180) - could be different length
        } else if (abs(i) % 30 == 0) {
            painter.drawLine(0, -98, 0, -88);
            int angle_to_display = (i < 0) ? -i : i;
             if (angle_to_display != 60 && angle_to_display != 120 && angle_to_display != 150) {
                QString text = QString::number(angle_to_display);
                painter.save();
                painter.translate(0, -78);
                painter.rotate(-i); // counter-rotate to keep text upright
                painter.drawText(QRect(-15, -10, 30, 20), Qt::AlignCenter, text);
                painter.restore();
            }
        } else {
            painter.drawLine(0, -98, 0, -94);
        }
        painter.restore();
    }
    
    // Restore painter state
    painter.restore();
}

void AttitudeIndicator::drawPitchAndRoll(QPainter &painter)
{
    int side = qMin(width(), height());
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 200.0, side / 200.0);
    
    // Set clipping for the main circle
    QRegion clipRegion(QRect(-98, -98, 196, 196), QRegion::Ellipse);
    painter.setClipRegion(clipRegion);

    // Rotate for roll
    painter.rotate(m_roll_degrees);

    // Translate for pitch
    // Pixels per degree of pitch
    const double pixels_per_degree = 1.5;
    painter.translate(0, -m_pitch_degrees * pixels_per_degree);

    // Sky and Ground
    QColor skyColor(50, 150, 250);
    QColor groundColor(140, 90, 40);
    painter.fillRect(QRect(-200, -200, 400, 200), skyColor);
    painter.fillRect(QRect(-200, 0, 400, 200), groundColor);
    
    // Horizon line
    painter.setPen(QPen(Qt::white, 2));
    painter.drawLine(-200, 0, 200, 0);

    // Pitch ladder
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    for (int i = -90; i <= 90; i += 10) {
        if (i == 0) continue;
        int y = static_cast<int>(-i * pixels_per_degree);
        int length = (abs(i) % 20 == 0 && i != 0) ? 40 : 20;
        painter.drawLine(-length / 2, y, length / 2, y);

        if (abs(i) % 20 == 0 && i != 0) {
             painter.drawText(-length / 2 - 25, y + 4, QString::number(i));
             painter.drawText(length / 2 + 5, y + 4, QString::number(i));
        }
    }
    
    painter.restore();
}

void AttitudeIndicator::drawFixedSymbol(QPainter &painter)
{
    int side = qMin(width(), height());
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(QPen(Qt::yellow, 2));
    painter.setBrush(Qt::NoBrush);

    // Fixed wings
    painter.drawLine(-40, 0, -10, 0);
    painter.drawLine(10, 0, 40, 0);
    
    // Center dot
    painter.drawPoint(0,0);

    // Top roll indicator triangle
    QPolygonF triangle;
    triangle << QPointF(0, -98) << QPointF(-5, -88) << QPointF(5, -88);
    painter.setBrush(Qt::yellow);
    painter.drawPolygon(triangle);

    painter.restore();
} 