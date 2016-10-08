#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
#include <QColor>
#include <QColorDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "snakeswidget.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cells(new snakesWidget(this))
{
    ui->setupUi(this);
    ui->cellsLayout->addWidget(cells);
    connect (ui->pushButton,SIGNAL(clicked(bool)), cells, SLOT(generateCells()));
    cells->generateCells();
}

MainWindow::~MainWindow()
{
    delete ui;
}
