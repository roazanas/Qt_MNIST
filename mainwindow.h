#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QPushButton>
#include <QSpacerItem>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <QColor>
#include <QDebug>
#include <QImage>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QBuffer>

#include "neuralnetwork.h"
#include "paintwidget.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    NeuralNetwork *neuralNetwork;

    // массивы элементов
    static const int outputSize = 10;
    QLabel *labelsGroup1[outputSize];
    QLabel *labelsGroup2[outputSize];
    QProgressBar *progressBars[outputSize];


    // интерфейс
    QWidget *centralWidget;

    QLabel *resultLabel;
    QGroupBox *groupBox1;
    QGroupBox *groupBox2;
    QGroupBox *groupBox3;
    PaintWidget *paintWidget;
    QPushButton *clearButton;
    QPushButton *calculateErrorButton;
    QSpacerItem *VSpacer0;
    QSpacerItem *VSpacer1;
    QSpacerItem *VSpacer2;
    QSpacerItem *VSpacer3;

    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* openWeightsAction;
    QAction* saveWeightsAction;
    QMenu* activationMenu; // Добавьте это
    QActionGroup* activationGroup; // И это
    NeuralNetwork::ActivationFunction currentActivationFunction = NeuralNetwork::ActivationFunction::Sigmoid;
    QAction* trainNetworkAction;


    // слои элементов
    QVBoxLayout *mainLayout;
    QHBoxLayout *HGroup1;
    QVBoxLayout *VGroup1;
    QHBoxLayout *HGroup2;
    QVBoxLayout *VGroup2;
    QVBoxLayout *VGroup3;

    // вспомогательные элементы
    int currentMaxI = 0;
    void resetResults();

private slots:
    void processImage();
    void openWeights();
    void saveWeights();
    void trainNetwork();
    void createActivationMenu();
    void updateActivationFunction(QAction *action);
    void calculateErrorPercentage();
};
#endif // MAINWINDOW_H
