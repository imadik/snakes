#include <QTimer>
#include <QTime>
#include <QRectF>
#include <QPainter>
#include "snakeswidget.h"
#include <vector>
#include <QMessageBox>



snakesWidget::snakesWidget(QWidget *parent) :
    QWidget(parent),
    timer(new QTimer(this))
{
    //timer interaval
    timer->setInterval(150);
    cells = new int[CANVAS_SIZE * CANVAS_SIZE];
    memset (cells, 0, (CANVAS_SIZE * CANVAS_SIZE) * sizeof(int));
    connect(timer, SIGNAL(timeout()), this, SLOT(makeMove()));
    QTime midnight(0,0,0);
    qsrand(midnight.secsTo(QTime::currentTime()));
}

snakesWidget::~snakesWidget()
{

}
//painting event
void snakesWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    paintCells(p);
    paintGrid(p);

}

// generating canvas and new snakes on it
void snakesWidget::generateCells()
{
    timer->stop();
    snakes.clear();
    memset (cells, 0, (CANVAS_SIZE * CANVAS_SIZE) * sizeof(int));
//    QTime midnight(0,0,0);
//    qsrand(midnight.secsTo(QTime::currentTime()));
    int cellsCount =  MIN_CELLS + (qrand() % (MAX_CELLS - MIN_CELLS));
    for (int i = 0; i < cellsCount; i++)
    {
        int j = 0, k = 0;
        while (cells[(k = (qrand() % CANVAS_SIZE)) * CANVAS_SIZE + (j = (qrand() % CANVAS_SIZE))])
        {}
        if (!cells[k * CANVAS_SIZE + j])
            cells[k * CANVAS_SIZE + j] = 1;
    }
    for (int i = 0; i < NUM_OF_SNAKES; i++)
    {
        std::list<SnakesCords> lsnake = generateSnake(SNAKES_START + i);;
        snakes.push_back(lsnake);
    }
    update();
    timer->start();
}

//generating new snake
std::list<SnakesCords> snakesWidget::generateSnake(int num)
{
    SnakesCords head;
    SnakesCords tail;
    std::vector<SnakesCords> snake;

    //randoming coords that we can trace, also they need to be lesser than snake radius
    while (cells[(head.y = (qrand() % CANVAS_SIZE)) * CANVAS_SIZE + (head.x = (qrand() % CANVAS_SIZE))])
    {}
    do{
        while (cells[(tail.y = (qrand() % CANVAS_SIZE)) * CANVAS_SIZE + (tail.x = (qrand() % CANVAS_SIZE))] || (abs(tail.x-head.x) > SNAKE_RADIUS) || (abs(tail.y-head.y) > SNAKE_RADIUS))
        {}
        snake = traceRoute(head, tail);
    }while(snake.size() < 2);
    //drawing snakes
    for (uint i = 0; i < snake.size(); i++)
    {
        cells [snake[i].y * CANVAS_SIZE + snake[i].x] = num;
    }
    std::list<SnakesCords> lsnake;
    std::copy(snake.begin(), snake.end(),std::back_inserter(lsnake));
    return lsnake;
}

