//
// Created by mamzi on 7/22/17.
//

#include "Server.h"
#define max(a, b) (a > b? a : b)
#define min(a, b) (a < b? a : b)


double get_rand(int st, int en)
{
    int res = st + qrand() % (en - st);
    return (double)res;
}

bool in_box(double X1, double X2, double Y1, double Y2, double minx, double maxx, double miny, double maxy)
{
    if((X2 - minx) * (maxx - X1) < 0)
        return false;
    if((Y2 - miny) * (maxy - Y1) < 0)
        return false;
    return true;
}

bool Server::vir_interact( QPair<double, double> V1)
{
    for(auto V2 : *viruses)
    {
        double dx = V1.first - V2.first, dy = V1.second - V2.second;
        if (dx * dx + dy * dy < virus_radio * virus_radio)
            return true;
    }
    return false;
}

void Server::virus_planting()
{
    int Xst = (int) game_width/10, Yst = (int) game_height/10;
    for(int i = 0; i < virus_number; i++)
    {
        pdd tmp;
        while(true)
        {
            tmp.first = get_rand(Xst, Xst * 8), tmp.second = get_rand(Yst, Yst * 8);
            if(vir_interact(tmp))
                continue;
            break;
        }
        //qDebug()<<tmp.first<<" "<<tmp.second<<"\n";
        viruses->append(tmp);
    }
}

void Server::point_planting()
{
    int cells = (int)game_height / point_cell;
    pointNumber = cells * cells;
    for(int i = 0; i < cells; i++)
        for(int j = 0; j < cells; j++)
        {
            if(points[i][j].first)
                continue;
            int Xst = i * point_cell + 1, Xen = (i + 1) * point_cell - 1;
            int Yst = j * point_cell + 1, Yen = (j + 1) * point_cell - 1;
            points[i][j] = qMakePair(get_rand(Xst, Xen), get_rand(Yst, Yen));
            //qDebug()<<points[i][j].first<<"===="<<points[i][j].second<<"\n";
        }
}

Server::Server()
{
    qsrand((uint)time(NULL));
    players = new QVector<Player*>; //player list initializing
    viruses = new QVector< pdd >; //virus list initializing
    virus_planting();

    //points planting int the map
    int cells = (int)game_height/point_cell + 10;
    points = new pdd*[cells];
    for(int i = 0; i < cells; i++)
        points[i] = new pdd[cells];
    point_planting();

    //server configuration
    tcpserver = new QTcpServer();
    QHostAddress *address = new QHostAddress("127.0.0.1");
    quint16 port = 8000;
    tcpserver->listen(*address, port);

    //connections
    connect(tcpserver, SIGNAL(newConnection()), this, SLOT(new_player()));

    //timer initializing
    timer = new QTimer();
    timer->setInterval(interval);
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(cycle()));

}

void Server::new_player()
{
    QTcpSocket *socket = tcpserver->nextPendingConnection(); //setting the socket

    // find empty place for new player
    int Xst =(int)game_width/10, Yst =(int)game_height/10;
    Ball tmp;
    while(true)
    {
        double X = get_rand(Xst, Xst * 9), Y = get_rand(Yst, Yst * 9);
        tmp.setX(X);
        tmp.setY(Y);
        bool flag = false;
        for(Player* p : *players)
            for(Ball *ball : p->get_balls())
                    flag = flag | tmp.is_eated_by(*ball);
        if(!flag)
            break;
    }

    //new player
    Player *nPlayer = new Player(socket, (int)tmp.getX(), (int)tmp.getY());
    players->append(nPlayer);
    qDebug()<<"new_player";

}

void Server::cycle()
{
    moveAll();
    shootChecking();
    checkEatAll();
    checkVirus();
    eatPointAll();
    sendAll();
    if(pointNumber < pnum / 4)
        point_planting();
}

void Server::moveAll()
{
    for(Player *p : *players)
        p->move();
}

void Server::sendAll()
{
    for(Player *p : *players)
    {
        if(p->offline)
            continue;

        sendThis(p);
    }
}

