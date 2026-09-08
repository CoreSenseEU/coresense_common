cmake_minimum_required(VERSION 3.8)
project(coresense_bt_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# find dependencies
find_package(ament_cmake REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(behaviortree_ros2 REQUIRED)
find_package(btcpp_ros2_interfaces REQUIRED)
find_package(pluginlib REQUIRED)
find_package(coresense_msgs REQUIRED)
find_package(triplestar_msgs REQUIRED)

set(DEPENDENCIES
  ${btcpp_ros2_interfaces_TARGETS}
  behaviortree_ros2::behaviortree_ros2
)

################
### Add executables and plugins

# Behavior Tree execution server
add_executable(bt_controller src/bt_controller.cpp)
target_include_directories(bt_controller PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
#ament_target_dependencies(bt_controller ${DEPENDENCIES})
target_link_libraries(bt_controller
  ${DEPENDENCIES}
  ${coresense_msgs_TARGETS}
  ${triplestar_msgs_TARGETS}
)


# TreeExecutionClient BT::ActionNode (Plugin version)
add_library(tree_execution_client_plugin SHARED src/tree_execution_client_bt_action.cpp)
target_include_directories(tree_execution_client_plugin PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_definitions(tree_execution_client_plugin PRIVATE BT_PLUGIN_EXPORT)
#ament_target_dependencies(tree_execution_client_plugin ${DEPENDENCIES})
target_link_libraries(tree_execution_client_plugin
  ${DEPENDENCIES}
)

###############
### Install targets and directories
install(TARGETS
  bt_controller
  tree_execution_client_plugin
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  behavior_trees
  config
  launch
  DESTINATION share/${PROJECT_NAME}
)

# Install plugins for other packages to load
install(TARGETS
  tree_execution_client_plugin
  LIBRARY DESTINATION share/${PROJECT_NAME}/bt_plugins
  ARCHIVE DESTINATION share/${PROJECT_NAME}/bt_plugins
  RUNTIME DESTINATION share/${PROJECT_NAME}/bt_plugins
)

# Install headers for plugins
install(DIRECTORY
  include/
  DESTINATION include
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# Export plugin dependencies for downstream packages
ament_export_dependencies(${DEPENDENCIES})
ament_export_include_directories(
  include
)

ament_package()
