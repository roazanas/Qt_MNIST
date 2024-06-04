#ifndef TRAININGTHREAD_H
#define TRAININGTHREAD_H

#include <QThread>
#include <QVector>
#include "neuralnetwork.h" // Подключите заголовок NeuralNetwork, если это необходимо

class TrainingThread : public QThread
{
    Q_OBJECT
public:
    TrainingThread(NeuralNetwork* network,
                   int correctNum,
                   const QVector<double>& correctImage,
                   NeuralNetwork::ActivationFunction activation);

    void run() override;

private:
    NeuralNetwork* network;
    int correctNum;
    QVector<double> correctImage;
    NeuralNetwork::ActivationFunction activation;
};

#endif // TRAININGTHREAD_H
