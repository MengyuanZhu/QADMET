#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filter.h"

#include <QtWidgets/QFileDialog>
#include <QProcess>
#include <QTextStream>
#include <QDebug>
#include <QSvgWidget>
#include <QtCore/qmath.h>
#include <QDesktopWidget>

#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
#include <openbabel/parsmart.h>
#include <openbabel/descriptor.h>
#include <openbabel/ring.h>


#include <iostream>


using namespace std;
using namespace OpenBabel;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    move((QApplication::desktop()->width()-this->width())/2,(QApplication::desktop()->height()-this->height())/2);

    ui->tb_molecule->setRowCount(1);
    ui->tb_molecule->horizontalHeader()->setStretchLastSection(true);
    ui->tb_molecule->setColumnWidth(1,100);
    ui->table_admet->horizontalHeader()->setStretchLastSection(true);
    ui->table_admet->horizontalHeader()->resizeSection(0,70);
    ui->table_admet->horizontalHeader()->resizeSection(1,150);
    ui->table_general->horizontalHeader()->setStretchLastSection(true);
    ui->table_general->horizontalHeader()->resizeSection(0,70);
    ui->table_general->horizontalHeader()->resizeSection(1,150);
    ui->tb_cns->horizontalHeader()->setStretchLastSection(true);
    ui->tb_group->horizontalHeader()->setStretchLastSection(true);
    ui->tb_oral->horizontalHeader()->setStretchLastSection(true);
    ui->table_admet->hideColumn(1);
    ui->menuBar->show();
    ui->radio_drug->clicked();
    this->row_i=0;


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bt_import_clicked()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open File"),QDir::homePath(),tr("SDF Files *.sdf (*.sdf);;ChemDraw Binary Files *.cdx (*.cdx);;Tripos MOL2 Files *.mol2 (*.mol2);;SMILES Files *.smiles (*.smiles);;MOL Files *.mol (*.mol)"));
    QString ext;
    //QString arg;
    QString filename;
    QString moleculepath;
    QString smilesfile;
    for (int i = 0; i < fileNames.size(); ++i)
    {
        filename=fileNames.at(i);
        ext=filename.right(filename.length()-filename.lastIndexOf(".")-1);
        moleculepath=filename.left(filename.lastIndexOf("/")+1);
        smilesfile=moleculepath+"temp.smi";

        ifstream ifs(filename.toUtf8().constData());
        istream* pIn= &ifs;
        ofstream ofs(smilesfile.toUtf8().constData());
        ostream* pOut= &ofs;
        OBConversion conv;
        OBFormat* inFormat=conv.FormatFromExt(filename.toUtf8().constData());
        OBFormat* outFormat=conv.FormatFromExt(smilesfile.toUtf8().constData());

        conv.SetInAndOutFormats(inFormat,outFormat);
        conv.Convert(pIn, pOut);

        QFile file(smilesfile);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return;
        QTextStream in(&file);

        while (!in.atEnd())
        {

            QString line = in.readLine();
            QStringList lstLine = line.split("	");
            ui->tb_molecule->insertRow(ui->tb_molecule->rowCount());
            ui->tb_molecule->setItem(row_i,0,new QTableWidgetItem(lstLine.at(1)));
            ui->tb_molecule->setItem(row_i,2,new QTableWidgetItem(lstLine.at(0)));

            OBMol mol;
            stringstream ss(ui->tb_molecule->item(row_i, 2)->text().toUtf8().constData());
            OBConversion conv(&ss);
            conv.SetInFormat("smi");
            conv.Read(&mol);
            conv.SetOutFormat("svg");
            conv.WriteFile(&mol, "structure.svg");
            QLabel *molecule_structure = new QLabel();
            molecule_structure->setPixmap(QPixmap("structure.svg"));
            ui->tb_molecule->setCellWidget(row_i,1,molecule_structure);
            ui->tb_molecule->setRowHeight(row_i,100);


            row_i++;
        }
        file.close();
    }

}

void MainWindow::on_bt_add2list_clicked()
{
    ui->tb_molecule->insertRow(ui->tb_molecule->rowCount());
    ui->tb_molecule->setItem(row_i,0,new QTableWidgetItem(QString("Molecule %1").arg(row_i+1)));
    ui->tb_molecule->setItem(row_i,2,new QTableWidgetItem(ui->le_SMILE->text()));
    row_i++;

}

void MainWindow::on_le_SMILE_returnPressed()
{
    on_bt_add2list_clicked();
}

void MainWindow::on_actionQuit_triggered()
{
     QApplication::quit();
}

void MainWindow::on_actionAbout_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::on_actionImport_triggered()
{
    on_bt_import_clicked();
}

