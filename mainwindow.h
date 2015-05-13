#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_bt_import_clicked();

    void on_bt_add2list_clicked();

    void on_le_SMILE_returnPressed();

    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

    void on_actionImport_triggered();

    void on_actionExport_triggered();



    void on_tb_molecule_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_actionFilter_database_triggered();

    void on_radio_lead_clicked();

    void on_radio_fragment_clicked();

    void on_radio_drug_clicked();


private:
    Ui::MainWindow *ui;
    int row_i;
};

#endif // MAINWINDOW_H
