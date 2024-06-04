#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <QObject>
#include <QVector>
#include <QRandomGenerator>
#include <QDebug>
#include <QtMath>
#include <QtMinMax>

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>


class NeuralNetwork : public QObject
{
    Q_OBJECT
public:
    NeuralNetwork(int inputNum, int hiddenNum, int outputNum);
    QVector<double> process(QVector<double> input);                 // сделать предсказание
    void train(int correctNum, QVector<double> correctImage);       // общая функция обучения

    QVector<int> getSizes();
    QVector<QVector<double>> &getWeightsInToHid();
    QVector<QVector<double>> &getWeightsHidToOut();
    QVector<double> &getBiasesHidden();
    QVector<double> &getBiasesOutput();

private:
    int sizeInput, sizeHidden, sizeOutput;                          // количества нейронов

    QVector<double> neuronsInput;                                   // входные нейроны
    QVector<double> neuronsHidden;                                  // скрытые нейроны
    QVector<double> neuronsOutput;                                  // выходные нейроны

    QVector<QVector<double>> weightsInToHid;                        // веса вход -> скр
    QVector<QVector<double>> weightsHidToOut;                       // веса скр  -> вых

    QVector<double> biasesHidden;                                   // смещения скрытых
    QVector<double> biasesOutput;                                   // смещения выходных


    double sigmoid(double x, bool isDerivative);                    // функция активации
    double ReLU(double x, bool isDerivative);                       // функция активации (более актуальна)
    double activationFunction(double x, bool isDerivative);

    void feedForward(QVector<double> input);                        // функция прямого распространения
    void backPropagation(QVector<double> targetOutput);             // функция обратного распространения
};

#endif // NEURALNETWORK_H