//making new move for each snake
void snakesWidget::makeMove()
{
    for (int i = 0; i < NUM_OF_SNAKES; i++)
    {
        int x = 0, y = 0;
        std::list<SnakesCords> bufList = snakes[i];
        std::list<SnakesCords>::iterator headPointer = bufList.end();
        headPointer--;
        std::list<SnakesCords>::iterator tailPointer = bufList.begin();
        //first checking that it still can go to it's tail, if doesnt - new snake
        std::vector<SnakesCords> checkRoute = traceRoute(*headPointer, *tailPointer);
        if (checkRoute.size() < 2)
        {
            deleteSnake(SNAKES_START + i);
            std::list<SnakesCords> lsnake = generateSnake(SNAKES_START + i);
            snakes[i] = lsnake;
            drawSnakes();
            continue;
        }
        else
        {
            cells[(*headPointer).y  * CANVAS_SIZE + (*headPointer).x] = SNAKES_START + i;
            cells[(*tailPointer).y  * CANVAS_SIZE + (*tailPointer).x] = SNAKES_START + i;
        }
        // randoming new step
        randCoords(x,y,headPointer, SNAKES_START + i);
        //if there are no way - generating new snake
        if ((x == -1) && (y == -1))
        {
            deleteSnake(SNAKES_START + i);
            std::list<SnakesCords> lsnake = generateSnake(SNAKES_START + i);
            snakes[i] = lsnake;
            drawSnakes();
            continue;
        }
        // if it collide with other snake on the canvas - new snake
        if ((cells[y * CANVAS_SIZE + x] >= SNAKES_START) && (cells[y * CANVAS_SIZE + x] < (SNAKES_START + NUM_OF_SNAKES)))
        {
            //нужно удалять после и генерировать змейку после удаления!!!  но возможно это не причина!!!
            deleteSnake(SNAKES_START + i);
            std::list<SnakesCords> lsnake = generateSnake(SNAKES_START + i);
            snakes[i] = lsnake;
            drawSnakes();
            continue;
        }
        SnakesCords newSnakeCell(x, y);
        snakes[i].pop_front();
        snakes[i].push_back(newSnakeCell);
        deleteSnake(SNAKES_START + i);
        drawSnakes();

    }
    update();


}
// randoming and choosing next step of the snake, it cannot  collide with static objects, canvas boarders and itself
void snakesWidget::randCoords(int &x, int &y, std::list<SnakesCords>::iterator headPointer, int num)
{
    std::list<SnakesCords>::iterator afterHeadPointer = headPointer;
    afterHeadPointer--;
    do{
        int way = rand() % 4;
        switch (way)
        {
        case 0:
            x = -1;
            y = 0;
            break;
        case 1:
            x = 1;
            y = 0;
            break;
        case 2:
            x = 0;
            y = -1;
            break;
        case 3:
            x = 0;
            y = 1;
            break;
        };

    }while(((headPointer->x + x) == (afterHeadPointer)->x) && ((headPointer->y + y) == (afterHeadPointer)->y));
    x = headPointer->x + x;
    y = headPointer->y + y;
    //FIXIT можно сделать компактнее
    if (!(x >= 0) || !(x < CANVAS_SIZE) || !(y >= 0) || !(y < CANVAS_SIZE) || (cells[y * CANVAS_SIZE + x] == 1) || (cells[y * CANVAS_SIZE + x] == num))
    {
        x = headPointer->x - 1;
        y = headPointer->y;
        // важно что сначала проверяем попадание в поле!!!
        if ((!(x >= 0) || !(x < CANVAS_SIZE) || !(y >= 0) || !(y < CANVAS_SIZE) || (cells[y * CANVAS_SIZE + x] == 1) || (cells[y * CANVAS_SIZE + x] == num)) \
                || ((x == (afterHeadPointer)->x) && (y == (afterHeadPointer)->y)))
        {
            x = headPointer->x + 1;
            y = headPointer->y;
            if ((!(x >= 0) || !(x < CANVAS_SIZE) || !(y >= 0) || !(y < CANVAS_SIZE) || (cells[y * CANVAS_SIZE + x] == 1) || (cells[y * CANVAS_SIZE + x] == num)) \
                    || ((x == (afterHeadPointer)->x) && (y == (afterHeadPointer)->y)))
            {
                x = headPointer->x;
                y = headPointer->y - 1;
                if ((!(x >= 0) || !(x < CANVAS_SIZE) || !(y >= 0) || !(y < CANVAS_SIZE) || (cells[y * CANVAS_SIZE + x] == 1) || (cells[y * CANVAS_SIZE + x] == num)) \
                        || ((x == (afterHeadPointer)->x) && (y == (afterHeadPointer)->y)))
                {
                    x = headPointer->x;
                    y = headPointer->y + 1;
                    if ((!(x >= 0) || !(x < CANVAS_SIZE) || !(y >= 0) || !(y < CANVAS_SIZE) || (cells[y * CANVAS_SIZE + x] == 1) || (cells[y * CANVAS_SIZE + x] == num)) \
                            || ((x == (afterHeadPointer)->x) && (y == (afterHeadPointer)->y)))
                    {
                        x = -1;
                        y = -1;
                    }
                }
            }
        }
    }
}
// trace route from one cell to another using LE algoritm that is also called waves
std::vector<SnakesCords> snakesWidget::traceRoute(SnakesCords head, SnakesCords tail)
{
    int currentWave = WAVES_START;
    cells[head.y  * CANVAS_SIZE + head.x] = currentWave++;
    cells[tail.y  * CANVAS_SIZE + tail.x] = 0;
    std::vector<SnakesCords> beforeIteration;
    std::vector<SnakesCords> afterIteration;
    std::vector<SnakesCords> outVector;
    beforeIteration.push_back(head);
    bool findFlag = false;
    int cellsMarked = 0;

    do
    {
        cellsMarked = 0;
        currentWave++;
        for (uint i = 0; i < beforeIteration.size(); i++)
        {
            //можно сделать компактнее
            if (((beforeIteration[i].x - 1) >= 0))
                if (cells[beforeIteration[i].y  * CANVAS_SIZE + (beforeIteration[i].x - 1)] == 0)
                {
                    cells[beforeIteration[i].y  * CANVAS_SIZE + (beforeIteration[i].x - 1)] = currentWave;
                    cellsMarked++;
                    SnakesCords cell((beforeIteration[i].x - 1), beforeIteration[i].y);
                    afterIteration.push_back(cell);
                    if (cell == tail)
                    {
                        findFlag = true;
                        break;

                    }
                }
            if (((beforeIteration[i].y - 1) >= 0))
                if (cells[(beforeIteration[i].y - 1)  * CANVAS_SIZE + beforeIteration[i].x] == 0)
                {
                    cells[(beforeIteration[i].y - 1)  * CANVAS_SIZE + beforeIteration[i].x] = currentWave;
                    cellsMarked++;
                    SnakesCords cell (beforeIteration[i].x, (beforeIteration[i].y - 1));
                    afterIteration.push_back(cell);
                    if (cell == tail)
                    {
                        findFlag = true;
                        break;

                    }
                }
            if (((beforeIteration[i].x + 1) < CANVAS_SIZE))
                if (cells[beforeIteration[i].y  * CANVAS_SIZE + (beforeIteration[i].x + 1)] == 0)
                {
                    cells[beforeIteration[i].y  * CANVAS_SIZE + (beforeIteration[i].x + 1)] = currentWave;
                    cellsMarked++;
                    SnakesCords cell((beforeIteration[i].x + 1), beforeIteration[i].y);
                    afterIteration.push_back(cell);
                    if (cell == tail)
                    {
                        findFlag = true;
                        break;

                    }
                }
            if (((beforeIteration[i].y + 1) < CANVAS_SIZE))
                if (cells[(beforeIteration[i].y + 1)  * CANVAS_SIZE + beforeIteration[i].x] == 0)
                {
                    cells[(beforeIteration[i].y + 1)  * CANVAS_SIZE + beforeIteration[i].x] = currentWave;
                    cellsMarked++;
                    SnakesCords cell (beforeIteration[i].x, (beforeIteration[i].y + 1));
                    afterIteration.push_back(cell);
                    if (cell == tail)
                    {
                        findFlag = true;
                        break;

                    }
                }


        }
        beforeIteration.clear();
        beforeIteration.swap(afterIteration);
    }while ((!findFlag) && (cellsMarked > 0));
    if (findFlag)
    {
        bool routeFlag = false;
        SnakesCords cellBefore, cellAfter;
        cellBefore = tail;
        outVector.push_back(tail);

        do
        {
            //можно сделать компактнее
            if ((cellBefore.x - 1) >= 0)
                if ((cells[cellBefore.y * CANVAS_SIZE + (cellBefore.x - 1)] >= WAVES_START) && (cells[cellBefore.y * CANVAS_SIZE + (cellBefore.x - 1)] < currentWave))
                {
                    SnakesCords cell ((cellBefore.x - 1), cellBefore.y);
                    cellAfter = cell;
                    if (cell == head)
                        routeFlag = true;
                }
            if ((cellBefore.y - 1) >= 0)
                if ((cells[(cellBefore.y - 1) * CANVAS_SIZE + (cellBefore.x)] >= WAVES_START) && (cells[(cellBefore.y - 1) * CANVAS_SIZE + (cellBefore.x)] < currentWave))
                {
                    SnakesCords cell ((cellBefore.x), (cellBefore.y - 1));
                    cellAfter = cell;
                    if (cell == head)
                        routeFlag = true;
                }
            if ((cellBefore.x + 1) < CANVAS_SIZE)
                if ((cells[cellBefore.y * CANVAS_SIZE + (cellBefore.x + 1)] >= WAVES_START) && (cells[cellBefore.y * CANVAS_SIZE + (cellBefore.x + 1)] < currentWave))
                {
                    SnakesCords cell ((cellBefore.x + 1), cellBefore.y);
                    cellAfter = cell;
                    if (cell == head)
                        routeFlag = true;
                }
            if ((cellBefore.y + 1) < CANVAS_SIZE)
                if ((cells[(cellBefore.y + 1) * CANVAS_SIZE + (cellBefore.x)] >= WAVES_START) && (cells[(cellBefore.y + 1) * CANVAS_SIZE + (cellBefore.x)] < currentWave))
                {
                    SnakesCords cell ((cellBefore.x), (cellBefore.y + 1));
                    cellAfter = cell;
                    if (cell == head)
                        routeFlag = true;
                }
            currentWave = cells[cellAfter.y * CANVAS_SIZE + cellAfter.x];
            //this is some memory leak error, and this IF is needed to pass it
            if (outVector.size() > (CANVAS_SIZE * CANVAS_SIZE))
            {
                outVector.clear();
                break;
            }
            outVector.push_back(cellAfter);


            cellBefore = cellAfter;
        }while(!routeFlag);



    }
    deleteWaves();
    return outVector;
}

