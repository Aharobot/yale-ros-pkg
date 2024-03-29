#ifndef CORDIAL_BASE__BASE_PLUGIN_H_
#define CORDIAL_BASE__BASE_PLUGIN_H_

#include <cordial_base_/cordial_base.h>


namespace cordial_plugin
{
  class BasePlugin : public cordial_base::CordialBase
  {
    protected:
      std::string _robot_name;

    public:
      BasePlugin() {}
      void initialize()
      {
        _robot_name = std::string("Base Plugin");
      }

      std::string robot_name() {return _robot_name;}

      bool viseme_init()
      {
        ROS_INFO( "Hello World! Base Plugin is working!" );
      }
      
      void viseme_execute( const dragon_msgs::VisemeGoalConstPtr &goal )
      {
        ROS_INFO( "setting goal: %s", goal->constant.c_str() );
        dragon_msgs::VisemeResult result;
        viseme_as->setSucceeded(result);
      }
  };
}

#endif
