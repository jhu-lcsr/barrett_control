#ifndef __BARD_COMPONENTS_CONTROLLERS_JOINT_TRAJECTORY
#define __BARD_COMPONENTS_CONTROLLERS_JOINT_TRAJECTORY

#include <iostream>

#include <boost/scoped_ptr.hpp>

#include <rtt/RTT.hpp>
#include <rtt/Port.hpp>

#include <kdl/jntarray.hpp>
#include <kdl/jntarrayvel.hpp>
#include <kdl/jntarrayacc.hpp>
#include <kdl/tree.hpp>
#include <kdl/chain.hpp>
#include <kdl/chainidsolver_recursive_newton_euler.hpp>

#include <trajectory_msgs/JointTrajectory.h>
#include <trajectory_msgs/JointTrajectoryPoint.h>

namespace bard_components {
  namespace controllers {
    class JointTrajectory : public RTT::TaskContext
    {
      // RTT Properties
      std::string robot_description_;
      std::string root_link_;
      std::string tip_link_;

      // RTT Ports
      RTT::InputPort<KDL::JntArrayVel> positions_in_port_;
      RTT::InputPort<trajectory_msgs::JointTrajectory> trajectories_in_port_;
      RTT::OutputPort<KDL::JntArrayVel> positions_out_port_;

    public:
      JointTrajectory(std::string const& name);
      bool configureHook();
      bool startHook();
      void updateHook();
      void stopHook();
      void cleanupHook();

      void feedback_cb();
      void command_cb();
      bool within_error(KDL::JntArray &pos_des);

      // Structure describing a quintic spline
      struct Spline {
        std::vector<double> coef;
        Spline() : coef(6, 0.0) {}
      };

      // Structure describing an n-DOF trajectory segment
      struct Segment {
        size_t traj_index;
        size_t index;
        double end_time;
        double duration;
        std::vector<Spline> splines;
      };
      typedef std::list<Segment> SplineTrajectory;

    private:
      // The active trajectory (list of list of splines)
      SplineTrajectory spline_traj_;
      SplineTrajectory::iterator active_segment_it_;
      ros::Time last_time_;
      RTT::os::MutexRecursive traj_cmd_mutex_;

      // Working variables
      unsigned int n_dof_;
      KDL::Chain kdl_chain_;
      KDL::Tree kdl_tree_;
      urdf::Model urdf_model_;
      std::vector<boost::shared_ptr<const urdf::Joint> > joints_;

      KDL::JntArrayVel positions_;
      KDL::JntArrayVel positions_des_;
      KDL::JntArrayAcc pva_des_;

      size_t traj_count_;

      std::vector<double> velocity_limits_;

      trajectory_msgs::JointTrajectory new_trajectory_;
      //trajectory_msgs::JointTrajectoryPoint last_point_;
      //std::list<trajectory_msgs::JointTrajectoryPoint> traj_points_;
    };
  }
}


#endif // ifndef __BARD_COMPONENTS_CONTROLLERS_JOINT_TRAJECTORY

