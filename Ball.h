//
// Created by mamzi on 7/22/17.
//

#ifndef AGAR_SERVER_BALL_H
#define AGAR_SERVER_BALL_H

#include <cmath>

class Ball
{
public:
    Ball(double = 0, double = 0, double = 0, Ball* = nullptr);
    bool is_eated_by(Ball);
    bool vir_interact(double, double, double);
    double operator -(Ball) const;
    double getX();
    double getY();
    double getR();
    void setX(double);
    void setY(double);
    void setR(double);
    void move(double, double, double);
    void eat(Ball*);
    void eat(double = 2);
    bool is_in(double x, double y);
    Ball* tof(double, double, double = 10, bool = false);
    bool isChild();
    void backToPar();
private:
    double X, Y, R;
    Ball *parent;
};


#endif //AGAR_SERVER_BALL_H
