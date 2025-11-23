cmake_minimum_required(VERSION 3.8)
project(final)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(OpenCV REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(ament_index_cpp REQUIRED)

find_package(aim_interfaces REQUIRED)

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

add_executable(final src/final.cpp)
add_executable(testnode src/testnode.cpp)

ament_target_dependencies(final sensor_msgs cv_bridge OpenCV aim_interfaces rclcpp)
ament_target_dependencies(testnode sensor_msgs cv_bridge OpenCV aim_interfaces rclcpp)

install(TARGETS final DESTINATION lib/${PROJECT_NAME})
install(TARGETS testnode DESTINATION lib/${PROJECT_NAME})
#install(DIRECTORY src/video_source/ DESTINATION share/${PROJECT_NAME}/video_source)
install(DIRECTORY src/image/ DESTINATION share/${PROJECT_NAME}/srcimmage)

target_link_libraries(final ${OpenCV_LIBS})
target_link_libraries(final ${aim_interfaces_LIBRARIES})



ament_package()
