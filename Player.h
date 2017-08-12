//
// Created by mamzi on 7/22/17.
//

#ifndef AGAR_SERVER_PLAYER_H
#define AGAR_SERVER_PLAYER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtNetwork/QTcpSocket>
#include "Ball.h"

class Player: public QObject
{
    Q_OBJECT
public:
    Player(QTcpSocket* = nullptr, double = 0, double = 0);
    void move();
    void socket_send(QString);
    QVector<Ball*> get_balls();
    double radioSum();
    double maxX();
    double minX();
    double maxY();
    double minY();
    double getdX();
    double getdY();
    QString get_username();
    int get_color();
    const double defaultR = 20;
    const double speedmult = 7;
    void addBall(Ball*);
    void clearBalls();
    void shootChecking();
    bool offline;
private:
    QVector<Ball*> *balls;
    QVector<Ball*> *shoot;
    QVector<int> *shootTime;
    QVector<QPair<double, double> > *shootWay;
    QTcpSocket *socket;
    double dX, dY;
    double get_speed();
    QString username;
    int color;

public slots:
    void new_way();
    void off();
};


#endif //AGAR_SERVER_PLAYER_H
