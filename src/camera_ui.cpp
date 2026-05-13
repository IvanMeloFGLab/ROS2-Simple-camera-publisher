#include <QApplication>

#include <rclcpp/rclcpp.hpp>
#include <chrono>
#include "cpp_camera/camera_ui_window.h"

using std::string;

class mApp : public QApplication {
public:
  rclcpp::Node::SharedPtr psn_, psn_helper_;
  string cam_node_name_ = "camera_publisher";

  explicit mApp(int& argc, char** argv) : QApplication(argc, argv) {
    rclcpp::init(argc, argv);
    psn_ = rclcpp::Node::make_shared("camera_ui");
    psn_helper_ = rclcpp::Node::make_shared("camera_ui_helper");
  }

  ~mApp() {
    rclcpp::shutdown();
  }

  bool checkForCamNode(){
    rclcpp::sleep_for(std::chrono::milliseconds(1500));

    auto graph = psn_->get_node_graph_interface();

    bool found_node = false;
    for (auto &pair : graph->get_node_names_and_namespaces()) {
      RCLCPP_INFO(psn_->get_logger(), "Node found: %s", pair.first.c_str());
      if (pair.first == cam_node_name_) {
        found_node = true;
        break;
      }
    }

    auto topics = psn_->get_topic_names_and_types();
    bool found_topic = topics.find("/camera/image_raw") == topics.end();

    return found_node && found_topic;

  }

  int exec() {

    if(!checkForCamNode()){
      RCLCPP_ERROR(psn_->get_logger(), "Camera publisher node not running or found. Shutting down.");
      return -1;
    }

    cpp_camera::CameraWindow CameraWin(psn_, psn_helper_, cam_node_name_);
    CameraWin.show();

    return QApplication::exec();
  }
};

int main(int argc, char** argv) {
  mApp app(argc, argv);
  return app.exec();
}