//bool snakesWidget::checkRoute (SnakesCords &cellBefore, SnakesCords &cellAfter, SnakeCords head, bool &routeFlag, int x, int y, int currentWave)
//{
//    if ((cellBefore.x + x) >= 0)
//        if ((cells[(cellBefore.y + y) * CANVAS_SIZE + (cellBefore.x + x)] >= WAVES_START) && (cells[(cellBefore.y + y) * CANVAS_SIZE + (cellBefore.x + x)] < currentWave))
//        {
//            SnakesCords cell ((cellBefore.x + x), (cellBefore.y + y));
//            cellAfter = cell;
//            if (cell == head)
//                routeFlag = true;
//        }
//    return routeFlag;
//}

void snakesWidget::deleteWaves() // delete waves after Li algorithm from cells array
{
    for(int k=0; k < CANVAS_SIZE; k++) {
        for(int j=0; j < CANVAS_SIZE; j++) {
            if(cells[k*CANVAS_SIZE + j] >= WAVES_START)
                cells[k*CANVAS_SIZE + j] = 0;
        }
    }
}
void snakesWidget::deleteSnake(int num) //delete snake NUM from cells array
{
    for(int k=0; k < CANVAS_SIZE; k++) {
        for(int j=0; j < CANVAS_SIZE; j++) {
            if(cells[k*CANVAS_SIZE + j] == num)
                cells[k*CANVAS_SIZE + j] = 0;
        }
    }
}
void snakesWidget::drawSnakes() // filling cells array with snakes that are on actual at this time
{
    for (int i = 0; i < NUM_OF_SNAKES; i++)
    {
        std::list<SnakesCords>::iterator cellPointer = snakes[i].begin(); //default snake is 1st
            while (cellPointer != snakes[i].end())
            {
                cells[cellPointer -> y * CANVAS_SIZE + cellPointer -> x] = SNAKES_START + i;
                cellPointer++;
            }
    }
}

