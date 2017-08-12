//
// Created by mamzi on 7/22/17.
//

#include "Ball.h"


Ball::Ball(double x, double y, double r, Ball *par)
{
    X = x;
    Y = y;
    R = r;
    parent = par;
}

bool Ball::is_eated_by(Ball eater)
{
    return ((*this - eater) < eater.getR()) && (R < eater.getR());
}

double Ball::operator-(Ball second) const
{
    double dX = this->X - second.getX(), dY = this->Y - second.getY();
    return sqrt(dX * dX + dY * dY);
}

double Ball::getX()
{
    return this->X;
}

double Ball::getY()
{
    return this->Y;
}

double Ball::getR()
{
    return this->R;
}

void Ball::setR(double radio)
{
    if(radio < 0)
        return;
    R = radio;

}

void Ball::setY(double y)
{
    if(y < 0)
        return;
    Y = y;
}

void Ball::setX(double x)
{
    if(x < 0)
        return;
    X = x;
}

void Ball::move(double speed, double dX, double dY)
{
    X += speed * dX;
    Y += speed * dY;
    X = (X > 0 ? X : 0);
    Y = (Y > 0 ? Y : 0);
    X = (X < 10000 ? X : 10000);
    Y = (Y < 10000 ? Y : 10000);
}

void Ball::eat(Ball *b)
{
    double r = b->getR();
    R = sqrt(R * R + r * r);
}



bool Ball::is_in(double x, double y)
{
    double dx = X - x, dy = Y - y;
    if(dx * dx + dy * dy < R * R)
        return true;
    return false;

}

void Ball::eat(double r)
{
    R = sqrt(R * R + r * r);
}

Ball *Ball::tof(double dX, double dY, double r, bool shoot)
{
    R = sqrt(R*R - r * r);
    Ball *ball = new Ball(X + (r + R + 5) * dX, Y + (r + R + 5) * dY, r, (shoot? this : nullptr));
    return ball;
}

bool Ball::vir_interact(double x, double y, double r)
{
    double dx = X - x, dy = Y - y;
    if(dx * dx + dy * dy < r * r && R > r)
        return true;
    return false;
}

bool Ball::isChild()
{
    return parent != nullptr;
}

void Ball::backToPar()
{
    double dis = *this - *parent, dx = parent->getX() - X, dy = parent->getY() - Y;
    dx /= dis; dy /= dis;
    parent->move(-0.1, dx, dy);


}

