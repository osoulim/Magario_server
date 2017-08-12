//
// Created by mamzi on 7/22/17.
//

#include "Player.h"
#define max(a, b) (a > b? a : b)
#define min(a, b) (a < b? a : b)



Player::Player(QTcpSocket* sock, double x, double y)
{
    balls = new QVector<Ball*>();
    Ball *tmp = new Ball(x, y, defaultR, 0);
    balls->push_back(tmp);

    socket = sock;

    dX = 0, dY = 0;

    username = "new_user";
    color = 0;
    offline = false;

    shoot = new QVector<Ball*>;
    shootTime = new QVector<int>;
    shootWay = new QVector<QPair<double, double> >;
    connect(socket, SIGNAL(readyRead()), this, SLOT(new_way()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(off()));
}

QVector<Ball*> Player::get_balls()
{
    return *balls;
}

double Player::get_speed()
{
    double mx = 0;
    for(Ball* b : *balls)
        if(b->getR() > mx)
            mx = b->getR();
    return defaultR / mx * speedmult;
}

void Player::move()
{
    double speed = get_speed();
    for(Ball* b : *balls)
    {
        b->move(speed, dX, dY);
        if(b->isChild())
            b->backToPar();
    }
}

double Player::radioSum()
{
    double ret = 0;
    for(Ball *b : *balls)
        ret += b->getR();

}

double Player::maxX()
{
    double ret = 0;
    for(Ball *b : *balls)
        ret = max(ret, b->getX() + b->getR());
    return ret;
}

double Player::minX()
{
    double ret = 100000;
    for(Ball *b : *balls)
        ret = min(ret, b->getX() - b->getR());
    return ret;
}

double Player::maxY()
{
    double ret = 0;
    for(Ball *b : *balls)
        ret = max(ret, b->getY() + b->getR());
    return ret;
}

double Player::minY()
{
    double ret = 100000;
    for(Ball *b : *balls)
        ret = min(ret, b->getY() - b->getR());
    return ret;
}

QString Player::get_username()
{
    return username;
}

int Player::get_color()
{
    return color;
}

void Player::socket_send(QString info)
{
    socket->write(info.toLocal8Bit());
}

void Player::new_way()
{

    QString m = socket->readLine();
    QStringList q = m.split(' ');
    if(q[0] == "w") //w for way
    {
        dX = q[1].toDouble();
        dY = q[2].toDouble();
    }
    else if(q[0] == "l")// l for login
    {
        username = q[1];
        color = q[2].toInt();
    }
    else if(q[0] == "p")// p for increasing size
    {
        for(Ball *b : *balls)
            b->eat(q[1].toDouble());
    }
    else if(q[0] == "t")// t for tof
    {
        for(Ball *ball : *balls)
        {
            if ((!dX && !dY) || ball->getR() < 30 || balls->size() == 10)
                return;
            Ball *b = ball->tof(dX, dY);
            balls->push_back(b);
            shoot->push_back(b);
            shootTime->push_back(q[1].toInt());
            shootWay->push_back(qMakePair(dX, dY));
        }
    }
    else if(q[0] == "n")// n for nesf
    {
        for(Ball *ball : *balls)
        {
            if ((!dX && !dY) || ball->getR() < 30 || balls->size() >= 10)
                return;
            Ball *b = ball->tof(dX, dY, ball->getR()/1.41, true);
            balls->push_back(b);
            shoot->push_back(b);
            shootTime->push_back(q[1].toInt());
            shootWay->push_back(qMakePair(dX, dY));
        }
    }
}

void Player::addBall(Ball *b)
{
    balls->push_back(b);
}

void Player::clearBalls()
{
    balls->clear();

}

void Player::shootChecking()
{
    QVector<Ball*> tmp;
    QVector<int> temp;
    QVector<QPair<double, double> > wemp;
    for(int i = 0; i < shoot->size(); i++)
    {
        Ball *b = shoot->at(i);
        b->move(speedmult, shootWay->at(i).first, shootWay->at(i).second);
        if(shootTime->at(i) > 1)
        {
            tmp.push_back(b);
            temp.push_back(shootTime->at(i) - 1);
            wemp.push_back(shootWay->at(i));
        }
    }
    shootTime->clear();
    shoot->clear();
    shootWay->clear();
    for(int i = 0; i < tmp.size(); i++)
    {
        shoot->push_back(tmp[i]);
        shootTime->push_back(temp[i]);
        shootWay->push_back(wemp[i]);
    }
}

void Player::off()
{
    offline = true;
    qDebug()<<username<<" leave the room";
}

double Player::getdX()
{
    return dX;
}

double Player::getdY()
{
    return dY;
}