void MainWindow::on_actionExport_triggered()
{

}


void MainWindow::on_tb_molecule_currentCellChanged(int currentRow, int , int , int )
{
    if ( currentRow > row_i - 1 )
        return;

    ////////////////////////////OpenBabel///////////////////////////////////////////
    OBMol mol;
    stringstream ss(ui->tb_molecule->item(currentRow, 2)->text().toUtf8().constData());
    OBConversion conv(&ss);
    conv.SetInFormat("smi");
    conv.Read(&mol);
    ///////////////////////////End of OpenBabel/////////////////////////////////////


    ////////////////////////////Structure Information///////////////////////////////////
    double mol_weight=mol.GetMolWt();
    int mol_heavy=mol.NumHvyAtoms();
    int mol_hbd=OBDescriptor::FindType("HBD")->Predict(&mol);
    int mol_hba1=OBDescriptor::FindType("HBA1")->Predict(&mol);
    double mol_logp=OBDescriptor::FindType("logP")->Predict(&mol);
    double mol_tpsa=OBDescriptor::FindType("TPSA")->Predict(&mol);
    double mol_mr=OBDescriptor::FindType("MR")->Predict(&mol);
    int mol_numatoms=OBDescriptor::FindType("atoms")->Predict(&mol);
    int mol_rotb=mol.NumRotors();
    int mol_arom=0;
    int mol_alerts=0;

    ///////////////////Number of aromatic ring//////////////////////////////////////////
    vector<OBRing*> vr=mol.GetSSSR();
    vector<OBRing*>::iterator i;
    vector<int>::iterator j;
    for (i=vr.begin(); i!=vr.end(); ++i)
    {
        if ((*i)->IsAromatic()) mol_arom++;
    }
    ///////////////////End of number of aromatic ring////////////////////////////////////

    //////////////////////////////////Number of alerts///////////////////////////////////
    OBSmartsPattern smarts;
    vector< vector <int> > maplist;
    vector< vector <int> >::iterator matches;
    QString unwanted;

    QFile file("unwanted-groups.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);

    while (!in.atEnd())
    {

        unwanted = in.readLine();
        smarts.Init(unwanted.toUtf8().constData());
        if (smarts.Match(mol))
              {
                maplist = smarts.GetUMapList();
                for (matches = maplist.begin(); matches != maplist.end(); matches++)
                  {                    
                    mol_alerts++;
                  }
              }
    }
    file.close();
    ///////////////////////////////////End of number of alerts///////////////////////////

    //int mol_charge=mol.GetTotalCharge();
    ///////////////////////////Structure Information End////////////////////////////////

    int ruleof5=0;
    if (mol_hbd>5) ruleof5 += 1;
    if (mol_hba1>10) ruleof5 += 1;
    if (mol_logp>5) ruleof5 +=1;
    if (mol_weight>500) ruleof5+=1;

    int veber=0;
    if (mol_tpsa>140) veber+=1;

    int ghose=0;
    if (mol_weight<160 or mol_weight>480) ghose+=1;
    if (mol_logp<-0.4 or mol_logp>5.6) ghose+=1;
    if (mol_mr<40 or mol_mr>130) ghose+=1;
    if (mol_numatoms<20 or mol_numatoms>70) ghose+=1;

    double gleeson=0;
    double gleeson_mw=0;
    gleeson_mw= (mol_weight<330)?0:mol_weight;
    gleeson=qAbs(2.5-mol_logp)/2+qAbs(330-gleeson_mw)/120;

    double qed;
    double qed_mw=0.66*qLn((2.817+392.575/(1+qExp(-(mol_weight-290.749+2.420/2)/49.223))*(1-1/(1+qExp(-(mol_weight-290.749-2.420/2)/65.371))))/104.981);
    double qed_logp=0.46*qLn((3.173+137.862/(1+qExp(-(mol_logp-2.535+4.581/2)/0.823))*(1-1/(1+qExp(-(mol_logp-2.535-4.581/2)/0.576))))/131.319);
    double qed_hbd=0.61*qLn((1.619+1010.051/(1+qExp(-(mol_hbd-0.985+0/2)/0.714))*(1-1/(1+qExp(-(mol_hbd-0.985-0/2)/0.921))))/258.163);
    double qed_hba1=0.05*qLn((2.949+160.461/(1+qExp(-(mol_hba1-3.615+4.436/2)/0.290))*(1-1/(1+qExp(-(mol_hba1-3.615-4.436/2)/1.301))))/148.776);
    double qed_tpsa=0.06*qLn((1.877+125.223/(1+qExp(-(mol_tpsa-62.908+87.834/2)/12.020))*(1-1/(1+qExp(-(mol_tpsa-62.908-87.834/2)/28.513))))/104.569);
    double qed_rotb=0.65*qLn((0.010+272.412/(1+qExp(-(mol_rotb-2.558+1.566/2)/1.272))*(1-1/(1+qExp(-(mol_rotb-2.558-1.566/2)/2.758))))/105.442);
    double qed_arom=0.48*qLn((3.218+957.737/(1+qExp(-(mol_arom-2.275+0/2)/1.318))*(1-1/(1+qExp(-(mol_arom-2.275-0/2)/0.376))))/312.337);
    double qed_alerts=0.95*qLn((0.010+1199.094/(1+qExp(-(mol_alerts+0.090+0/2)/0.186))*(1-1/(1+qExp(-(mol_alerts+0.090-0/2)/0.875))))/417.725);
    qed=qExp((qed_mw+qed_logp+qed_hbd+qed_hba1+qed_tpsa+qed_rotb+qed_arom+qed_alerts)/3.92);


    //bool b_desai;

    QString b_hitchcock;
    if((mol_tpsa<90) & (mol_hbd<2)) b_hitchcock="Pass"; else b_hitchcock="Failed";

    QString b_ghose;
    //if ((mol_tpsa<76) & (mol_hbd<3) & (mol_volume<970) & (mol_volume>740) & (mol_sasa<580) & (mol_sasa>460) & (mol_nitrogen>=0))  b_ghose="True"; else b_ghose="False";

    QString b_raub;
    if((mol_logp<4)&(mol_tpsa>40)&(mol_tpsa<80)) b_raub="True"; else b_raub="False";

    QString b_pajouhesh;
    if((mol_weight<450)&(mol_logp<5)& (mol_hbd<3)&(mol_hba1<7)&(mol_rotb<8)&(mol_tpsa<70)) b_pajouhesh="True"; else b_pajouhesh="False";

    QString b_norinder;
    //if(((mol_oxygen+mol_nitrogen)<5) or ((mol_logp-mol_oxygen-mol_nitrogen)>0)) b_norinder="True"; else b_norinder="False";



    ui->table_general->setItem(0,0, new QTableWidgetItem(QString::number(mol_weight)));


    if (ui->radio_drug->isChecked())
    {
        ui->table_general->setItem(1,0, new QTableWidgetItem(QString::number(mol_logp)));
        ui->table_general->setItem(2,0, new QTableWidgetItem(QString::number(mol_mr)));
        //ui->table_general->setItem(3,0, new QTableWidgetItem(QString::number(mol_logs)));
        ui->table_general->setItem(4,0, new QTableWidgetItem(QString::number(mol_hbd)));
        ui->table_general->setItem(5,0, new QTableWidgetItem(QString::number(mol_hba1)));
        ui->table_general->setItem(6,0, new QTableWidgetItem(QString::number(mol_numatoms)));
        ui->table_general->setItem(7,0, new QTableWidgetItem(QString::number(mol_tpsa)));
        ui->table_general->setItem(8,0, new QTableWidgetItem(QString::number(mol_rotb)));
        ui->table_general->setItem(9,0, new QTableWidgetItem(QString::number(mol_heavy)));
        //ui->table_general->setItem(10,0, new QTableWidgetItem(QString::number(mol_sasa)));
        //ui->table_general->setItem(11,0, new QTableWidgetItem(QString::number(mol_volume)));
        ui->tb_oral->setItem(0,0, new QTableWidgetItem(QString::number(ruleof5)));
        ui->tb_oral->setItem(1,0, new QTableWidgetItem(QString::number(veber)));
        ui->tb_oral->setItem(2,0, new QTableWidgetItem(QString::number(ghose)));
        ui->tb_oral->setItem(3,0, new QTableWidgetItem(QString::number(gleeson)));
        ui->tb_oral->setItem(4,0, new QTableWidgetItem(QString::number(qed)));
        ui->tb_cns->setItem(3,0,new QTableWidgetItem(QString(b_raub)));
        ui->tb_cns->setItem(4,0,new QTableWidgetItem(QString(b_hitchcock)));
        ui->tb_cns->setItem(5,0,new QTableWidgetItem(QString(b_pajouhesh)));
        ui->tb_cns->setItem(6,0,new QTableWidgetItem(QString(b_norinder)));

        if (mol_weight>=200 and mol_weight<=600) ui->table_general->item(0,0)->setBackgroundColor(Qt::green); else ui->table_general->item(0,0)->setBackgroundColor(Qt::red);
        if (mol_logp>=-5 and mol_logp<=6) ui->table_general->item(1,0)->setBackgroundColor(Qt::green); else ui->table_general->item(1,0)->setBackgroundColor(Qt::red);
        if (mol_mr>=10 and mol_mr<=130) ui->table_general->item(2,0)->setBackgroundColor(Qt::green); else ui->table_general->item(2,0)->setBackgroundColor(Qt::red);
        //if (mol_logs>=-5 and mol_logs<=6) ui->table_general->item(3,0)->setBackgroundColor(Qt::green); else ui->table_general->item(3,0)->setBackgroundColor(Qt::red);
        if (mol_hbd>=0 and mol_hbd<=6) ui->table_general->item(4,0)->setBackgroundColor(Qt::green); else ui->table_general->item(4,0)->setBackgroundColor(Qt::red);
        if (mol_hba1>=0 and mol_hba1<=8) ui->table_general->item(5,0)->setBackgroundColor(Qt::green); else ui->table_general->item(5,0)->setBackgroundColor(Qt::red);
        if (mol_numatoms>=0 and mol_numatoms<=70) ui->table_general->item(6,0)->setBackgroundColor(Qt::green); else ui->table_general->item(6,0)->setBackgroundColor(Qt::red);
        if (mol_tpsa>=0 and mol_tpsa<=140) ui->table_general->item(7,0)->setBackgroundColor(Qt::green); else ui->table_general->item(7,0)->setBackgroundColor(Qt::red);
        if (mol_rotb>=0 and mol_rotb<=20) ui->table_general->item(8,0)->setBackgroundColor(Qt::green); else ui->table_general->item(8,0)->setBackgroundColor(Qt::red);
        if (mol_heavy>=15 and mol_heavy<=35) ui->table_general->item(9,0)->setBackgroundColor(Qt::green); else ui->table_general->item(9,0)->setBackgroundColor(Qt::red);
        if (ruleof5<1) ui->tb_oral->item(0,0)->setBackgroundColor(Qt::green); else ui->tb_oral->item(0,0)->setBackgroundColor(Qt::red);
        if (veber<1) ui->tb_oral->item(1,0)->setBackgroundColor(Qt::green); else ui->tb_oral->item(1,0)->setBackgroundColor(Qt::red);
        if (ghose<1) ui->tb_oral->item(2,0)->setBackgroundColor(Qt::green); else ui->tb_oral->item(2,0)->setBackgroundColor(Qt::red);
        if (gleeson<2) ui->tb_oral->item(3,0)->setBackgroundColor(Qt::green); else ui->tb_oral->item(3,0)->setBackgroundColor(Qt::red);
        if (qed>0.1) ui->tb_oral->item(4,0)->setBackgroundColor(Qt::green); else ui->tb_oral->item(4,0)->setBackgroundColor(Qt::red);

    }

    if (ui->radio_fragment->isChecked())
    {

    }

    if (ui->radio_lead->isChecked())
    {

    }




}

void MainWindow::on_actionFilter_database_triggered()
{
    filter filterwindow;
    filterwindow.setModal(true);
    filterwindow.exec();
}

void MainWindow::on_radio_lead_clicked()
{
    ui->table_general->showRow(1);

}

void MainWindow::on_radio_fragment_clicked()
{
    ui->table_general->hideRow(1);
    ui->table_general->setItem(0,1, new QTableWidgetItem("0 to 350"));
}

void MainWindow::on_radio_drug_clicked()
{
    ui->table_general->showRow(1);
    ui->table_general->showRow(10);
    ui->table_general->showRow(11);
    ui->table_general->showRow(12);
    ui->table_general->showRow(13);
    ui->table_general->showRow(14);
    ui->table_general->setItem(0,1, new QTableWidgetItem("200 to 600"));
    ui->table_general->setItem(1,1, new QTableWidgetItem("-5 to 6"));
    ui->table_general->setItem(4,1, new QTableWidgetItem("0 to 6"));
    ui->table_general->setItem(5,1, new QTableWidgetItem("0 to 8"));
    ui->table_general->setItem(7,1, new QTableWidgetItem("0 to 140 Å²"));
    ui->table_general->setItem(8,1, new QTableWidgetItem("0 to 20"));
    ui->table_general->setItem(9,1, new QTableWidgetItem("15 to 35"));
    ui->tb_oral->setItem(0,1, new QTableWidgetItem("0"));
    ui->tb_oral->setItem(1,1, new QTableWidgetItem("0"));
    ui->tb_oral->setItem(2,1, new QTableWidgetItem("0"));
    ui->tb_oral->setItem(3,1, new QTableWidgetItem("0 to 3"));
    ui->tb_oral->setItem(4,1, new QTableWidgetItem("0.1 to 1"));
}


