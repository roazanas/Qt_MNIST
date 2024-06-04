#include "mainwindow.h"
#include "trainingthread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    neuralNetwork = new NeuralNetwork(784,     100,     10      );
                                   // входные  скрытые  выходные


    // настройка интерфейса
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout();
    centralWidget->setLayout(mainLayout);

    // 1-ая группа виджетов (верхняя)
    resultLabel = new QLabel("-");
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setFont(QFont("Segoe UI", 35, QFont::Bold));
    HGroup1 = new QHBoxLayout();
    for (int i = 0; i < outputSize; ++i)
    {
        labelsGroup1[i] = new QLabel(QString::number(i));
        labelsGroup1[i]->setAlignment(Qt::AlignCenter);
        labelsGroup1[i]->setFont(QFont("Segoe UI", 10));
        HGroup1->addWidget(labelsGroup1[i]);
    }
    VGroup1 = new QVBoxLayout();
    VGroup1->addWidget(resultLabel);
    VSpacer0 = new QSpacerItem(10, 10, QSizePolicy::Maximum, QSizePolicy::Maximum);
    VGroup1->addItem(VSpacer0);
    VGroup1->addLayout(HGroup1);
    groupBox1 = new QGroupBox();
    groupBox1->setLayout(VGroup1);

    mainLayout->addWidget(groupBox1);
    VSpacer1 = new QSpacerItem(10, 10, QSizePolicy::Maximum, QSizePolicy::Maximum);
    mainLayout->addItem(VSpacer1);


    // 2-ая группа виджетов (центральная)
    HGroup2 = new QHBoxLayout();
    for (int i = 0; i < outputSize; ++i)
    {
        progressBars[i] = new QProgressBar();
        progressBars[i]->setRange(0, 100);
        progressBars[i]->setOrientation(Qt::Vertical);

        labelsGroup2[i] = new QLabel(QString::number(i));
        labelsGroup2[i]->setAlignment(Qt::AlignCenter);
        labelsGroup2[i]->setFont(QFont("Segoe UI", 10));

        VGroup2 = new QVBoxLayout();
        VGroup2->addWidget(progressBars[i]);
        VGroup2->addWidget(labelsGroup2[i]);
        HGroup2->addLayout(VGroup2);
    }
    groupBox2 = new QGroupBox();
    groupBox2->setLayout(HGroup2);

    mainLayout->addWidget(groupBox2);
    VSpacer2 = new QSpacerItem(10, 10, QSizePolicy::Maximum, QSizePolicy::Maximum);
    mainLayout->addItem(VSpacer2);


    // 3-ья группа виджетов
    paintWidget = new PaintWidget();
    VGroup3 = new QVBoxLayout();
    clearButton = new QPushButton("Очистить");
    VSpacer3 = new QSpacerItem(10, 10, QSizePolicy::Maximum, QSizePolicy::Maximum);
    VGroup3->addItem(VSpacer3);
    VGroup3->addWidget(paintWidget);
    VGroup3->addWidget(clearButton);
    VGroup3->setAlignment(Qt::AlignCenter);
    groupBox3 = new QGroupBox();
    groupBox3->setLayout(VGroup3);

    mainLayout->addWidget(groupBox3);


    // меню
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    fileMenu = menuBar->addMenu("Файл");
    openWeightsAction = fileMenu->addAction("Открыть веса");
    saveWeightsAction = fileMenu->addAction("Сохранить веса");
    activationMenu = menuBar->addMenu("Функция активации");
    fileMenu->addSeparator();
    trainNetworkAction = fileMenu->addAction("Обучить нейросеть (*.csv)");

    // сигналы
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::resetResults);
    connect(paintWidget, &PaintWidget::imageUpdated, this, &MainWindow::processImage);

    connect(openWeightsAction, &QAction::triggered, this, &MainWindow::openWeights);
    connect(saveWeightsAction, &QAction::triggered, this, &MainWindow::saveWeights);
    connect(trainNetworkAction, &QAction::triggered, this, &MainWindow::trainNetwork);

    createActivationMenu();
    connect(activationGroup, &QActionGroup::triggered, this, &MainWindow::updateActivationFunction);
}

