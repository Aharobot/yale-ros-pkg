Get Nao dependencies:
Install rosinstall: http://www.ros.org/wiki/rosinstall#Installation
Install Nao ROS packages: http://www.ros.org/wiki/nao/Installation

Install some of Dave's old code:
$ svn co https://usc-ros-pkg.svn.sourceforge.net/svnroot/usc-ros-pkg/trunk/turn_taking/tf_to_pose
$ rospack profile
$ rosdep install tf_to_pose
$ rosmake tf_to_pose

Run the following after package creation to make sure on path:
$ rospack profile
$ rospack find imitation

Install dependencies:
$ rosdep install imitation

Make package:
$ rosmake imitation

Make nodes executable:
$ chmod +x imitation/nodes/angle_calculator.py

To run roscore:
$ roscore

To play the bag file:
$ rosbag play --clock sandbox/skeleton_data/data/2012-02-21-15-37-28.bag

To run rviz:
$ rosparam set use_sim_time true
$ rosrun rviz rviz
Set Fixed Frame to: /openni_rgb_frame
Set Target Frame to: <Fixed Frame>

If you want to use the fake Nao model in rviz:
$ rosmake fake_nao
$ roslaunch imitation fake_robo.launch

To run angle_calculator:
$ rosrun imitation angle_calculator.py

Before committing:
$ cd sandbox/imitation
$ make clean

Coordinates:
Z coordinate is front/back of robot
X coordinate is left/right of robot
Y coordinate is up/down (up is a smaller y value)
