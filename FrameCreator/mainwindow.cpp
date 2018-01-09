#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QDebug"
#include "QList"
#include <vector>
#include <algorithm>

#include "Frame.h"

#include "QStandardItemModel"
#include "QStandardItem"

#include "QMessageBox"
#include "QFileDialog"
#include <QTime>

//todo: create executable of program

//tasks for "post abgabe"
//todo: implement mirror buttons


//variables
std::vector<Frame> vectorOfFrames;
bool currentPinArray[16][16];
Frame currentFrame("Frame1", currentPinArray);
int numberOfFrames = 1;
bool preview = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //set start values
    ui->lineEdit_delay->setText("350");
    ui->lineEdit_frameName->setText("Frame");
    ui->lineEdit_frameName2->setText("1");

    //not yet implemented
    ui->bt_enableMirrorMode->setEnabled(false);
    ui->bt_mirrorHorizontal->setEnabled(false);
    ui->bt_mirrorVertical->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//this method handles the button clicked event for creating a new project and resetting all progress
void MainWindow::on_bt_new_clicked()
{
    //security check
    QMessageBox msgBox;
    msgBox.setText("Create new project.");
    msgBox.setInformativeText("Are you sure you want to create a new project?\nCurrent progress may be lost.");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int returnValue = msgBox.exec();

    //delete all content, set start values
    if (returnValue == QMessageBox::Yes) {
        numberOfFrames = 1;

        ui->listWidget->clear();
        ui->lineEdit_delay->setText("250");
        ui->lineEdit_totalAmount->setText(QString::number(numberOfFrames));
        ui->lineEdit_frameName->setText("Frame");
        ui->lineEdit_frameName2->setText("1");
        vectorOfFrames.clear();

        //unselect all radioButtons
        on_bt_deselectAll_clicked();
    }
}

//this method handles the button clicked event for loading a file
void MainWindow::on_bt_load_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Load existing project.");
    msgBox.setInformativeText("Do you want to append your frames of the loaded project to the currently openend one? \n\n\nOr do you want only your loaded project?\n");
    QAbstractButton* pButtonAppend = msgBox.addButton(tr("Append"), QMessageBox::YesRole);
    QAbstractButton* pButtonLoad = msgBox.addButton(tr("Load"), QMessageBox::YesRole);
    QAbstractButton* pButtonCancel = msgBox.addButton(tr("Cancel"), QMessageBox::YesRole);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.exec();

    if (msgBox.clickedButton() != pButtonCancel) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open Frame Project"), "/home", tr("Text files (*.txt)"));

        //check if suffix is correct
        QFileInfo fileInfo(fileName);
        if (fileInfo.suffix() != "txt") {
            QMessageBox info;
            info.setText("Please only open files in '.txt' format.");
            info.exec();
        } else if (fileName != "") {
            if (msgBox.clickedButton() == pButtonAppend) {
                //append
                loadFile(fileName, true);
            }
            else if (msgBox.clickedButton() == pButtonLoad) {
                //load
                loadFile(fileName, false);
            }
        }
    }
}

//this method handles the button clicked event for saving a project to a file
void MainWindow::on_bt_save_clicked()
{
    QMessageBox info;
    if (vectorOfFrames.size() == 0) {
        info.setText("Please add some Frames before saving or exporting.");
        info.exec();
        return;
    }
    //open dialog where to save
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Frame Project"), "", tr("Text files (*.txt)"));
    if (filePath != "") {
        //save file to path
        bool success = saveFile(filePath);
        if (success) {
            info.setText("Successfully saved file to\n" + filePath + ".");
            info.exec();
        }
    }
}

//this method handles the button clicked event for exporting the project to a file to then show the frames on a raspberry
void MainWindow::on_bt_export_clicked()
{
    QMessageBox info;
    if (vectorOfFrames.size() == 0) {
        info.setText("Please add some Frames before saving or exporting.");
        info.exec();
        return;
    }
    //open dialog where to export
    QString preferredName = QDir::homePath() + QDir::separator() + "frames.txt";
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export Frame Project"), preferredName, tr("Text files (*.txt)"));
    if (filePath != "") {
        //export vector of frames to specific filePath
        bool success = exportFile(filePath);
        if (success) {
            info.setText("Successfully exported file to\n" + filePath + ".");
            info.exec();
        }
    }
}

