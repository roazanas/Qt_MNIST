#include "trainingthread.h"

TrainingThread::TrainingThread(NeuralNetwork* network,
                               int correctNum,
                               const QVector<double>& correctImage,
                               NeuralNetwork::ActivationFunction activation)
    : network(network)
    , correctNum(correctNum)
    , correctImage(correctImage)
    , activation(activation)
{}

void TrainingThread::run()
{
    network->train(correctNum, correctImage);
}
