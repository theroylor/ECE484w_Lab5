#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , udpSocket(new QUdpSocket(this))
    , udpServerIP("192.168.1.255") // broadcast address
    , udpServerPort(5005)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_Load_clicked()
{
    // https://doc.qt.io/qt-5/qfiledialog.html
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Base Image"),      	// window text
                                                    "",                         	// starting directory
                                                    tr("Bitmap (*.bmp)"));      	// file type
    if (!fileName.isEmpty())	// if file is not empty
    {
        // load file
        base_image.load(fileName);

        //timer.start();

        update_input();

        image_not_set=0;

        update_output();

        //qDebug() << "Load-to-display took: " << timer.elapsed();
    }
}

void MainWindow::update_input()
{
    // sanitize file
    base_image = base_image.convertToFormat(QImage::Format_Grayscale8);
    base_image = base_image.convertToFormat(QImage::Format_Indexed8);


    width = base_image.width();
    height = base_image.height();

    // output_image will share everything with base_image except data
//    output_image = base_image;

    int h = ui -> label_input -> height();
    int w = ui -> label_input -> width();
    ui -> label_input -> setPixmap(QPixmap::fromImage(base_image).scaled(w,h,Qt::KeepAspectRatio));
}


//void MainWindow::on_pushButton_Save_clicked()
//{
//    if(image_not_set)
//    {return;}   // do not run unless image has been set

//    QString fileName = QFileDialog::getSaveFileName(this,
//                                                    tr("Save COmposite Image"), // window text
//                                                    "",                     	// starting directory
//                                                    tr("Bitmap (*.bmp)"));  	// file type
//    output_image.save(fileName);
//}



void MainWindow::on_pushButton_brightness_clicked()
{
    ui->verticalSlider_brightness->setValue(0);
}

void MainWindow::on_verticalSlider_brightness_valueChanged(int value)
{

    brightness = value;

    // update text label with value
    QString t;
    t = "Brightness: ";
    if (brightness>=0){t.append("+");}
    t.append(QString::number(brightness));
    ui->label_brightness->setText(t);

    update_output();
    sendUdpInteger(0x2b,brightness);
}

void MainWindow::on_pushButton_contrast_clicked()
{
    ui->verticalSlider_contrast->setValue(99);
}

void MainWindow::on_verticalSlider_contrast_valueChanged(int value)
{

    contrast = value;

    // update text label with value
    QString t;
    t = "Contrast: ";
    t.append(QString::number(contrast));
    ui->label_contrast->setText(t);

    update_output();
    sendUdpInteger(0x2c,contrast);
}

void MainWindow::update_output()
{
    sendUdpInteger(0x20,status);
    //ui->label_Contrast->setText("Contrast: "&QString::number))
    if(image_not_set)
    {return;}   // do not run unless image has been set

    //innitialize histogram for new image
//    histogram.fill(0,256);
//    pixel_scale = 1.0 /(height * width);

//    for(int x = 0; x < width; x++)
//    {
//        for(int y = 0; y < height; y++)
//        {
//            // vector stores count of pixel with index value
//            int i = base_image.pixelIndex(x,y);
//            histogram[i]++;
//        }
//    }

//    int sum = 0;
//    for(int i = 0; i < 256; i++)
//    {
//        // resolve histogram equalization
//        sum += histogram.at(i);
//        histogram.replace(i,(brightness + sum * contrast * 1.0 / width / height));
//    }

//    for(int x = 0; x < width; x++)
//    {
//        for(int y = 0; y < height; y++)
//        {
//            // pixel value at a coordinate is an index
//            // value at index is the replacement value for that pixel
//            int temp = base_image.pixelIndex(x,y);
//            temp = histogram.at(temp);
//            temp = qBound(0,temp,255);
//            output_image.setPixel(x,y,temp);
//        }
//    }

    // display file
//    int h = ui -> label_output -> height();
//    int w = ui -> label_output -> width();
//    ui -> label_output -> setPixmap(QPixmap::fromImage(output_image).scaled(w,h,Qt::KeepAspectRatio));
}



void MainWindow::sendUdpImage(quint32 messageId, const QImage &image) {
    // Convert the image to a QByteArray (e.g., PNG format)
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "BMP"); // Save the image in BMP format

    // Use sendUdpData to send the serialized image
    sendUdpData(messageId, data);

    qDebug() << "Image sent as UDP data with Message ID:" << messageId;
}


void MainWindow::sendUdpInteger(quint32 messageId, quint32 integerValue) {
    // Serialize the integer into a QByteArray
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream.setByteOrder(QDataStream::LittleEndian); // Set to little-endian format

    // Write the integer value into the stream
    stream << integerValue;

    // Use sendUdpData to send the serialized integer
    sendUdpData(messageId, data);

    qDebug() << "Integer value" << integerValue << "sent as UDP data with Message ID:" << messageId;
}

void MainWindow::sendUdpData(quint32 messageId, const QByteArray &data) {
    if (data.isEmpty()) {
        qDebug() << "No data to send for Message ID:" << messageId;
        return;
    }

    const int chunkSize = packetSize-headerSize;
    int totalChunks = (data.size() + chunkSize - 1) / chunkSize; // Calculate total chunks

    for (int i = 0; i < totalChunks; ++i) {
        QByteArray chunkPacket;
        QDataStream stream(&chunkPacket, QIODevice::WriteOnly);

        stream.setByteOrder(QDataStream::LittleEndian); // Set to little-endian format

        stream << messageId;          // Message ID -- 4 bytes
        stream << quint32(i);         // Sequence number -- 4 bytes
        stream << quint32(totalChunks); // Total chunks -- 4 bytes
        stream << data.mid(i * chunkSize, chunkSize); // Extract chunk data (up to `chunkSize`)

        udpSocket->writeDatagram(chunkPacket, QHostAddress(udpServerIP), udpServerPort);

        qDebug() << chunkPacket.size() << "bytes in packet. Chunk" << i + 1 << "of" << totalChunks << "sent for Message ID:" << messageId;
        qDebug() << "First byte of data in chunk: " << data.mid(i * chunkSize, 1).toHex();
        QThread::msleep(10);

    }
}

void MainWindow::on_checkBox_overlay_toggle_stateChanged(int arg1)
{
    if (arg1){status = 0x10002;}
    else {status = 0x10001;}
    sendUdpInteger(0x20,status);
}

void MainWindow::on_pushButton_Send_Base_clicked()
{
    if(base_image.isNull())
    {return;}   // do not run unless image has been set
    sendUdpImage(0x13,base_image);
    sendUdpInteger(0x20,status);
    sendUdpInteger(0x2B,brightness);
    sendUdpInteger(0x2C,contrast);
}

