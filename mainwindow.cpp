#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QString>
#include <QTextStream>
#include <QUrl>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_dialog = new QFileDialog(this);
    m_dialog->setNameFilter(tr("File (*.json)"));


    QObject::connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(loadData()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadData()
{
    QStringList fileNames;
      if (m_dialog->exec())
          fileNames = m_dialog->selectedFiles();

    foreach (const QString &str, fileNames) {
        QFile file(str);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            ui->textBrowser->append("------------------------------------------------------");
            ui->textBrowser->append("Could not open the file " + file.fileName() + "for reading: " + file.errorString());
            return;
        }

        QTextStream out(&file);
        QString content = out.readAll();
        file.close();

        if (content.isEmpty()) {
            ui->textBrowser->append("------------------------------------------------------");
            ui->textBrowser->append("No data was currently available for reading from file" + file.fileName());
            return;
        }


        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8());
        if (!doc.isObject()) {
            ui->textBrowser->append("------------------------------------------------------");
            ui->textBrowser->append("This is not a valid json document");
            return;
        }

        //get the jsonObject
        QJsonObject jObject = doc.object();

        int numberOfPositions = jObject.value("NumberOfPositions").toInt();
        int combinedX = 0;
        int combinedY = 0;

        for(int i=0; i<numberOfPositions;i++){
            QJsonObject position = jObject.value("Position" + QString::number(i)).toObject();
            combinedX += position.value("x").toInt();
            combinedY += position.value("y").toInt();
        }

        QString result = "------------------------------------------------------";
        result += "\nNumber of Elements: " + QString::number(numberOfPositions);
        result += "\nCombined value of X: " + QString::number(combinedX);
        result += "\nCombined value of Y: " + QString::number(combinedY);

        ui->textBrowser->append(result);
    }

}
