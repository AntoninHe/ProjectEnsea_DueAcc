#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include<QDebug>
#include <QSerialPortInfo>

#include <fstream>
#include <string>
#include <iostream>
using namespace  std;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->pushButtonBreak->hide();
    ui->pushButtonStart->hide();
    ui->pushButtonClear->hide();
    ui->pushButtonClose->hide();
    ui->progressBarFilter->setValue(0);

    initmenu();

    initialization_file();

    initialization_timer();

    initialization_serial();

    initialization_custumplot();

    initialization_tab();

    fillPortsInfo();

    //stockage
    stockage = new std::vector<std::vector<int>>();

}

void MainWindow::initmenu()
{
    creditsAct = new QAction(tr("Crédits"), this);
    aboutQtAct = new QAction(tr("About"), this);
    helpQtAct = new QAction(tr("Help"), this);


    QMenu *helpMenu = new QMenu(tr("Help"), this);
    helpMenu->addAction(helpQtAct);
    helpMenu->addAction(creditsAct);
    helpMenu->addAction(aboutQtAct);
    ui->menuBar->addMenu(helpMenu);

    connect(creditsAct, SIGNAL(triggered()), this, SLOT(about()));
    connect(aboutQtAct, SIGNAL(triggered()), this, SLOT(aboutQt()));
    connect(helpQtAct, SIGNAL(triggered()), this, SLOT(helpQt()));
}


void MainWindow::about()
{
    QMessageBox::information(this, "Crédits", "Antonin HÉRÉSON & Marly BALDE \nEnsea 2017-2018");
}

void MainWindow::aboutQt()
{
    QMessageBox::information(this, "About", "Qt version 5.9\nApp V1");
}

void MainWindow::helpQt()
{
    QMessageBox::information(this, "Help", "<a href=\"https://antoninhe.github.io/ProjectEnsea_DueAcc/\">Documentation (GithubPages)</a>");
}




MainWindow::~MainWindow()
{
    delete ui;//destruction de la fênetre
    serialDue->close();//fermeture de la liaison serie
    delete serialDue;//supression de du port
    delete readDelayTimer;//supression du timer
    delete tableFilter;//supression des tableaux
    delete instantvalueXYZ;
    delete stockage;
}

void MainWindow::initialization_tab()
{
    if(instantvalueXYZ!=NULL)
        delete instantvalueXYZ;

    instantvalueXYZ= new vector<vector<int>>(tableFilter->size(),vector<int>(3,0));
}

void MainWindow::update_tab()
{
    ui->listWidgetFilter->clear();
    for(int unsigned i=0;i<tableFilter->size();i++){
        ui->listWidgetFilter->addItem(QString().setNum(tableFilter->at(i)));}
}


void MainWindow::initialization_timer()
{
    readDelayTimer = new QTimer(this);
    readDelayTimer->setSingleShot(true);
    connect(readDelayTimer,SIGNAL(timeout()),this,SLOT(on_readingprocess()));
}

void MainWindow::initialization_serial()
{
    serialDue= new QSerialPort(this);
    serialDue->setBaudRate(QSerialPort::Baud115200);
    serialDue->setDataBits(QSerialPort::Data8);
    serialDue->setParity(QSerialPort::NoParity);
    serialDue->setStopBits(QSerialPort::OneStop);
    serialDue->setFlowControl(QSerialPort::NoFlowControl);
    connect(serialDue,SIGNAL(readyRead()),this,SLOT(on_serialReceived()));
}

void MainWindow::initialization_custumplot()
{
    ui->Widgetcplot->addGraph(); // blue line
    ui->Widgetcplot->graph(0)->setPen(QPen(Qt::blue));
    ui->Widgetcplot->addGraph(); // red line
    ui->Widgetcplot->graph(1)->setPen(QPen(Qt::red));
    ui->Widgetcplot->addGraph();// green line
    ui->Widgetcplot->graph(2)->setPen(QPen(Qt::green));
    // give the axes labels:
    ui->Widgetcplot->xAxis->setLabel("Time");
    ui->Widgetcplot->yAxis->setLabel("Acceleration");
    ui->Widgetcplot->yAxis->setRange(-130,130);
}

