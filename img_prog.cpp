#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <fstream>  
#include <sstream>
 
 
using namespace cv;
using namespace std;


class img_prog
{
  ros::NodeHandle nh;
  image_transport::ImageTransport it;
  image_transport::Publisher pub;
  image_transport::Subscriber sub;

public:
  img_prog():it(nh){
    sub = it.subscribe("/camera/rgb/image_raw", 1, &img_prog::imageCallbackrgb,this);
    pub = it.advertise("/canny_img",1);

  };


  void imageCallbackrgb(const sensor_msgs::ImageConstPtr& msg);
  bool proc();
  

private:
  Mat src,src_gray,canny_output; 
  int thresh = 30; 
  sensor_msgs::ImagePtr msg;

};

void img_prog::imageCallbackrgb(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    cv_bridge::CvImagePtr cv_ptr;
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    src = cv_ptr->image;

    proc ();
     
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

bool img_prog::proc(){
	cvtColor( src, src_gray, CV_BGR2GRAY );//灰度化 	
	GaussianBlur( src, src, Size(3,3), 0.1, 0, BORDER_DEFAULT ); 	
	blur( src_gray, src_gray, Size(3,3) ); //滤波 	
 
	Canny( src_gray, canny_output, thresh, thresh*3, 3 ); 	
  msg = cv_bridge::CvImage(std_msgs::Header(), "mono8", canny_output).toImageMsg();


  pub.publish(msg);
}


 
 
 
int main(int argc, char **argv)
{
  ros::init(argc, argv, "image_prog");
  img_prog img_canny;

  ros::spin();

  return 0;
}