//this method handles the button clicked event for adding a frame to the vector and listwidget
void MainWindow::on_bt_addFrame_clicked()
{
    QString nameOfFrame = ui->lineEdit_frameName->text() + " " + ui->lineEdit_frameName2->text();
    if (nameOfFrame == "") {
        QMessageBox msgBox;
        msgBox.setText("Could not add Frame.");
        msgBox.setInformativeText("Make sure to give your new frame a name.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

    //add new frame
    else {
        currentFrame.~Frame();
        new(&currentFrame) Frame(nameOfFrame, currentPinArray);

        //add new frame to vector
        vectorOfFrames.push_back(currentFrame);

        addFrameToListWidget(currentFrame.getName());

        //set new values
        numberOfFrames++;
        ui->lineEdit_frameName2->setText(QString::number(numberOfFrames));
        ui->lineEdit_totalAmount->setText(QString::number(vectorOfFrames.size()));
    }
}

//this method handles the button clicked event for deleting the selected frame
void MainWindow::on_bt_deleteFrame_clicked()
{
    //security check
    QMessageBox msgBox;
    msgBox.setText("Warning!");
    msgBox.setInformativeText("Are you sure you want to delete the selected frame?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    //delete frame
    if (msgBox.exec() == QMessageBox::Yes) {
        //get selected listWidget
        QList<QListWidgetItem*> framesToDelete = ui->listWidget->selectedItems();

        //loop through selected items, probably will only be one element
        foreach (QListWidgetItem* frameToDelete, framesToDelete) {
            QString frameName = frameToDelete->text();

            //delete out of vector
            auto it = findElementInVectorByName(frameName);
            if (it != vectorOfFrames.end()) {
                vectorOfFrames.erase(it);
            }

            //delete out of listWidget
            delete ui->listWidget->takeItem(ui->listWidget->row(frameToDelete));
            ui->lineEdit_totalAmount->setText(QString::number(vectorOfFrames.size()));
        }
    }
}

//this method handles the button clicked event for overwriting the selected frame with the current pinFrame
void MainWindow::on_bt_overwriteFrame_clicked()
{
    //security check
    QMessageBox msgBox;
    msgBox.setText("Warning!");
    msgBox.setInformativeText("Are you sure you want to overwrite the selected frame?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);

    //overwrite frame
    if (msgBox.exec() == QMessageBox::Yes) {
        //get selected listWidget
        QList<QListWidgetItem*> framesToOverwrite = ui->listWidget->selectedItems();

        //loop through selected items, probably will only be one element
        foreach (QListWidgetItem* frameToOverwrite, framesToOverwrite) {
            QString frameName = frameToOverwrite->text();

            currentFrame.~Frame();
            new(&currentFrame) Frame(frameName, currentPinArray);

            //point to different frame
            auto it = findElementInVectorByName(frameName);
            *(it) = currentFrame;
        }
    }
}

//this method handles the button clicked event for copying the selected frame
void MainWindow::on_bt_copyFrame_clicked()
{
    //get selected frame out of listview
    QList<QListWidgetItem*> framesToCopy = ui->listWidget->selectedItems();

    //loop through selected items, probably will only be one element
    foreach (QListWidgetItem* frameToCopy, framesToCopy) {
        QString frameNameToCopy = frameToCopy->text();
        QString newFrameName = ui->lineEdit_frameName->text() + " " + ui->lineEdit_frameName2->text();

        //create new frame with same pinFrame as selected frame
        auto it = findElementInVectorByName(frameNameToCopy);
        currentFrame.~Frame();
        new(&currentFrame) Frame(newFrameName, it->pinFrame);
        vectorOfFrames.push_back(currentFrame);

        addFrameToListWidget(newFrameName);

        numberOfFrames++;
        ui->lineEdit_frameName2->setText(QString::number(numberOfFrames));
        ui->lineEdit_totalAmount->setText(QString::number(vectorOfFrames.size()));
    }
}

//this method handles the button clicked event to activate preview mode
void MainWindow::on_bt_preview_clicked()
{
    if (vectorOfFrames.size() < 2)
        return;
    //starts preview mode, showing each frame with set delaytime
    PreviewFrames();
}

//this method activates preview mode, for a limited time nothing can be interacted with
void MainWindow::PreviewFrames() {
    ui->bt_preview->setText("Preview Mode (on)");
    //deactivate all edit buttons etc
    enableOrDisableEditButtons(false);
    int delayTime = (ui->lineEdit_delay->text()).toInt();
    int i = 0;

    //show all frames with delay
    for (auto it = vectorOfFrames.begin(); it != vectorOfFrames.end(); ++it) {
        ui->listWidget->setCurrentRow(i);
        i++;
        setPinFrameFromArray(it->pinFrame);
        delay(delayTime);
    }

    //preview mode off:
    ui->bt_preview->setText("Preview Mode (off)");
    //activate all buttons
    enableOrDisableEditButtons(true);

    if (ui->listWidget->model()->rowCount() > 0 && vectorOfFrames.size() > 0) {
        //select first frame
        currentFrame = *(vectorOfFrames.begin());
        ui->listWidget->setCurrentIndex(ui->listWidget->model()->index(0, 0));
        setPinFrameFromArray(vectorOfFrames.begin()->pinFrame);
    }
}

void MainWindow::addFrameToListWidget(QString frameName) {
    QListWidgetItem *newItem = new QListWidgetItem;
    newItem->setText(frameName);
    ui->listWidget->addItem(newItem);
}

//loops through vector and returns a iterator of type frame with the given name
std::vector<Frame>::iterator MainWindow::findElementInVectorByName(QString frameName) {

    for (auto it = vectorOfFrames.begin(); it != vectorOfFrames.end(); ++it) {
        if (it->getName() == frameName) {
            return it;
        }
    }
    return vectorOfFrames.end();
}

//add frame when (+) is pressed
void MainWindow::keyReleaseEvent(QKeyEvent* ke)
{
    //enter is pressed (Qt::Key_Enter does not work here)
    //if(ke->key() == 16777220)
    if (ke->text() == "+")
    {
        on_bt_addFrame_clicked();
        ke->accept();
        return;
    }
    else
        QMainWindow::keyPressEvent(ke); // handling of all other keys
}

//method changes the pinArray to the selected frame when widgetitem gets doubleclicked
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *frameToShow)
{
    QString frameName = frameToShow->text();

    //search in vector and set radioButtons
    auto it = findElementInVectorByName(frameName);
    if (it != vectorOfFrames.end()) {
        setPinFrameFromArray(it->pinFrame);
    }
}

//automatically sets frame name
void MainWindow::on_lineEdit_frameName_textChanged(const QString &arg1)
{
    ui->groupBox->setTitle(arg1 + " " + ui->lineEdit_frameName2->text());
}

//automatically sets frame name
void MainWindow::on_lineEdit_frameName2_textChanged(const QString &arg2)
{
    ui->groupBox->setTitle(ui->lineEdit_frameName->text() + " " + arg2);
}

//saves file to given path
bool MainWindow::saveFile(QString filePath) {
    //check if file already exists, if yes then delete
    QFileInfo check_file(filePath);
    if (check_file.exists() && check_file.isFile()) {
        QDir dir;
        dir.remove(filePath);
    }
    QFile file(filePath);
    qDebug() << "saving to" << filePath;

    //open file and write into
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << vectorOfFrames.size() << "\n";        //write amount of frames
        stream << ui->lineEdit_delay->text() << "\n";   //write delaytime

        //loop through vector
        for (auto it = vectorOfFrames.begin(); it != vectorOfFrames.end(); ++it) {
            stream << it->getName() << "\n";            //write frame name

            //copy pinFrame to myFrame
            bool myFrame[16][16];
            std::copy(&it->pinFrame[0][0], &it->pinFrame[0][0]+16*16, &myFrame[0][0]);

            //iterate through array and write all values
            for (int j = 0; j < 16; j++) {
                for (int k = 0; k < 16; k++) {
                    stream << myFrame[k][j] << " ";
                }
                stream << "\n";
            }
        }
    }
    file.close();
    return true;
}

//exports file to given path, using syntax for my python library (FrameViewer)
bool MainWindow::exportFile(QString fileName) {
    //check if file exists, if yes then delete
    QFileInfo check_file(fileName);
    if (check_file.exists() && check_file.isFile()) {
        QDir dir;
        dir.remove(fileName);
    }
    QFile file(fileName);
    qDebug() << "exporting to" << fileName;

    //open file and write into
    if (file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream(&file);
        //write delaytime
        stream << ui->lineEdit_delay->text() << "\n";

        //loop through map
        for (auto it = vectorOfFrames.begin(); it != vectorOfFrames.end(); ++it) {

            /* for my python library for raspberry pi i need an array for each matrix, so 4 arrays then packed into one array
             * i write one bit for each led, one byte represents one line of a matrix (8 led's)
             * 2x2 max7219 8x8LED matrixes:
             *
             *       16 ##########################
             *          #            #           #
             *          #            #           #
             *          #      1     #     2     #
             *          #            #           #
             *          #            #           #
             *        8 ##########################
             *          #            #           #
             *          #            #           #
             *        y #      3     #     4     #
             *        ^ #            #           #
             *        | #            #           #
             *        | ##########################
             *        0 --> x        8          16
            */

            //get current frame
            bool myFrame[16][16];
            std::copy(&it->pinFrame[0][0], &it->pinFrame[0][0]+16*16, &myFrame[0][0]);

            // iterate through frame in 4 segments, writing one matrix each

            // max7219 8x8LED matrix 1
            for (int x = 0; x < 8; x++) {
                stream << "0b";
                for (int y = 8; y < 16; y++) {
                    stream << myFrame[x][y];
                }
                if (x != 7)
                    stream << ",";

            }
            stream << " ";
            // max7219 8x8LED matrix 2
            for (int x = 8; x < 16; x++) {
                stream << "0b";
                for (int y = 8; y < 16; y++) {
                    stream << myFrame[x][y];
                }
                if (x != 15)
                    stream << ",";

            }
            stream << " ";
            // max7219 8x8LED matrix 3
            for (int x = 0; x < 8; x++) {
                stream << "0b";
                for (int y = 0; y < 8; y++) {
                    stream << myFrame[x][y];
                }
                if (x != 7)
                    stream << ",";

            }
            stream << " ";
            // max7219 8x8LED matrix 4
            for (int x = 8; x < 16; x++) {
                stream << "0b";
                for (int y = 0; y < 8; y++) {
                    stream << myFrame[x][y];
                }
                if (x != 15)
                    stream << ",";

            }
            stream << "\n";
        }
    }
    file.close();
    return true;
}

//loads or appends a .txt file that was saved with FrameCreator before
void MainWindow::loadFile(QString fileName, bool append) {
    qDebug() << "reading from" << fileName;
    QFile file(fileName);
    int framesAdded;

    if (!append) {
        vectorOfFrames.clear();
        ui->listWidget->clear();
        numberOfFrames = 1;
    }

    //open file and read out of it
    if (file.open(QIODevice::ReadWrite) )
    {
        QTextStream stream(&file);
        QString nameOfFrame;

        int amountOfFrames = 1;
        int lineNumber = 1;
        int arrayRow = 0;
        int x = 0;

        try {
            //read each line
            while (!stream.atEnd()) {
                QString line = stream.readLine();
                if (lineNumber == 1) {                  //line 1: amount of total frames
                    framesAdded = line.toInt();
                }
                else if (lineNumber == 2) {             //line 2: delaytime
                    ui->lineEdit_delay->setText(line);
                }
                else if ((lineNumber-3) % 17 == 0) {      //line 3, 20, 37, 54 etc. : name of first frame
                    if (lineNumber != 3) {
                        //create frame we looped through before, get name for new frame and add frame to vector
                        currentFrame.~Frame();
                        new(&currentFrame) Frame(nameOfFrame, currentPinArray);
                        vectorOfFrames.push_back(currentFrame);
                        amountOfFrames++;
                        arrayRow = 0;

                        //add frame name to widget
                        QListWidgetItem *newItem = new QListWidgetItem;
                        newItem->setText(currentFrame.getName());
                        ui->listWidget->addItem(newItem);
                    }

                    nameOfFrame = line;
                }
                else { //line contains row for pinArray
                    QStringList arrayLine = line.split(" ");
                    x = 0;
                    for (QStringList::iterator it = arrayLine.begin(); it != arrayLine.end(); ++it) {
                        QString valueAsString = *it;
                        bool value = valueAsString.toInt();
                        currentPinArray[x][arrayRow] = value;
                        x++;
                    }
                    arrayRow++;
                }
                lineNumber++;
            }

            //create last frame
            currentFrame.~Frame();
            new(&currentFrame) Frame(nameOfFrame, currentPinArray);
            vectorOfFrames.push_back(currentFrame);

            addFrameToListWidget(currentFrame.getName());
            numberOfFrames = vectorOfFrames.size()+1;
            ui->lineEdit_frameName2->setText(QString::number(numberOfFrames));
            ui->lineEdit_totalAmount->setText(QString::number(vectorOfFrames.size()));

            file.close();
        } catch(std::exception &e) {
            QMessageBox info;
            info.setText("The .txt file could not be parsed.\n");
            info.exec();
        }

    }

    QMessageBox msgBox;
    if (append)
        msgBox.setText("Successfully loaded your Project.\nYour opened Project has been expanded by " + QString::number(framesAdded) + " Frames.");
    else
        msgBox.setText("Successfully loaded your Project.\nIt contained " + QString::number(framesAdded) + " Frames.");
    msgBox.exec();


    //set first frame to radioButtons
    currentFrame = *(vectorOfFrames.begin());
    ui->listWidget->setCurrentIndex(ui->listWidget->model()->index(0, 0));
    setPinFrameFromArray(vectorOfFrames.begin()->pinFrame);
}

//method to pause the application (used for preview)
void MainWindow::delay(int millisecondsToWait)
{
    QTime dieTime = QTime::currentTime().addMSecs(millisecondsToWait);
    while(QTime::currentTime() < dieTime)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

//enables or disables all buttons to interact with the user interface
void MainWindow::enableOrDisableEditButtons(bool enable) {
    ui->lineEdit_delay->setEnabled(enable);
    ui->lineEdit_frameName->setEnabled(enable);
    ui->lineEdit_frameName2->setEnabled(enable);
    ui->lineEdit_totalAmount->setEnabled(enable);

    ui->bt_addFrame->setEnabled(enable);
    ui->bt_copyFrame->setEnabled(enable);
    ui->bt_deleteFrame->setEnabled(enable);
    ui->bt_deselectAll->setEnabled(enable);
    ui->bt_export->setEnabled(enable);
    ui->bt_InvertAll->setEnabled(enable);
    ui->bt_load->setEnabled(enable);
    ui->bt_new->setEnabled(enable);
    ui->bt_overwriteFrame->setEnabled(enable);
    ui->bt_preview->setEnabled(enable);
    ui->bt_save->setEnabled(enable);
    ui->bt_selectAll->setEnabled(enable);
    ui->bt_setRandom->setEnabled(enable);

    ui->bt_chooseRow1->setEnabled(enable);
    ui->bt_chooseRow2->setEnabled(enable);
    ui->bt_chooseRow3->setEnabled(enable);
    ui->bt_chooseRow4->setEnabled(enable);
    ui->bt_chooseRow5->setEnabled(enable);
    ui->bt_chooseRow6->setEnabled(enable);
    ui->bt_chooseRow7->setEnabled(enable);
    ui->bt_chooseRow8->setEnabled(enable);
    ui->bt_chooseRow9->setEnabled(enable);
    ui->bt_chooseRow10->setEnabled(enable);
    ui->bt_chooseRow11->setEnabled(enable);
    ui->bt_chooseRow12->setEnabled(enable);
    ui->bt_chooseRow13->setEnabled(enable);
    ui->bt_chooseRow14->setEnabled(enable);
    ui->bt_chooseRow15->setEnabled(enable);
    ui->bt_chooseRow16->setEnabled(enable);

    ui->bt_chooseColumn1->setEnabled(enable);
    ui->bt_chooseColumn2->setEnabled(enable);
    ui->bt_chooseColumn3->setEnabled(enable);
    ui->bt_chooseColumn4->setEnabled(enable);
    ui->bt_chooseColumn5->setEnabled(enable);
    ui->bt_chooseColumn6->setEnabled(enable);
    ui->bt_chooseColumn7->setEnabled(enable);
    ui->bt_chooseColumn8->setEnabled(enable);
    ui->bt_chooseColumn9->setEnabled(enable);
    ui->bt_chooseColumn10->setEnabled(enable);
    ui->bt_chooseColumn11->setEnabled(enable);
    ui->bt_chooseColumn12->setEnabled(enable);
    ui->bt_chooseColumn13->setEnabled(enable);
    ui->bt_chooseColumn14->setEnabled(enable);
    ui->bt_chooseColumn15->setEnabled(enable);
    ui->bt_chooseColumn16->setEnabled(enable);
}

//the following methods are for easier manipulation of radio buttons
void MainWindow::on_bt_selectAll_clicked()
{
    bool whatToDo = true;
    setChecked(whatToDo);
}

void MainWindow::on_bt_deselectAll_clicked()
{
    bool whatToDo = false;
    setChecked(whatToDo);
}

void MainWindow::setChecked(bool whatToDo) {
    ui->radioButton_1_1->setChecked(whatToDo);
    ui->radioButton_2_1->setChecked(whatToDo);
    ui->radioButton_3_1->setChecked(whatToDo);
    ui->radioButton_4_1->setChecked(whatToDo);
    ui->radioButton_5_1->setChecked(whatToDo);
    ui->radioButton_6_1->setChecked(whatToDo);
    ui->radioButton_7_1->setChecked(whatToDo);
    ui->radioButton_8_1->setChecked(whatToDo);
    ui->radioButton_9_1->setChecked(whatToDo);
    ui->radioButton_10_1->setChecked(whatToDo);
    ui->radioButton_11_1->setChecked(whatToDo);
    ui->radioButton_12_1->setChecked(whatToDo);
    ui->radioButton_13_1->setChecked(whatToDo);
    ui->radioButton_14_1->setChecked(whatToDo);
    ui->radioButton_15_1->setChecked(whatToDo);
    ui->radioButton_16_1->setChecked(whatToDo);

    ui->radioButton_1_2->setChecked(whatToDo);
    ui->radioButton_2_2->setChecked(whatToDo);
    ui->radioButton_3_2->setChecked(whatToDo);
    ui->radioButton_4_2->setChecked(whatToDo);
    ui->radioButton_5_2->setChecked(whatToDo);
    ui->radioButton_6_2->setChecked(whatToDo);
    ui->radioButton_7_2->setChecked(whatToDo);
    ui->radioButton_8_2->setChecked(whatToDo);
    ui->radioButton_9_2->setChecked(whatToDo);
    ui->radioButton_10_2->setChecked(whatToDo);
    ui->radioButton_11_2->setChecked(whatToDo);
    ui->radioButton_12_2->setChecked(whatToDo);
    ui->radioButton_13_2->setChecked(whatToDo);
    ui->radioButton_14_2->setChecked(whatToDo);
    ui->radioButton_15_2->setChecked(whatToDo);
    ui->radioButton_16_2->setChecked(whatToDo);

    ui->radioButton_1_3->setChecked(whatToDo);
    ui->radioButton_2_3->setChecked(whatToDo);
    ui->radioButton_3_3->setChecked(whatToDo);
    ui->radioButton_4_3->setChecked(whatToDo);
    ui->radioButton_5_3->setChecked(whatToDo);
    ui->radioButton_6_3->setChecked(whatToDo);
    ui->radioButton_7_3->setChecked(whatToDo);
    ui->radioButton_8_3->setChecked(whatToDo);
    ui->radioButton_9_3->setChecked(whatToDo);
    ui->radioButton_10_3->setChecked(whatToDo);
    ui->radioButton_11_3->setChecked(whatToDo);
    ui->radioButton_12_3->setChecked(whatToDo);
    ui->radioButton_13_3->setChecked(whatToDo);
    ui->radioButton_14_3->setChecked(whatToDo);
    ui->radioButton_15_3->setChecked(whatToDo);
    ui->radioButton_16_3->setChecked(whatToDo);

    ui->radioButton_1_4->setChecked(whatToDo);
    ui->radioButton_2_4->setChecked(whatToDo);
    ui->radioButton_3_4->setChecked(whatToDo);
    ui->radioButton_4_4->setChecked(whatToDo);
    ui->radioButton_5_4->setChecked(whatToDo);
    ui->radioButton_6_4->setChecked(whatToDo);
    ui->radioButton_7_4->setChecked(whatToDo);
    ui->radioButton_8_4->setChecked(whatToDo);
    ui->radioButton_9_4->setChecked(whatToDo);
    ui->radioButton_10_4->setChecked(whatToDo);
    ui->radioButton_11_4->setChecked(whatToDo);
    ui->radioButton_12_4->setChecked(whatToDo);
    ui->radioButton_13_4->setChecked(whatToDo);
    ui->radioButton_14_4->setChecked(whatToDo);
    ui->radioButton_15_4->setChecked(whatToDo);
    ui->radioButton_16_4->setChecked(whatToDo);

    ui->radioButton_1_5->setChecked(whatToDo);
    ui->radioButton_2_5->setChecked(whatToDo);
    ui->radioButton_3_5->setChecked(whatToDo);
    ui->radioButton_4_5->setChecked(whatToDo);
    ui->radioButton_5_5->setChecked(whatToDo);
    ui->radioButton_6_5->setChecked(whatToDo);
    ui->radioButton_7_5->setChecked(whatToDo);
    ui->radioButton_8_5->setChecked(whatToDo);
    ui->radioButton_9_5->setChecked(whatToDo);
    ui->radioButton_10_5->setChecked(whatToDo);
    ui->radioButton_11_5->setChecked(whatToDo);
    ui->radioButton_12_5->setChecked(whatToDo);
    ui->radioButton_13_5->setChecked(whatToDo);
    ui->radioButton_14_5->setChecked(whatToDo);
    ui->radioButton_15_5->setChecked(whatToDo);
    ui->radioButton_16_5->setChecked(whatToDo);

    ui->radioButton_1_6->setChecked(whatToDo);
    ui->radioButton_2_6->setChecked(whatToDo);
    ui->radioButton_3_6->setChecked(whatToDo);
    ui->radioButton_4_6->setChecked(whatToDo);
    ui->radioButton_5_6->setChecked(whatToDo);
    ui->radioButton_6_6->setChecked(whatToDo);
    ui->radioButton_7_6->setChecked(whatToDo);
    ui->radioButton_8_6->setChecked(whatToDo);
    ui->radioButton_9_6->setChecked(whatToDo);
    ui->radioButton_10_6->setChecked(whatToDo);
    ui->radioButton_11_6->setChecked(whatToDo);
    ui->radioButton_12_6->setChecked(whatToDo);
    ui->radioButton_13_6->setChecked(whatToDo);
    ui->radioButton_14_6->setChecked(whatToDo);
    ui->radioButton_15_6->setChecked(whatToDo);
    ui->radioButton_16_6->setChecked(whatToDo);

    ui->radioButton_1_7->setChecked(whatToDo);
    ui->radioButton_2_7->setChecked(whatToDo);
    ui->radioButton_3_7->setChecked(whatToDo);
    ui->radioButton_4_7->setChecked(whatToDo);
    ui->radioButton_5_7->setChecked(whatToDo);
    ui->radioButton_6_7->setChecked(whatToDo);
    ui->radioButton_7_7->setChecked(whatToDo);
    ui->radioButton_8_7->setChecked(whatToDo);
    ui->radioButton_9_7->setChecked(whatToDo);
    ui->radioButton_10_7->setChecked(whatToDo);
    ui->radioButton_11_7->setChecked(whatToDo);
    ui->radioButton_12_7->setChecked(whatToDo);
    ui->radioButton_13_7->setChecked(whatToDo);
    ui->radioButton_14_7->setChecked(whatToDo);
    ui->radioButton_15_7->setChecked(whatToDo);
    ui->radioButton_16_7->setChecked(whatToDo);

    ui->radioButton_1_8->setChecked(whatToDo);
    ui->radioButton_2_8->setChecked(whatToDo);
    ui->radioButton_3_8->setChecked(whatToDo);
    ui->radioButton_4_8->setChecked(whatToDo);
    ui->radioButton_5_8->setChecked(whatToDo);
    ui->radioButton_6_8->setChecked(whatToDo);
    ui->radioButton_7_8->setChecked(whatToDo);
    ui->radioButton_8_8->setChecked(whatToDo);
    ui->radioButton_9_8->setChecked(whatToDo);
    ui->radioButton_10_8->setChecked(whatToDo);
    ui->radioButton_11_8->setChecked(whatToDo);
    ui->radioButton_12_8->setChecked(whatToDo);
    ui->radioButton_13_8->setChecked(whatToDo);
    ui->radioButton_14_8->setChecked(whatToDo);
    ui->radioButton_15_8->setChecked(whatToDo);
    ui->radioButton_16_8->setChecked(whatToDo);

    ui->radioButton_1_9->setChecked(whatToDo);
    ui->radioButton_2_9->setChecked(whatToDo);
    ui->radioButton_3_9->setChecked(whatToDo);
    ui->radioButton_4_9->setChecked(whatToDo);
    ui->radioButton_5_9->setChecked(whatToDo);
    ui->radioButton_6_9->setChecked(whatToDo);
    ui->radioButton_7_9->setChecked(whatToDo);
    ui->radioButton_8_9->setChecked(whatToDo);
    ui->radioButton_9_9->setChecked(whatToDo);
    ui->radioButton_10_9->setChecked(whatToDo);
    ui->radioButton_11_9->setChecked(whatToDo);
    ui->radioButton_12_9->setChecked(whatToDo);
    ui->radioButton_13_9->setChecked(whatToDo);
    ui->radioButton_14_9->setChecked(whatToDo);
    ui->radioButton_15_9->setChecked(whatToDo);
    ui->radioButton_16_9->setChecked(whatToDo);

    ui->radioButton_1_10->setChecked(whatToDo);
    ui->radioButton_2_10->setChecked(whatToDo);
    ui->radioButton_3_10->setChecked(whatToDo);
    ui->radioButton_4_10->setChecked(whatToDo);
    ui->radioButton_5_10->setChecked(whatToDo);
    ui->radioButton_6_10->setChecked(whatToDo);
    ui->radioButton_7_10->setChecked(whatToDo);
    ui->radioButton_8_10->setChecked(whatToDo);
    ui->radioButton_9_10->setChecked(whatToDo);
    ui->radioButton_10_10->setChecked(whatToDo);
    ui->radioButton_11_10->setChecked(whatToDo);
    ui->radioButton_12_10->setChecked(whatToDo);
    ui->radioButton_13_10->setChecked(whatToDo);
    ui->radioButton_14_10->setChecked(whatToDo);
    ui->radioButton_15_10->setChecked(whatToDo);
    ui->radioButton_16_10->setChecked(whatToDo);

    ui->radioButton_1_11->setChecked(whatToDo);
    ui->radioButton_2_11->setChecked(whatToDo);
    ui->radioButton_3_11->setChecked(whatToDo);
    ui->radioButton_4_11->setChecked(whatToDo);
    ui->radioButton_5_11->setChecked(whatToDo);
    ui->radioButton_6_11->setChecked(whatToDo);
    ui->radioButton_7_11->setChecked(whatToDo);
    ui->radioButton_8_11->setChecked(whatToDo);
    ui->radioButton_9_11->setChecked(whatToDo);
    ui->radioButton_10_11->setChecked(whatToDo);
    ui->radioButton_11_11->setChecked(whatToDo);
    ui->radioButton_12_11->setChecked(whatToDo);
    ui->radioButton_13_11->setChecked(whatToDo);
    ui->radioButton_14_11->setChecked(whatToDo);
    ui->radioButton_15_11->setChecked(whatToDo);
    ui->radioButton_16_11->setChecked(whatToDo);

    ui->radioButton_1_12->setChecked(whatToDo);
    ui->radioButton_2_12->setChecked(whatToDo);
    ui->radioButton_3_12->setChecked(whatToDo);
    ui->radioButton_4_12->setChecked(whatToDo);
    ui->radioButton_5_12->setChecked(whatToDo);
    ui->radioButton_6_12->setChecked(whatToDo);
    ui->radioButton_7_12->setChecked(whatToDo);
    ui->radioButton_8_12->setChecked(whatToDo);
    ui->radioButton_9_12->setChecked(whatToDo);
    ui->radioButton_10_12->setChecked(whatToDo);
    ui->radioButton_11_12->setChecked(whatToDo);
    ui->radioButton_12_12->setChecked(whatToDo);
    ui->radioButton_13_12->setChecked(whatToDo);
    ui->radioButton_14_12->setChecked(whatToDo);
    ui->radioButton_15_12->setChecked(whatToDo);
    ui->radioButton_16_12->setChecked(whatToDo);

    ui->radioButton_1_13->setChecked(whatToDo);
    ui->radioButton_2_13->setChecked(whatToDo);
    ui->radioButton_3_13->setChecked(whatToDo);
    ui->radioButton_4_13->setChecked(whatToDo);
    ui->radioButton_5_13->setChecked(whatToDo);
    ui->radioButton_6_13->setChecked(whatToDo);
    ui->radioButton_7_13->setChecked(whatToDo);
    ui->radioButton_8_13->setChecked(whatToDo);
    ui->radioButton_9_13->setChecked(whatToDo);
    ui->radioButton_10_13->setChecked(whatToDo);
    ui->radioButton_11_13->setChecked(whatToDo);
    ui->radioButton_12_13->setChecked(whatToDo);
    ui->radioButton_13_13->setChecked(whatToDo);
    ui->radioButton_14_13->setChecked(whatToDo);
    ui->radioButton_15_13->setChecked(whatToDo);
    ui->radioButton_16_13->setChecked(whatToDo);

    ui->radioButton_1_14->setChecked(whatToDo);
    ui->radioButton_2_14->setChecked(whatToDo);
    ui->radioButton_3_14->setChecked(whatToDo);
    ui->radioButton_4_14->setChecked(whatToDo);
    ui->radioButton_5_14->setChecked(whatToDo);
    ui->radioButton_6_14->setChecked(whatToDo);
    ui->radioButton_7_14->setChecked(whatToDo);
    ui->radioButton_8_14->setChecked(whatToDo);
    ui->radioButton_9_14->setChecked(whatToDo);
    ui->radioButton_10_14->setChecked(whatToDo);
    ui->radioButton_11_14->setChecked(whatToDo);
    ui->radioButton_12_14->setChecked(whatToDo);
    ui->radioButton_13_14->setChecked(whatToDo);
    ui->radioButton_14_14->setChecked(whatToDo);
    ui->radioButton_15_14->setChecked(whatToDo);
    ui->radioButton_16_14->setChecked(whatToDo);

    ui->radioButton_1_15->setChecked(whatToDo);
    ui->radioButton_2_15->setChecked(whatToDo);
    ui->radioButton_3_15->setChecked(whatToDo);
    ui->radioButton_4_15->setChecked(whatToDo);
    ui->radioButton_5_15->setChecked(whatToDo);
    ui->radioButton_6_15->setChecked(whatToDo);
    ui->radioButton_7_15->setChecked(whatToDo);
    ui->radioButton_8_15->setChecked(whatToDo);
    ui->radioButton_9_15->setChecked(whatToDo);
    ui->radioButton_10_15->setChecked(whatToDo);
    ui->radioButton_11_15->setChecked(whatToDo);
    ui->radioButton_12_15->setChecked(whatToDo);
    ui->radioButton_13_15->setChecked(whatToDo);
    ui->radioButton_14_15->setChecked(whatToDo);
    ui->radioButton_15_15->setChecked(whatToDo);
    ui->radioButton_16_15->setChecked(whatToDo);

    ui->radioButton_1_16->setChecked(whatToDo);
    ui->radioButton_2_16->setChecked(whatToDo);
    ui->radioButton_3_16->setChecked(whatToDo);
    ui->radioButton_4_16->setChecked(whatToDo);
    ui->radioButton_5_16->setChecked(whatToDo);
    ui->radioButton_6_16->setChecked(whatToDo);
    ui->radioButton_7_16->setChecked(whatToDo);
    ui->radioButton_8_16->setChecked(whatToDo);
    ui->radioButton_9_16->setChecked(whatToDo);
    ui->radioButton_10_16->setChecked(whatToDo);
    ui->radioButton_11_16->setChecked(whatToDo);
    ui->radioButton_12_16->setChecked(whatToDo);
    ui->radioButton_13_16->setChecked(whatToDo);
    ui->radioButton_14_16->setChecked(whatToDo);
    ui->radioButton_15_16->setChecked(whatToDo);
    ui->radioButton_16_16->setChecked(whatToDo);
}

//inverts all radio buttons through calling all methods that switch radioButtons of a column
void MainWindow::on_bt_InvertAll_clicked()
{
    on_bt_chooseColumn1_clicked();
    on_bt_chooseColumn2_clicked();
    on_bt_chooseColumn3_clicked();
    on_bt_chooseColumn4_clicked();
    on_bt_chooseColumn5_clicked();
    on_bt_chooseColumn6_clicked();
    on_bt_chooseColumn7_clicked();
    on_bt_chooseColumn8_clicked();
    on_bt_chooseColumn9_clicked();
    on_bt_chooseColumn10_clicked();
    on_bt_chooseColumn11_clicked();
    on_bt_chooseColumn12_clicked();
    on_bt_chooseColumn13_clicked();
    on_bt_chooseColumn14_clicked();
    on_bt_chooseColumn15_clicked();
    on_bt_chooseColumn16_clicked();
}

//this method uses a twodimensional array to set all radio buttons to the right state
void MainWindow::setPinFrameFromArray(bool pinFrameArray[16][16]) {
    ui->radioButton_1_1->setChecked(pinFrameArray[1-1][1-1]);
    ui->radioButton_2_1->setChecked(pinFrameArray[2-1][1-1]);
    ui->radioButton_3_1->setChecked(pinFrameArray[3-1][1-1]);
    ui->radioButton_4_1->setChecked(pinFrameArray[4-1][1-1]);
    ui->radioButton_5_1->setChecked(pinFrameArray[5-1][1-1]);
    ui->radioButton_6_1->setChecked(pinFrameArray[6-1][1-1]);
    ui->radioButton_7_1->setChecked(pinFrameArray[7-1][1-1]);
    ui->radioButton_8_1->setChecked(pinFrameArray[8-1][1-1]);
    ui->radioButton_9_1->setChecked(pinFrameArray[9-1][1-1]);
    ui->radioButton_10_1->setChecked(pinFrameArray[10-1][1-1]);
    ui->radioButton_11_1->setChecked(pinFrameArray[11-1][1-1]);
    ui->radioButton_12_1->setChecked(pinFrameArray[12-1][1-1]);
    ui->radioButton_13_1->setChecked(pinFrameArray[13-1][1-1]);
    ui->radioButton_14_1->setChecked(pinFrameArray[14-1][1-1]);
    ui->radioButton_15_1->setChecked(pinFrameArray[15-1][1-1]);
    ui->radioButton_16_1->setChecked(pinFrameArray[16-1][1-1]);

    ui->radioButton_1_2->setChecked(pinFrameArray[1-1][2-1]);
    ui->radioButton_2_2->setChecked(pinFrameArray[2-1][2-1]);
    ui->radioButton_3_2->setChecked(pinFrameArray[3-1][2-1]);
    ui->radioButton_4_2->setChecked(pinFrameArray[4-1][2-1]);
    ui->radioButton_5_2->setChecked(pinFrameArray[5-1][2-1]);
    ui->radioButton_6_2->setChecked(pinFrameArray[6-1][2-1]);
    ui->radioButton_7_2->setChecked(pinFrameArray[7-1][2-1]);
    ui->radioButton_8_2->setChecked(pinFrameArray[8-1][2-1]);
    ui->radioButton_9_2->setChecked(pinFrameArray[9-1][2-1]);
    ui->radioButton_10_2->setChecked(pinFrameArray[10-1][2-1]);
    ui->radioButton_11_2->setChecked(pinFrameArray[11-1][2-1]);
    ui->radioButton_12_2->setChecked(pinFrameArray[12-1][2-1]);
    ui->radioButton_13_2->setChecked(pinFrameArray[13-1][2-1]);
    ui->radioButton_14_2->setChecked(pinFrameArray[14-1][2-1]);
    ui->radioButton_15_2->setChecked(pinFrameArray[15-1][2-1]);
    ui->radioButton_16_2->setChecked(pinFrameArray[16-1][2-1]);

    ui->radioButton_1_3->setChecked(pinFrameArray[1-1][3-1]);
    ui->radioButton_2_3->setChecked(pinFrameArray[2-1][3-1]);
    ui->radioButton_3_3->setChecked(pinFrameArray[3-1][3-1]);
    ui->radioButton_4_3->setChecked(pinFrameArray[4-1][3-1]);
    ui->radioButton_5_3->setChecked(pinFrameArray[5-1][3-1]);
    ui->radioButton_6_3->setChecked(pinFrameArray[6-1][3-1]);
    ui->radioButton_7_3->setChecked(pinFrameArray[7-1][3-1]);
    ui->radioButton_8_3->setChecked(pinFrameArray[8-1][3-1]);
    ui->radioButton_9_3->setChecked(pinFrameArray[9-1][3-1]);
    ui->radioButton_10_3->setChecked(pinFrameArray[10-1][3-1]);
    ui->radioButton_11_3->setChecked(pinFrameArray[11-1][3-1]);
    ui->radioButton_12_3->setChecked(pinFrameArray[12-1][3-1]);
    ui->radioButton_13_3->setChecked(pinFrameArray[13-1][3-1]);
    ui->radioButton_14_3->setChecked(pinFrameArray[14-1][3-1]);
    ui->radioButton_15_3->setChecked(pinFrameArray[15-1][3-1]);
    ui->radioButton_16_3->setChecked(pinFrameArray[16-1][3-1]);

    ui->radioButton_1_4->setChecked(pinFrameArray[1-1][4-1]);
    ui->radioButton_2_4->setChecked(pinFrameArray[2-1][4-1]);
    ui->radioButton_3_4->setChecked(pinFrameArray[3-1][4-1]);
    ui->radioButton_4_4->setChecked(pinFrameArray[4-1][4-1]);
    ui->radioButton_5_4->setChecked(pinFrameArray[5-1][4-1]);
    ui->radioButton_6_4->setChecked(pinFrameArray[6-1][4-1]);
    ui->radioButton_7_4->setChecked(pinFrameArray[7-1][4-1]);
    ui->radioButton_8_4->setChecked(pinFrameArray[8-1][4-1]);
    ui->radioButton_9_4->setChecked(pinFrameArray[9-1][4-1]);
    ui->radioButton_10_4->setChecked(pinFrameArray[10-1][4-1]);
    ui->radioButton_11_4->setChecked(pinFrameArray[11-1][4-1]);
    ui->radioButton_12_4->setChecked(pinFrameArray[12-1][4-1]);
    ui->radioButton_13_4->setChecked(pinFrameArray[13-1][4-1]);
    ui->radioButton_14_4->setChecked(pinFrameArray[14-1][4-1]);
    ui->radioButton_15_4->setChecked(pinFrameArray[15-1][4-1]);
    ui->radioButton_16_4->setChecked(pinFrameArray[16-1][4-1]);

    ui->radioButton_1_5->setChecked(pinFrameArray[1-1][5-1]);
    ui->radioButton_2_5->setChecked(pinFrameArray[2-1][5-1]);
    ui->radioButton_3_5->setChecked(pinFrameArray[3-1][5-1]);
    ui->radioButton_4_5->setChecked(pinFrameArray[4-1][5-1]);
    ui->radioButton_5_5->setChecked(pinFrameArray[5-1][5-1]);
    ui->radioButton_6_5->setChecked(pinFrameArray[6-1][5-1]);
    ui->radioButton_7_5->setChecked(pinFrameArray[7-1][5-1]);
    ui->radioButton_8_5->setChecked(pinFrameArray[8-1][5-1]);
    ui->radioButton_9_5->setChecked(pinFrameArray[9-1][5-1]);
    ui->radioButton_10_5->setChecked(pinFrameArray[10-1][5-1]);
    ui->radioButton_11_5->setChecked(pinFrameArray[11-1][5-1]);
    ui->radioButton_12_5->setChecked(pinFrameArray[12-1][5-1]);
    ui->radioButton_13_5->setChecked(pinFrameArray[13-1][5-1]);
    ui->radioButton_14_5->setChecked(pinFrameArray[14-1][5-1]);
    ui->radioButton_15_5->setChecked(pinFrameArray[15-1][5-1]);
    ui->radioButton_16_5->setChecked(pinFrameArray[16-1][5-1]);

    ui->radioButton_1_6->setChecked(pinFrameArray[1-1][6-1]);
    ui->radioButton_2_6->setChecked(pinFrameArray[2-1][6-1]);
    ui->radioButton_3_6->setChecked(pinFrameArray[3-1][6-1]);
    ui->radioButton_4_6->setChecked(pinFrameArray[4-1][6-1]);
    ui->radioButton_5_6->setChecked(pinFrameArray[5-1][6-1]);
    ui->radioButton_6_6->setChecked(pinFrameArray[6-1][6-1]);
    ui->radioButton_7_6->setChecked(pinFrameArray[7-1][6-1]);
    ui->radioButton_8_6->setChecked(pinFrameArray[8-1][6-1]);
    ui->radioButton_9_6->setChecked(pinFrameArray[9-1][6-1]);
    ui->radioButton_10_6->setChecked(pinFrameArray[10-1][6-1]);
    ui->radioButton_11_6->setChecked(pinFrameArray[11-1][6-1]);
    ui->radioButton_12_6->setChecked(pinFrameArray[12-1][6-1]);
    ui->radioButton_13_6->setChecked(pinFrameArray[13-1][6-1]);
    ui->radioButton_14_6->setChecked(pinFrameArray[14-1][6-1]);
    ui->radioButton_15_6->setChecked(pinFrameArray[15-1][6-1]);
    ui->radioButton_16_6->setChecked(pinFrameArray[16-1][6-1]);

    ui->radioButton_1_7->setChecked(pinFrameArray[1-1][7-1]);
    ui->radioButton_2_7->setChecked(pinFrameArray[2-1][7-1]);
    ui->radioButton_3_7->setChecked(pinFrameArray[3-1][7-1]);
    ui->radioButton_4_7->setChecked(pinFrameArray[4-1][7-1]);
    ui->radioButton_5_7->setChecked(pinFrameArray[5-1][7-1]);
    ui->radioButton_6_7->setChecked(pinFrameArray[6-1][7-1]);
    ui->radioButton_7_7->setChecked(pinFrameArray[7-1][7-1]);
    ui->radioButton_8_7->setChecked(pinFrameArray[8-1][7-1]);
    ui->radioButton_9_7->setChecked(pinFrameArray[9-1][7-1]);
    ui->radioButton_10_7->setChecked(pinFrameArray[10-1][7-1]);
    ui->radioButton_11_7->setChecked(pinFrameArray[11-1][7-1]);
    ui->radioButton_12_7->setChecked(pinFrameArray[12-1][7-1]);
    ui->radioButton_13_7->setChecked(pinFrameArray[13-1][7-1]);
    ui->radioButton_14_7->setChecked(pinFrameArray[14-1][7-1]);
    ui->radioButton_15_7->setChecked(pinFrameArray[15-1][7-1]);
    ui->radioButton_16_7->setChecked(pinFrameArray[16-1][7-1]);

    ui->radioButton_1_8->setChecked(pinFrameArray[1-1][8-1]);
    ui->radioButton_2_8->setChecked(pinFrameArray[2-1][8-1]);
    ui->radioButton_3_8->setChecked(pinFrameArray[3-1][8-1]);
    ui->radioButton_4_8->setChecked(pinFrameArray[4-1][8-1]);
    ui->radioButton_5_8->setChecked(pinFrameArray[5-1][8-1]);
    ui->radioButton_6_8->setChecked(pinFrameArray[6-1][8-1]);
    ui->radioButton_7_8->setChecked(pinFrameArray[7-1][8-1]);
    ui->radioButton_8_8->setChecked(pinFrameArray[8-1][8-1]);
    ui->radioButton_9_8->setChecked(pinFrameArray[9-1][8-1]);
    ui->radioButton_10_8->setChecked(pinFrameArray[10-1][8-1]);
    ui->radioButton_11_8->setChecked(pinFrameArray[11-1][8-1]);
    ui->radioButton_12_8->setChecked(pinFrameArray[12-1][8-1]);
    ui->radioButton_13_8->setChecked(pinFrameArray[13-1][8-1]);
    ui->radioButton_14_8->setChecked(pinFrameArray[14-1][8-1]);
    ui->radioButton_15_8->setChecked(pinFrameArray[15-1][8-1]);
    ui->radioButton_16_8->setChecked(pinFrameArray[16-1][8-1]);

    ui->radioButton_1_9->setChecked(pinFrameArray[1-1][9-1]);
    ui->radioButton_2_9->setChecked(pinFrameArray[2-1][9-1]);
    ui->radioButton_3_9->setChecked(pinFrameArray[3-1][9-1]);
    ui->radioButton_4_9->setChecked(pinFrameArray[4-1][9-1]);
    ui->radioButton_5_9->setChecked(pinFrameArray[5-1][9-1]);
    ui->radioButton_6_9->setChecked(pinFrameArray[6-1][9-1]);
    ui->radioButton_7_9->setChecked(pinFrameArray[7-1][9-1]);
    ui->radioButton_8_9->setChecked(pinFrameArray[8-1][9-1]);
    ui->radioButton_9_9->setChecked(pinFrameArray[9-1][9-1]);
    ui->radioButton_10_9->setChecked(pinFrameArray[10-1][9-1]);
    ui->radioButton_11_9->setChecked(pinFrameArray[11-1][9-1]);
    ui->radioButton_12_9->setChecked(pinFrameArray[12-1][9-1]);
    ui->radioButton_13_9->setChecked(pinFrameArray[13-1][9-1]);
    ui->radioButton_14_9->setChecked(pinFrameArray[14-1][9-1]);
    ui->radioButton_15_9->setChecked(pinFrameArray[15-1][9-1]);
    ui->radioButton_16_9->setChecked(pinFrameArray[16-1][9-1]);

    ui->radioButton_1_10->setChecked(pinFrameArray[1-1][10-1]);
    ui->radioButton_2_10->setChecked(pinFrameArray[2-1][10-1]);
    ui->radioButton_3_10->setChecked(pinFrameArray[3-1][10-1]);
    ui->radioButton_4_10->setChecked(pinFrameArray[4-1][10-1]);
    ui->radioButton_5_10->setChecked(pinFrameArray[5-1][10-1]);
    ui->radioButton_6_10->setChecked(pinFrameArray[6-1][10-1]);
    ui->radioButton_7_10->setChecked(pinFrameArray[7-1][10-1]);
    ui->radioButton_8_10->setChecked(pinFrameArray[8-1][10-1]);
    ui->radioButton_9_10->setChecked(pinFrameArray[9-1][10-1]);
    ui->radioButton_10_10->setChecked(pinFrameArray[10-1][10-1]);
    ui->radioButton_11_10->setChecked(pinFrameArray[11-1][10-1]);
    ui->radioButton_12_10->setChecked(pinFrameArray[12-1][10-1]);
    ui->radioButton_13_10->setChecked(pinFrameArray[13-1][10-1]);
    ui->radioButton_14_10->setChecked(pinFrameArray[14-1][10-1]);
    ui->radioButton_15_10->setChecked(pinFrameArray[15-1][10-1]);
    ui->radioButton_16_10->setChecked(pinFrameArray[16-1][10-1]);

    ui->radioButton_1_11->setChecked(pinFrameArray[1-1][11-1]);
    ui->radioButton_2_11->setChecked(pinFrameArray[2-1][11-1]);
    ui->radioButton_3_11->setChecked(pinFrameArray[3-1][11-1]);
    ui->radioButton_4_11->setChecked(pinFrameArray[4-1][11-1]);
    ui->radioButton_5_11->setChecked(pinFrameArray[5-1][11-1]);
    ui->radioButton_6_11->setChecked(pinFrameArray[6-1][11-1]);
    ui->radioButton_7_11->setChecked(pinFrameArray[7-1][11-1]);
    ui->radioButton_8_11->setChecked(pinFrameArray[8-1][11-1]);
    ui->radioButton_9_11->setChecked(pinFrameArray[9-1][11-1]);
    ui->radioButton_10_11->setChecked(pinFrameArray[10-1][11-1]);
    ui->radioButton_11_11->setChecked(pinFrameArray[11-1][11-1]);
    ui->radioButton_12_11->setChecked(pinFrameArray[12-1][11-1]);
    ui->radioButton_13_11->setChecked(pinFrameArray[13-1][11-1]);
    ui->radioButton_14_11->setChecked(pinFrameArray[14-1][11-1]);
    ui->radioButton_15_11->setChecked(pinFrameArray[15-1][11-1]);
    ui->radioButton_16_11->setChecked(pinFrameArray[16-1][11-1]);

    ui->radioButton_1_12->setChecked(pinFrameArray[1-1][12-1]);
    ui->radioButton_2_12->setChecked(pinFrameArray[2-1][12-1]);
    ui->radioButton_3_12->setChecked(pinFrameArray[3-1][12-1]);
    ui->radioButton_4_12->setChecked(pinFrameArray[4-1][12-1]);
    ui->radioButton_5_12->setChecked(pinFrameArray[5-1][12-1]);
    ui->radioButton_6_12->setChecked(pinFrameArray[6-1][12-1]);
    ui->radioButton_7_12->setChecked(pinFrameArray[7-1][12-1]);
    ui->radioButton_8_12->setChecked(pinFrameArray[8-1][12-1]);
    ui->radioButton_9_12->setChecked(pinFrameArray[9-1][12-1]);
    ui->radioButton_10_12->setChecked(pinFrameArray[10-1][12-1]);
    ui->radioButton_11_12->setChecked(pinFrameArray[11-1][12-1]);
    ui->radioButton_12_12->setChecked(pinFrameArray[12-1][12-1]);
    ui->radioButton_13_12->setChecked(pinFrameArray[13-1][12-1]);
    ui->radioButton_14_12->setChecked(pinFrameArray[14-1][12-1]);
    ui->radioButton_15_12->setChecked(pinFrameArray[15-1][12-1]);
    ui->radioButton_16_12->setChecked(pinFrameArray[16-1][12-1]);

    ui->radioButton_1_13->setChecked(pinFrameArray[1-1][13-1]);
    ui->radioButton_2_13->setChecked(pinFrameArray[2-1][13-1]);
    ui->radioButton_3_13->setChecked(pinFrameArray[3-1][13-1]);
    ui->radioButton_4_13->setChecked(pinFrameArray[4-1][13-1]);
    ui->radioButton_5_13->setChecked(pinFrameArray[5-1][13-1]);
    ui->radioButton_6_13->setChecked(pinFrameArray[6-1][13-1]);
    ui->radioButton_7_13->setChecked(pinFrameArray[7-1][13-1]);
    ui->radioButton_8_13->setChecked(pinFrameArray[8-1][13-1]);
    ui->radioButton_9_13->setChecked(pinFrameArray[9-1][13-1]);
    ui->radioButton_10_13->setChecked(pinFrameArray[10-1][13-1]);
    ui->radioButton_11_13->setChecked(pinFrameArray[11-1][13-1]);
    ui->radioButton_12_13->setChecked(pinFrameArray[12-1][13-1]);
    ui->radioButton_13_13->setChecked(pinFrameArray[13-1][13-1]);
    ui->radioButton_14_13->setChecked(pinFrameArray[14-1][13-1]);
    ui->radioButton_15_13->setChecked(pinFrameArray[15-1][13-1]);
    ui->radioButton_16_13->setChecked(pinFrameArray[16-1][13-1]);

    ui->radioButton_1_14->setChecked(pinFrameArray[1-1][14-1]);
    ui->radioButton_2_14->setChecked(pinFrameArray[2-1][14-1]);
    ui->radioButton_3_14->setChecked(pinFrameArray[3-1][14-1]);
    ui->radioButton_4_14->setChecked(pinFrameArray[4-1][14-1]);
    ui->radioButton_5_14->setChecked(pinFrameArray[5-1][14-1]);
    ui->radioButton_6_14->setChecked(pinFrameArray[6-1][14-1]);
    ui->radioButton_7_14->setChecked(pinFrameArray[7-1][14-1]);
    ui->radioButton_8_14->setChecked(pinFrameArray[8-1][14-1]);
    ui->radioButton_9_14->setChecked(pinFrameArray[9-1][14-1]);
    ui->radioButton_10_14->setChecked(pinFrameArray[10-1][14-1]);
    ui->radioButton_11_14->setChecked(pinFrameArray[11-1][14-1]);
    ui->radioButton_12_14->setChecked(pinFrameArray[12-1][14-1]);
    ui->radioButton_13_14->setChecked(pinFrameArray[13-1][14-1]);
    ui->radioButton_14_14->setChecked(pinFrameArray[14-1][14-1]);
    ui->radioButton_15_14->setChecked(pinFrameArray[15-1][14-1]);
    ui->radioButton_16_14->setChecked(pinFrameArray[16-1][14-1]);

    ui->radioButton_1_15->setChecked(pinFrameArray[1-1][15-1]);
    ui->radioButton_2_15->setChecked(pinFrameArray[2-1][15-1]);
    ui->radioButton_3_15->setChecked(pinFrameArray[3-1][15-1]);
    ui->radioButton_4_15->setChecked(pinFrameArray[4-1][15-1]);
    ui->radioButton_5_15->setChecked(pinFrameArray[5-1][15-1]);
    ui->radioButton_6_15->setChecked(pinFrameArray[6-1][15-1]);
    ui->radioButton_7_15->setChecked(pinFrameArray[7-1][15-1]);
    ui->radioButton_8_15->setChecked(pinFrameArray[8-1][15-1]);
    ui->radioButton_9_15->setChecked(pinFrameArray[9-1][15-1]);
    ui->radioButton_10_15->setChecked(pinFrameArray[10-1][15-1]);
    ui->radioButton_11_15->setChecked(pinFrameArray[11-1][15-1]);
    ui->radioButton_12_15->setChecked(pinFrameArray[12-1][15-1]);
    ui->radioButton_13_15->setChecked(pinFrameArray[13-1][15-1]);
    ui->radioButton_14_15->setChecked(pinFrameArray[14-1][15-1]);
    ui->radioButton_15_15->setChecked(pinFrameArray[15-1][15-1]);
    ui->radioButton_16_15->setChecked(pinFrameArray[16-1][15-1]);

    ui->radioButton_1_16->setChecked(pinFrameArray[1-1][16-1]);
    ui->radioButton_2_16->setChecked(pinFrameArray[2-1][16-1]);
    ui->radioButton_3_16->setChecked(pinFrameArray[3-1][16-1]);
    ui->radioButton_4_16->setChecked(pinFrameArray[4-1][16-1]);
    ui->radioButton_5_16->setChecked(pinFrameArray[5-1][16-1]);
    ui->radioButton_6_16->setChecked(pinFrameArray[6-1][16-1]);
    ui->radioButton_7_16->setChecked(pinFrameArray[7-1][16-1]);
    ui->radioButton_8_16->setChecked(pinFrameArray[8-1][16-1]);
    ui->radioButton_9_16->setChecked(pinFrameArray[9-1][16-1]);
    ui->radioButton_10_16->setChecked(pinFrameArray[10-1][16-1]);
    ui->radioButton_11_16->setChecked(pinFrameArray[11-1][16-1]);
    ui->radioButton_12_16->setChecked(pinFrameArray[12-1][16-1]);
    ui->radioButton_13_16->setChecked(pinFrameArray[13-1][16-1]);
    ui->radioButton_14_16->setChecked(pinFrameArray[14-1][16-1]);
    ui->radioButton_15_16->setChecked(pinFrameArray[15-1][16-1]);
    ui->radioButton_16_16->setChecked(pinFrameArray[16-1][16-1]);

}

//this method sets a random pattern to the radio buttons
void MainWindow::on_bt_setRandom_clicked()
{
    srand(time(0));
    int whatToDo = rand() % 2 == 0;
    ui->radioButton_1_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_2_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_3_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_4_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_5_1->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_1->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_1->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_1->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_1->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_1->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_2->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_2->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_2_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_3_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_4_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_5_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_6_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_7_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_8_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_9_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_10_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_3->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_4->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_4->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_5->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_5->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_2_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_3_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_4_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_5_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_6_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_7_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_8_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_9_6->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_10_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_11_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_12_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_13_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_14_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_15_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_16_6->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;

    ui->radioButton_1_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_7->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_7->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_2_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_3_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_4_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_5_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_6_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_7_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_8_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_9_8->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_10_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_11_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_12_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_13_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_14_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_15_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_16_8->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;

    ui->radioButton_1_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_2_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_3_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_4_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_5_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_6_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_7_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_8_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_9_9->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_10_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_11_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_12_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_13_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_14_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_15_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_16_9->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;

    ui->radioButton_1_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_10->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_10->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_2_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_3_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_4_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_5_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_6_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_7_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_8_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_9_11->setChecked(whatToDo);   whatToDo = rand() % 2 == 0;
    ui->radioButton_10_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_11_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_12_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_13_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_14_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_15_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_16_11->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;

    ui->radioButton_1_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_12->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_12->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_13->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_13->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_14->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_14->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_15->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_15->setChecked(whatToDo); whatToDo = rand() % 2 == 0;

    ui->radioButton_1_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_2_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_3_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_4_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_5_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_6_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_7_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_8_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_9_16->setChecked(whatToDo);  whatToDo = rand() % 2 == 0;
    ui->radioButton_10_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_11_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_12_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_13_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_14_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_15_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
    ui->radioButton_16_16->setChecked(whatToDo); whatToDo = rand() % 2 == 0;
}

//the following 32 methods toggle a complete row/column of radio buttons
void MainWindow::on_bt_chooseRow1_clicked()
{
    ui->radioButton_1_1->click();
    ui->radioButton_2_1->click();
    ui->radioButton_3_1->click();
    ui->radioButton_4_1->click();
    ui->radioButton_5_1->click();
    ui->radioButton_6_1->click();
    ui->radioButton_7_1->click();
    ui->radioButton_8_1->click();
    ui->radioButton_9_1->click();
    ui->radioButton_10_1->click();
    ui->radioButton_11_1->click();
    ui->radioButton_12_1->click();
    ui->radioButton_13_1->click();
    ui->radioButton_14_1->click();
    ui->radioButton_15_1->click();
    ui->radioButton_16_1->click();
}

void MainWindow::on_bt_chooseRow2_clicked()
{
    ui->radioButton_1_2->click();
    ui->radioButton_2_2->click();
    ui->radioButton_3_2->click();
    ui->radioButton_4_2->click();
    ui->radioButton_5_2->click();
    ui->radioButton_6_2->click();
    ui->radioButton_7_2->click();
    ui->radioButton_8_2->click();
    ui->radioButton_9_2->click();
    ui->radioButton_10_2->click();
    ui->radioButton_11_2->click();
    ui->radioButton_12_2->click();
    ui->radioButton_13_2->click();
    ui->radioButton_14_2->click();
    ui->radioButton_15_2->click();
    ui->radioButton_16_2->click();
}

void MainWindow::on_bt_chooseRow3_clicked()
{
    ui->radioButton_1_3->click();
    ui->radioButton_2_3->click();
    ui->radioButton_3_3->click();
    ui->radioButton_4_3->click();
    ui->radioButton_5_3->click();
    ui->radioButton_6_3->click();
    ui->radioButton_7_3->click();
    ui->radioButton_8_3->click();
    ui->radioButton_9_3->click();
    ui->radioButton_10_3->click();
    ui->radioButton_11_3->click();
    ui->radioButton_12_3->click();
    ui->radioButton_13_3->click();
    ui->radioButton_14_3->click();
    ui->radioButton_15_3->click();
    ui->radioButton_16_3->click();
}

void MainWindow::on_bt_chooseRow4_clicked()
{
    ui->radioButton_1_4->click();
    ui->radioButton_2_4->click();
    ui->radioButton_3_4->click();
    ui->radioButton_4_4->click();
    ui->radioButton_5_4->click();
    ui->radioButton_6_4->click();
    ui->radioButton_7_4->click();
    ui->radioButton_8_4->click();
    ui->radioButton_9_4->click();
    ui->radioButton_10_4->click();
    ui->radioButton_11_4->click();
    ui->radioButton_12_4->click();
    ui->radioButton_13_4->click();
    ui->radioButton_14_4->click();
    ui->radioButton_15_4->click();
    ui->radioButton_16_4->click();
}

void MainWindow::on_bt_chooseRow5_clicked()
{
    ui->radioButton_1_5->click();
    ui->radioButton_2_5->click();
    ui->radioButton_3_5->click();
    ui->radioButton_4_5->click();
    ui->radioButton_5_5->click();
    ui->radioButton_6_5->click();
    ui->radioButton_7_5->click();
    ui->radioButton_8_5->click();
    ui->radioButton_9_5->click();
    ui->radioButton_10_5->click();
    ui->radioButton_11_5->click();
    ui->radioButton_12_5->click();
    ui->radioButton_13_5->click();
    ui->radioButton_14_5->click();
    ui->radioButton_15_5->click();
    ui->radioButton_16_5->click();
}

void MainWindow::on_bt_chooseRow6_clicked()
{
    ui->radioButton_1_6->click();
    ui->radioButton_2_6->click();
    ui->radioButton_3_6->click();
    ui->radioButton_4_6->click();
    ui->radioButton_5_6->click();
    ui->radioButton_6_6->click();
    ui->radioButton_7_6->click();
    ui->radioButton_8_6->click();
    ui->radioButton_9_6->click();
    ui->radioButton_10_6->click();
    ui->radioButton_11_6->click();
    ui->radioButton_12_6->click();
    ui->radioButton_13_6->click();
    ui->radioButton_14_6->click();
    ui->radioButton_15_6->click();
    ui->radioButton_16_6->click();
}

void MainWindow::on_bt_chooseRow7_clicked()
{
    ui->radioButton_1_7->click();
    ui->radioButton_2_7->click();
    ui->radioButton_3_7->click();
    ui->radioButton_4_7->click();
    ui->radioButton_5_7->click();
    ui->radioButton_6_7->click();
    ui->radioButton_7_7->click();
    ui->radioButton_8_7->click();
    ui->radioButton_9_7->click();
    ui->radioButton_10_7->click();
    ui->radioButton_11_7->click();
    ui->radioButton_12_7->click();
    ui->radioButton_13_7->click();
    ui->radioButton_14_7->click();
    ui->radioButton_15_7->click();
    ui->radioButton_16_7->click();
}

void MainWindow::on_bt_chooseRow8_clicked()
{
    ui->radioButton_1_8->click();
    ui->radioButton_2_8->click();
    ui->radioButton_3_8->click();
    ui->radioButton_4_8->click();
    ui->radioButton_5_8->click();
    ui->radioButton_6_8->click();
    ui->radioButton_7_8->click();
    ui->radioButton_8_8->click();
    ui->radioButton_9_8->click();
    ui->radioButton_10_8->click();
    ui->radioButton_11_8->click();
    ui->radioButton_12_8->click();
    ui->radioButton_13_8->click();
    ui->radioButton_14_8->click();
    ui->radioButton_15_8->click();
    ui->radioButton_16_8->click();
}

void MainWindow::on_bt_chooseRow9_clicked()
{
    ui->radioButton_1_9->click();
    ui->radioButton_2_9->click();
    ui->radioButton_3_9->click();
    ui->radioButton_4_9->click();
    ui->radioButton_5_9->click();
    ui->radioButton_6_9->click();
    ui->radioButton_7_9->click();
    ui->radioButton_8_9->click();
    ui->radioButton_9_9->click();
    ui->radioButton_10_9->click();
    ui->radioButton_11_9->click();
    ui->radioButton_12_9->click();
    ui->radioButton_13_9->click();
    ui->radioButton_14_9->click();
    ui->radioButton_15_9->click();
    ui->radioButton_16_9->click();
}

void MainWindow::on_bt_chooseRow10_clicked()
{
    ui->radioButton_1_10->click();
    ui->radioButton_2_10->click();
    ui->radioButton_3_10->click();
    ui->radioButton_4_10->click();
    ui->radioButton_5_10->click();
    ui->radioButton_6_10->click();
    ui->radioButton_7_10->click();
    ui->radioButton_8_10->click();
    ui->radioButton_9_10->click();
    ui->radioButton_10_10->click();
    ui->radioButton_11_10->click();
    ui->radioButton_12_10->click();
    ui->radioButton_13_10->click();
    ui->radioButton_14_10->click();
    ui->radioButton_15_10->click();
    ui->radioButton_16_10->click();
}

void MainWindow::on_bt_chooseRow11_clicked()
{
    ui->radioButton_1_11->click();
    ui->radioButton_2_11->click();
    ui->radioButton_3_11->click();
    ui->radioButton_4_11->click();
    ui->radioButton_5_11->click();
    ui->radioButton_6_11->click();
    ui->radioButton_7_11->click();
    ui->radioButton_8_11->click();
    ui->radioButton_9_11->click();
    ui->radioButton_10_11->click();
    ui->radioButton_11_11->click();
    ui->radioButton_12_11->click();
    ui->radioButton_13_11->click();
    ui->radioButton_14_11->click();
    ui->radioButton_15_11->click();
    ui->radioButton_16_11->click();
}

void MainWindow::on_bt_chooseRow12_clicked()
{
    ui->radioButton_1_12->click();
    ui->radioButton_2_12->click();
    ui->radioButton_3_12->click();
    ui->radioButton_4_12->click();
    ui->radioButton_5_12->click();
    ui->radioButton_6_12->click();
    ui->radioButton_7_12->click();
    ui->radioButton_8_12->click();
    ui->radioButton_9_12->click();
    ui->radioButton_10_12->click();
    ui->radioButton_11_12->click();
    ui->radioButton_12_12->click();
    ui->radioButton_13_12->click();
    ui->radioButton_14_12->click();
    ui->radioButton_15_12->click();
    ui->radioButton_16_12->click();
}

void MainWindow::on_bt_chooseRow13_clicked()
{
    ui->radioButton_1_13->click();
    ui->radioButton_2_13->click();
    ui->radioButton_3_13->click();
    ui->radioButton_4_13->click();
    ui->radioButton_5_13->click();
    ui->radioButton_6_13->click();
    ui->radioButton_7_13->click();
    ui->radioButton_8_13->click();
    ui->radioButton_9_13->click();
    ui->radioButton_10_13->click();
    ui->radioButton_11_13->click();
    ui->radioButton_12_13->click();
    ui->radioButton_13_13->click();
    ui->radioButton_14_13->click();
    ui->radioButton_15_13->click();
    ui->radioButton_16_13->click();
}

void MainWindow::on_bt_chooseRow14_clicked()
{
    ui->radioButton_1_14->click();
    ui->radioButton_2_14->click();
    ui->radioButton_3_14->click();
    ui->radioButton_4_14->click();
    ui->radioButton_5_14->click();
    ui->radioButton_6_14->click();
    ui->radioButton_7_14->click();
    ui->radioButton_8_14->click();
    ui->radioButton_9_14->click();
    ui->radioButton_10_14->click();
    ui->radioButton_11_14->click();
    ui->radioButton_12_14->click();
    ui->radioButton_13_14->click();
    ui->radioButton_14_14->click();
    ui->radioButton_15_14->click();
    ui->radioButton_16_14->click();
}

void MainWindow::on_bt_chooseRow15_clicked()
{
    ui->radioButton_1_15->click();
    ui->radioButton_2_15->click();
    ui->radioButton_3_15->click();
    ui->radioButton_4_15->click();
    ui->radioButton_5_15->click();
    ui->radioButton_6_15->click();
    ui->radioButton_7_15->click();
    ui->radioButton_8_15->click();
    ui->radioButton_9_15->click();
    ui->radioButton_10_15->click();
    ui->radioButton_11_15->click();
    ui->radioButton_12_15->click();
    ui->radioButton_13_15->click();
    ui->radioButton_14_15->click();
    ui->radioButton_15_15->click();
    ui->radioButton_16_15->click();
}

void MainWindow::on_bt_chooseRow16_clicked()
{
    ui->radioButton_1_16->click();
    ui->radioButton_2_16->click();
    ui->radioButton_3_16->click();
    ui->radioButton_4_16->click();
    ui->radioButton_5_16->click();
    ui->radioButton_6_16->click();
    ui->radioButton_7_16->click();
    ui->radioButton_8_16->click();
    ui->radioButton_9_16->click();
    ui->radioButton_10_16->click();
    ui->radioButton_11_16->click();
    ui->radioButton_12_16->click();
    ui->radioButton_13_16->click();
    ui->radioButton_14_16->click();
    ui->radioButton_15_16->click();
    ui->radioButton_16_16->click();
}

void MainWindow::on_bt_chooseColumn1_clicked()
{
    ui->radioButton_1_1->click();
    ui->radioButton_1_2->click();
    ui->radioButton_1_3->click();
    ui->radioButton_1_4->click();
    ui->radioButton_1_5->click();
    ui->radioButton_1_6->click();
    ui->radioButton_1_7->click();
    ui->radioButton_1_8->click();
    ui->radioButton_1_9->click();
    ui->radioButton_1_10->click();
    ui->radioButton_1_11->click();
    ui->radioButton_1_12->click();
    ui->radioButton_1_13->click();
    ui->radioButton_1_14->click();
    ui->radioButton_1_15->click();
    ui->radioButton_1_16->click();
}

void MainWindow::on_bt_chooseColumn2_clicked()
{
    ui->radioButton_2_1->click();
    ui->radioButton_2_2->click();
    ui->radioButton_2_3->click();
    ui->radioButton_2_4->click();
    ui->radioButton_2_5->click();
    ui->radioButton_2_6->click();
    ui->radioButton_2_7->click();
    ui->radioButton_2_8->click();
    ui->radioButton_2_9->click();
    ui->radioButton_2_10->click();
    ui->radioButton_2_11->click();
    ui->radioButton_2_12->click();
    ui->radioButton_2_13->click();
    ui->radioButton_2_14->click();
    ui->radioButton_2_15->click();
    ui->radioButton_2_16->click();
}

void MainWindow::on_bt_chooseColumn3_clicked()
{
    ui->radioButton_3_1->click();
    ui->radioButton_3_2->click();
    ui->radioButton_3_3->click();
    ui->radioButton_3_4->click();
    ui->radioButton_3_5->click();
    ui->radioButton_3_6->click();
    ui->radioButton_3_7->click();
    ui->radioButton_3_8->click();
    ui->radioButton_3_9->click();
    ui->radioButton_3_10->click();
    ui->radioButton_3_11->click();
    ui->radioButton_3_12->click();
    ui->radioButton_3_13->click();
    ui->radioButton_3_14->click();
    ui->radioButton_3_15->click();
    ui->radioButton_3_16->click();
}

void MainWindow::on_bt_chooseColumn4_clicked()
{
    ui->radioButton_4_1->click();
    ui->radioButton_4_2->click();
    ui->radioButton_4_3->click();
    ui->radioButton_4_4->click();
    ui->radioButton_4_5->click();
    ui->radioButton_4_6->click();
    ui->radioButton_4_7->click();
    ui->radioButton_4_8->click();
    ui->radioButton_4_9->click();
    ui->radioButton_4_10->click();
    ui->radioButton_4_11->click();
    ui->radioButton_4_12->click();
    ui->radioButton_4_13->click();
    ui->radioButton_4_14->click();
    ui->radioButton_4_15->click();
    ui->radioButton_4_16->click();
}

void MainWindow::on_bt_chooseColumn5_clicked()
{
    ui->radioButton_5_1->click();
    ui->radioButton_5_2->click();
    ui->radioButton_5_3->click();
    ui->radioButton_5_4->click();
    ui->radioButton_5_5->click();
    ui->radioButton_5_6->click();
    ui->radioButton_5_7->click();
    ui->radioButton_5_8->click();
    ui->radioButton_5_9->click();
    ui->radioButton_5_10->click();
    ui->radioButton_5_11->click();
    ui->radioButton_5_12->click();
    ui->radioButton_5_13->click();
    ui->radioButton_5_14->click();
    ui->radioButton_5_15->click();
    ui->radioButton_5_16->click();
}

void MainWindow::on_bt_chooseColumn6_clicked()
{
    ui->radioButton_6_1->click();
    ui->radioButton_6_2->click();
    ui->radioButton_6_3->click();
    ui->radioButton_6_4->click();
    ui->radioButton_6_5->click();
    ui->radioButton_6_6->click();
    ui->radioButton_6_7->click();
    ui->radioButton_6_8->click();
    ui->radioButton_6_9->click();
    ui->radioButton_6_10->click();
    ui->radioButton_6_11->click();
    ui->radioButton_6_12->click();
    ui->radioButton_6_13->click();
    ui->radioButton_6_14->click();
    ui->radioButton_6_15->click();
    ui->radioButton_6_16->click();
}

void MainWindow::on_bt_chooseColumn7_clicked()
{
    ui->radioButton_7_1->click();
    ui->radioButton_7_2->click();
    ui->radioButton_7_3->click();
    ui->radioButton_7_4->click();
    ui->radioButton_7_5->click();
    ui->radioButton_7_6->click();
    ui->radioButton_7_7->click();
    ui->radioButton_7_8->click();
    ui->radioButton_7_9->click();
    ui->radioButton_7_10->click();
    ui->radioButton_7_11->click();
    ui->radioButton_7_12->click();
    ui->radioButton_7_13->click();
    ui->radioButton_7_14->click();
    ui->radioButton_7_15->click();
    ui->radioButton_7_16->click();
}

void MainWindow::on_bt_chooseColumn8_clicked()
{
    ui->radioButton_8_1->click();
    ui->radioButton_8_2->click();
    ui->radioButton_8_3->click();
    ui->radioButton_8_4->click();
    ui->radioButton_8_5->click();
    ui->radioButton_8_6->click();
    ui->radioButton_8_7->click();
    ui->radioButton_8_8->click();
    ui->radioButton_8_9->click();
    ui->radioButton_8_10->click();
    ui->radioButton_8_11->click();
    ui->radioButton_8_12->click();
    ui->radioButton_8_13->click();
    ui->radioButton_8_14->click();
    ui->radioButton_8_15->click();
    ui->radioButton_8_16->click();
}

void MainWindow::on_bt_chooseColumn9_clicked()
{
    ui->radioButton_9_1->click();
    ui->radioButton_9_2->click();
    ui->radioButton_9_3->click();
    ui->radioButton_9_4->click();
    ui->radioButton_9_5->click();
    ui->radioButton_9_6->click();
    ui->radioButton_9_7->click();
    ui->radioButton_9_8->click();
    ui->radioButton_9_9->click();
    ui->radioButton_9_10->click();
    ui->radioButton_9_11->click();
    ui->radioButton_9_12->click();
    ui->radioButton_9_13->click();
    ui->radioButton_9_14->click();
    ui->radioButton_9_15->click();
    ui->radioButton_9_16->click();
}

void MainWindow::on_bt_chooseColumn10_clicked()
{
    ui->radioButton_10_1->click();
    ui->radioButton_10_2->click();
    ui->radioButton_10_3->click();
    ui->radioButton_10_4->click();
    ui->radioButton_10_5->click();
    ui->radioButton_10_6->click();
    ui->radioButton_10_7->click();
    ui->radioButton_10_8->click();
    ui->radioButton_10_9->click();
    ui->radioButton_10_10->click();
    ui->radioButton_10_11->click();
    ui->radioButton_10_12->click();
    ui->radioButton_10_13->click();
    ui->radioButton_10_14->click();
    ui->radioButton_10_15->click();
    ui->radioButton_10_16->click();
}

void MainWindow::on_bt_chooseColumn11_clicked()
{
    ui->radioButton_11_1->click();
    ui->radioButton_11_2->click();
    ui->radioButton_11_3->click();
    ui->radioButton_11_4->click();
    ui->radioButton_11_5->click();
    ui->radioButton_11_6->click();
    ui->radioButton_11_7->click();
    ui->radioButton_11_8->click();
    ui->radioButton_11_9->click();
    ui->radioButton_11_10->click();
    ui->radioButton_11_11->click();
    ui->radioButton_11_12->click();
    ui->radioButton_11_13->click();
    ui->radioButton_11_14->click();
    ui->radioButton_11_15->click();
    ui->radioButton_11_16->click();
}

void MainWindow::on_bt_chooseColumn12_clicked()
{
    ui->radioButton_12_1->click();
    ui->radioButton_12_2->click();
    ui->radioButton_12_3->click();
    ui->radioButton_12_4->click();
    ui->radioButton_12_5->click();
    ui->radioButton_12_6->click();
    ui->radioButton_12_7->click();
    ui->radioButton_12_8->click();
    ui->radioButton_12_9->click();
    ui->radioButton_12_10->click();
    ui->radioButton_12_11->click();
    ui->radioButton_12_12->click();
    ui->radioButton_12_13->click();
    ui->radioButton_12_14->click();
    ui->radioButton_12_15->click();
    ui->radioButton_12_16->click();
}

void MainWindow::on_bt_chooseColumn13_clicked()
{
    ui->radioButton_13_1->click();
    ui->radioButton_13_2->click();
    ui->radioButton_13_3->click();
    ui->radioButton_13_4->click();
    ui->radioButton_13_5->click();
    ui->radioButton_13_6->click();
    ui->radioButton_13_7->click();
    ui->radioButton_13_8->click();
    ui->radioButton_13_9->click();
    ui->radioButton_13_10->click();
    ui->radioButton_13_11->click();
    ui->radioButton_13_12->click();
    ui->radioButton_13_13->click();
    ui->radioButton_13_14->click();
    ui->radioButton_13_15->click();
    ui->radioButton_13_16->click();
}

void MainWindow::on_bt_chooseColumn14_clicked()
{
    ui->radioButton_14_1->click();
    ui->radioButton_14_2->click();
    ui->radioButton_14_3->click();
    ui->radioButton_14_4->click();
    ui->radioButton_14_5->click();
    ui->radioButton_14_6->click();
    ui->radioButton_14_7->click();
    ui->radioButton_14_8->click();
    ui->radioButton_14_9->click();
    ui->radioButton_14_10->click();
    ui->radioButton_14_11->click();
    ui->radioButton_14_12->click();
    ui->radioButton_14_13->click();
    ui->radioButton_14_14->click();
    ui->radioButton_14_15->click();
    ui->radioButton_14_16->click();
}

void MainWindow::on_bt_chooseColumn15_clicked()
{
    ui->radioButton_15_1->click();
    ui->radioButton_15_2->click();
    ui->radioButton_15_3->click();
    ui->radioButton_15_4->click();
    ui->radioButton_15_5->click();
    ui->radioButton_15_6->click();
    ui->radioButton_15_7->click();
    ui->radioButton_15_8->click();
    ui->radioButton_15_9->click();
    ui->radioButton_15_10->click();
    ui->radioButton_15_11->click();
    ui->radioButton_15_12->click();
    ui->radioButton_15_13->click();
    ui->radioButton_15_14->click();
    ui->radioButton_15_15->click();
    ui->radioButton_15_16->click();
}

void MainWindow::on_bt_chooseColumn16_clicked()
{
    ui->radioButton_16_1->click();
    ui->radioButton_16_2->click();
    ui->radioButton_16_3->click();
    ui->radioButton_16_4->click();
    ui->radioButton_16_5->click();
    ui->radioButton_16_6->click();
    ui->radioButton_16_7->click();
    ui->radioButton_16_8->click();
    ui->radioButton_16_9->click();
    ui->radioButton_16_10->click();
    ui->radioButton_16_11->click();
    ui->radioButton_16_12->click();
    ui->radioButton_16_13->click();
    ui->radioButton_16_14->click();
    ui->radioButton_16_15->click();
    ui->radioButton_16_16->click();
}

//for following 256 methods:
//if a status of a radio button gets toggled the belonging field in the twodimensional array gets toggled as well
void MainWindow::on_radioButton_1_1_toggled(bool checked)
{
    currentPinArray[1-1][1-1] = checked;
}

void MainWindow::on_radioButton_1_2_toggled(bool checked)
{
    currentPinArray[1-1][2-1] = checked;
}

void MainWindow::on_radioButton_1_3_toggled(bool checked)
{
    currentPinArray[1-1][3-1] = checked;
}

void MainWindow::on_radioButton_1_4_toggled(bool checked)
{
    currentPinArray[1-1][4-1] = checked;
}

void MainWindow::on_radioButton_1_5_toggled(bool checked)
{
    currentPinArray[1-1][5-1] = checked;
}

void MainWindow::on_radioButton_1_6_toggled(bool checked)
{
    currentPinArray[1-1][6-1] = checked;
}

void MainWindow::on_radioButton_1_7_toggled(bool checked)
{
    currentPinArray[1-1][7-1] = checked;
}

void MainWindow::on_radioButton_1_8_toggled(bool checked)
{
    currentPinArray[1-1][8-1] = checked;
}

void MainWindow::on_radioButton_1_9_toggled(bool checked)
{
    currentPinArray[1-1][9-1] = checked;
}

void MainWindow::on_radioButton_1_10_toggled(bool checked)
{
    currentPinArray[1-1][10-1] = checked;
}

void MainWindow::on_radioButton_1_11_toggled(bool checked)
{
    currentPinArray[1-1][11-1] = checked;
}

void MainWindow::on_radioButton_1_12_toggled(bool checked)
{
    currentPinArray[1-1][12-1] = checked;
}

void MainWindow::on_radioButton_1_13_toggled(bool checked)
{
    currentPinArray[1-1][13-1] = checked;
}

void MainWindow::on_radioButton_1_14_toggled(bool checked)
{
    currentPinArray[1-1][14-1] = checked;
}

void MainWindow::on_radioButton_1_15_toggled(bool checked)
{
    currentPinArray[1-1][15-1] = checked;
}

void MainWindow::on_radioButton_1_16_toggled(bool checked)
{
    currentPinArray[1-1][16-1] = checked;
}

void MainWindow::on_radioButton_2_1_toggled(bool checked)
{
    currentPinArray[2-1][1-1] = checked;
}

void MainWindow::on_radioButton_2_2_toggled(bool checked)
{
    currentPinArray[2-1][2-1] = checked;
}

void MainWindow::on_radioButton_2_3_toggled(bool checked)
{
    currentPinArray[2-1][3-1] = checked;
}

void MainWindow::on_radioButton_2_4_toggled(bool checked)
{
    currentPinArray[2-1][4-1] = checked;
}

void MainWindow::on_radioButton_2_5_toggled(bool checked)
{
    currentPinArray[2-1][5-1] = checked;
}

void MainWindow::on_radioButton_2_6_toggled(bool checked)
{
    currentPinArray[2-1][6-1] = checked;
}

void MainWindow::on_radioButton_2_7_toggled(bool checked)
{
    currentPinArray[2-1][7-1] = checked;
}

void MainWindow::on_radioButton_2_8_toggled(bool checked)
{
    currentPinArray[2-1][8-1] = checked;
}

void MainWindow::on_radioButton_2_9_toggled(bool checked)
{
    currentPinArray[2-1][9-1] = checked;
}

void MainWindow::on_radioButton_2_10_toggled(bool checked)
{
    currentPinArray[2-1][10-1] = checked;
}

void MainWindow::on_radioButton_2_11_toggled(bool checked)
{
    currentPinArray[2-1][11-1] = checked;
}

void MainWindow::on_radioButton_2_12_toggled(bool checked)
{
    currentPinArray[2-1][12-1] = checked;
}

void MainWindow::on_radioButton_2_13_toggled(bool checked)
{
    currentPinArray[2-1][13-1] = checked;
}

void MainWindow::on_radioButton_2_14_toggled(bool checked)
{
    currentPinArray[2-1][14-1] = checked;
}

void MainWindow::on_radioButton_2_15_toggled(bool checked)
{
    currentPinArray[2-1][15-1] = checked;
}

void MainWindow::on_radioButton_2_16_toggled(bool checked)
{
    currentPinArray[2-1][16-1] = checked;
}

void MainWindow::on_radioButton_3_1_toggled(bool checked)
{
    currentPinArray[3-1][1-1] = checked;
}

void MainWindow::on_radioButton_3_2_toggled(bool checked)
{
    currentPinArray[3-1][2-1] = checked;
}

void MainWindow::on_radioButton_3_3_toggled(bool checked)
{
    currentPinArray[3-1][3-1] = checked;
}

void MainWindow::on_radioButton_3_4_toggled(bool checked)
{
    currentPinArray[3-1][4-1] = checked;
}

void MainWindow::on_radioButton_3_5_toggled(bool checked)
{
    currentPinArray[3-1][5-1] = checked;
}

void MainWindow::on_radioButton_3_6_toggled(bool checked)
{
    currentPinArray[3-1][6-1] = checked;
}

void MainWindow::on_radioButton_3_7_toggled(bool checked)
{
    currentPinArray[3-1][7-1] = checked;
}

void MainWindow::on_radioButton_3_8_toggled(bool checked)
{
    currentPinArray[3-1][8-1] = checked;
}

void MainWindow::on_radioButton_3_9_toggled(bool checked)
{
    currentPinArray[3-1][9-1] = checked;
}

void MainWindow::on_radioButton_3_10_toggled(bool checked)
{
    currentPinArray[3-1][10-1] = checked;
}

void MainWindow::on_radioButton_3_11_toggled(bool checked)
{
    currentPinArray[3-1][11-1] = checked;
}

void MainWindow::on_radioButton_3_12_toggled(bool checked)
{
    currentPinArray[3-1][12-1] = checked;
}

void MainWindow::on_radioButton_3_13_toggled(bool checked)
{
    currentPinArray[3-1][13-1] = checked;
}

void MainWindow::on_radioButton_3_14_toggled(bool checked)
{
    currentPinArray[3-1][14-1] = checked;
}

void MainWindow::on_radioButton_3_15_toggled(bool checked)
{
    currentPinArray[3-1][15-1] = checked;
}

void MainWindow::on_radioButton_3_16_toggled(bool checked)
{
    currentPinArray[3-1][16-1] = checked;
}

void MainWindow::on_radioButton_4_1_toggled(bool checked)
{
    currentPinArray[4-1][1-1] = checked;
}

void MainWindow::on_radioButton_4_2_toggled(bool checked)
{
    currentPinArray[4-1][2-1] = checked;
}

void MainWindow::on_radioButton_4_3_toggled(bool checked)
{
    currentPinArray[4-1][3-1] = checked;
}

void MainWindow::on_radioButton_4_4_toggled(bool checked)
{
    currentPinArray[4-1][4-1] = checked;
}

void MainWindow::on_radioButton_4_5_toggled(bool checked)
{
    currentPinArray[4-1][5-1] = checked;
}

void MainWindow::on_radioButton_4_6_toggled(bool checked)
{
    currentPinArray[4-1][6-1] = checked;
}

void MainWindow::on_radioButton_4_7_toggled(bool checked)
{
    currentPinArray[4-1][7-1] = checked;
}

void MainWindow::on_radioButton_4_8_toggled(bool checked)
{
    currentPinArray[4-1][8-1] = checked;
}

void MainWindow::on_radioButton_4_9_toggled(bool checked)
{
    currentPinArray[4-1][9-1] = checked;
}

void MainWindow::on_radioButton_4_10_toggled(bool checked)
{
    currentPinArray[4-1][10-1] = checked;
}

void MainWindow::on_radioButton_4_11_toggled(bool checked)
{
    currentPinArray[4-1][11-1] = checked;
}

void MainWindow::on_radioButton_4_12_toggled(bool checked)
{
    currentPinArray[4-1][12-1] = checked;
}

void MainWindow::on_radioButton_4_13_toggled(bool checked)
{
    currentPinArray[4-1][13-1] = checked;
}

void MainWindow::on_radioButton_4_14_toggled(bool checked)
{
    currentPinArray[4-1][14-1] = checked;
}

void MainWindow::on_radioButton_4_15_toggled(bool checked)
{
    currentPinArray[4-1][15-1] = checked;
}

void MainWindow::on_radioButton_4_16_toggled(bool checked)
{
    currentPinArray[4-1][16-1] = checked;
}

void MainWindow::on_radioButton_5_1_toggled(bool checked)
{
    currentPinArray[5-1][1-1] = checked;
}

void MainWindow::on_radioButton_5_2_toggled(bool checked)
{
    currentPinArray[5-1][2-1] = checked;
}

void MainWindow::on_radioButton_5_3_toggled(bool checked)
{
    currentPinArray[5-1][3-1] = checked;
}

void MainWindow::on_radioButton_5_4_toggled(bool checked)
{
    currentPinArray[5-1][4-1] = checked;
}

void MainWindow::on_radioButton_5_5_toggled(bool checked)
{
    currentPinArray[5-1][5-1] = checked;
}

void MainWindow::on_radioButton_5_6_toggled(bool checked)
{
    currentPinArray[5-1][6-1] = checked;
}

void MainWindow::on_radioButton_5_7_toggled(bool checked)
{
    currentPinArray[5-1][7-1] = checked;
}

void MainWindow::on_radioButton_5_8_toggled(bool checked)
{
    currentPinArray[5-1][8-1] = checked;
}

void MainWindow::on_radioButton_5_9_toggled(bool checked)
{
    currentPinArray[5-1][9-1] = checked;
}

void MainWindow::on_radioButton_5_10_toggled(bool checked)
{
    currentPinArray[5-1][10-1] = checked;
}

void MainWindow::on_radioButton_5_11_toggled(bool checked)
{
    currentPinArray[5-1][11-1] = checked;
}

void MainWindow::on_radioButton_5_12_toggled(bool checked)
{
    currentPinArray[5-1][12-1] = checked;
}

void MainWindow::on_radioButton_5_13_toggled(bool checked)
{
    currentPinArray[5-1][13-1] = checked;
}

void MainWindow::on_radioButton_5_14_toggled(bool checked)
{
    currentPinArray[5-1][14-1] = checked;
}

void MainWindow::on_radioButton_5_15_toggled(bool checked)
{
    currentPinArray[5-1][15-1] = checked;
}

void MainWindow::on_radioButton_5_16_toggled(bool checked)
{
    currentPinArray[5-1][16-1] = checked;
}

void MainWindow::on_radioButton_6_1_toggled(bool checked)
{
    currentPinArray[6-1][1-1] = checked;
}

void MainWindow::on_radioButton_6_2_toggled(bool checked)
{
    currentPinArray[6-1][2-1] = checked;
}

void MainWindow::on_radioButton_6_3_toggled(bool checked)
{
    currentPinArray[6-1][3-1] = checked;
}

void MainWindow::on_radioButton_6_4_toggled(bool checked)
{
    currentPinArray[6-1][4-1] = checked;
}

void MainWindow::on_radioButton_6_5_toggled(bool checked)
{
    currentPinArray[6-1][5-1] = checked;
}

void MainWindow::on_radioButton_6_6_toggled(bool checked)
{
    currentPinArray[6-1][6-1] = checked;
}

void MainWindow::on_radioButton_6_7_toggled(bool checked)
{
    currentPinArray[6-1][7-1] = checked;
}

void MainWindow::on_radioButton_6_8_toggled(bool checked)
{
    currentPinArray[6-1][8-1] = checked;
}

void MainWindow::on_radioButton_6_9_toggled(bool checked)
{
    currentPinArray[6-1][9-1] = checked;
}

void MainWindow::on_radioButton_6_10_toggled(bool checked)
{
    currentPinArray[6-1][10-1] = checked;
}

void MainWindow::on_radioButton_6_11_toggled(bool checked)
{
    currentPinArray[6-1][11-1] = checked;
}

void MainWindow::on_radioButton_6_12_toggled(bool checked)
{
    currentPinArray[6-1][12-1] = checked;
}

void MainWindow::on_radioButton_6_13_toggled(bool checked)
{
    currentPinArray[6-1][13-1] = checked;
}

void MainWindow::on_radioButton_6_14_toggled(bool checked)
{
    currentPinArray[6-1][14-1] = checked;
}

void MainWindow::on_radioButton_6_15_toggled(bool checked)
{
    currentPinArray[6-1][15-1] = checked;
}

void MainWindow::on_radioButton_6_16_toggled(bool checked)
{
    currentPinArray[6-1][16-1] = checked;
}

void MainWindow::on_radioButton_7_1_toggled(bool checked)
{
    currentPinArray[7-1][1-1] = checked;
}

void MainWindow::on_radioButton_7_2_toggled(bool checked)
{
    currentPinArray[7-1][2-1] = checked;
}

void MainWindow::on_radioButton_7_3_toggled(bool checked)
{
    currentPinArray[7-1][3-1] = checked;
}

void MainWindow::on_radioButton_7_4_toggled(bool checked)
{
    currentPinArray[7-1][4-1] = checked;
}

void MainWindow::on_radioButton_7_5_toggled(bool checked)
{
    currentPinArray[7-1][5-1] = checked;
}

void MainWindow::on_radioButton_7_6_toggled(bool checked)
{
    currentPinArray[7-1][6-1] = checked;
}


void MainWindow::on_radioButton_7_7_toggled(bool checked)
{
    currentPinArray[7-1][7-1] = checked;
}

void MainWindow::on_radioButton_7_8_toggled(bool checked)
{
    currentPinArray[7-1][8-1] = checked;
}

void MainWindow::on_radioButton_7_9_toggled(bool checked)
{
    currentPinArray[7-1][9-1] = checked;
}

void MainWindow::on_radioButton_7_10_toggled(bool checked)
{
    currentPinArray[7-1][10-1] = checked;
}

void MainWindow::on_radioButton_7_11_toggled(bool checked)
{
    currentPinArray[7-1][11-1] = checked;
}

void MainWindow::on_radioButton_7_12_toggled(bool checked)
{
    currentPinArray[7-1][12-1] = checked;
}

void MainWindow::on_radioButton_7_13_toggled(bool checked)
{
    currentPinArray[7-1][13-1] = checked;
}

void MainWindow::on_radioButton_7_14_toggled(bool checked)
{
    currentPinArray[7-1][14-1] = checked;
}

void MainWindow::on_radioButton_7_15_toggled(bool checked)
{
    currentPinArray[7-1][15-1] = checked;
}

void MainWindow::on_radioButton_7_16_toggled(bool checked)
{
    currentPinArray[7-1][16-1] = checked;
}

void MainWindow::on_radioButton_8_1_toggled(bool checked)
{
    currentPinArray[8-1][1-1] = checked;
}

void MainWindow::on_radioButton_8_2_toggled(bool checked)
{
    currentPinArray[8-1][2-1] = checked;
}

void MainWindow::on_radioButton_8_3_toggled(bool checked)
{
    currentPinArray[8-1][3-1] = checked;
}

void MainWindow::on_radioButton_8_4_toggled(bool checked)
{
    currentPinArray[8-1][4-1] = checked;
}

void MainWindow::on_radioButton_8_5_toggled(bool checked)
{
    currentPinArray[8-1][5-1] = checked;
}

void MainWindow::on_radioButton_8_6_toggled(bool checked)
{
    currentPinArray[8-1][6-1] = checked;
}

void MainWindow::on_radioButton_8_7_toggled(bool checked)
{
    currentPinArray[8-1][7-1] = checked;
}

void MainWindow::on_radioButton_8_8_toggled(bool checked)
{
    currentPinArray[8-1][8-1] = checked;
}

void MainWindow::on_radioButton_8_9_toggled(bool checked)
{
    currentPinArray[8-1][9-1] = checked;
}

void MainWindow::on_radioButton_8_10_toggled(bool checked)
{
    currentPinArray[8-1][10-1] = checked;
}


void MainWindow::on_radioButton_8_11_toggled(bool checked)
{
    currentPinArray[8-1][11-1] = checked;
}

void MainWindow::on_radioButton_8_12_toggled(bool checked)
{
    currentPinArray[8-1][12-1] = checked;
}

void MainWindow::on_radioButton_8_13_toggled(bool checked)
{
    currentPinArray[8-1][13-1] = checked;
}

void MainWindow::on_radioButton_8_14_toggled(bool checked)
{
    currentPinArray[8-1][14-1] = checked;
}

void MainWindow::on_radioButton_8_15_toggled(bool checked)
{
    currentPinArray[8-1][15-1] = checked;
}

void MainWindow::on_radioButton_8_16_toggled(bool checked)
{
    currentPinArray[8-1][16-1] = checked;
}

void MainWindow::on_radioButton_9_1_toggled(bool checked)
{
    currentPinArray[9-1][1-1] = checked;
}

void MainWindow::on_radioButton_9_2_toggled(bool checked)
{
    currentPinArray[9-1][2-1] = checked;
}

void MainWindow::on_radioButton_9_3_toggled(bool checked)
{
    currentPinArray[9-1][3-1] = checked;
}

void MainWindow::on_radioButton_9_4_toggled(bool checked)
{
    currentPinArray[9-1][4-1] = checked;
}

void MainWindow::on_radioButton_9_5_toggled(bool checked)
{
    currentPinArray[9-1][5-1] = checked;
}

void MainWindow::on_radioButton_9_6_toggled(bool checked)
{
    currentPinArray[9-1][6-1] = checked;
}

void MainWindow::on_radioButton_9_7_toggled(bool checked)
{
    currentPinArray[9-1][7-1] = checked;
}

void MainWindow::on_radioButton_9_8_toggled(bool checked)
{
    currentPinArray[9-1][8-1] = checked;
}

void MainWindow::on_radioButton_9_9_toggled(bool checked)
{
    currentPinArray[9-1][9-1] = checked;
}

void MainWindow::on_radioButton_9_10_toggled(bool checked)
{
    currentPinArray[9-1][10-1] = checked;
}

void MainWindow::on_radioButton_9_11_toggled(bool checked)
{
    currentPinArray[9-1][11-1] = checked;
}

void MainWindow::on_radioButton_9_12_toggled(bool checked)
{
    currentPinArray[9-1][12-1] = checked;
}

void MainWindow::on_radioButton_9_13_toggled(bool checked)
{
    currentPinArray[9-1][13-1] = checked;
}

void MainWindow::on_radioButton_9_14_toggled(bool checked)
{
    currentPinArray[9-1][14-1] = checked;
}

void MainWindow::on_radioButton_9_15_toggled(bool checked)
{
    currentPinArray[9-1][15-1] = checked;
}

void MainWindow::on_radioButton_9_16_toggled(bool checked)
{
    currentPinArray[9-1][16-1] = checked;
}

void MainWindow::on_radioButton_10_1_toggled(bool checked)
{
    currentPinArray[10-1][1-1] = checked;
}

void MainWindow::on_radioButton_10_2_toggled(bool checked)
{
    currentPinArray[10-1][2-1] = checked;
}

void MainWindow::on_radioButton_10_3_toggled(bool checked)
{
    currentPinArray[10-1][3-1] = checked;
}

void MainWindow::on_radioButton_10_4_toggled(bool checked)
{
    currentPinArray[10-1][4-1] = checked;
}

void MainWindow::on_radioButton_10_5_toggled(bool checked)
{
    currentPinArray[10-1][5-1] = checked;
}

void MainWindow::on_radioButton_10_6_toggled(bool checked)
{
    currentPinArray[10-1][6-1] = checked;
}

void MainWindow::on_radioButton_10_7_toggled(bool checked)
{
    currentPinArray[10-1][7-1] = checked;
}

void MainWindow::on_radioButton_10_8_toggled(bool checked)
{
    currentPinArray[10-1][8-1] = checked;
}

void MainWindow::on_radioButton_10_9_toggled(bool checked)
{
    currentPinArray[10-1][9-1] = checked;
}

void MainWindow::on_radioButton_10_10_toggled(bool checked)
{
    currentPinArray[10-1][10-1] = checked;
}

void MainWindow::on_radioButton_10_11_toggled(bool checked)
{
    currentPinArray[10-1][11-1] = checked;
}

void MainWindow::on_radioButton_10_12_toggled(bool checked)
{
    currentPinArray[10-1][12-1] = checked;
}

void MainWindow::on_radioButton_10_13_toggled(bool checked)
{
    currentPinArray[10-1][13-1] = checked;
}

void MainWindow::on_radioButton_10_14_toggled(bool checked)
{
    currentPinArray[10-1][14-1] = checked;
}

void MainWindow::on_radioButton_10_15_toggled(bool checked)
{
    currentPinArray[10-1][15-1] = checked;
}

void MainWindow::on_radioButton_10_16_toggled(bool checked)
{
    currentPinArray[10-1][16-1] = checked;
}

void MainWindow::on_radioButton_11_1_toggled(bool checked)
{
    currentPinArray[11-1][1-1] = checked;
}

void MainWindow::on_radioButton_11_2_toggled(bool checked)
{
    currentPinArray[11-1][2-1] = checked;
}

void MainWindow::on_radioButton_11_3_toggled(bool checked)
{
    currentPinArray[11-1][3-1] = checked;
}

void MainWindow::on_radioButton_11_4_toggled(bool checked)
{
    currentPinArray[11-1][4-1] = checked;
}

void MainWindow::on_radioButton_11_5_toggled(bool checked)
{
    currentPinArray[11-1][5-1] = checked;
}

void MainWindow::on_radioButton_11_6_toggled(bool checked)
{
    currentPinArray[11-1][6-1] = checked;
}

void MainWindow::on_radioButton_11_7_toggled(bool checked)
{
    currentPinArray[11-1][7-1] = checked;
}

void MainWindow::on_radioButton_11_8_toggled(bool checked)
{
    currentPinArray[11-1][8-1] = checked;
}

void MainWindow::on_radioButton_11_9_toggled(bool checked)
{
    currentPinArray[11-1][9-1] = checked;
}

void MainWindow::on_radioButton_11_10_toggled(bool checked)
{
    currentPinArray[11-1][10-1] = checked;
}

void MainWindow::on_radioButton_11_11_toggled(bool checked)
{
    currentPinArray[11-1][11-1] = checked;
}

void MainWindow::on_radioButton_11_12_toggled(bool checked)
{
    currentPinArray[11-1][12-1] = checked;
}

void MainWindow::on_radioButton_11_13_toggled(bool checked)
{
    currentPinArray[11-1][13-1] = checked;
}

void MainWindow::on_radioButton_11_14_toggled(bool checked)
{
    currentPinArray[11-1][14-1] = checked;
}

void MainWindow::on_radioButton_11_15_toggled(bool checked)
{
    currentPinArray[11-1][15-1] = checked;
}

void MainWindow::on_radioButton_11_16_toggled(bool checked)
{
    currentPinArray[11-1][16-1] = checked;
}

void MainWindow::on_radioButton_12_1_toggled(bool checked)
{
    currentPinArray[12-1][1-1] = checked;
}


void MainWindow::on_radioButton_12_2_toggled(bool checked)
{
    currentPinArray[12-1][2-1] = checked;
}

void MainWindow::on_radioButton_12_3_toggled(bool checked)
{
    currentPinArray[12-1][3-1] = checked;
}

void MainWindow::on_radioButton_12_4_toggled(bool checked)
{
    currentPinArray[12-1][4-1] = checked;
}

void MainWindow::on_radioButton_12_5_toggled(bool checked)
{
    currentPinArray[12-1][5-1] = checked;
}

void MainWindow::on_radioButton_12_6_toggled(bool checked)
{
    currentPinArray[12-1][6-1] = checked;
}

void MainWindow::on_radioButton_12_7_toggled(bool checked)
{
    currentPinArray[12-1][7-1] = checked;
}

void MainWindow::on_radioButton_12_8_toggled(bool checked)
{
    currentPinArray[12-1][8-1] = checked;
}

void MainWindow::on_radioButton_12_9_toggled(bool checked)
{
    currentPinArray[12-1][9-1] = checked;
}

void MainWindow::on_radioButton_12_10_toggled(bool checked)
{
    currentPinArray[12-1][10-1] = checked;
}

void MainWindow::on_radioButton_12_11_toggled(bool checked)
{
    currentPinArray[12-1][11-1] = checked;
}

void MainWindow::on_radioButton_12_12_toggled(bool checked)
{
    currentPinArray[12-1][12-1] = checked;
}

void MainWindow::on_radioButton_12_13_toggled(bool checked)
{
    currentPinArray[12-1][13-1] = checked;
}

void MainWindow::on_radioButton_12_14_toggled(bool checked)
{
    currentPinArray[12-1][14-1] = checked;
}

void MainWindow::on_radioButton_12_15_toggled(bool checked)
{
    currentPinArray[12-1][15-1] = checked;
}

void MainWindow::on_radioButton_12_16_toggled(bool checked)
{
    currentPinArray[12-1][16-1] = checked;
}

void MainWindow::on_radioButton_13_1_toggled(bool checked)
{
    currentPinArray[13-1][1-1] = checked;
}

void MainWindow::on_radioButton_13_2_toggled(bool checked)
{
    currentPinArray[13-1][2-1] = checked;
}

void MainWindow::on_radioButton_13_3_toggled(bool checked)
{
    currentPinArray[13-1][3-1] = checked;
}

void MainWindow::on_radioButton_13_4_toggled(bool checked)
{
    currentPinArray[13-1][4-1] = checked;
}

void MainWindow::on_radioButton_13_5_toggled(bool checked)
{
    currentPinArray[13-1][5-1] = checked;
}

void MainWindow::on_radioButton_13_6_toggled(bool checked)
{
    currentPinArray[13-1][6-1] = checked;
}

void MainWindow::on_radioButton_13_7_toggled(bool checked)
{
    currentPinArray[13-1][7-1] = checked;
}

void MainWindow::on_radioButton_13_8_toggled(bool checked)
{
    currentPinArray[13-1][8-1] = checked;
}

void MainWindow::on_radioButton_13_9_toggled(bool checked)
{
    currentPinArray[13-1][9-1] = checked;
}

void MainWindow::on_radioButton_13_10_toggled(bool checked)
{
    currentPinArray[13-1][10-1] = checked;
}

void MainWindow::on_radioButton_13_11_toggled(bool checked)
{
    currentPinArray[13-1][11-1] = checked;
}

void MainWindow::on_radioButton_13_12_toggled(bool checked)
{
    currentPinArray[13-1][12-1] = checked;
}

void MainWindow::on_radioButton_13_13_toggled(bool checked)
{
    currentPinArray[13-1][13-1] = checked;
}

void MainWindow::on_radioButton_13_14_toggled(bool checked)
{
    currentPinArray[13-1][14-1] = checked;
}

void MainWindow::on_radioButton_13_15_toggled(bool checked)
{
    currentPinArray[13-1][15-1] = checked;
}

void MainWindow::on_radioButton_13_16_toggled(bool checked)
{
    currentPinArray[13-1][16-1] = checked;
}

void MainWindow::on_radioButton_14_1_toggled(bool checked)
{
    currentPinArray[14-1][1-1] = checked;
}

void MainWindow::on_radioButton_14_2_toggled(bool checked)
{
    currentPinArray[14-1][2-1] = checked;
}

void MainWindow::on_radioButton_14_3_toggled(bool checked)
{
    currentPinArray[14-1][3-1] = checked;
}

void MainWindow::on_radioButton_14_4_toggled(bool checked)
{
    currentPinArray[14-1][4-1] = checked;
}

void MainWindow::on_radioButton_14_5_toggled(bool checked)
{
    currentPinArray[14-1][5-1] = checked;
}

void MainWindow::on_radioButton_14_6_toggled(bool checked)
{
    currentPinArray[14-1][6-1] = checked;
}

void MainWindow::on_radioButton_14_7_toggled(bool checked)
{
    currentPinArray[14-1][7-1] = checked;
}

void MainWindow::on_radioButton_14_8_toggled(bool checked)
{
    currentPinArray[14-1][8-1] = checked;
}

void MainWindow::on_radioButton_14_9_toggled(bool checked)
{
    currentPinArray[14-1][9-1] = checked;
}

void MainWindow::on_radioButton_14_10_toggled(bool checked)
{
    currentPinArray[14-1][10-1] = checked;
}

void MainWindow::on_radioButton_14_11_toggled(bool checked)
{
    currentPinArray[14-1][11-1] = checked;
}

void MainWindow::on_radioButton_14_12_toggled(bool checked)
{
    currentPinArray[14-1][12-1] = checked;
}

void MainWindow::on_radioButton_14_13_toggled(bool checked)
{
    currentPinArray[14-1][13-1] = checked;
}

void MainWindow::on_radioButton_14_14_toggled(bool checked)
{
    currentPinArray[14-1][14-1] = checked;
}

void MainWindow::on_radioButton_14_15_toggled(bool checked)
{
    currentPinArray[14-1][15-1] = checked;
}

void MainWindow::on_radioButton_14_16_toggled(bool checked)
{
    currentPinArray[14-1][16-1] = checked;
}

void MainWindow::on_radioButton_15_1_toggled(bool checked)
{
    currentPinArray[15-1][1-1] = checked;
}

void MainWindow::on_radioButton_15_2_toggled(bool checked)
{
    currentPinArray[15-1][2-1] = checked;
}

void MainWindow::on_radioButton_15_3_toggled(bool checked)
{
    currentPinArray[15-1][3-1] = checked;
}

void MainWindow::on_radioButton_15_4_toggled(bool checked)
{
    currentPinArray[15-1][4-1] = checked;
}

void MainWindow::on_radioButton_15_5_toggled(bool checked)
{
    currentPinArray[15-1][5-1] = checked;
}

void MainWindow::on_radioButton_15_6_toggled(bool checked)
{
    currentPinArray[15-1][6-1] = checked;
}

void MainWindow::on_radioButton_15_7_toggled(bool checked)
{
    currentPinArray[15-1][7-1] = checked;
}

void MainWindow::on_radioButton_15_8_toggled(bool checked)
{
    currentPinArray[15-1][8-1] = checked;
}

void MainWindow::on_radioButton_15_9_toggled(bool checked)
{
    currentPinArray[15-1][9-1] = checked;
}

void MainWindow::on_radioButton_15_10_toggled(bool checked)
{
    currentPinArray[15-1][10-1] = checked;
}

void MainWindow::on_radioButton_15_11_toggled(bool checked)
{
    currentPinArray[15-1][11-1] = checked;
}

void MainWindow::on_radioButton_15_12_toggled(bool checked)
{
    currentPinArray[15-1][12-1] = checked;
}

void MainWindow::on_radioButton_15_13_toggled(bool checked)
{
    currentPinArray[15-1][13-1] = checked;
}

void MainWindow::on_radioButton_15_14_toggled(bool checked)
{
    currentPinArray[15-1][14-1] = checked;
}

void MainWindow::on_radioButton_15_15_toggled(bool checked)
{
    currentPinArray[15-1][15-1] = checked;
}

void MainWindow::on_radioButton_15_16_toggled(bool checked)
{
    currentPinArray[15-1][16-1] = checked;
}

void MainWindow::on_radioButton_16_1_toggled(bool checked)
{
    currentPinArray[16-1][1-1] = checked;
}

void MainWindow::on_radioButton_16_2_toggled(bool checked)
{
    currentPinArray[16-1][2-1] = checked;
}

void MainWindow::on_radioButton_16_3_toggled(bool checked)
{
    currentPinArray[16-1][3-1] = checked;
}

void MainWindow::on_radioButton_16_4_toggled(bool checked)
{
    currentPinArray[16-1][4-1] = checked;
}

void MainWindow::on_radioButton_16_5_toggled(bool checked)
{
    currentPinArray[16-1][5-1] = checked;
}

void MainWindow::on_radioButton_16_6_toggled(bool checked)
{
    currentPinArray[16-1][6-1] = checked;
}

void MainWindow::on_radioButton_16_7_toggled(bool checked)
{
    currentPinArray[16-1][7-1] = checked;
}

void MainWindow::on_radioButton_16_8_toggled(bool checked)
{
    currentPinArray[16-1][8-1] = checked;
}

void MainWindow::on_radioButton_16_9_toggled(bool checked)
{
    currentPinArray[16-1][9-1] = checked;
}

void MainWindow::on_radioButton_16_10_toggled(bool checked)
{
    currentPinArray[16-1][10-1] = checked;
}

void MainWindow::on_radioButton_16_11_toggled(bool checked)
{
    currentPinArray[16-1][11-1] = checked;
}

void MainWindow::on_radioButton_16_12_toggled(bool checked)
{
    currentPinArray[16-1][12-1] = checked;
}

void MainWindow::on_radioButton_16_13_toggled(bool checked)
{
    currentPinArray[16-1][13-1] = checked;
}

void MainWindow::on_radioButton_16_14_toggled(bool checked)
{
    currentPinArray[16-1][14-1] = checked;
}

void MainWindow::on_radioButton_16_15_toggled(bool checked)
{
    currentPinArray[16-1][15-1] = checked;
}

void MainWindow::on_radioButton_16_16_toggled(bool checked)
{
    currentPinArray[16-1][16-1] = checked;
}

//show infos about author and easier usage
void MainWindow::on_bt_info_clicked()
{
    QString infoMessage = "error reading info.txt";
    //read in info file
    try {
        QFile file("info.txt");
        infoMessage = file.readAll();
    }
    QMessageBox info;
    info.setText(infoMessage);
    info.exec();
}
