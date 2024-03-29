/*
 * keyFrame
 * Copyright (c) 2013, Aditi Ramachandran
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the yale-ros-pkgs nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <ros/ros.h>
#include <actionlib/client/simple_action_client.h>
#include <actionlib/client/terminal_state.h>
#include <dragon_msgs/IKAction.h>
#include <geometry_msgs/Pose.h>


#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Editor.H>
#include <Fl/Fl_Text_Buffer.H>
#include <Fl/Fl_Select_Browser.H>
#include <Fl/Fl_Input.H>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

Fl_Hor_Value_Slider* sliders[5];
Fl_Select_Browser* output;
Fl_Input* textBox;
ofstream outputFile;
bool firstFrame = true;

//ros::Publisher ik_publisher;
actionlib::SimpleActionClient<dragon_msgs::IKAction> *ac;

void slider_cb(Fl_Widget* o, void*) {
  Fl_Hor_Value_Slider *slider = (Fl_Hor_Value_Slider*)o;

  double xVal = sliders[0]->value();
  double yVal = sliders[1]->value();
  double zVal = sliders[2]->value();
  double tVal = sliders[3]->value();
  double nVal = sliders[4]->value();

  printf("(%0.2f, %0.2f, %0.2f) : (%0.2f, %0.2f)\n", xVal, yVal, zVal, tVal, nVal);
  
  dragon_msgs::IKGoal goal;
  goal.x = xVal;
  goal.y = yVal;
  goal.z = zVal;
  goal.theta = tVal;
  goal.neck = nVal;
  goal.state = std::string("on");
  ac->sendGoal(goal);
  
}

void button_cb(Fl_Widget* o, void*) {
  Fl_Button *text = (Fl_Button*) o;

  double xVal = sliders[0]->value();
  double yVal = sliders[1]->value();
  double zVal = sliders[2]->value();
  double tVal = sliders[3]->value();
  double nVal = sliders[4]->value();

  stringstream convertX;
  convertX << xVal;
  stringstream convertY;
  convertY << yVal;
  stringstream convertZ;
  convertZ << zVal;
  stringstream convertT;
  convertT << tVal;
  stringstream convertN;
  convertN << nVal;

  string s = convertX.str() + "," + convertY.str() + "," + convertZ.str() + "," + convertT.str() + "," + convertN.str(); 
  output->add(s.c_str());

  printf("%0.2f, %0.2f, %0.2f, %0.2f, %0.2f\n", xVal, yVal, zVal, tVal, nVal);

  if (firstFrame){
    string name = textBox->value();
    outputFile << "- name: \"" << name << "\"" << endl;
    outputFile << "  frames:" << endl;
    firstFrame = false;
  }
  outputFile << "\t- [" << s << "]" << endl;

}

void button2_cb(Fl_Widget* o, void*) {
  Fl_Button *button = (Fl_Button*) o;

  //find which line in the select browser is SELECTED and delete it
  int value = output->value();
  if (value!=0){
    output->remove(value);	
  }

}

void button3_cb(Fl_Widget* o, void*) {
  output->clear();
}

int main(int argc, char **argv) {

  ros::init(argc,argv,"keyframer");
  ros::NodeHandle nh;
  ac = new actionlib::SimpleActionClient<dragon_msgs::IKAction>("IK_Server", true);
  ac->waitForServer();
  //ik_publisher = nh.advertise<geometry_msgs::Pose>("ik",5);

  //outputFile.open(argv[1]);
  outputFile.open("savedFrames.yaml");

  Fl_Window *window = new Fl_Window(380,600);

  Fl_Hor_Value_Slider *xSlider = new Fl_Hor_Value_Slider(20,20,310,20, "x value");
  Fl_Hor_Value_Slider *ySlider = new Fl_Hor_Value_Slider(20,60,310,20, "y value");
  Fl_Hor_Value_Slider *zSlider = new Fl_Hor_Value_Slider(20,100,310,20, "z value");
  Fl_Hor_Value_Slider *tSlider = new Fl_Hor_Value_Slider(20,140,310,20, "theta value");
  Fl_Hor_Value_Slider *nSlider = new Fl_Hor_Value_Slider(20,180,310,20, "neck value");

  xSlider->bounds(-7.6199, 7.3942);
  ySlider->bounds(-2.4999, 4.8488);
  zSlider->bounds(-7.62, 7.62);
  tSlider->bounds(-0.7853, 0.7621);
  nSlider->bounds(-0.1745, 0.7621);

  sliders[0] = xSlider;
  sliders[1] = ySlider;
  sliders[2] = zSlider;
  sliders[3] = tSlider;
  sliders[4] = nSlider;

  int frame_yPos = 220;

  Fl_Button *button = new Fl_Button(110, frame_yPos, 100, 30, "FRAME");

  textBox = new Fl_Input(100, frame_yPos+40, 200, 30, "name:");

  output = new Fl_Select_Browser(20, frame_yPos+80, 310, 200, "recorded frames" );

  xSlider->callback(slider_cb);
  ySlider->callback(slider_cb);
  zSlider->callback(slider_cb);
  tSlider->callback(slider_cb);
  nSlider->callback(slider_cb);
  button->callback(button_cb);

  Fl_Button *delButton = new Fl_Button(50, frame_yPos+330, 100, 30, "delete frame");
  delButton->callback(button2_cb);

  Fl_Button *clearButton = new Fl_Button(50, frame_yPos+380, 100, 30, "clear all frames");
  clearButton->callback(button3_cb);

  Fl_Button *saveButton = new Fl_Button(180, frame_yPos+330, 100, 30, "save frames");

  Fl_Button *playButton = new Fl_Button(180, frame_yPos+380, 100, 30, "play all");

  button->type(FL_NORMAL_BUTTON);
  window->end();
  window->show(argc, argv);

  //return Fl::run();
  ros::Rate loop_rate(100);

  while( ros::ok() && Fl::check() )
  {
    ros::spinOnce();
    loop_rate.sleep();
  }

}