void MainWindow::processImage()
{
    QVector<double> pixelData = paintWidget->getPixelData();
    QVector<double> answer = neuralNetwork->process(pixelData);
    double maxV = 0;
    int maxI = 0;
    for (int i = 0; i < outputSize; ++i)
    {
        progressBars[i]->setValue(answer[i] * 100);
        if (answer[i] > maxV)
        {
            maxV = answer[i];
            maxI = i;
        }
    }
    labelsGroup1[currentMaxI]->setFont(QFont("Segoe UI", 10));
    currentMaxI = maxI;
    labelsGroup1[maxI]->setFont(QFont("Segoe UI", 15, QFont::Bold));
    resultLabel->setText(QString::number(maxI));
}

void MainWindow::resetResults()
{
    for (int i = 0; i < outputSize; ++i)
    {
        progressBars[i]->setValue(0);
    }
    labelsGroup1[currentMaxI]->setFont(QFont("Segoe UI", 10));
    resultLabel->setText("-");
    paintWidget->clear();
}

void MainWindow::trainNetwork()
{
    // Открываем диалог выбора файла для обучения
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Выбрать файл для обучения"),
                                                    "",
                                                    tr("Файлы данных с разделителями-запятыми (*.csv)"));

    // Если файл выбран
    if (!filePath.isEmpty())
    {
        // Создаем информационное окно с прогрессбаром
        QDialog *progressDialog = new QDialog(this);
        progressDialog->setWindowTitle("Обучение нейросети");
        QVBoxLayout *layout = new QVBoxLayout(progressDialog);

        QLabel *infoLabel = new QLabel("Загрузка...", progressDialog);
        infoLabel->setAlignment(Qt::AlignCenter);
        infoLabel->setFont(QFont("Segoe UI", 10));
        infoLabel->setWordWrap(true); // Включаем перенос слов
        layout->addWidget(infoLabel);

        QProgressBar *progressBar = new QProgressBar(progressDialog);
        progressBar->setRange(0, 100);
        layout->addWidget(progressBar);

        progressDialog->setMinimumSize(300, 150); // Минимальный размер окна
        progressDialog->setModal(false); // Не блокируем главное окно
        progressDialog->show();
        progressBar->setValue(0);
        QApplication::processEvents(); // Обрабатываем события, чтобы окно обновилось

        // Открываем файл для чтения
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);

            // Пропускаем первую строку (заголовок)
            in.readLine();

            // Подсчитываем количество строк для прогрессбара
            int totalLines = 0;
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (!line.isEmpty()) totalLines++;
            }
            file.seek(0); // Возвращаем указатель файла в начало
            in.readLine(); // Снова пропускаем заголовок

            // Создаем вектор для хранения потоков обучения
            QVector<TrainingThread*> threads;

            // Счетчик обработанных строк для прогрессбара
            int processedLines = 0;

            // Читаем файл построчно
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.isEmpty()) continue; // Пропускаем пустые строки

                QStringList values = line.split(",");
                int correctNum = values.at(0).toInt();

                // Считываем и нормализуем значения пикселей
                QVector<double> readedData;
                for (int i = 1; i < values.size(); ++i) {
                    readedData.append(values.at(i).toDouble() / 255.0);
                }

                // Создаем поток обучения и передаем ему данные
                TrainingThread* thread = new TrainingThread(neuralNetwork, correctNum, readedData, currentActivationFunction);
                threads.append(thread);
                thread->start(); // Запускаем поток

                // Обновляем прогрессбар
                processedLines++;
                int progress = (int)((double)processedLines / totalLines * 100);
                infoLabel->setText(QString("%1 из %2 строк уже обработано").arg(processedLines).arg(totalLines));
                progressBar->setValue(progress);
                QApplication::processEvents(); // Обрабатываем события, чтобы прогрессбар обновился
            }

            // Ожидаем завершения всех потоков
            for (auto* thread : threads) {
                thread->wait();
                delete thread; // Освобождаем память после завершения потока
            }

            file.close();
        }

        progressDialog->close(); // Закрываем информационное окно
    }
}

