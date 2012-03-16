#ifndef __BARD_COMPONENTS_WAM_H
#define __BARD_COMPONENTS_WAM_H

#include <iostream>

#include <boost/scoped_ptr.hpp>

#include <kdl/jntarray.hpp>
#include <rtt/RTT.hpp>
#include <rtt/Port.hpp>

#include <sensor_msgs/JointState.h>

#include <leoCAN/RTSocketCAN.h>
#include <barrett_direct/WAM.h>


namespace bard_components {
  class WAM : public RTT::TaskContext
  {
    // RTT Properties
    int n_arm_dof_;
    std::string can_dev_name_;
    std::string robot_description_;
    std::string joint_prefix_;
    std::vector<double> initial_positions_;
    RTT::os::TimeService::Seconds joint_state_throttle_period_;
    
    // RTT Ports
    RTT::InputPort<KDL::JntArray> torques_in_port_;
    RTT::OutputPort<KDL::JntArray> positions_out_port_;
    RTT::OutputPort<KDL::JntArray> velocities_out_port_;
    RTT::OutputPort<sensor_msgs::JointState> joint_state_out_port_;

    // See: http://eigen.tuxfamily.org/dox/TopicStructHavingEigenMembers.html
    // See: http://www.orocos.org/forum/orocos/orocos-users/some-info-eigen-and-orocos
    // EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

  public:
    WAM(std::string const& name);
    bool configureHook();
    bool startHook();
    void updateHook(); 
    void stopHook();
    void cleanupHook();

    // Operations
    void calibrate_position(std::vector<double> &actual_positions);

    void set_velocity_warn(unsigned int thresh);
    void set_velocity_fault(unsigned int thresh);
    void set_torque_warn(unsigned int thresh);
    void set_torque_fault(unsigned int thresh);

  private:
    void cleanup_internal();

    // Hardware hooks
    boost::scoped_ptr<leoCAN::RTSocketCAN> canbus_;
    boost::scoped_ptr<barrett_direct::WAM> robot_;

    // Working variables
    bool needs_calibration_;
    KDL::JntArray torques_;
    KDL::JntArray positions_;
    KDL::JntArray positions_new_;
    KDL::JntArray velocities_;
    sensor_msgs::JointState joint_state_;
    RTT::os::TimeService::ticks joint_state_pub_time_;
    RTT::os::TimeService::ticks last_loop_time_;
  };
}


#endif // ifndef __BARD_COMPONENTS_WAM_H
