#include "../include/tf_test/mapping.h"

using namespace std;

mapping::mapping(ros::NodeHandle& nh)
{
    // gps_position_sub_ = nh.subscribe("/gps", 10,  GpsPositionCallback);
    odom_publish = nh.advertise<nav_msgs::Odometry>("odom", 100);
    path_position_sub_ = nh.subscribe("/globalEstimator/global_path", 100,  &mapping::odom_sub, this);
    point_cloud_sub_ = nh.subscribe("/point_cloud1",100,&mapping::point_cloud_sub, this);
}

mapping::~mapping(){

}

void mapping::GpsPositionCallback(const sensor_msgs::NavSatFix& gps_msg) {
    // Check the gps_status.
    if(!initGPS)
    {
        string log_folder = "/home";
         ros::param::get("log_folder", log_folder);
        // ros::param::get("log_folder", log_folder);
        // Log.
        // cout<<log_folder<<endl;
        file_frist_gps_.open(log_folder+"/map/first_gps.csv", ios::out);
        if(!file_frist_gps_)
        { 
            cout<<"error !"<<endl;
        }

        geoConverter.Reset(gps_msg.latitude, gps_msg.longitude, gps_msg.altitude);

        cout<< std::fixed << std::setprecision(15)
         << gps_msg.latitude<< "," << gps_msg.longitude << "," << gps_msg.altitude<<"23333"<<endl;

        file_frist_gps_ << std::fixed << std::setprecision(15)
         << gps_msg.latitude<< "," << gps_msg.longitude << "," << gps_msg.altitude<< "\n";
        
        initGPS = true;
        file_frist_gps_.close();
    }
}

void mapping::odom_sub(const nav_msgs::Path& odom_path)
{
    geometry_msgs::PoseStamped pose;
    pose=odom_path.poses.back();
    if(!initxyz){
        X=pose.pose.position.x;
        Y=pose.pose.position.y;
        initxyz=true;
    }
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

void mapping::point_cloud_sub(const sensor_msgs::PointCloud &pc1)
{ 
    point_cloud1 = pc1;
}
