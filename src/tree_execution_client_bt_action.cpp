#include <behaviortree_ros2/plugins.hpp>
#include <btcpp_ros2_interfaces/msg/node_status.hpp>

#include "coresense_bt_controller/tree_execution_client_bt_action.hpp"


bool TreeExecutionClient::setGoal(Goal& goal)
{
  // TODO: abstract away this call so that code isn't copied
  if (!getInput("payload", goal.payload))
  {
    RCLCPP_ERROR(logger(), "%s: setGoal with error: no blackboard entry for {%s}", 
        name().c_str(), "payload");
    return false;
  }
  if (!getInput("target_tree", goal.target_tree))
  {
    RCLCPP_ERROR(logger(), "%s: setGoal with error: no blackboard entry for {%s}", 
        name().c_str(), "target_tree");
    return false;
  }

  return true;
}


BT::NodeStatus TreeExecutionClient::onResultReceived(const WrappedResult& wr)
{
  if(wr.result->node_status.status == btcpp_ros2_interfaces::msg::NodeStatus::SUCCESS)
  {
    setOutput("return_message", wr.result->return_message);
    return BT::NodeStatus::SUCCESS;
  } else {
    RCLCPP_ERROR(logger(), "%s: onResultRecieved exited with status {%s} and message: {%s}", 
        name().c_str(), toStr(wr.result->node_status.status), wr.result->return_message.c_str());
    return BT::NodeStatus::FAILURE;
  }
}


BT::NodeStatus TreeExecutionClient::onFeedback(const std::shared_ptr<const Feedback> feedback)
{
  RCLCPP_INFO(logger(), "%s: onFeedback: {%s}",
      name().c_str(), 
      feedback->message.c_str());
  return BT::NodeStatus::RUNNING;
}


BT::NodeStatus TreeExecutionClient::onFailure(BT::ActionNodeErrorCode error)
{
  RCLCPP_ERROR(logger(), "%s: onFailure with error: %s", name().c_str(), toStr(error));
  return BT::NodeStatus::FAILURE;
}


void TreeExecutionClient::onHalt()
{
  RCLCPP_WARN(logger(), "%s: onHalt with warning: goal was cancelled", name().c_str());
}

// Register this node as a plugin with the BT factory
CreateRosNodePlugin(TreeExecutionClient, "RunExternalSubTree");
