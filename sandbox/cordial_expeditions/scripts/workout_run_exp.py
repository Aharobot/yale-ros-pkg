#!/usr/bin/env python

#Elaine Short
#Expeditions Year 1 Experiment

import roslib; roslib.load_manifest('expeditions_year1')
import rospy
import smach
import smach_ros
import sys

# class definitions for states are in the file experiment_states.py
from experiment_states import *

from dragonbot_manager import DragonbotManager
#from dragonbot_simulator import DragonbotManager
from tablet_manager import TabletManager
import yaml



def main():
    rospy.init_node('experiment_controller')

    if not rospy.has_param("~max_time"):
        rospy.set_param("~max_time", 600)
    rospy.set_param("~start_time", rospy.Time.now().secs)

    day = rospy.get_param("~lesson")
    day_num = rospy.get_param("~day")
    
    sm = smach.StateMachine(outcomes=['end'])

    lesson_list = {'lunchbox':("whole_grains", "drinks"),
                   'snacks':("snacks1", "snacks2"),
                   'breakfast':("cereal", "breakfast"),
                   'dinner':("sides1","sides2")}

   

    dm = DragonbotManager()
    dm.eye_open()
    tm = TabletManager()

    #info: day, lessons
    lessons = lesson_list[day]
    day = day + str(day_num)
    info = (day,lessons)
    
    rospy.loginfo("Reading food phrases file.")
    if day_num == 1:
        with open(roslib.packages.get_pkg_dir("expeditions_year1")+ "/yaml/day1_food_phrases.yaml", 'r') as f:
            s = f.read()
    elif day_num == 2:
        with open(roslib.packages.get_pkg_dir("expeditions_year1")+ "/yaml/day2_food_phrases.yaml", 'r') as f:
            s = f.read()
    else:
        print "Usage: day number must be 1 or 2"
        sys.exit()

    # file format is:
    # lesson:
    #  intro: phrase_id
    #  reminder: phrase_idfg
    #  no_choice: phrase_id
    #  foodname: (food chosen before)
    #    foodname: phrase_id (food chosen after)
    # index as food_info["lesson"]["choice1"]["choice2"]
    # nb: food_info["lesson"]["none"].keys() will give all foods (and "none")
    food_info = yaml.load(s)

    rospy.loginfo("Reading other dialogue phrases file.")
    with open(roslib.packages.get_pkg_dir("expeditions_year1")+ "/yaml/dialogue_specification.yaml", 'r') as f:
        s = f.read()
    
    # file format is:
    
    dialogue_info = yaml.load(s)
    

    #rospy.loginfo("Loading phrase information file.")
    #dm.load_phrases("phrases.yaml")
    #rospy.loginfo("Done loading files.")

    if day_num == 1:
        food_state = FoodChoiceDay1(dm, tm, info, dialogue_info[day + "_foods"],food_info)
    elif day_num == 2:
        food_state = FoodChoiceDay2(dm, tm, info, dialogue_info[day + "_foods"],food_info)
 
    with sm:
        smach.StateMachine.add('WORKOUT', Workout(dm, tm, info, dialogue_info[day + "_workout"]),
                               transitions={'panic':'SLEEP',
                                            'continue':'WORKOUT',
                                            'end':'OUTRO',
                                            'timeout':'OUTRO'})


        smach.StateMachine.add('SLEEP', Sleep(dm, tm, info),
                               transitions={'wakeup':'INTRO',
                                            'done':'end'})
        smach.StateMachine.add('INTRO', Intro(dm, tm, info, dialogue_info[day + "_intro"]),
                               transitions={'panic':'SLEEP',
                                            'end':'F_CHOICE'})
        smach.StateMachine.add('F_CHOICE', food_state,
                               transitions={'panic':'SLEEP',
                                            'next_round':'F_CHOICE',
                                            'end':'WORKOUT',
                                            'timeout':'OUTRO'})


 
        smach.StateMachine.add('OUTRO', Outro(dm, tm, info, dialogue_info[day + "_outro"]),
                               transitions={'end':'SLEEP',
                                            'panic':'SLEEP'})

    outcome = sm.execute()

if __name__ == '__main__':
    main()
