#ifndef CS_BT_CONTROLLER_BT_CONTROLLER_HPP_
#define CS_BT_CONTROLLER_BT_CONTROLLER_HPP_

#include <behaviortree_cpp/basic_types.h>
#include <behaviortree_ros2/tree_execution_server.hpp>
#include <rclcpp/rclcpp.hpp>
#include "coresense_msgs/srv/register_behavior_tree.hpp"
#include "triplestar_msgs/srv/sparql_query.hpp"

#include <string>
#include <optional>
#include <memory>


class BTController : public BT::TreeExecutionServer
{
public:
  rclcpp::Service<coresense_msgs::srv::RegisterBehaviorTree>::SharedPtr register_behavior_tree_server_ptr;

  BTController(const rclcpp::NodeOptions& options)
    : TreeExecutionServer(std::make_unique<rclcpp::Node>("bt_controller", options))
  {
    //TODO: get a kb client
    get_modelets_client_ptr = node()->create_client<triplestar_msgs::srv::SPARQLQuery>("/triplestar/query/get_modelets");
    // TODO: remove this from the blackboard and just set the query service in AssembleDecisionHeuristic
    //this->node()->declare_parameter("prolog_query_service", "/query");
    //this->node()->declare_parameter("prolog_assert_topic", "/assert");
    //this->node()->declare_parameter("prolog_retract_topic", "/retract");
    register_behavior_tree_server_ptr = node()->create_service<coresense_msgs::srv::RegisterBehaviorTree>("/bt_controller/register_bt", std::bind(&BTController::registerBehaviorTreeCB, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    node()->declare_parameter("external_behavior_trees", rclcpp::ParameterType::PARAMETER_STRING_ARRAY);
  }

  void registerBehaviorTreeCB(
  std::shared_ptr<rclcpp::Service<coresense_msgs::srv::RegisterBehaviorTree>> register_behavior_tree_server_ptr,
  const std::shared_ptr<rmw_request_id_t> request_header,
  const std::shared_ptr<coresense_msgs::srv::RegisterBehaviorTree::Request> request);

protected:
  rclcpp::Client<triplestar_msgs::srv::SPARQLQuery>::SharedPtr get_modelets_client_ptr;
  /**
   * @brief Optional callback invoked when a goal is received and before the
   * tree is created. If it returns false, the goal will be rejected.
   */
  bool onGoalReceived(const std::string& tree_name, const std::string& payload) override;
  void set_blackboard_from_kb(BT::GoalResources& session, std::string key_str);


  /**
   * @brief Optional callback invoked after the tree is created.
   * It can be used, for instance, to initialize a logger or the global blackboard.
   *
   * @param session The session resources specific to the goal
   */
  void onTreeCreated(BT::GoalResources& session) override;


  /**
   * @brief Optional callback invoked after the plugins were registered into
   * the BT::BehaviorTreeFactory.
   * It can be used to register additional custom nodes manually.
   *
   * @param factory The factory to use to register nodes
   */
  void registerNodesIntoFactory(BT::BehaviorTreeFactory& factory) override;

  /**
   * @brief Optional method invoked at each loop, after tree.tickOnce().
   * If it returns a valid NodeStatus, the tree will stop and return that status.
   * Return std::nullopt to continue the execution.
   *
   * @param status The status of the tree after the last tick
   */
  // std::optional<BT::NodeStatus> onLoopAfterTick(BT::NodeStatus status) override;

  /**
   * @brief Optional callback invoked after the tree execution is completed,
   * i.e. if it returned SUCCESS/FAILURE or if the action was cancelled by the Action Client.
   *
   * @param status The status of the tree after the last tick
   * @param was_cancelled True if the action was cancelled by the Action Client
   *
   * @return if not std::nullopt, the string will be sent as [return_message] to the Action Client.
   */
  std::optional<std::string> onTreeExecutionCompleted(BT::NodeStatus status, bool was_cancelled, BT::GoalResources& session) override;

  /**
   * @brief onLoopFeedback is a callback invoked at each loop, after tree.tickOnce().
   * If it returns a valid string, it will be sent as feedback to the Action Client.
   *
   * If you don't want to return any feedback, return std::nullopt.
   */
  // std::optional<std::string> onLoopFeedback() override;
};


inline const char* toStr(const BT::NodeStatus& status)
{
  switch(status)
  {
    case BT::NodeStatus::IDLE:
      return "IDLE";
    case BT::NodeStatus::RUNNING:
      return "RUNNING";
    case BT::NodeStatus::SUCCESS:
      return "SUCCESS";
    case BT::NodeStatus::FAILURE:
      return "FAILURE";
    case BT::NodeStatus::SKIPPED:
      return "SKIPPED";
  }
  return nullptr;
}

#endif