void MainWindow::fillPortsInfo()
{
    ui->serialPortInfoListBox->clear();//vide la liste affiché des ports
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {//recupère toutes les infos des ports
        QString portname;
        portname=info.portName();//on recupère uniquement les noms
        ui->serialPortInfoListBox->addItem(portname, portname);
    }
}

void MainWindow::initialization_file()
{
    if(tableFilter!=NULL)
    {
        //ajout semaphore pour proteger de l'utilisation de tableFilter alors qu'il est suprimé
        delete tableFilter;
        namefile = QFileDialog::getOpenFileName() ;//ouvre la fênetre pour choisir le filtre
    }
    if(namefile.isEmpty())//On recupère les données brutes
    {
        tableFilter   = new vector<int>(1,0);
        tableFilter->at(0)=1;
        ui->progressBarFilter->hide();
        update_tab();
    }
    else
    {
        tableFilter =filterLoad();//on crée le filtre
        update_tab();
        ui->progressBarFilter->show();//on montre la barre de chargement
    }
    i_shift=tableFilter->size()-1;//on met à jour
}

void MainWindow::on_pushButtonFilter_clicked()
{
    on_pushButtonClose_clicked();
    initialization_file();
    initialization_tab();
}


vector<int> *MainWindow::filterLoad()
{
    vector<int> *filtre = new vector<int>();
    string mon_fichier = namefile.toStdString();
    ifstream fichier(mon_fichier.c_str(), ios::in);
    if(fichier)  // si l'ouverture a réussi
    {
        int entier;
        while (!fichier.eof()){
            fichier >> entier;
            filtre->push_back(entier);
        }
        fichier.close();  // je referme le fichier
    }
    else  // sinon
        qDebug() << "Erreur à l'ouverture !" << endl;
    return filtre;
}

void MainWindow::on_serialReceived()
{
    readDelayTimer->start(5);//delai en ms
}

void MainWindow::on_readingprocess()
{
    QByteArray value(serialDue->readAll());//lecture de la trame
    bool ok;
    //extraction de la cordonnée en Hexa
    int x = intsigned_to_int( (value.left(2)).toInt(&ok,16)  );
    int y = intsigned_to_int( (value.mid(2,2)).toInt(&ok,16) );
    int z = intsigned_to_int( (value.right(2)).toInt(&ok,16) );
    ui->lcdNumberX->display(x);
    ui->lcdNumberY->display(y);
    ui->lcdNumberZ->display(z);

    //Filtrage
    vector<int> instantFilteredXYZ(3,0);
    instantvalueXYZ->at(i_shift)={x,y,z};

    int unsigned i,j;
    for( i=0, j=i_shift;i<tableFilter->size();i++,j++)  //on applique le filtre
    {
        if(j>(tableFilter->size()-1))
            j=0;

        instantFilteredXYZ.at(0)+=instantvalueXYZ->at(j).at(0)*tableFilter->at(i);
        instantFilteredXYZ.at(1)+=instantvalueXYZ->at(j).at(1)*tableFilter->at(i);
        instantFilteredXYZ.at(2)+=instantvalueXYZ->at(j).at(2)*tableFilter->at(i);
    }
    i_shift--;
    if(i_shift<0)
        i_shift=tableFilter->size()-1;

    //Filtrage END

    if(exportvalue==2)
    {
        stockage->push_back(instantFilteredXYZ);
    }

    ui->Widgetcplot->graph(0)->addData(icmpt,instantFilteredXYZ.at(0)); //on trace les points dans le widget
    ui->Widgetcplot->graph(1)->addData(icmpt,instantFilteredXYZ.at(1));
    ui->Widgetcplot->graph(2)->addData(icmpt,instantFilteredXYZ.at(2));

    ui->Widgetcplot->xAxis->setRange(icmpt,200, Qt::AlignRight); //permet le décalage en continue de l'axe de abscisse

    if((unsigned int)icmpt<=tableFilter->size())
    {
        ui->progressBarFilter->setValue((icmpt*100)/tableFilter->size());
    }

    ui->Widgetcplot->replot(); //on rafraichi l'affichage du widget avec les nouveau points

    icmpt++;


}

