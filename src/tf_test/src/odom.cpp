#include "../include/tf_test/odom.h"

using namespace std;

mapload::mapload(ros::NodeHandle& nh)
{
    initxyz();                                                                                                                                                  

    // state_pub_ = nh.advertise<nav_msgs::Path>("gps_path", 10);
    odom_publish = nh.advertise<nav_msgs::Odometry>("odom", 50);
    // odom_publisher_ = nh_.advertise<nav_msgs::Odometry>("raw_odom", 50);
    gps_position_sub_ = nh.subscribe("/gps", 1,  &mapload::local, this);
    path_position_sub_ = nh.subscribe("/globalEstimator/global_path", 10,  &mapload::odom_pub, this);
    
}



mapload::~mapload() 
{
    
}


void mapload::local(const sensor_msgs::NavSatFix& gps_msg)
{
    if(init)
    {
        geoConverter.Forward(gps_msg.latitude,gps_msg.longitude, gps_msg.altitude,   X, Y, Z);
        cout<<X<<","<<Y<<"," <<endl;
        init = false;
    }
    double th =0;
    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = current_time;
    odom_trans.header.frame_id = "world";
    odom_trans.child_frame_id = "odom";
    odom_trans.transform.translation.x = X;
    odom_trans.transform.translation.y = Y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;
    //send the transform
    odom_world_broadcaster.sendTransform(odom_trans);
    odom_world_broadcaster;
}


void mapload::odom_pub(const nav_msgs::Path& odom_path)
{
    geometry_msgs::PoseStamped pose;
    pose=odom_path.poses.back();

      // first, we'll publish the transform over tf
    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = current_time;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = pose.pose.position.x-X;
    odom_trans.transform.translation.y = pose.pose.position.y-Y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = pose.pose.orientation;

    //send the transform
    odom_baselink_broadcaster.sendTransform(odom_trans);

    //next, we'll publish the odometry message over ROS
    nav_msgs::Odometry odom;
    odom.header.stamp = current_time;
    odom.header.frame_id = "odom";

    //set the position
    odom.pose.pose.position.x = pose.pose.position.x-X;
    odom.pose.pose.position.y = pose.pose.position.y-Y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = pose.pose.orientation;

    //set the velocity
    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = 0;
    odom.twist.twist.linear.y = 0;
    odom.twist.twist.angular.z = 0;

    //publish the message
    odom_publish.publish(odom);
}

//读取创建地图时的gps，作为初始化
void mapload::initxyz()
{
        //读取文件
        std::string log_folder = "/home";
        ros::param::get("log_folder", log_folder);
        cout<<log_folder<<endl;
        file_frist_gps_.open(log_folder +"/map/frist_gps.csv");
        if(!file_frist_gps_)
        {
            cout <<"error open gps_frist_file!!!";
        }
        string s;
        file_frist_gps_>>s;

        //解析数据
        vector<double> result;
        int start=0;
        int pos = s.find_first_of(",",0);
        double num=0;
        while(pos <= s.length())
        {
                num=stold(s.substr(start,pos-start));
                result.push_back(num);
                start=pos+1;
                pos=s.find_first_of(",",start);
        }
        num = stold(s.substr(start,s.length()-start));
        result.push_back(num);

        //初始化坐标
        geoConverter.Reset(result[1], result[2], result[3]);
        file_frist_gps_.close();
}
