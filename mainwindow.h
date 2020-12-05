#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtOpenGL>
#include <QGLShaderProgram>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();

    void on_turnXPos_clicked();
    void on_turnXNeg_clicked();
    void on_turnYPos_clicked();
    void on_turnYNeg_clicked();
    void on_turnZPos_clicked();
    void on_turnZNeg_clicked();

    void on_shiftXPos_clicked();
    void on_shiftXNeg_clicked();
    void on_shiftYPos_clicked();
    void on_shiftYNeg_clicked();
    void on_shiftZPos_clicked();
    void on_shiftZNeg_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::MainWindow *ui;

    void paintEvent(QPaintEvent* event);
    void keyPressEvent(QKeyEvent* event);
};
#endif // MAINWINDOW_H