int MainWindow::intsigned_to_int(int value)
{
    if(value>=128)
        value=value-255;
    return(value);
}


void MainWindow::on_pushButtonOpen_clicked()
{
    serialDue->setPortName(ui->serialPortInfoListBox->currentText());//recupère le port choisi
    if (serialDue->open(QIODevice::ReadWrite)==true)
    {
        serialDue->flush();
        ui->pushButtonBreak->show();
        ui->pushButtonStart->show();
        ui->pushButtonClear->show();
        ui->pushButtonOpen->hide();
        ui->pushButtonClose->show();
        ui->pushButtonRefresh->setDisabled(true);
        ui->serialPortInfoListBox->setDisabled(true);
    }

}

void MainWindow::on_pushButtonClose_clicked()
{
    serialDue->close();

    on_pushButtonBreak_clicked();
    ui->pushButtonBreak->hide();
    ui->pushButtonStart->hide();
    ui->pushButtonClear->hide();
    icmpt=0;
    ui->progressBarFilter->setValue(0);
    ui->pushButtonOpen->show();
    ui->pushButtonClose->hide();
    ui->pushButtonRefresh->setDisabled(false);
    ui->serialPortInfoListBox->setDisabled(false);


}

void MainWindow::on_pushButtonStart_clicked()
{
    serialDue->flush();
    serialDue->write("s");
}

void MainWindow::on_pushButtonBreak_clicked()
{
    serialDue->write("b");
}

void MainWindow::on_pushButtonClear_clicked()
{
    icmpt=0;
    serialDue->write("b");
    ui->Widgetcplot->xAxis->setRange(icmpt,200, Qt::AlignRight);
    ui->Widgetcplot->replot();// on efface la courbe et on la recentre en 0
    serialDue->flush();

}

void MainWindow::exportCSV()
{
    QString namefileReturn = QFileDialog::getSaveFileName(this, tr("Sauvegarde image"),
                                                          "Data.csv",
                                                          tr("(*.csv)"));

    ofstream fichier(namefileReturn.toStdString().c_str(), ios::out | ios::trunc);               //déclaration du flux et ouverture du fichier
    if(fichier)  // si l'ouverture a réussi
    {
        for(unsigned int i = 0;i<stockage->size();i++){
            for(int j = 0;j<3;j++){//on stocke les X Y Z
                fichier << stockage->at(i).at(j);
                if(j<2)
                {
                    fichier << "," ;//la virgule qui sépare x,y,z
                }
                else
                {
                    fichier << endl ;//un retour à la ligne
                }
            }
        }
        fichier.close();  // on referme le fichier
    }
    else
        cerr << "Erreur à l'ouverture !" << endl;
    //return 0;
}

void MainWindow::on_pushButtonExport_clicked()
{
    on_pushButtonClose_clicked();
    exportvalue=0;
    ui->checkBoxExpOnoff->setCheckState(Qt::Unchecked);
    exportCSV();
}



void MainWindow::on_pushButtonNewStockage_clicked()
{
    exportvalue=0;
    stockage->clear();
    ui->checkBoxExpOnoff->setCheckState(Qt::Unchecked);
}

void MainWindow::on_checkBoxExpOnoff_stateChanged(int arg1)
{
    exportvalue=arg1;// if checked 2 even 0
}


void MainWindow::on_verticalSlider_sliderMoved(int position)
{
    position=position*10;
    ui->Widgetcplot->yAxis->setRange(-130-position,130+position);
    ui->Widgetcplot->replot();

}

void MainWindow::on_pushButtonRefresh_clicked()
{
    fillPortsInfo();
}
