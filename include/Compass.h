#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>
#include <QPainter>

class Compass : public QWidget
{
    Q_OBJECT

public:
    explicit Compass(QWidget *parent = nullptr);

public slots:
    void setHeading(float heading);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawCompass(QPainter &painter);

    float m_heading_degrees = 0.0f;
};

#endif // COMPASS_H 