void MainWindow::saveWeights()
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить веса"),
                                                    "",
                                                    tr("Файлы весов (*.nnw)"));

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            QVector<int> sizes = neuralNetwork->getSizes();
            int sizeInput = sizes[0];
            int sizeHidden = sizes[1];
            int sizeOutput = sizes[2];

            // Сохранение размеров сети
            out << sizeInput << "\n";
            out << sizeHidden << "\n";
            out << sizeOutput << "\n";

            // Сохранение весов weightsInToHid
            for (int i = 0; i < sizeInput; ++i) {
                for (int j = 0; j < sizeHidden; ++j) {
                    out << neuralNetwork->getWeightsInToHid()[i][j] << " ";
                }
                out << "\n";
            }

            // Сохранение весов weightsHidToOut
            for (int i = 0; i < sizeHidden; ++i) {
                for (int j = 0; j < sizeOutput; ++j) {
                    out << neuralNetwork->getWeightsHidToOut()[i][j] << " ";
                }
                out << "\n";
            }

            // Сохранение смещений biasesHidden
            for (int i = 0; i < sizeHidden; ++i) {
                out << neuralNetwork->getBiasesHidden()[i] << " ";
            }
            out << "\n";

            // Сохранение смещений biasesOutput
            for (int i = 0; i < sizeOutput; ++i) {
                out << neuralNetwork->getBiasesOutput()[i] << " ";
            }

            file.close();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
        }
    }
}

void MainWindow::openWeights()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть веса"),
                                                    "",
                                                    tr("Файлы весов (*.nnw)"));

    if (!filePath.isEmpty()) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);

            QVector<int> sizes = neuralNetwork->getSizes();
            int sizeInput = sizes[0];
            int sizeHidden = sizes[1];
            int sizeOutput = sizes[2];

            // Чтение размеров сети
            int inputNum, hiddenNum, outputNum;
            in >> inputNum >> hiddenNum >> outputNum;

            // Проверка размеров сети
            if (inputNum != sizeInput ||
                hiddenNum != sizeHidden ||
                outputNum != sizeOutput)
            {
                QMessageBox::warning(this, "Ошибка", "Размеры загруженной сети не совпадают с текущей.");
                return;
            }

            // Чтение весов weightsInToHid
            for (int i = 0; i < sizeInput; ++i) {
                for (int j = 0; j < sizeHidden; ++j) {
                    in >> neuralNetwork->getWeightsInToHid()[i][j];
                }
            }

            // Чтение весов weightsHidToOut
            for (int i = 0; i < sizeHidden; ++i) {
                for (int j = 0; j < sizeOutput; ++j) {
                    in >> neuralNetwork->getWeightsHidToOut()[i][j];
                }
            }

            // Чтение смещений biasesHidden
            for (int i = 0; i < sizeHidden; ++i) {
                in >> neuralNetwork->getBiasesHidden()[i];
            }

            // Чтение смещений biasesOutput
            for (int i = 0; i < sizeOutput; ++i) {
                in >> neuralNetwork->getBiasesOutput()[i];
            }

            file.close();
            resetResults();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для чтения.");
        }
    }
}

void MainWindow::createActivationMenu() {
    activationGroup = new QActionGroup(this);
    activationGroup->setExclusive(true);

    QAction *sigmoidAction = activationMenu->addAction("Сигмоида");
    sigmoidAction->setCheckable(true);
    sigmoidAction->setChecked(true); // По умолчанию выбрана сигмоида
    activationGroup->addAction(sigmoidAction);

    QAction *reluAction = activationMenu->addAction("ReLU");
    reluAction->setCheckable(true);
    activationGroup->addAction(reluAction);
}

void MainWindow::updateActivationFunction(QAction *action) {
    if (action->text() == "Сигмоида") {
        NeuralNetwork::currentActivationFunction = NeuralNetwork::ActivationFunction::Sigmoid; // Обратитесь к статической переменной через имя класса
    } else if (action->text() == "ReLU") {
        NeuralNetwork::currentActivationFunction = NeuralNetwork::ActivationFunction::ReLU; // Обратитесь к статической переменной через имя класса
    }
}

MainWindow::~MainWindow()
{
}
