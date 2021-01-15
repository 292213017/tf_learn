#pragma once
#include <ros/ros.h>
#include <fstream>
#include <iomanip>
#include<iostream>
#include <sensor_msgs/NavSatFix.h>
#include <nav_msgs/Path.h>
#include <Eigen/Core>
#include <tf/transform_broadcaster.h>
#include "../../ThirdParty/GeographicLib/include/LocalCartesian.hpp"
#include "nav_msgs/OccupancyGrid.h"
#include <nav_msgs/Odometry.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/time_synchronizer.h>
#include <sensor_msgs/PointCloud.h>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/point_field_conversion.h>

//23333

class mapping
{
   public:
    mapping(ros::NodeHandle& nh);
    ~mapping();
    void GpsPositionCallback(const sensor_msgs::NavSatFix& gps_msg) ;
    void odom_sub(const nav_msgs::Path& odom_path);
    void point_cloud_sub(const sensor_msgs::PointCloud &pc1);
    double X,Y,Z;

    ros::Time current_time, last_time;
    ros::Subscriber gps_position_sub_;
    ros::Subscriber path_position_sub_;
    ros::Subscriber point_cloud_sub_;
    ros::Publisher map_pub_;
    ros::Publisher odom_publish;
    sensor_msgs::PointCloud point_cloud1;

    std::ofstream file_frist_gps_;
    nav_msgs::OccupancyGrid map;


    bool initGPS=false;
    bool initxyz = false;
    GeographicLib::LocalCartesian geoConverter;

    tf::TransformBroadcaster odom_baselink_broadcaster;

};
