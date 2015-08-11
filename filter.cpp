#include "filter.h"
#include "ui_filter.h"
#include <QtWidgets/QFileDialog>
#include <QTextStream>
#include <QDebug>

filter::filter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::filter)
{
    ui->setupUi(this);
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setColumnWidth(0,300);
    ui->treeWidget->headerItem()->setText(0,"Property");
    ui->treeWidget->headerItem()->setText(1,"Value");
    ui->le_import->setEnabled(false);
    ui->pb_fimport->setEnabled(false);
    ui->radio_fdrug->click();

}

filter::~filter()
{
    delete ui;
}


void filter::on_radio_frommainwindow_clicked()
{
    ui->le_import->setEnabled(false);
    ui->pb_fimport->setEnabled(false);
}

void filter::on_radio_fromfile_clicked()
{
    ui->le_import->setEnabled(true);
    ui->pb_fimport->setEnabled(true);
}

void filter::on_pb_cancel_clicked()
{
    close();
}

void filter::on_pb_start_clicked()
{


}

void filter::on_pb_fexport_clicked()
{
    outputfilename = QFileDialog::getSaveFileName(this, tr("Save File"),QDir::homePath(),tr("Tripos MOL2 Files *.mol2 (*.mol2);;SDF Files *.sdf (*.sdf);;SMILES Files *.smiles (*.smiles);;MOL Files *.mol (*.mol)"));
    ui->le_export->setText(outputfilename);

}

void filter::on_pb_fimport_clicked()
{
    QString inputfilename= QFileDialog::getOpenFileName(this, tr("Open File"),QDir::homePath(),tr("Tripos MOL2 Files *.mol2 (*.mol2);;SDF Files *.sdf (*.sdf);;SMILES Files *.smiles (*.smiles);;MOL Files *.mol (*.mol)"));
    ui->le_import->setText(inputfilename);
}


void filter::AddChild(QTreeWidgetItem *parent, QString property, QString value)
{
    QTreeWidgetItem *itm=new QTreeWidgetItem();
    itm->setText(0, property);
    itm->setText(1, value);
    parent->addChild(itm);

}


