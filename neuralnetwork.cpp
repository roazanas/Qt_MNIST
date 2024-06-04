#include "neuralnetwork.h"

NeuralNetwork::ActivationFunction NeuralNetwork::currentActivationFunction = NeuralNetwork::ActivationFunction::Sigmoid;

NeuralNetwork::NeuralNetwork(int inputNum, int hiddenNum, int outputNum)
{
    sizeInput = inputNum;
    sizeHidden = hiddenNum;
    sizeOutput = outputNum;

    neuronsInput.resize(sizeInput);
    neuronsHidden.resize(sizeHidden);
    neuronsOutput.resize(sizeOutput);

    // инициализация весов между входным и скрытым слоями
    weightsInToHid.resize(sizeInput);
    for (int i = 0; i < sizeInput; ++i)
    {
        weightsInToHid[i].resize(sizeHidden);
        for (int j = 0; j < sizeHidden; ++j)
        {
            weightsInToHid[i][j] = 0.005 * QRandomGenerator::global()->generateDouble();
            weightsInToHid[i][j] *= QRandomGenerator::global()->generateDouble() >= 0.5 ?
                                        1 : -1;
        }
    }

    // инициализация весов между скрытым и выходным слоями
    weightsHidToOut.resize(sizeHidden);
    for (int i = 0; i < sizeHidden; ++i)
    {
        weightsHidToOut[i].resize(sizeOutput);
        for (int j = 0; j < sizeOutput; ++j)
        {
            weightsHidToOut[i][j] = 0.005 * QRandomGenerator::global()->generateDouble();
            weightsHidToOut[i][j] *= QRandomGenerator::global()->generateDouble() >= 0.5 ?
                                         1 : -1;
        }
    }

    // инициализация смещений скрытого слоя
    biasesHidden.resize(sizeHidden);
    for (int i = 0; i < sizeHidden; ++i)
    {
        biasesHidden[i] = 0.005 * QRandomGenerator::global()->generateDouble();
        biasesHidden[i] *= QRandomGenerator::global()->generateDouble() >= 0.5 ?
                               1 : -1;
    }

    // инициализация смещений выходного слоя
    biasesOutput.resize(sizeOutput);
    for (int i = 0; i < sizeOutput; ++i)
    {
        biasesOutput[i] = 0.005 * QRandomGenerator::global()->generateDouble();
        biasesOutput[i] *= QRandomGenerator::global()->generateDouble() >= 0.5 ?
                               1 : -1;
    }
}

QVector<double> NeuralNetwork::process(QVector<double> input)
{
    feedForward(input);
    return neuronsOutput;
}

void NeuralNetwork::train(int correctNum, QVector<double> correctImage, ActivationFunction activation)
{
    feedForward(correctImage);
    QVector<double> correctOutput(sizeOutput, 0.0);
    correctOutput[correctNum] = 1.0;
    backPropagation(correctOutput, activation);
}

QVector<int> NeuralNetwork::getSizes()
{
    return QVector<int>({sizeInput, sizeHidden, sizeOutput});
}

QVector<QVector<double> > &NeuralNetwork::getWeightsInToHid()
{
    return weightsInToHid;
}

QVector<QVector<double> > &NeuralNetwork::getWeightsHidToOut()
{
    return weightsHidToOut;
}

QVector<double> &NeuralNetwork::getBiasesHidden()
{
    return biasesHidden;
}

QVector<double> &NeuralNetwork::getBiasesOutput()
{
    return biasesOutput;
}

double NeuralNetwork::sigmoid(double x, bool isDerivative)
{
    if (!isDerivative) return 1 / (1 + qExp(-x));

    double sig = activationFunction(x, false);
    return sig * (1 - sig);
}

double NeuralNetwork::ReLU(double x, bool isDerivative)
{
    if (!isDerivative) return qMax(0.0, x);

    return (x > 0)? 1:0;
}

double NeuralNetwork::activationFunction(double x, bool isDerivative)
{
    if (currentActivationFunction == ActivationFunction::Sigmoid) {
        return sigmoid(x, isDerivative);
    } else {
        return ReLU(x, isDerivative);
    }
}

void NeuralNetwork::feedForward(QVector<double> input)
{
    for (int i = 0; i < sizeInput; ++i)
    {
        neuronsInput[i] = input[i];
    }

    // расчёт значений скрытых нейронов
    for (int i = 0; i < sizeHidden; ++i)
    {
        double activation = biasesHidden[i];
        for (int j = 0; j < sizeInput; ++j)
        {
            activation += input[j] * weightsInToHid[j][i];
        }
        neuronsHidden[i] = activationFunction(activation);
    }

    // расчёт значений выходных нейронов
    for (int i = 0; i < sizeOutput; ++i)
    {
        double activation = biasesOutput[i];
        for (int j = 0; j < sizeHidden; ++j)
        {
            activation += neuronsHidden[j] * weightsHidToOut[j][i];
        }
        neuronsOutput[i] = activationFunction(activation);
    }
}

void NeuralNetwork::backPropagation(QVector<double> targetOutput, ActivationFunction activation)
{
    double learningRate = 0.1;

    // 1. вычисление ошибки выходного слоя
    QVector<double> outputErrors(sizeOutput);
    for (int i = 0; i < sizeOutput; ++i)
    {
        outputErrors[i] = (targetOutput[i] - neuronsOutput[i]) * activationFunction(neuronsOutput[i], true);
    }

    // 2. вычисление ошибки скрытого слоя
    QVector<double> hiddenErrors(sizeHidden);
    for (int i = 0; i < sizeHidden; ++i)
    {
        double error = 0.0;
        for (int j = 0; j < sizeOutput; ++j)
        {
            error += outputErrors[j] * weightsHidToOut[i][j];
        }
        hiddenErrors[i] = error * activationFunction(neuronsHidden[i], true);
    }

    // 3. обновление весов и смещений между скрытым и выходным слоями
    for (int i = 0; i < sizeHidden; ++i)
    {
        for (int j = 0; j < sizeOutput; ++j)
        {
            weightsHidToOut[i][j] += learningRate * outputErrors[j] * neuronsHidden[i];
        }
    }
    for (int i = 0; i < sizeOutput; ++i)
    {
        biasesOutput[i] += learningRate * outputErrors[i];
    }

    // 4. обновление весов и смещений между входным и скрытым слоями
    for (int i = 0; i < sizeInput; ++i)
    {
        for (int j = 0; j < sizeHidden; ++j)
        {
            weightsInToHid[i][j] += learningRate * hiddenErrors[j] * neuronsInput[i];
        }
    }
    for (int i = 0; i < sizeHidden; ++i)
    {
        biasesHidden[i] += learningRate * hiddenErrors[i];
    }
}
