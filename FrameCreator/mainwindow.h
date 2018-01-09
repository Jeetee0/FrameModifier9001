#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Frame.h"

#include <QListWidgetItem>

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
    void PreviewFrames();

    void on_bt_chooseRow1_clicked();

    void on_bt_chooseRow2_clicked();

    void on_bt_chooseRow3_clicked();

    void on_bt_chooseRow4_clicked();

    void on_bt_chooseRow5_clicked();

    void on_bt_chooseRow6_clicked();

    void on_bt_chooseRow7_clicked();

    void on_bt_chooseRow8_clicked();

    void on_bt_chooseRow9_clicked();

    void on_bt_chooseRow10_clicked();

    void on_bt_chooseRow11_clicked();

    void on_bt_chooseRow12_clicked();

    void on_bt_chooseRow13_clicked();

    void on_bt_chooseRow14_clicked();

    void on_bt_chooseRow15_clicked();

    void on_bt_chooseRow16_clicked();

    void on_bt_chooseColumn1_clicked();

    void on_bt_chooseColumn2_clicked();

    void on_bt_chooseColumn3_clicked();

    void on_bt_chooseColumn4_clicked();

    void on_bt_chooseColumn5_clicked();

    void on_bt_chooseColumn6_clicked();

    void on_bt_chooseColumn7_clicked();

    void on_bt_chooseColumn8_clicked();

    void on_bt_chooseColumn9_clicked();

    void on_bt_chooseColumn10_clicked();

    void on_bt_chooseColumn11_clicked();

    void on_bt_chooseColumn12_clicked();

    void on_bt_chooseColumn13_clicked();

    void on_bt_chooseColumn14_clicked();

    void on_bt_chooseColumn15_clicked();

    void on_bt_chooseColumn16_clicked();

    void on_bt_selectAll_clicked();

    void on_bt_InvertAll_clicked();

    void on_bt_deselectAll_clicked();

    void on_bt_new_clicked();

    void on_bt_load_clicked();

    void on_bt_save_clicked();

    void on_bt_export_clicked();

    void on_bt_addFrame_clicked();

    void on_bt_deleteFrame_clicked();

    void on_bt_overwriteFrame_clicked();

    void on_bt_copyFrame_clicked();

    void setChecked(bool whatToDo);

    bool saveFile(QString fileName);

    bool exportFile(QString fileName);

    std::vector<Frame>::iterator findElementInVectorByName(QString frameName);

    void on_lineEdit_frameName_textChanged(const QString &arg1);

    void on_radioButton_1_1_toggled(bool checked);

    void on_radioButton_2_1_toggled(bool checked);

    void on_radioButton_1_2_toggled(bool checked);

    void on_radioButton_1_3_toggled(bool checked);

    void on_radioButton_1_4_toggled(bool checked);

    void on_radioButton_1_5_toggled(bool checked);

    void on_radioButton_1_6_toggled(bool checked);

    void on_radioButton_1_7_toggled(bool checked);

    void on_radioButton_1_8_toggled(bool checked);

    void on_radioButton_1_9_toggled(bool checked);

    void on_radioButton_1_10_toggled(bool checked);

    void on_radioButton_1_11_toggled(bool checked);

    void on_radioButton_1_12_toggled(bool checked);

    void on_radioButton_1_13_toggled(bool checked);

    void on_radioButton_1_14_toggled(bool checked);

    void on_radioButton_1_15_toggled(bool checked);

    void on_radioButton_1_16_toggled(bool checked);

    void on_radioButton_2_2_toggled(bool checked);

    void on_radioButton_2_3_toggled(bool checked);

    void on_radioButton_2_4_toggled(bool checked);

    void on_radioButton_2_5_toggled(bool checked);

    void on_radioButton_2_6_toggled(bool checked);

    void on_radioButton_2_7_toggled(bool checked);

    void on_radioButton_2_8_toggled(bool checked);

    void on_radioButton_2_9_toggled(bool checked);

    void on_radioButton_2_10_toggled(bool checked);

    void on_radioButton_2_11_toggled(bool checked);

    void on_radioButton_2_12_toggled(bool checked);

    void on_radioButton_2_13_toggled(bool checked);

    void on_radioButton_2_14_toggled(bool checked);

    void on_radioButton_2_15_toggled(bool checked);

    void on_radioButton_2_16_toggled(bool checked);

    void on_radioButton_3_1_toggled(bool checked);

    void on_radioButton_3_2_toggled(bool checked);

    void on_radioButton_3_3_toggled(bool checked);

    void on_radioButton_3_4_toggled(bool checked);

    void on_radioButton_3_5_toggled(bool checked);

    void on_radioButton_3_6_toggled(bool checked);

    void on_radioButton_3_7_toggled(bool checked);

    void on_radioButton_3_8_toggled(bool checked);

    void on_radioButton_3_9_toggled(bool checked);

    void on_radioButton_3_10_toggled(bool checked);

    void on_radioButton_3_11_toggled(bool checked);

    void on_radioButton_3_12_toggled(bool checked);

    void on_radioButton_3_13_toggled(bool checked);

    void on_radioButton_3_14_toggled(bool checked);

    void on_radioButton_3_15_toggled(bool checked);

    void on_radioButton_3_16_toggled(bool checked);

    void on_radioButton_4_1_toggled(bool checked);

    void on_radioButton_4_2_toggled(bool checked);

    void on_radioButton_4_3_toggled(bool checked);

    void on_radioButton_4_4_toggled(bool checked);

    void on_radioButton_4_5_toggled(bool checked);

    void on_radioButton_4_6_toggled(bool checked);

    void on_radioButton_4_7_toggled(bool checked);

    void on_radioButton_4_8_toggled(bool checked);

    void on_radioButton_4_9_toggled(bool checked);

    void on_radioButton_4_10_toggled(bool checked);

    void on_radioButton_4_11_toggled(bool checked);

    void on_radioButton_4_12_toggled(bool checked);

    void on_radioButton_4_13_toggled(bool checked);

    void on_radioButton_4_14_toggled(bool checked);

    void on_radioButton_4_15_toggled(bool checked);

    void on_radioButton_4_16_toggled(bool checked);

    void on_radioButton_5_1_toggled(bool checked);

    void on_radioButton_5_2_toggled(bool checked);

    void on_radioButton_5_3_toggled(bool checked);

    void on_radioButton_5_4_toggled(bool checked);

    void on_radioButton_5_5_toggled(bool checked);

    void on_radioButton_5_6_toggled(bool checked);

    void on_radioButton_5_7_toggled(bool checked);

    void on_radioButton_5_8_toggled(bool checked);

    void on_radioButton_5_9_toggled(bool checked);

    void on_radioButton_5_10_toggled(bool checked);

    void on_radioButton_5_11_toggled(bool checked);

    void on_radioButton_5_12_toggled(bool checked);

    void on_radioButton_5_13_toggled(bool checked);

    void on_radioButton_5_14_toggled(bool checked);

    void on_radioButton_5_15_toggled(bool checked);

    void on_radioButton_5_16_toggled(bool checked);

    void on_radioButton_6_1_toggled(bool checked);

    void on_radioButton_6_2_toggled(bool checked);

    void on_radioButton_6_3_toggled(bool checked);

    void on_radioButton_6_4_toggled(bool checked);

    void on_radioButton_6_5_toggled(bool checked);

    void on_radioButton_6_6_toggled(bool checked);

    void on_radioButton_6_7_toggled(bool checked);

    void on_radioButton_6_8_toggled(bool checked);

    void on_radioButton_6_9_toggled(bool checked);

    void on_radioButton_6_10_toggled(bool checked);

    void on_radioButton_6_11_toggled(bool checked);

    void on_radioButton_6_12_toggled(bool checked);

    void on_radioButton_6_13_toggled(bool checked);

    void on_radioButton_6_14_toggled(bool checked);

    void on_radioButton_6_15_toggled(bool checked);

    void on_radioButton_6_16_toggled(bool checked);

    void on_radioButton_7_1_toggled(bool checked);

    void on_radioButton_7_2_toggled(bool checked);

    void on_radioButton_7_3_toggled(bool checked);

    void on_radioButton_7_4_toggled(bool checked);

    void on_radioButton_7_5_toggled(bool checked);

    void on_radioButton_7_6_toggled(bool checked);

    void on_radioButton_7_7_toggled(bool checked);

    void on_radioButton_7_8_toggled(bool checked);

    void on_radioButton_7_9_toggled(bool checked);

    void on_radioButton_7_10_toggled(bool checked);

    void on_radioButton_7_11_toggled(bool checked);

    void on_radioButton_7_12_toggled(bool checked);

    void on_radioButton_7_13_toggled(bool checked);

    void on_radioButton_7_14_toggled(bool checked);

    void on_radioButton_7_15_toggled(bool checked);

    void on_radioButton_7_16_toggled(bool checked);

    void on_radioButton_8_1_toggled(bool checked);

    void on_radioButton_8_2_toggled(bool checked);

    void on_radioButton_8_3_toggled(bool checked);

    void on_radioButton_8_4_toggled(bool checked);

    void on_radioButton_8_5_toggled(bool checked);

    void on_radioButton_8_6_toggled(bool checked);

    void on_radioButton_8_7_toggled(bool checked);

    void on_radioButton_8_8_toggled(bool checked);

    void on_radioButton_8_9_toggled(bool checked);

    void on_radioButton_8_10_toggled(bool checked);

    void on_radioButton_8_11_toggled(bool checked);

    void on_radioButton_8_12_toggled(bool checked);

    void on_radioButton_8_13_toggled(bool checked);

    void on_radioButton_8_14_toggled(bool checked);

    void on_radioButton_8_15_toggled(bool checked);

    void on_radioButton_8_16_toggled(bool checked);

    void on_radioButton_9_1_toggled(bool checked);

    void on_radioButton_9_2_toggled(bool checked);

    void on_radioButton_9_3_toggled(bool checked);

    void on_radioButton_9_4_toggled(bool checked);

    void on_radioButton_9_5_toggled(bool checked);

    void on_radioButton_9_6_toggled(bool checked);

    void on_radioButton_9_7_toggled(bool checked);

    void on_radioButton_9_8_toggled(bool checked);

    void on_radioButton_9_9_toggled(bool checked);

    void on_radioButton_9_10_toggled(bool checked);

    void on_radioButton_9_11_toggled(bool checked);

    void on_radioButton_9_12_toggled(bool checked);

    void on_radioButton_9_13_toggled(bool checked);

    void on_radioButton_9_14_toggled(bool checked);

    void on_radioButton_9_15_toggled(bool checked);

    void on_radioButton_9_16_toggled(bool checked);

    void on_radioButton_10_1_toggled(bool checked);

    void on_radioButton_10_2_toggled(bool checked);

    void on_radioButton_10_3_toggled(bool checked);

    void on_radioButton_10_4_toggled(bool checked);

    void on_radioButton_10_5_toggled(bool checked);

    void on_radioButton_10_6_toggled(bool checked);

    void on_radioButton_10_7_toggled(bool checked);

    void on_radioButton_10_8_toggled(bool checked);

    void on_radioButton_10_9_toggled(bool checked);

    void on_radioButton_10_10_toggled(bool checked);

    void on_radioButton_10_11_toggled(bool checked);

    void on_radioButton_10_12_toggled(bool checked);

    void on_radioButton_10_13_toggled(bool checked);

    void on_radioButton_10_14_toggled(bool checked);

    void on_radioButton_10_15_toggled(bool checked);

    void on_radioButton_10_16_toggled(bool checked);

    void on_radioButton_11_1_toggled(bool checked);

    void on_radioButton_11_2_toggled(bool checked);

    void on_radioButton_11_3_toggled(bool checked);

    void on_radioButton_11_4_toggled(bool checked);

    void on_radioButton_11_5_toggled(bool checked);

    void on_radioButton_11_6_toggled(bool checked);

    void on_radioButton_11_7_toggled(bool checked);

    void on_radioButton_11_8_toggled(bool checked);

    void on_radioButton_11_9_toggled(bool checked);

    void on_radioButton_11_10_toggled(bool checked);

    void on_radioButton_11_11_toggled(bool checked);

    void on_radioButton_11_12_toggled(bool checked);

    void on_radioButton_11_13_toggled(bool checked);

    void on_radioButton_11_14_toggled(bool checked);

    void on_radioButton_11_15_toggled(bool checked);

    void on_radioButton_11_16_toggled(bool checked);

    void on_radioButton_12_1_toggled(bool checked);

    void on_radioButton_12_2_toggled(bool checked);

    void on_radioButton_12_3_toggled(bool checked);

    void on_radioButton_12_4_toggled(bool checked);

    void on_radioButton_12_5_toggled(bool checked);

    void on_radioButton_12_6_toggled(bool checked);

    void on_radioButton_12_7_toggled(bool checked);

    void on_radioButton_12_8_toggled(bool checked);

    void on_radioButton_12_9_toggled(bool checked);

    void on_radioButton_12_10_toggled(bool checked);

    void on_radioButton_12_11_toggled(bool checked);

    void on_radioButton_12_12_toggled(bool checked);

    void on_radioButton_12_13_toggled(bool checked);

    void on_radioButton_12_14_toggled(bool checked);

    void on_radioButton_12_15_toggled(bool checked);

    void on_radioButton_12_16_toggled(bool checked);

    void on_radioButton_13_1_toggled(bool checked);

    void on_radioButton_13_2_toggled(bool checked);

    void on_radioButton_13_3_toggled(bool checked);

    void on_radioButton_13_4_toggled(bool checked);

    void on_radioButton_13_5_toggled(bool checked);

    void on_radioButton_13_6_toggled(bool checked);

    void on_radioButton_13_7_toggled(bool checked);

    void on_radioButton_13_8_toggled(bool checked);

    void on_radioButton_13_9_toggled(bool checked);

    void on_radioButton_13_10_toggled(bool checked);

    void on_radioButton_13_11_toggled(bool checked);

    void on_radioButton_13_12_toggled(bool checked);

    void on_radioButton_13_13_toggled(bool checked);

    void on_radioButton_13_14_toggled(bool checked);

    void on_radioButton_13_15_toggled(bool checked);

    void on_radioButton_13_16_toggled(bool checked);

    void on_radioButton_14_1_toggled(bool checked);

    void on_radioButton_14_2_toggled(bool checked);

    void on_radioButton_14_3_toggled(bool checked);

    void on_radioButton_14_4_toggled(bool checked);

    void on_radioButton_14_5_toggled(bool checked);

    void on_radioButton_14_6_toggled(bool checked);

    void on_radioButton_14_7_toggled(bool checked);

    void on_radioButton_14_8_toggled(bool checked);

    void on_radioButton_14_9_toggled(bool checked);

    void on_radioButton_14_10_toggled(bool checked);

    void on_radioButton_14_11_toggled(bool checked);

    void on_radioButton_14_12_toggled(bool checked);

    void on_radioButton_14_13_toggled(bool checked);

    void on_radioButton_14_14_toggled(bool checked);

    void on_radioButton_14_15_toggled(bool checked);

    void on_radioButton_14_16_toggled(bool checked);

    void on_radioButton_15_1_toggled(bool checked);

    void on_radioButton_15_2_toggled(bool checked);

    void on_radioButton_15_3_toggled(bool checked);

    void on_radioButton_15_4_toggled(bool checked);

    void on_radioButton_15_5_toggled(bool checked);

    void on_radioButton_15_6_toggled(bool checked);

    void on_radioButton_15_7_toggled(bool checked);

    void on_radioButton_15_8_toggled(bool checked);

    void on_radioButton_15_9_toggled(bool checked);

    void on_radioButton_15_10_toggled(bool checked);

    void on_radioButton_15_11_toggled(bool checked);

    void on_radioButton_15_12_toggled(bool checked);

    void on_radioButton_15_13_toggled(bool checked);

    void on_radioButton_15_14_toggled(bool checked);

    void on_radioButton_15_15_toggled(bool checked);

    void on_radioButton_15_16_toggled(bool checked);

    void on_radioButton_16_1_toggled(bool checked);

    void on_radioButton_16_2_toggled(bool checked);

    void on_radioButton_16_3_toggled(bool checked);

    void on_radioButton_16_4_toggled(bool checked);

    void on_radioButton_16_5_toggled(bool checked);

    void on_radioButton_16_6_toggled(bool checked);

    void on_radioButton_16_7_toggled(bool checked);

    void on_radioButton_16_8_toggled(bool checked);

    void on_radioButton_16_9_toggled(bool checked);

    void on_radioButton_16_10_toggled(bool checked);

    void on_radioButton_16_11_toggled(bool checked);

    void on_radioButton_16_12_toggled(bool checked);

    void on_radioButton_16_13_toggled(bool checked);

    void on_radioButton_16_14_toggled(bool checked);

    void on_radioButton_16_15_toggled(bool checked);

    void on_radioButton_16_16_toggled(bool checked);

    void setPinFrameFromArray(bool pinFrameArray[16][16]);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_bt_preview_clicked();

    void enableOrDisableEditButtons(bool enable);

    void loadFile(QString fileName, bool append);

    void addFrameToListWidget(QString frameName);

    void delay(int millisecondsToWait);

    void on_lineEdit_frameName2_textChanged(const QString &arg1);

    void on_bt_setRandom_clicked();

    void keyReleaseEvent(QKeyEvent* ke);

    void on_bt_info_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
