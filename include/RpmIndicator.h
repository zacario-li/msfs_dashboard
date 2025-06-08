#ifndef RPMINDICATOR_H
#define RPMINDICATOR_H

#include <QWidget>
#include <QPainter>

class RpmIndicator : public QWidget
{
    Q_OBJECT

public:
    explicit RpmIndicator(QWidget *parent = nullptr);
    void setTitle(const QString &title);

public slots:
    void setRpmPercent(float rpm);
    void setThrottlePercent(float throttle);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float m_rpm_percent = 0.0f;
    float m_throttle_percent = 0.0f;
    QString m_title;
};

#endif // RPMINDICATOR_H 