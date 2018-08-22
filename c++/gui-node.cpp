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

CameraWindow *win;

void camera_callback(const std::string &msg)
{
    std::vector<uchar> buf(msg.begin(), msg.end());
    cv::Mat frame = cv::imdecode(buf, cv::IMREAD_COLOR);
    QImage img((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    win->setImage(img);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    b0::Node node("camera-gui");

    win = new CameraWindow(node);

    b0::Subscriber camera_sub(&node, "camera", camera_callback);
    node.init();

    win->show();
    int ret = app.exec();

    node.cleanup();

    return ret;
}

