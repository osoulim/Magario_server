//
// Created by mamzi on 7/22/17.
//

#ifndef AGAR_SERVER_SERVER_H
#define AGAR_SERVER_SERVER_H

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QTcpServer>
#include <QtCore/QTimer>
#include "Player.h"

typedef QPair<double, double> pdd;


class Server: public QObject
{
    Q_OBJECT
public:
    Server();
    const double virus_radio = 30;
    const double game_width = 10000, game_height = 10000;
    const int virus_number = 20;
    const int point_cell = 100;
    const int interval = 50;
    const int pnum = (int)(game_height * game_height) / (point_cell * point_cell);
private:
    QVector<Player*> *players;
    QVector< pdd > *viruses;
    QTcpServer *tcpserver;
    QTimer *timer;
    pdd **points;
    void virus_planting();
    bool vir_interact(QPair<double, double>);
    void point_planting();
    void moveAll();
    void sendAll();
    void sendThis(Player*);
    void checkEatAll();
    void checkEat(Player*);
    void eatPointAll();
    void shootChecking();
    void checkVirus();
    int pointNumber;
public slots:
    void new_player();
    void cycle();

};


#endif //AGAR_SERVER_SERVER_H
