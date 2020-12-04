#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent* event) {

    double sizeButton = 1.0/13.0 * height();
    ui->pushButton->setFixedSize(sizeButton, sizeButton);
    ui->pushButton->setIconSize(QSize(sizeButton - 4, sizeButton - 4));

    ui->pushButton_2->setFixedSize(sizeButton, sizeButton);
    ui->pushButton_2->setIconSize(QSize(sizeButton - 4, sizeButton - 4));

    ui->pushButton_3->setFixedSize(sizeButton, sizeButton);
    ui->pushButton_3->setIconSize(QSize(sizeButton - 4, sizeButton - 4));
}

void MainWindow::on_pushButton_clicked() {
    ui->openGLWidget->addBrick(":/legoBricks/models/4x2B.obj");
}

void MainWindow::on_pushButton_2_clicked() {
    ui->openGLWidget->addBrick(":/legoBricks/models/4x2L.obj");
}

void MainWindow::on_pushButton_3_clicked() {
    ui->openGLWidget->addMyModel();
}

void MainWindow::on_turnXPos_clicked() {
    ui->openGLWidget->setTurnXPos();
}

void MainWindow::on_turnXNeg_clicked() {
    ui->openGLWidget->setTurnXNeg();
}

void MainWindow::on_turnYPos_clicked() {
    ui->openGLWidget->setTurnYPos();
}

void MainWindow::on_turnYNeg_clicked() {
    ui->openGLWidget->setTurnYNeg();
}

void MainWindow::on_turnZPos_clicked() {
    ui->openGLWidget->setTurnZPos();
}

void MainWindow::on_turnZNeg_clicked() {
    ui->openGLWidget->setTurnZNeg();
}

void MainWindow::on_shiftXPos_clicked() {
   ui->openGLWidget->setShiftXPos();
}


void MainWindow::on_shiftXNeg_clicked() {
    ui->openGLWidget->setShiftXNeg();
}

void MainWindow::on_shiftYPos_clicked() {
    ui->openGLWidget->setShiftYPos();
}

void MainWindow::on_shiftYNeg_clicked() {
    ui->openGLWidget->setShiftYNeg();
}

void MainWindow::on_shiftZPos_clicked() {
    ui->openGLWidget->setShiftZPos();
}

void MainWindow::on_shiftZNeg_clicked() {
    ui->openGLWidget->setShiftZNeg();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {

    // Закрыть окно при нажатии клавиши Escape
    if (event->key() == Qt::Key_Escape)
        close();
    else if (event->key() == Qt::Key_A)
        ui->openGLWidget->setTurnXPos();
    else if (event->key() == Qt::Key_D)
        ui->openGLWidget->setTurnXNeg();
    else if (event->key() == Qt::Key_Q)
        ui->openGLWidget->setTurnYPos();
    else if (event->key() == Qt::Key_E)
        ui->openGLWidget->setTurnYNeg();
    else if (event->key() == Qt::Key_S)
        ui->openGLWidget->setTurnZPos();
    else if (event->key() == Qt::Key_W)
        ui->openGLWidget->setTurnZNeg();

}

