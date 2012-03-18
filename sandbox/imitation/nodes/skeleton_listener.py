#!/usr/bin/env python  
import roslib
roslib.load_manifest('imitation')
import rospy
import math
import tf

def get_coords(side_name):
    """
    Given a side ("left" or "right"),
    Find all the coordinates of the shoulder, elbow, and wrist
    in terms of the opposite shoulder.
    Return a tuple of (shoulder_coords, elbow_coords, wrist_coords) 
    """
    if side_name == 'right':
        opposite_shoulder = '/left_shoulder'
    elif side_name == 'left':
        opposite_shoulder = '/right_shoulder'
    shoulder_trans, shoulder_rot = listener.lookupTransform('%s_shoulder' % side_name, opposite_shoulder, rospy.Time(0))
    elbow_trans, elbow_rot = listener.lookupTransform('%s_elbow' % side_name, opposite_shoulder, rospy.Time(0))
    wrist_trans, wrist_rot = listener.lookupTransform('%s_hand' % side_name, opposite_shoulder, rospy.Time(0))
    return (shoulder_trans, elbow_trans, wrist_trans)

def shoulder_pitch_roll(shoulder_coords, elbow_coords):
    """
    Given the shoulder and elbow coordinates
    (relative to the opposite shoulder at 0,0,0)
    find the pitch and roll of the shoulder joint.
    Return a tuple (pitch, roll) in radians
    """
    #distance from opposite shoulder to elbow
    os2e = math.sqrt(sum([x**2 for x in elbow_coords]))
    #distance from opposite shoulder to shoulder
    os2s = math.sqrt(sum([x**2 for x in shoulder_coords])) #TODO this should be constant
    #distance from shoulder to elbow
    s2e = math.sqrt(sum([(e-s)**2 for e,s in zip(elbow_coords, shoulder_coords)])) #TODO this should be constant
    
    #ROLL
    #law of cosines
    roll = math.acos((os2s**2 + s2e**2 - os2e**2) / (2 * os2s * s2e))
    
    #PITCH
    #find equation of plane through shoulder, perpendicular to line through shoulders
        #shoulder coords of (A,B,C)  and opposite shoulder at (0,0,0) 
        #yield plane of Ax + By + Cz + (A^2 + B^2 + C^2) = 0
    #project elbow onto plane - http://www.9math.com/book/projection-point-plane
    #project neck onto plane (assumes that this is always "up" perpendicular to shoulders)
    #Find elbow-shoulder-neck angle in this plane using law of cosines
    #Somehow determine whether the angle is positive or negative
    
    pitch = 3
    return (pitch, roll)

def elbow_yaw_roll(shoulder_coords, elbow_coords, wrist_coords):
    """
    Given the shoulder, elbow, and wrist coordinates
    (relative to the opposite shoulder at 0,0,0)
    find the yaw and roll of the elbow joint.
    Return a tuple (yaw, roll) in radians
    """
    #distance from shoulder to elbow
    s2e = math.sqrt(sum([(e-s)**2 for e,s in zip(elbow_coords, shoulder_coords)])) #TODO this should be constant
    #distance from elbow to wrist
    e2w = math.sqrt(sum([(w-e)**2 for w,e in zip(wrist_coords, elbow_coords)])) #TODO this should be constant
    #distance from shoulder to wrist
    s2w = math.sqrt(sum([(w-s)**2 for w,s in zip(wrist_coords, shoulder_coords)]))
    
    #ROLL
    #law of cosines
    roll = math.acos((s2e**2 + e2w**2 - s2w**2) / (2 * s2e * e2w))
    
    #YAW
    #find equation of plane through elbow, perpendicular to line from shoulder to elbow
        #elbow coords of (A,B,C) and shoulder coords of (D,E,F) 
        #yield plane of (A-D)x + (B-E)y + (C-F)z + (AD + BE + CF - A^2 - B^2 - C^2) = 0
    #project wrist onto plane - http://www.9math.com/book/projection-point-plane
    #... how to orient this angle?!
    yaw = 3
    return (yaw, roll)
    
if __name__ == '__main__':
    rospy.init_node('skeleton_listener')
    listener = tf.TransformListener()
    
    rate = rospy.Rate(10.0)
    while not rospy.is_shutdown():
        try:
            for side in ('left', 'right'):
                shoulder_coords, elbow_coords, wrist_coords = get_coords(side)
                s_pitch, s_roll = shoulder_pitch_roll(shoulder_coords, elbow_coords)
                print "%s shoulder pitch/roll: %s/%s" % (side, s_pitch, s_roll)

        except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
            continue
        
        rate.sleep()
