#ifndef ATTITUDEINDICATOR_H
#define ATTITUDEINDICATOR_H

#include <QWidget>
#include <QPainter>

class AttitudeIndicator : public QWidget
{
    Q_OBJECT

public:
    explicit AttitudeIndicator(QWidget *parent = nullptr);

public slots:
    void setRoll(float roll);
    void setPitch(float pitch);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void drawBackground(QPainter &painter);
    void drawPitchAndRoll(QPainter &painter);
    void drawFixedSymbol(QPainter &painter);

    float m_roll_degrees = 0.0f;
    float m_pitch_degrees = 0.0f;
};

#endif // ATTITUDEINDICATOR_H 