void Server::sendThis(Player *player)
{
    if(player->get_balls().size() == 0)
    {
        player->socket_send("0 0 0 0\n");
        player->offline = true;
    }

    //finding the area
    double maxx = player->maxX(), minx = player->minX(), maxy = player->maxY(), miny = player->minY(), disx = (maxx - minx), disy = (maxy - miny);
    double extra = 250, alpha = 1.5;
    maxx += alpha * disx + extra;
    maxy += alpha * disy + extra;
    minx -= alpha * disx + extra;
    miny -= alpha * disy + extra;

    //view rectangle
    QString vrect;
    vrect = QString::number(minx) + " " + QString::number(miny) + " " + QString::number(maxx) + " " + QString::number(maxy);

    minx = max(minx, 0);
    maxx = min(maxx, game_width);

    miny = max(miny, 0);
    maxy = min(maxy, game_height);

    //this player balls
    QString myStr = player->get_username() + " " + QString::number(player->get_color()) + " " + QString::number(player->get_balls().size()) + " ";
    for(Ball *b : player->get_balls())
        myStr += QString::number(b->getX()) + " " + QString::number(b->getY()) + " " + QString::number(b->getR()) + " ";


    //find the balls in the area
    QString pStr;
    int pcount = 0;
    for(Player *p : *players)
    {
        if(p == player)
            continue;
        QVector<Ball*> tmp;
        for(Ball *b : p->get_balls())
            if(in_box(b->getX() - b->getR(),b->getX() + b->getR(), b->getY() - b->getR(), b->getY() + b->getR(), minx, maxx, miny, maxy))
                tmp.push_back(b);
        if(tmp.isEmpty())
            continue;
        pcount++;
        pStr += p->get_username() + " " + QString::number(p->get_color()) + " " + QString::number(tmp.size()) + " ";
        for(Ball *b : tmp)
            pStr += QString::number(b->getX()) + " " + QString::number(b->getY()) + " " + QString::number(b->getR()) + " ";
    }
    pStr = QString::number(pcount) + " " + pStr;

    //find the viruses in the area
    QString vStr;
    int vcount = 0;
    for(pdd v : *viruses)
        if(in_box(v.first - virus_radio, v.first + virus_radio, v.second - virus_radio, v.second + virus_radio, minx, maxx, miny, maxy))
        {
            vcount++;
            vStr += QString::number(v.first) + " " + QString::number(v.second) + " ";
        }
    vStr = QString::number(vcount) + " " + vStr;

    //find the points in the area
    QString poStr;
    int Xst = (int) minx / point_cell, Xen = (int) maxx / point_cell, Yst = (int) miny / point_cell, Yen = (int) maxy / point_cell;
    int pocount = 0;
    for(int i = Xst; i < Xen; i++)
        for(int j = Yst; j < Yen; j++)
            if(points[i][j].first)
            {
                pocount++;
                poStr += QString::number(points[i][j].first) + " " + QString::number(points[i][j].second) + " ";
            }
    poStr = QString::number(pocount) + " " + poStr;


    //qDebug()<<"-----------------\n"<<pStr<<"\n"<<vStr<<"\n"<<poStr<<"------------------\n";

    QString sendinfo = vrect + " " + myStr + " " + pStr + " " + vStr + " " + poStr;
    player->socket_send(sendinfo.simplified()+"\n");
    //timer->stop();

    //qDebug()<<sendinfo;
}

void Server::checkEatAll()
{
    for(Player* p: *players)
        checkEat(p);
}

void Server::checkEat(Player *play)
{
    QVector<Ball*> tmp;
    for(Ball *ball: play->get_balls())
    {
        bool eated = false;
        for(Player *p: *players)
        {
            for(Ball *b: p->get_balls())
            {
                if(b == ball)
                    continue;
                if(ball->is_eated_by(*b))
                {
                    b->eat(ball);
                    eated = true;
                }
            }
        }
        if(!eated)
            tmp.push_back(ball);
    }
    play->clearBalls();
    for(Ball* b: tmp)
        play->addBall(b);
}

void Server::eatPointAll()
{
    for(Player *p: *players)
        for(Ball *b: p->get_balls())
        {
            double minx = b->getX() - b->getR(), miny = b->getY() - b->getR(), maxx = b->getX() + b->getR(), maxy = b->getY() + b->getR();
            int xes = (int) minx / point_cell, yes = (int) miny / point_cell, xen = (int) maxx / point_cell, yen = (int) maxy / point_cell;
            for(int i = xes; i <= xen; i++)
                for(int j = yes; j <= yen; j++)
                {
                    double x = points[i][j].first, y = points[i][j].second;
                    if(b->is_in(x, y))
                    {
                        b->eat();
                        pointNumber--;
                        points[i][j] = qMakePair(0, 0);
                        qDebug()<<pointNumber;
                    }
                }
        }
}

void Server::shootChecking()
{
    for(Player *p: *players)
    {
        p->shootChecking();
    }

}

void Server::checkVirus()
{
    for(Player *p: *players)
    {
        for(Ball *b : p->get_balls())
        {
            for(auto vir: *viruses)
            {
                if(b->vir_interact(vir.first, vir.second, virus_radio))
                {
                    p->addBall(b->tof(p->getdY(),p->getdX(), b->getR() / 1.41));
                }
            }
        }
    }

}
