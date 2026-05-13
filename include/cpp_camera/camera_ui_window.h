#include <QWidget>
#include <QScreen>
#include <QRect>
#include <QColor>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QVariantAnimation>
#include <QParallelAnimationGroup>
#include <QResizeEvent>
#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QImage>
#include <QTimer>
#include <QSignalBlocker>
#include <QFileDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>

#include <math.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cv_bridge/cv_bridge.h>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <rcl_interfaces/msg/parameter_event.hpp>
#include <ament_index_cpp/get_package_share_directory.hpp>

using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::to_string;
using std::exp;
using std::min;
using std::max;
using std::round;

namespace cpp_camera {

class CameraWindow : public QWidget {
  Q_OBJECT
public:
  CameraWindow(rclcpp::Node::SharedPtr& node_handle, rclcpp::Node::SharedPtr& node_handle_helper, string cam_node_name, QWidget* parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
  ~CameraWindow();

protected:
  void paintEvent(QPaintEvent* event);
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void onUpdate();
  void camUpdate();

private:
  void paramEventCalb(const rcl_interfaces::msg::ParameterEvent::ConstSharedPtr event);
  void rawImgCalb(const sensor_msgs::msg::Image::ConstSharedPtr msg);
  void compImgCalb(const sensor_msgs::msg::CompressedImage::ConstSharedPtr msg);
  void expandPanel();
  void expandComp();
  void exportAnim();
  void updatePrevButtonStyle(double t);
  void updateExportButtonStyle(double t);
  void paramsGet();
  void onFlipHChanged(int state);
  void onFlipVChanged(int state);
  void onVCamChanged();
  void onFPSChanged();
  void onModeChanged(int value);
  void onCompChanged(int state);
  void onFormatChanged(int value);
  void onQualChanged();
  void saveConfig();

  rclcpp::Parameter paramGet(string name);
  rclcpp::Node::SharedPtr pns_, pns_helper_;
  rclcpp::Subscription<rcl_interfaces::msg::ParameterEvent>::SharedPtr param_event_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr img_sub_;
  rclcpp::Subscription<sensor_msgs::msg::CompressedImage>::SharedPtr comp_img_sub_;
  std::shared_ptr<rclcpp::SyncParametersClient> param_client_, param_setter_client_;
  std::vector<rclcpp::Parameter> param_list_;

  QImage frame_;
  QTimer *update_timer_, *cam_timer_;
  QScreen *screen_;
  QBoxLayout *main_lay_, *set_flip_h_lay_, *set_flip_v_lay_;
  QVBoxLayout *set_lay_, *prev_lay_, *set_comp_i_lay_;
  QHBoxLayout *set_title_lay_, *set_prev_btn_lay_, *prev_title_lay_, *set_flip_lay_, *set_vcam_lay_, *set_vcam_i_lay_, *set_fps_lay_, *set_fps_i_lay_, *set_mode_lay_, *set_mode_i_lay_, *set_comp_lay_, *set_comp_i_top_lay_, *set_comp_i_bot_lay_, *set_qual_lay_, *set_expt_btn_lay_;
  QWidget *set_wid_, *prev_wid_, *set_title_wid_, *s1_wid_, *prev_title_wid_, *set_flip_h_wid_, *set_flip_v_wid_, *set_vcam_wid_, *set_fps_wid_, *set_mode_wid_, *set_comp_wid_, *set_qual_wid_;
  QPushButton *prev_btn_, *expt_btn_;
  QGraphicsDropShadowEffect *glow1_, *glow2_, *glow3_, *glow4_, *glow5_, *glow6_, *glow7_, *glow8_, *glow9_, *glow10_, *glow11_, *glow12_;
  QPropertyAnimation *prev_btn_anim_, *comp_anim_, *comp_exp_anim_, *comp_spc_;
  QVariantAnimation *prev_btn_fade_anim_, *export_btn_fade_anim_;
  QParallelAnimationGroup *prev_btn_anim_grp_, *comp_anim_grp_;
  QLabel *set_icn_lbl_, *set_txt_lbl_, *cam_lbl_, *prev_icn_lbl_, *prev_txt_lbl_, *set_vcam_lbl_, *set_fps_lbl_, *set_mode_lbl_, *set_comp_lbl_, *set_qual_lbl_, *prev_fps_lbl_;
  QPixmap *conf_icn_, *prev_icn_;
  QCheckBox *flip_h_chb_, *flip_v_chb_, *comp_chb_;
  QSpinBox *vcam_spb_, *fps_spb_, *qual_spb_;
  QComboBox *mode_cb_, *comp_cb_;

  int width_, heigh_, set_spc_;
  string images_path_, cam_node_name_, src_dir_;
  bool expanded_, comp_, reverse_done_;

  std::chrono::time_point<std::chrono::high_resolution_clock> t_0_, t_i_;
  std::chrono::duration<double> t_;
};

}
