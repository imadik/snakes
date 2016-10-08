#ifndef SNAKESWIDGET_H
#define SNAKESWIDGET_H

#include <QColor>
#include <QWidget>

#define CANVAS_SIZE 100 // размер поля
#define MIN_CELLS 100 //минимальное количество заполненых клеток
#define MAX_CELLS 1000 // максимальное количество заполненых ячеек
#define SNAKE_RADIUS 25 // размер змеек
#define NUM_OF_SNAKES 2 // количество змеек
#define SNAKES_START 2 // начало обозначения змеек на поле
#define MAXIMUM_SNAKES 5
#define WAVES_START 10 //от этого зависит макс количество змеек

class SnakesCords;
class snakesWidget: public QWidget
{
    Q_OBJECT
protected:
    void paintEvent(QPaintEvent *);
    QTimer* timer;

public:
    explicit snakesWidget(QWidget *parent = 0);
    ~snakesWidget();
private:
    int *cells;
    std::vector<SnakesCords> traceRoute(SnakesCords head, SnakesCords tail);
    std::list<SnakesCords> generateSnake(int);
    void deleteWaves();
    std::vector<std::list<SnakesCords>> snakes;
    void deleteSnake(int);
    void drawSnakes();
    void randCoords(int &x, int &y, std::list<SnakesCords>::iterator headPointer, int num);


public slots:
    void generateCells();


private slots:
    void paintGrid(QPainter &p);
    void paintCells(QPainter &p);
    void makeMove();



};

// class to storage snake cordinates
class SnakesCords
{
public:
    int x, y;
    SnakesCords():x(0),y(0) {}
    SnakesCords(int j, int k):x(j),y(k) {}
    ~SnakesCords(){}
    bool operator< (SnakesCords obj)
    {
        return ((x + y) < (obj.x + obj.y));
    }
    bool operator== (SnakesCords obj)
    {
        return ((x == obj.x) && (y == obj.y));
    }
};
#endif // SNAKESWIDGET_H
