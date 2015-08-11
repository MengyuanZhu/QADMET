#ifndef FILTER_H
#define FILTER_H
#include <QTreeWidgetItem>
#include <QDialog>
#include <QString>
namespace Ui {
class filter;
}

class filter : public QDialog
{
    Q_OBJECT

public:
    explicit filter(QWidget *parent = 0);
    ~filter();



private slots:
    void on_radio_frommainwindow_clicked();

    void on_radio_fromfile_clicked();

    void on_pb_cancel_clicked();

    void on_pb_start_clicked();

    void on_pb_fexport_clicked();

    void on_pb_fimport_clicked();

    void AddChild(QTreeWidgetItem *parent, QString property, QString value);

    void on_radio_fdrug_clicked();

    void on_radio_flead_clicked();

    void on_radio_ffragment_clicked();



    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::filter *ui;
    QString inputfilename;
    QString outputfilename;

};

#endif // FILTER_H
