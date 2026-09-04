#ifndef CS_BT_CONTROLLER_TREE_EXECUTION_CLIENT_HPP_
#define CS_BT_CONTROLLER_TREE_EXECUTION_CLIENT_HPP_

#include <behaviortree_cpp/basic_types.h>
#include <behaviortree_ros2/bt_action_node.hpp>
#include <btcpp_ros2_interfaces/action/execute_tree.hpp>
#include <btcpp_ros2_interfaces/msg/node_status.hpp>


class TreeExecutionClient : public BT::RosActionNode<btcpp_ros2_interfaces::action::ExecuteTree>
{
public:
  explicit TreeExecutionClient(const std::string& name, const BT::NodeConfig& config,
               const BT::RosNodeParams& params)
    : BT::RosActionNode<btcpp_ros2_interfaces::action::ExecuteTree>(name, config, params)
  {}

  /**
   * @brief Required method that creates list of BT ports
   *
   * Must call providedBasicPorts in it.
   *
   * @return PortsList Containing basic ports along with node-specific ports
   */
  static BT::PortsList providedPorts()
  {    
    return providedBasicPorts({ 
        // TODO: make this node responsible for updating the available behavior trees?
        // BT::InputPort("xml", "Optional XML source file to be run."),
        BT::InputPort("target_tree", "", "Name of the behavior tree to execute"),
        BT::InputPort("payload", "An optional, implementation-dependent payload for the target tree"),
        BT::OutputPort("return_message", "The result payload or error message")
        });
  }

  /** 
   * @brief Required callback that allows the user to set the goal message.
   *
   * @param goal The goal to be sent to the action server.
   *
   * @return false if the request should not be sent. In that case,
   * RosActionNode::onFailure(INVALID_GOAL) will be called.
   */
  bool setGoal(Goal& goal) override;

  /** 
   * @brief Required callback invoked when the result is received by the server.
   * 
   * It is up to the user to define if the action returns SUCCESS or FAILURE.
   */
  BT::NodeStatus onResultReceived(const WrappedResult& wr) override;

  /** 
   * @brief Optional callback invoked when the feedback is received.
   * 
   * It generally returns RUNNING, but the user can also use this callback to cancel the
   * current action and return SUCCESS or FAILURE.
   */
  BT::NodeStatus onFeedback(const std::shared_ptr<const Feedback> feedback) override;

  /**
   * @brief Optional callback invoked when something goes wrong. 
   *
   * It must return either SUCCESS or FAILURE.
   */
  BT::NodeStatus onFailure(BT::ActionNodeErrorCode error) override;

  /**
   * @brief Optional callback executed when the node is halted. 
   *
   * Note that cancelGoal() is done automatically.
   */
  void onHalt() override;
};


inline const char* toStr(const uint8_t& status)
{
  switch(status)
  {
    case btcpp_ros2_interfaces::msg::NodeStatus::IDLE:
      return "IDLE";
    case btcpp_ros2_interfaces::msg::NodeStatus::RUNNING:
      return "RUNNING";
    case btcpp_ros2_interfaces::msg::NodeStatus::SUCCESS:
      return "SUCCESS";
    case btcpp_ros2_interfaces::msg::NodeStatus::FAILURE:
      return "FAILURE";
    case btcpp_ros2_interfaces::msg::NodeStatus::SKIPPED:
      return "SKIPPED";
  }
  return nullptr;
}

#endif
