#include "RpmIndicator.h"
#include <QPainter>

RpmIndicator::RpmIndicator(QWidget *parent) : QWidget(parent)
{
    setMinimumSize(100, 100);
}

void RpmIndicator::setTitle(const QString &title)
{
    m_title = title;
}

void RpmIndicator::setRpmPercent(float rpm)
{
    m_rpm_percent = rpm;
    update();
}

void RpmIndicator::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    painter.save();
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(side / 100.0, side / 100.0);

    // Background
    painter.setPen(QPen(QColor(50, 50, 50), 4));
    painter.drawEllipse(QPointF(0, 0), 45, 45);

    // RPM Arc
    QPen arcPen(Qt::green, 4);
    painter.setPen(arcPen);
    int startAngle = 90 * 16;
    int spanAngle = -static_cast<int>(m_rpm_percent / 100.0f * 360.0f * 16.0f);
    painter.drawArc(QRectF(-45, -45, 90, 90), startAngle, spanAngle);

    // Title (e.g., "ENG 1")
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 8));
    painter.drawText(QRectF(-40, -45, 80, 20), Qt::AlignCenter, m_title);

    // Percentage Text
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(QRectF(-40, -15, 80, 30), Qt::AlignCenter, QString::number(m_rpm_percent, 'f', 1) + "%");

    painter.restore();
} 