void filter::on_radio_fdrug_clicked()
{
    ui->treeWidget->clear();
    int group=0;
    QFile file("filter_drug.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString filter_data;

    QTreeWidgetItem *itm=new QTreeWidgetItem(ui->treeWidget);
    itm->setText(0, "General");
    QTreeWidgetItem *specific=new QTreeWidgetItem(ui->treeWidget);
    specific->setText(0, "Undesirable Functional Groups");
    QTreeWidgetItem *eliminate=new QTreeWidgetItem(ui->treeWidget);
    eliminate->setText(0, "Eliminated Functional Groups");
    QTreeWidgetItem *allowed=new QTreeWidgetItem(ui->treeWidget);
    allowed->setText(0, "Allowed Functional Groups");
    QTreeWidgetItem *other=new QTreeWidgetItem(ui->treeWidget);
    other->setText(0, "Other Functional Groups");

    while (!in.atEnd())
    {
        filter_data = in.readLine();

        if (filter_data.trimmed()=="") continue;
        if (filter_data.at(0)=='#')
        {
            if(filter_data.left(9)=="#specific")
            {
                group=1;
                continue;
            }
            if(filter_data.left(10)=="#eliminate")
            {
                group=2;
                continue;
            }
            if(filter_data.left(8)=="#allowed")
            {
                group=3;
                continue;
            }
            if(filter_data.left(6)=="#other")
            {
                group=4;
                continue;
            }
        }
        if (filter_data.left(4)=="RULE")
        {
            QStringList lstLine = filter_data.split("  ");
            switch (group)
            {
            case 1:
                AddChild(specific, lstLine.at(2),lstLine.at(1));
                continue;
            case 2:
                AddChild(eliminate, lstLine.at(2),lstLine.at(1));
                continue;
            case 3:
                AddChild(allowed, lstLine.at(2),lstLine.at(1));
                continue;
            case 4:
                AddChild(other, lstLine.at(2),lstLine.at(1));
                continue;
            }
        }

        else
        {
            QStringList lstLine = filter_data.split(" ");
            AddChild(itm, lstLine.at(0),lstLine.at(1));
        }
    }
    file.close();
}

void filter::on_radio_flead_clicked()
{
    ui->treeWidget->clear();
    int group=0;
    QFile file("filter_lead.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString filter_data;

    QTreeWidgetItem *itm=new QTreeWidgetItem(ui->treeWidget);
    itm->setText(0, "General");
    QTreeWidgetItem *specific=new QTreeWidgetItem(ui->treeWidget);
    specific->setText(0, "Undesirable Functional Groups");
    QTreeWidgetItem *eliminate=new QTreeWidgetItem(ui->treeWidget);
    eliminate->setText(0, "Eliminated Functional Groups");
    QTreeWidgetItem *allowed=new QTreeWidgetItem(ui->treeWidget);
    allowed->setText(0, "Allowed Functional Groups");
    QTreeWidgetItem *other=new QTreeWidgetItem(ui->treeWidget);
    other->setText(0, "Other Functional Groups");

    while (!in.atEnd())
    {
        filter_data = in.readLine();

        if (filter_data.trimmed()=="") continue;
        if (filter_data.at(0)=='#')
        {
            if(filter_data.left(9)=="#specific")
            {
                group=1;
                continue;
            }
            if(filter_data.left(10)=="#eliminate")
            {
                group=2;
                continue;
            }
            if(filter_data.left(8)=="#allowed")
            {
                group=3;
                continue;
            }
            if(filter_data.left(6)=="#other")
            {
                group=4;
                continue;
            }
        }
        if (filter_data.left(4)=="RULE")
        {
            QStringList lstLine = filter_data.split("  ");
            switch (group)
            {
            case 1:
                AddChild(specific, lstLine.at(2),lstLine.at(1));
                continue;
            case 2:
                AddChild(eliminate, lstLine.at(2),lstLine.at(1));
                continue;
            case 3:
                AddChild(allowed, lstLine.at(2),lstLine.at(1));
                continue;
            case 4:
                AddChild(other, lstLine.at(2),lstLine.at(1));
                continue;
            }
        }

        else
        {
            QStringList lstLine = filter_data.split(" ");
            AddChild(itm, lstLine.at(0),lstLine.at(1));
        }
    }
    file.close();

}

void filter::on_radio_ffragment_clicked()
{
    ui->treeWidget->clear();
    int group=0;
    QFile file("filter_frag.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(&file);
    QString filter_data;

    QTreeWidgetItem *itm=new QTreeWidgetItem(ui->treeWidget);
    itm->setText(0, "General");
    QTreeWidgetItem *specific=new QTreeWidgetItem(ui->treeWidget);
    specific->setText(0, "Undesirable Functional Groups");
    QTreeWidgetItem *eliminate=new QTreeWidgetItem(ui->treeWidget);
    eliminate->setText(0, "Eliminated Functional Groups");
    QTreeWidgetItem *allowed=new QTreeWidgetItem(ui->treeWidget);
    allowed->setText(0, "Allowed Functional Groups");
    QTreeWidgetItem *other=new QTreeWidgetItem(ui->treeWidget);
    other->setText(0, "Other Functional Groups");

    while (!in.atEnd())
    {
        filter_data = in.readLine();

        if (filter_data.trimmed()=="") continue;
        if (filter_data.at(0)=='#')
        {
            if(filter_data.left(9)=="#specific")
            {
                group=1;
                continue;
            }
            if(filter_data.left(10)=="#eliminate")
            {
                group=2;
                continue;
            }
            if(filter_data.left(8)=="#allowed")
            {
                group=3;
                continue;
            }
            if(filter_data.left(6)=="#other")
            {
                group=4;
                continue;
            }
        }
        if (filter_data.left(4)=="RULE")
        {
            QStringList lstLine = filter_data.split("  ");
            switch (group)
            {
            case 1:
                AddChild(specific, lstLine.at(2),lstLine.at(1));
                continue;
            case 2:
                AddChild(eliminate, lstLine.at(2),lstLine.at(1));
                continue;
            case 3:
                AddChild(allowed, lstLine.at(2),lstLine.at(1));
                continue;
            case 4:
                AddChild(other, lstLine.at(2),lstLine.at(1));
                continue;
            }
        }

        else
        {
            QStringList lstLine = filter_data.split(" ");
            AddChild(itm, lstLine.at(0),lstLine.at(1));
        }
    }
    file.close();
}


void filter::on_treeWidget_itemClicked(QTreeWidgetItem *item, int)
{
    if (ui->radio_fcustom->isChecked())  item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
}
