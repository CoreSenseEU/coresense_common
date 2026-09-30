#include <behaviortree_cpp/basic_types.h>
#include <behaviortree_ros2/bt_utils.hpp>
#include <vector>
#include <unistd.h>

#include "coresense_bt_controller/bt_controller.hpp"


void BTController::registerNodesIntoFactory(BT::BehaviorTreeFactory& factory)
{
  std::vector<std::string> external_directories;

  // Get the values if they are set
  if (node()->get_parameter<std::vector<std::string>>("external_behavior_trees", external_directories))
  {

    for (auto directory_path : external_directories)
    {
      BT::LoadBehaviorTrees(factory, directory_path);
    }
  }
}

void BTController::registerBehaviorTreeCB(
  std::shared_ptr<rclcpp::Service<coresense_msgs::srv::RegisterBehaviorTree>> register_behavior_tree_server_ptr,
  const std::shared_ptr<rmw_request_id_t> request_header,
  const std::shared_ptr<coresense_msgs::srv::RegisterBehaviorTree::Request> request)
{
  RCLCPP_INFO(node()->get_logger(), "Registering tree:\n%s", request->tree.c_str());
  factory().registerBehaviorTreeFromText(request->tree);
  coresense_msgs::srv::RegisterBehaviorTree::Response response;
  response.success = true;
  register_behavior_tree_server_ptr->send_response(*request_header, response);
}


void BTController::onTreeCreated(BT::GoalResources& session)
{
  RCLCPP_INFO(node()->get_logger(), "%s: onTreeCreated: tree %s created successfully.",
      node()->get_name(), session.tree.rootNode()->name().c_str());

  globalBlackboard(session)->set("payload", goalPayload(session));
  //TODO move prolog stuff to somewhere? decision subtree?
  //globalBlackboard(session)->set("prolog_query", node()->get_parameter("prolog_query_service").as_string());
  //globalBlackboard(session)->set("prolog_assert", node()->get_parameter("prolog_assert_topic").as_string());
  //globalBlackboard(session)->set("prolog_retract", node()->get_parameter("prolog_retract_topic").as_string());
  globalBlackboard(session)->createEntry("return_message", BT::TypeInfo::Create<std::string>());

  std::set<std::string> keys;
  RCLCPP_INFO(node()->get_logger(), "Root node blackboard");
  for (auto key : session.tree.rootBlackboard()->getKeys()) {
    std::string key_str = std::string(key);
    if (key_str.rfind("MODELET_", 0) == 0) {
      RCLCPP_INFO(node()->get_logger(), "Collecting input: %s", key_str.c_str());
    } else {
      //RCLCPP_INFO(node()->get_logger(), "Not getting %s", key_str.c_str());
    }
  }
}



bool BTController::onGoalReceived(const std::string& tree_name, const std::string& payload)
{
  RCLCPP_INFO(node()->get_logger(), "%s: onGoalRecieved: {tree_name: '%s'; payload: '%s'.",
      node()->get_name(), tree_name.c_str(), payload.c_str());

  return true;
}

std::optional<std::string> BTController::onTreeExecutionCompleted(
    BT::NodeStatus status, bool was_cancelled, BT::GoalResources& session)
{
  RCLCPP_DEBUG(node()->get_logger(), 
      "%s: onTreeExecutionCompleted: {status: '%s'; was_cancelled: '%s'}",
      node()->get_name(), toStr(status), was_cancelled ? "true" : "false");

  std::string message;
  if (status == BT::NodeStatus::SUCCESS && !was_cancelled)
  {
    try
    {
      if (globalBlackboard(session)->get("return_message", message))
      {
        return message;
      } else {
        RCLCPP_DEBUG(node()->get_logger(), 
            "%s: onTreeExecutionCompleted: No return message from global blackboard",
            node()->get_name());
      }
    } 
    catch (std::runtime_error& e)
    {
      RCLCPP_ERROR(node()->get_logger(), 
          "%s: onTreeExecutionCompleted: %s",
          node()->get_name(), e.what());
    }
  }
  return std::nullopt;
}


int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  RCLCPP_INFO(rclcpp::get_logger("Initializing CoreSense BT Controller"), "pid: %i", getpid());

  rclcpp::NodeOptions options;
  auto action_server = std::make_shared<BTController>(options);

  // TODO: This workaround is for a bug in MultiThreadedExecutor where it can deadlock when spinning without a timeout.
  // Deadlock is caused when Publishers or Subscribers are dynamically removed as the node is spinning.
  rclcpp::executors::MultiThreadedExecutor exec(rclcpp::ExecutorOptions(), 0, false,
                                                std::chrono::milliseconds(250));
  exec.add_node(action_server->node());
  exec.spin();
  RCLCPP_INFO(rclcpp::get_logger("Stopping CoreSense BT Controller"), "pid: %i", getpid());
  exec.remove_node(action_server->node());

  rclcpp::shutdown();
}
