#include <b0/node.h>
#include <b0/publisher.h>
#include <opencv2/opencv.hpp>
#include <boost/thread.hpp>

int main()
{
    cv::VideoCapture cap(0);
    if(!cap.isOpened()) return -1;

    b0::Node node("camera-node");
    b0::Publisher camera_pub(&node, "camera");
    node.init();

    while(!node.shutdownRequested())
    {
        cv::Mat frame;
        cap >> frame;
        cv::flip(frame, frame, 1);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        std::vector<uchar> buf;
        if(cv::imencode(".jpg", frame, buf))
        {
            std::cout << "Sending message " << buf.size() << std::endl;
            std::string msg(reinterpret_cast<const char*>(buf.data()), buf.size());
            camera_pub.publish(msg);
        }

        node.spinOnce();
        boost::this_thread::sleep_for(boost::chrono::milliseconds{1000/24});
    }

    node.cleanup();

    return 0;
}