void snakesWidget::paintGrid(QPainter &p)
{
    QRect borders(0, 0, width()-1, height()-1); // borders of canvas
    QColor gridColor = "#000"; // color of the grid (BLACK)
    gridColor.setAlpha(10);
    p.setPen(gridColor);
    double cellWidth = (double)width()/CANVAS_SIZE; // width of the widget / number of cells at one row
    for(double k = cellWidth; k <= width(); k += cellWidth)
        p.drawLine(k, 0, k, height());
    double cellHeight = (double)height()/CANVAS_SIZE; // height of the widget / number of cells at one row
    for(double k = cellHeight; k <= height(); k += cellHeight)
        p.drawLine(0, k, width(), k);
    p.drawRect(borders);
}


void snakesWidget::paintCells(QPainter &p)
{
    double cellWidth = (double)width()/CANVAS_SIZE;
    double cellHeight = (double)height()/CANVAS_SIZE;
    for(int k=0; k < CANVAS_SIZE; k++) {
        for(int j=0; j < CANVAS_SIZE; j++) {
            if(cells[k*CANVAS_SIZE + j] == 1) { // painting static opjects in canvas
                qreal left = (qreal)(cellWidth*j); // margin from left
                qreal top  = (qreal)(cellHeight*k); // margin from top
                QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                p.fillRect(r, QBrush("#000")); // fill cell with BLACK
            }
            else if ((cells[k*CANVAS_SIZE + j] == 2) ) { // painting first snake
                    qreal left = (qreal)(cellWidth*j); // margin from left
                    qreal top  = (qreal)(cellHeight*k); // margin from top
                    QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                    p.fillRect(r, QBrush("#7FFF00")); // fill cell with GREEN
                }
            else if(cells[k*CANVAS_SIZE + j] == 3) { // painting second snake
                    qreal left = (qreal)(cellWidth*j); // margin from left
                    qreal top  = (qreal)(cellHeight*k); // margin from top
                    QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                    p.fillRect(r, QBrush("#FF8C00")); // fill cell with ORANGE
                }
            else if(cells[k*CANVAS_SIZE + j] > 3) { // painting any other snake or object on canvas
                    qreal left = (qreal)(cellWidth*j); // margin from left
                    qreal top  = (qreal)(cellHeight*k); // margin from top
                    QRectF r(left, top, (qreal)cellWidth, (qreal)cellHeight);
                    p.fillRect(r, QBrush("#F14f00")); // fill cell with RED
                }
        }
    }
}
