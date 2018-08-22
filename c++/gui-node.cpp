#include <b0/node.h>
#include <b0/subscriber.h>
#include <opencv2/opencv.hpp>
#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QImage>
#include <QPainter>
#include <QTime>
#include <QQueue>

class CameraWindow : public QMainWindow
{
public:
    CameraWindow(b0::Node &node)
        : QMainWindow(),
          node_(node)
    {
        setWindowTitle("Camera GUI");
        resize(320, 200);

        label = new QLabel(this);
        setCentralWidget(label);

        QTimer *spinTimer = new QTimer(this);
        connect(spinTimer, &QTimer::timeout, [this](){this->node_.spinOnce();});
        spinTimer->start(10);
    }

    void setImage(QImage image)
    {
        label->setPixmap(QPixmap::fromImage(image));
    }

private:
    b0::Node &node_;
    QLabel *label;
};

class FPSMeter
{
public:
    FPSMeter()
    {
        frameTime.start();
    }

    int measureTime()
    {
        return frameTime.restart();
    }

    float getAverageFPS()
    {
        int t = measureTime();
        frameTimes.enqueue(t);
        while(frameTimes.length() > averageNumSamples)
            frameTimes.dequeue();
        int sum = 0;
        for(int t : frameTimes) sum += t;
        return frameTimes.length() * 1000. / sum;
    }

    void draw(QImage *img)
    {
        QString text;
        QPoint pos(20, 50), off(3, 3);
        text.sprintf("FPS: %.1f", getAverageFPS());
        QPainter p(img);
        p.setFont(QFont("sans", 36));
        p.setPen(Qt::black);
        p.drawText(pos + off, text);
        p.setPen(Qt::green);
        p.drawText(pos, text);
    }

private:
    QTime frameTime;
    QQueue<int> frameTimes;
    const int averageNumSamples = 20;
};

CameraWindow *win;
FPSMeter *fpsMeter;

void camera_callback(const std::string &msg)
{
    std::vector<uchar> buf(msg.begin(), msg.end());
    cv::Mat frame = cv::imdecode(buf, cv::IMREAD_COLOR);
    QImage img((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    if(fpsMeter) fpsMeter->draw(&img);
    win->setImage(img);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    b0::Node node("camera-gui");

    win = new CameraWindow(node);
    fpsMeter = new FPSMeter;

    b0::Subscriber camera_sub(&node, "camera", camera_callback);
    node.init();

    win->show();
    int ret = app.exec();

    node.cleanup();

    return ret;
}

