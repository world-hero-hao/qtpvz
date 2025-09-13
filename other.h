#ifndef OTHER_H
#define OTHER_H
#include <QObject>
#include <QGraphicsItem>
#include <QtWidgets>
#include <QPainter>

class other : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    enum{Type=UserType+3};
    other();
    int type() const override;
};

#endif // OTHER_H
