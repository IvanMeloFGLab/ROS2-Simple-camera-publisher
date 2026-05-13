#include "cpp_camera/camera_ui_window.h"

using clk = std::chrono::high_resolution_clock;

namespace cpp_camera {

CameraWindow::CameraWindow(rclcpp::Node::SharedPtr& node_handle, rclcpp::Node::SharedPtr& node_handle_helper, string cam_node_name, QWidget* parent, Qt::WindowFlags f): QWidget(parent, f), set_spc_(1), src_dir_(CPP_CAMERA_SOURCE_DIR), reverse_done_(false) {

  /*-----------------------------------------ROS2 Declarations---------------------------------------------*/
  pns_ = node_handle;
  pns_helper_ = node_handle_helper;
  cam_node_name_= cam_node_name;
  param_client_ = std::make_shared<rclcpp::SyncParametersClient>(pns_helper_, cam_node_name_);
  param_setter_client_ = std::make_shared<rclcpp::SyncParametersClient>(pns_helper_, cam_node_name_);
  paramsGet();
  param_event_sub_ = pns_->create_subscription<rcl_interfaces::msg::ParameterEvent>("/parameter_events", 10, std::bind(&CameraWindow::paramEventCalb, this, std::placeholders::_1));
  img_sub_ = pns_->create_subscription<sensor_msgs::msg::Image>("video_source/raw", 10, std::bind(&CameraWindow::rawImgCalb, this, std::placeholders::_1));
  comp_img_sub_ = pns_->create_subscription<sensor_msgs::msg::CompressedImage>("video_source/compressed", 10, std::bind(&CameraWindow::compImgCalb, this, std::placeholders::_1));
  comp_ = paramGet("Compression").as_bool();
  expanded_ = paramGet("preview").as_bool();

  images_path_ = (ament_index_cpp::get_package_share_directory("cpp_camera") + "/images/");
  /*-----------------------------------------ROS2 Declarations---------------------------------------------*/

  screen_ = screen();

  QRect main_size = screen_->geometry();
  int w = main_size.width()*.8; int h = main_size.height()*.8;
  resize(w, h);
  move((main_size.width() - w) / 2, (main_size.height() - h) / 2);
  setStyleSheet("background-color: #02031c;");

  /*******************************************MAIN WINDOW**********************************************/
  main_lay_ = new QBoxLayout(QBoxLayout::RightToLeft);


  /*--------------------------------------------Spacer------------------------------------------------*/
  s1_wid_ = new QWidget();
  s1_wid_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  s1_wid_->setMinimumWidth(0);
  s1_wid_->setMaximumWidth(0);
  main_lay_->addWidget(s1_wid_);
  /*--------------------------------------------Spacer------------------------------------------------*/



  /*-------------------------------------------Settings-----------------------------------------------*/
  set_wid_ = new QWidget();
  set_wid_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  set_wid_->setStyleSheet("background-color: #02031c;border: 2px solid #030550;border-radius: 15px;");
  glow1_ = new QGraphicsDropShadowEffect();
  glow1_->setOffset(0, 0);
  glow1_->setColor(QColor("#030550"));
  set_wid_->setGraphicsEffect(glow1_);

  set_lay_ = new QVBoxLayout();
  set_lay_->setContentsMargins(0, 0, 0, 0);
  set_lay_->setSpacing(0);

  set_title_wid_ = new QWidget();
  set_title_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-top-right-radius: 15px;border-top-left-radius: 15px;border-bottom-right-radius: 0;border-bottom-left-radius: 0;");
  glow2_ = new QGraphicsDropShadowEffect();
  glow2_->setOffset(0, 0);
  glow2_->setColor(QColor("#030550"));
  set_title_wid_->setGraphicsEffect(glow2_);

  set_title_lay_ = new QHBoxLayout();
  set_title_lay_->setContentsMargins(0, 0, 0, 0);
  set_title_lay_->setSpacing(0);
  set_icn_lbl_ = new QLabel();
  conf_icn_ = new QPixmap((images_path_+"conf.png").c_str());
  set_icn_lbl_->setStyleSheet("background-color: transparent;border: 0;border-radius: 0;");
  set_icn_lbl_->setAlignment(Qt::AlignCenter);
  set_title_lay_->addWidget(set_icn_lbl_, 1);
  set_txt_lbl_ = new QLabel("Settings");
  set_txt_lbl_->setStyleSheet("color: white;font-size: 15px;background-color: transparent;border: 0;border-radius: 0;");
  set_title_lay_->addWidget(set_txt_lbl_, 9);
  set_title_wid_->setLayout(set_title_lay_);

  set_lay_->addWidget(set_title_wid_, 8);
  set_lay_->addStretch(set_spc_);

  set_flip_lay_ = new QHBoxLayout();
  set_flip_h_wid_ = new QWidget();
  set_flip_h_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-radius: 15px;");
  glow5_ = new QGraphicsDropShadowEffect();
  glow5_->setOffset(0, 0);
  glow5_->setColor(QColor("#030550"));
  set_flip_h_wid_->setGraphicsEffect(glow5_);
  set_flip_h_lay_ = new QBoxLayout(QBoxLayout::TopToBottom);
  set_flip_h_lay_->setContentsMargins(10, 0, 10, 0);
  set_flip_h_lay_->setSpacing(0);
  flip_h_chb_ = new QCheckBox("Horizontal Flip");
  flip_h_chb_->setChecked(paramGet("Hflip").as_bool());
  flip_h_chb_->setCursor(Qt::PointingHandCursor);
  flip_h_chb_->setStyleSheet(
    "QCheckBox {"
    "    spacing: 8px;"
    "    color: white;"
    "    font-size: 13px;"
    "    border: 0"
    "}"
    "QCheckBox::indicator {"
    "    width: 40px;"
    "    height: 20px;"
    "}"
    "QCheckBox::indicator:unchecked {"
    "    border-radius: 10px;"
    "    background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 gray,stop:1 black);"
    "    border: 0;"
    "}"
    "QCheckBox::indicator:checked {"
    "    border-radius: 10px;"
    "    background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 hsl(213.12, 85.56%, 70.59%),stop:1 hsl(213.12, 85.56%, 35%));"
    "    border: 0;"
    "}"
  );
  set_flip_h_lay_->addWidget(flip_h_chb_);
  set_flip_h_wid_->setLayout(set_flip_h_lay_);
  set_flip_v_wid_ = new QWidget();
  set_flip_v_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-radius: 15px;");
  glow6_ = new QGraphicsDropShadowEffect();
  glow6_->setOffset(0, 0);
  glow6_->setColor(QColor("#030550"));
  set_flip_v_wid_->setGraphicsEffect(glow6_);
  set_flip_v_lay_ = new QBoxLayout(QBoxLayout::TopToBottom);
  set_flip_v_lay_->setContentsMargins(10, 0, 10, 0);
  set_flip_v_lay_->setSpacing(0);
  flip_v_chb_ = new QCheckBox("Vertical Flip");
  flip_v_chb_->setChecked(paramGet("Vflip").as_bool());
  flip_v_chb_->setCursor(Qt::PointingHandCursor);
  flip_v_chb_->setStyleSheet(
    "QCheckBox {"
    "    spacing: 8px;"
    "    color: white;"
    "    font-size: 13px;"
    "    border: 0"
    "}"
    "QCheckBox::indicator {"
    "    width: 40px;"
    "    height: 20px;"
    "}"
    "QCheckBox::indicator:unchecked {"
    "    border-radius: 10px;"
    "    background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 gray,stop:1 black);"
    "    border: 0;"
    "}"
    "QCheckBox::indicator:checked {"
    "    border-radius: 10px;"
    "    background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 hsl(213.12, 85.56%, 70.59%),stop:1 hsl(213.12, 85.56%, 35%));"
    "    border: 0;"
    "}"
  );
  set_flip_v_lay_->addWidget(flip_v_chb_);
  set_flip_v_wid_->setLayout(set_flip_v_lay_);
  set_flip_lay_->addWidget(set_flip_h_wid_);
  set_flip_lay_->addWidget(set_flip_v_wid_);
  set_lay_->addLayout(set_flip_lay_, 12);
  set_lay_->addStretch(set_spc_);

  set_vcam_lay_ = new QHBoxLayout();
  set_vcam_wid_ = new QWidget();
  set_vcam_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-radius: 15px;");
  glow7_ = new QGraphicsDropShadowEffect();
  glow7_->setOffset(0, 0);
  glow7_->setColor(QColor("#030550"));
  set_vcam_wid_->setGraphicsEffect(glow7_);
  set_vcam_i_lay_ = new QHBoxLayout();
  set_vcam_lbl_ = new QLabel("Virtual camera number:");
  set_vcam_lbl_->setStyleSheet("color: white;font-size: 13px;background-color: transparent;border: 0;border-radius: 0;");
  set_vcam_i_lay_->addWidget(set_vcam_lbl_, 7);
  vcam_spb_ = new QSpinBox();
  vcam_spb_->setRange(0, 1000);
  vcam_spb_->setValue(paramGet("VCam_num").as_int());
  vcam_spb_->setAlignment(Qt::AlignCenter);
  vcam_spb_->setStyleSheet(
    "QSpinBox {"
    "    background-color: #020330;"
    "    color: white;"
    "    border: 2px solid #030550;"
    "    border-radius: 10px;"
    "}"
    "QSpinBox::up-button, QSpinBox::down-button {"
    "    width: 0px;"
    "    border: none;"
    "}"
  );
  set_vcam_i_lay_->addWidget(vcam_spb_, 3);
  set_vcam_wid_->setLayout(set_vcam_i_lay_);
  set_vcam_lay_->addWidget(set_vcam_wid_);
  set_lay_->addLayout(set_vcam_lay_, 12);
  set_lay_->addStretch(set_spc_);

  set_fps_lay_ = new QHBoxLayout();
  set_fps_wid_ = new QWidget();
  set_fps_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-radius: 15px;");
  glow8_ = new QGraphicsDropShadowEffect();
  glow8_->setOffset(0, 0);
  glow8_->setColor(QColor("#030550"));
  set_fps_wid_->setGraphicsEffect(glow8_);
  set_fps_i_lay_ = new QHBoxLayout();
  set_fps_lbl_ = new QLabel("Sending FPS:");
  set_fps_lbl_->setStyleSheet("color: white;font-size: 13px;background-color: transparent;border: 0;border-radius: 0;");
  set_fps_i_lay_->addWidget(set_fps_lbl_, 7);
  fps_spb_ = new QSpinBox();
  fps_spb_->setRange(0, 1000);
  fps_spb_->setValue(paramGet("fps").as_int());
  fps_spb_->setAlignment(Qt::AlignCenter);
  fps_spb_->setStyleSheet(
    "QSpinBox {"
    "    background-color: #020330;"
    "    color: white;"
    "    border: 2px solid #030550;"
    "    border-radius: 10px;"
    "}"
    "QSpinBox::up-button, QSpinBox::down-button {"
    "    width: 0px;"
    "    border: none;"
    "}"
  );
  set_fps_i_lay_->addWidget(fps_spb_, 3);
  set_fps_wid_->setLayout(set_fps_i_lay_);
  set_fps_lay_->addWidget(set_fps_wid_);
  set_lay_->addLayout(set_fps_lay_, 12);
  set_lay_->addStretch(set_spc_);

  set_mode_lay_ = new QHBoxLayout();
  set_mode_wid_ = new QWidget();
  set_mode_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-radius: 15px;");
  glow9_ = new QGraphicsDropShadowEffect();
  glow9_->setOffset(0, 0);
  glow9_->setColor(QColor("#030550"));
  set_mode_wid_->setGraphicsEffect(glow9_);
  set_mode_i_lay_ = new QHBoxLayout();
  set_mode_lbl_ = new QLabel("Resolution mode:");
  set_mode_lbl_->setStyleSheet("color: white;font-size: 13px;background-color: transparent;border: 0;border-radius: 0;");
  set_mode_i_lay_->addWidget(set_mode_lbl_, 4);
  mode_cb_ = new QComboBox();
  if (paramGet("board").as_string() != "rasp") {
    mode_cb_->addItems(QStringList() << "640x480@30" << "640x480@60" << "1280x720@30" << "1280x720@60" << "1640x1232@30" << "1920x1080@30" << "3280x1848@28" << "3264x2464@21");
  } else {
    mode_cb_->addItems(QStringList() << "640x480@30" << "640x480@60" << "640x480@90" << "640x480@103" << "1640x1232@30" << "1640x1232@41" << "1920x1080@30" << "1920x1080@74");
  }
  mode_cb_->setCurrentIndex(paramGet("mode").as_int());
  mode_cb_->setStyleSheet(
    "QComboBox {"
    "    background-color: #020330;"
    "    color: white;"
    "    border: 2px solid #030550;"
    "    border-radius: 10px;"
    "}"
  );
  set_mode_i_lay_->addWidget(mode_cb_, 6);
  set_mode_wid_->setLayout(set_mode_i_lay_);
  set_mode_lay_->addWidget(set_mode_wid_);
  set_lay_->addLayout(set_mode_lay_, 12);
  set_lay_->addStretch(set_spc_);

  set_comp_lay_ = new QHBoxLayout();
  set_comp_wid_ = new QWidget();
  set_comp_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-radius: 15px;");
  glow10_ = new QGraphicsDropShadowEffect();
  glow10_->setOffset(0, 0);
  glow10_->setColor(QColor("#030550"));
  set_comp_wid_->setGraphicsEffect(glow10_);

  set_comp_i_lay_ = new QVBoxLayout();
  set_comp_i_lay_->setContentsMargins(0, 0, 0, 0);
  if (comp_) {
    set_comp_i_lay_->setSpacing(screen_->geometry().height()*.01);
  } else {
    set_comp_i_lay_->setSpacing(0);
  }
  set_comp_i_top_lay_ = new QHBoxLayout();
  set_comp_i_top_lay_->setSpacing(0);
  set_comp_lbl_ = new QLabel("Compression:");
  set_comp_lbl_->setStyleSheet("color: white;font-size: 13px;background-color: transparent;border: 0;border-radius: 0;");
  set_comp_i_top_lay_->addWidget(set_comp_lbl_, 3);
  comp_chb_ = new QCheckBox("");
  comp_chb_->setChecked(comp_);
  comp_chb_->setCursor(Qt::PointingHandCursor);
  comp_chb_->setStyleSheet(
    "QCheckBox {"
    "    spacing: 8px;"
    "    color: white;"
    "    font-size: 13px;"
    "    border: 0"
    "}"
    "QCheckBox::indicator {"
    "    width: 40px;"
    "    height: 20px;"
    "}"
    "QCheckBox::indicator:unchecked {"
    "    border-radius: 10px;"
    "    background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 gray,stop:1 black);"
    "    border: 0;"
    "}"
    "QCheckBox::indicator:checked {"
    "    border-radius: 10px;"
    "    background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 hsl(213.12, 85.56%, 70.59%),stop:1 hsl(213.12, 85.56%, 35%));"
    "    border: 0;"
    "}"
  );
  set_comp_i_top_lay_->addWidget(comp_chb_, 1);
  comp_cb_ = new QComboBox();
  comp_cb_->addItems(QStringList() << "JPEG" << "PNG" << "WEBP");
  comp_cb_->setCurrentIndex(paramGet("format").as_int());
  comp_cb_->setStyleSheet(
    "QComboBox {"
    "    background-color: #020330;"
    "    color: white;"
    "    border: 2px solid #030550;"
    "    border-radius: 10px;"
    "}"
  );
  if (comp_){
    comp_cb_->setMaximumWidth(screen_->geometry().width()*.05);
  } else {
    comp_cb_->setMaximumWidth(0);
  }
  set_comp_i_top_lay_->addWidget(comp_cb_, 2);

  set_comp_i_bot_lay_ = new QHBoxLayout();
  set_comp_i_bot_lay_->setSpacing(0);
  set_qual_wid_ = new QWidget();
  set_qual_lay_ = new QHBoxLayout();
  set_qual_lbl_ = new QLabel("Quality:");
  set_qual_lbl_->setStyleSheet("color: white;font-size: 13px;background-color: transparent;border: 0;border-radius: 0;");
  set_qual_lay_->addWidget(set_qual_lbl_, 7);
  qual_spb_ = new QSpinBox();
  switch (paramGet("format").as_int()) {
    case 0:
      qual_spb_->setRange(0, 100);
      qual_spb_->setValue(paramGet("JPEG_quality").as_int());
      break;
    case 1:
      qual_spb_->setRange(0, 9);
      qual_spb_->setValue(paramGet("PNG_quality").as_int());
      break;
    case 2:
      qual_spb_->setRange(0, 101);
      qual_spb_->setValue(paramGet("WEBP_quality").as_int());
      break;
  }
  qual_spb_->setAlignment(Qt::AlignCenter);
  qual_spb_->setStyleSheet(
    "QSpinBox {"
    "    background-color: #020330;"
    "    color: white;"
    "    border: 2px solid #030550;"
    "    border-radius: 10px;"
    "}"
    "QSpinBox::up-button, QSpinBox::down-button {"
    "    width: 0px;"
    "    border: none;"
    "}"
  );
  set_qual_lay_->addWidget(qual_spb_, 3);
  set_qual_lay_->setSpacing(0);
  set_qual_lay_->setContentsMargins(0, 0, 0, 0);
  set_qual_wid_->setStyleSheet("background-color: transparent;border: 0;border-radius: 0;");
  if (comp_) {
    set_qual_wid_->setMaximumHeight(screen_->geometry().height()*.05);
  } else {
    set_qual_wid_->setMaximumHeight(0);
  }

  set_qual_wid_->setLayout(set_qual_lay_);
  set_comp_i_bot_lay_->addWidget(set_qual_wid_);

  set_comp_i_lay_->addLayout(set_comp_i_top_lay_);
  set_comp_i_lay_->addLayout(set_comp_i_bot_lay_);
  set_comp_wid_->setLayout(set_comp_i_lay_);
  set_comp_lay_->addWidget(set_comp_wid_);
  set_lay_->addLayout(set_comp_lay_, 12);


  set_prev_btn_lay_ = new QHBoxLayout();
  set_prev_btn_lay_->setSpacing(0);
  prev_btn_ = new QPushButton("Camera Preview");
  prev_btn_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  QColor mblue, mblued;
  mblue.setHsvF(0.592, 0.8556, 0.7559);
  mblued.setHsvF(0.592, 0.8556, 0.35);
  if (expanded_) {
    prev_btn_->setStyleSheet(QString("color: white;font-size: 17px;background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 %1,stop:1 %2);border: 0;border-radius: 15px;").arg(mblued.name(), mblue.name()));
  } else {
    prev_btn_->setStyleSheet(QString("color: white;font-size: 17px;background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 %1,stop:1 %2);border: 0;border-radius: 15px;").arg(mblue.name(), mblued.name()));
  }
  glow11_ = new QGraphicsDropShadowEffect();
  glow11_->setOffset(0, 0);
  glow11_->setColor(QColor("#030550"));
  prev_btn_->setGraphicsEffect(glow11_);
  set_prev_btn_lay_->addWidget(prev_btn_);
  set_lay_->addLayout(set_prev_btn_lay_, 12);

  set_expt_btn_lay_ = new QHBoxLayout();
  set_expt_btn_lay_->setSpacing(0);
  expt_btn_ = new QPushButton("Export to launch file");
  expt_btn_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  QColor mgreen, mgreend;
  mgreen.setHsvF(0.45, 0.85, 0.7);
  mgreend.setHsvF(0.44, 0.85, 0.35);
  expt_btn_->setStyleSheet(QString("color: white;font-size: 17px;background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 %1,stop:1 %2);border: 0;border-radius: 15px;").arg(mgreen.name(), mgreend.name()));
  glow12_ = new QGraphicsDropShadowEffect();
  glow12_->setOffset(0, 0);
  glow12_->setColor(QColor("#035050"));
  expt_btn_->setGraphicsEffect(glow12_);
  set_expt_btn_lay_->addWidget(expt_btn_);
  set_lay_->addLayout(set_expt_btn_lay_, 12);

  set_wid_->setLayout(set_lay_);
  main_lay_->addWidget(set_wid_);
  /*-------------------------------------------Settings-----------------------------------------------*/



  /*--------------------------------------------Preview-----------------------------------------------*/
  prev_wid_ = new QWidget();
  prev_wid_->setStyleSheet("background-color: #02031c;border: 2px solid #030550;border-radius: 15px;");
  prev_wid_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  glow3_ = new QGraphicsDropShadowEffect();
  glow3_->setOffset(0, 0);
  glow3_->setColor(QColor("#030550"));
  prev_wid_->setGraphicsEffect(glow3_);
  if (expanded_) {
    prev_wid_->setMaximumWidth(screen_->geometry().width()*.9);
  } else {
    prev_wid_->setMaximumWidth(0);
  }

  prev_lay_ = new QVBoxLayout();
  prev_lay_->setContentsMargins(0, 0, 0, 0);
  prev_lay_->setSpacing(0);

  prev_title_wid_ = new QWidget();
  prev_title_wid_->setStyleSheet("background-color: #020330;border: 2px solid #030550;border-top-right-radius: 15px;border-top-left-radius: 15px;border-bottom-right-radius: 0;border-bottom-left-radius: 0;");
  glow4_ = new QGraphicsDropShadowEffect();
  glow4_->setOffset(0, 0);
  glow4_->setColor(QColor("#030550"));
  prev_title_wid_->setGraphicsEffect(glow4_);

  prev_title_lay_ = new QHBoxLayout();
  prev_title_lay_->setContentsMargins(0, 0, 0, 0);
  prev_title_lay_->setSpacing(0);
  prev_icn_lbl_ = new QLabel();
  prev_icn_ = new QPixmap((images_path_+"prev.png").c_str());
  prev_icn_lbl_->setStyleSheet("background-color: transparent;border: 0;border-radius: 0;");
  prev_icn_lbl_->setAlignment(Qt::AlignCenter);
  prev_title_lay_->addWidget(prev_icn_lbl_, 1);
  prev_txt_lbl_ = new QLabel("Preview");
  prev_txt_lbl_->setStyleSheet("color: white;font-size: 15px;background-color: transparent;border: 0;border-radius: 0;");
  prev_title_lay_->addWidget(prev_txt_lbl_, 14);
  prev_fps_lbl_ = new QLabel("Receive FPS: ");
  prev_fps_lbl_->setStyleSheet("color: white;font-size: 15px;background-color: transparent;border: 0;border-radius: 0;");
  prev_title_lay_->addWidget(prev_fps_lbl_, 5);

  prev_title_wid_->setLayout(prev_title_lay_);

  prev_lay_->addWidget(prev_title_wid_, 8);

  cam_lbl_ = new QLabel();
  cam_lbl_->setStyleSheet("background-color: transparent;border: 0;border-radius: 0;");
  cam_lbl_->setMinimumWidth(0);
  cam_lbl_->setAlignment(Qt::AlignCenter);
  prev_lay_->addWidget(cam_lbl_, 92);

  prev_wid_->setLayout(prev_lay_);

  main_lay_->addWidget(prev_wid_);
  /*--------------------------------------------Preview-----------------------------------------------*/


  prev_btn_anim_grp_ = new QParallelAnimationGroup();

  prev_btn_anim_ = new QPropertyAnimation(prev_wid_, "maximumWidth", this);
  prev_btn_anim_->setDuration(600);
  //prev_btn_anim_->setEasingCurve(QEasingCurve::OutCubic);

  prev_btn_fade_anim_ = new QVariantAnimation();
  prev_btn_fade_anim_ ->setDuration(400);
  connect(prev_btn_fade_anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
    CameraWindow::updatePrevButtonStyle(v.toDouble());
  });

  prev_btn_anim_grp_->addAnimation(prev_btn_anim_);
  prev_btn_anim_grp_->addAnimation(prev_btn_fade_anim_);

  connect(prev_btn_, &QPushButton::clicked, this, &CameraWindow::expandPanel);

  export_btn_fade_anim_ = new QVariantAnimation();
  export_btn_fade_anim_ ->setDuration(200);
  connect(export_btn_fade_anim_, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
    CameraWindow::updateExportButtonStyle(v.toDouble());
  });
  connect(export_btn_fade_anim_, &QVariantAnimation::finished, this, [this]() {
    if (!reverse_done_) {
      reverse_done_ = true;
      export_btn_fade_anim_->setStartValue(1.0);
      export_btn_fade_anim_->setEndValue(0.0);
      export_btn_fade_anim_->start();
    } else {
      reverse_done_ = false; // reset for next time
    }
  });
  connect(expt_btn_, &QPushButton::clicked, this, &CameraWindow::exportAnim);
  connect(expt_btn_, &QPushButton::clicked, this, &CameraWindow::saveConfig);

  comp_anim_grp_ = new QParallelAnimationGroup();

  comp_anim_ = new QPropertyAnimation(comp_cb_, "maximumWidth", this);
  comp_anim_->setDuration(500);
  comp_anim_grp_->addAnimation(comp_anim_);

  comp_exp_anim_ = new QPropertyAnimation(set_qual_wid_, "maximumHeight", this);
  comp_exp_anim_->setDuration(750);
  comp_anim_grp_->addAnimation(comp_exp_anim_);

  comp_spc_ = new QPropertyAnimation(set_comp_i_lay_, "spacing", this);
  comp_spc_->setDuration(750);
  comp_anim_grp_->addAnimation(comp_spc_);

  connect(comp_chb_, &QCheckBox::stateChanged, this, &CameraWindow::expandComp);

  setLayout(main_lay_);
  /*******************************************MAIN WINDOW**********************************************/

  update_timer_ = new QTimer(this);
  update_timer_->setInterval(5);
  update_timer_->start();

  cam_timer_ = new QTimer(this);
  cam_timer_->setInterval(16);
  cam_timer_->start();

  connect(cam_timer_, SIGNAL(timeout()), this, SLOT(camUpdate()));
  connect(update_timer_, SIGNAL(timeout()), this, SLOT(onUpdate()));

  connect(flip_h_chb_, &QCheckBox::stateChanged, this, &CameraWindow::onFlipHChanged);
  connect(flip_v_chb_, &QCheckBox::stateChanged, this, &CameraWindow::onFlipVChanged);
  connect(vcam_spb_, &QSpinBox::editingFinished, this, &CameraWindow::onVCamChanged);
  connect(fps_spb_, &QSpinBox::editingFinished, this, &CameraWindow::onFPSChanged);
  connect(mode_cb_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CameraWindow::onModeChanged);
  connect(comp_chb_, &QCheckBox::stateChanged, this, &CameraWindow::onCompChanged);
  connect(comp_cb_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CameraWindow::onFormatChanged);
  connect(qual_spb_, &QSpinBox::editingFinished, this, &CameraWindow::onQualChanged);

  RCLCPP_INFO(pns_->get_logger(), "Starting Camera Ui with node name %s.", pns_->get_fully_qualified_name());

  t_0_ = clk::now();
}

CameraWindow::~CameraWindow() {
  delete main_lay_;
  delete set_wid_;
  delete prev_wid_;
}

void CameraWindow::expandPanel() {
  prev_btn_anim_grp_->stop();

  expanded_=!expanded_;

  if (expanded_) {
    prev_btn_anim_->setStartValue(prev_wid_->maximumWidth());
    prev_btn_anim_->setEndValue(screen_->geometry().width()*.9);

    prev_btn_fade_anim_->setStartValue(0.0);
    prev_btn_fade_anim_->setEndValue(1.0);
  } else {
    prev_btn_anim_->setStartValue(prev_wid_->maximumWidth());
    prev_btn_anim_->setEndValue(0);

    prev_btn_fade_anim_->setStartValue(1.0);
    prev_btn_fade_anim_->setEndValue(0.0);
  }

  //param_setter_client_->set_parameters({rclcpp::Parameter("preview", expanded_)});  // Link to cam original preview

  prev_btn_anim_grp_->start();
}

void CameraWindow::expandComp() {
  comp_anim_grp_->stop();
  comp_=!comp_;

  if (comp_) {
    comp_anim_->setStartValue(comp_cb_->maximumWidth());
    comp_anim_->setEndValue(screen_->geometry().width()*.05);

    comp_exp_anim_->setStartValue(set_qual_wid_->maximumHeight());
    comp_exp_anim_->setEndValue(screen_->geometry().height()*.05);

    comp_spc_->setStartValue(0);
    comp_spc_->setEndValue(screen_->geometry().height()*.01);
  } else {
    comp_anim_->setStartValue(comp_cb_->maximumWidth());
    comp_anim_->setEndValue(0);

    comp_exp_anim_->setStartValue(set_qual_wid_->maximumHeight());
    comp_exp_anim_->setEndValue(0);

    comp_spc_->setStartValue(screen_->geometry().height()*.01);
    comp_spc_->setEndValue(0);
  }

  comp_anim_grp_->start();
}

void CameraWindow::exportAnim() {
  reverse_done_ = false;
  export_btn_fade_anim_->setStartValue(0.0);
  export_btn_fade_anim_->setEndValue(1.0);
  export_btn_fade_anim_->start();
}

void CameraWindow::updatePrevButtonStyle(double t) {
  QColor edge("#1a5fb4");
  QColor center("#1a5fb4");

  edge.setHsvF(0.592, 0.8556, 0.4 + 0.3559*(1.0-t));
  center.setHsvF(0.592, 0.8556, 0.35 + 0.3059*t);

  QString style = QString("color: white;font-size: 17px;background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 %1,stop:1 %2);border: 0;border-radius: 15px;").arg(edge.name(), center.name());

  prev_btn_->setStyleSheet(style);
}

void CameraWindow::updateExportButtonStyle(double t) {
  QColor edge("#1a5fb4");
  QColor center("#1a5fb4");

  edge.setHsvF(0.45, 0.85, 0.4 + 0.30*(1.0-t));
  center.setHsvF(0.44, 0.85, 0.35 + 0.3*t);

  QString style = QString("color: white;font-size: 17px;background-color: qradialgradient(cx:0.5,cy:0.5,radius:0.7,fx:0.5, fy:0.5,stop:0 %1,stop:1 %2);border: 0;border-radius: 15px;").arg(edge.name(), center.name());

  expt_btn_->setStyleSheet(style);
}

void CameraWindow::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  int w = event->size().width();
  int h = event->size().height();

  main_lay_->setSpacing(w*.02);
  main_lay_->setContentsMargins(w*.01, w*.01, 0, w*.01);

  set_prev_btn_lay_->setContentsMargins(w*.02, h*.03, w*.02, h*.01);
  set_expt_btn_lay_->setContentsMargins(w*.02, h*.02, w*.02, h*.02);

  set_flip_lay_->setSpacing(w*.02);
  set_flip_lay_->setContentsMargins(w*.02, h*.02, w*.02, 0);

  set_vcam_lay_->setContentsMargins(w*.02, h*.02, w*.02, 0);
  set_vcam_i_lay_->setContentsMargins(w*.04, 0, w*.04, 0);
  set_vcam_i_lay_->setSpacing(w*.02);

  set_fps_lay_->setContentsMargins(w*.02, h*.02, w*.02, 0);
  set_fps_i_lay_->setContentsMargins(w*.04, 0, w*.04, 0);
  set_fps_i_lay_->setSpacing(w*.02);

  set_mode_lay_->setContentsMargins(w*.02, h*.02, w*.02, 0);
  set_mode_i_lay_->setContentsMargins(w*.04, 0, w*.04, 0);
  set_mode_i_lay_->setSpacing(w*.02);

  set_comp_lay_->setContentsMargins(w*.02, h*.02, w*.02, 0);
  set_comp_i_lay_->setContentsMargins(w*.04, 0, w*.04, 0);

  set_comp_i_top_lay_->setContentsMargins(0, h*.02, 0, 0);
  set_comp_i_bot_lay_->setContentsMargins(0, 0, 0, h*.02);

  if (w >= screen_->geometry().width()*.65) {
    set_wid_->setMinimumWidth(w*.37);
    set_wid_->setMaximumWidth(w*.37);

    cam_lbl_->setContentsMargins(w*(1.0-min(w/10000.0+0.8716, 0.98)), h*.02, w*.02, h*.02);
  } else {
    cam_lbl_->setContentsMargins(w*(min((w/10000.0)-0.03, 0.04)), h*.02, w*.02, h*.02);
  }
  cam_lbl_->setMaximumWidth(w*min(w/1600.0, 0.57));

  set_icn_lbl_->setPixmap(conf_icn_->scaled(h*.035, h*.035, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  prev_icn_lbl_->setPixmap(prev_icn_->scaled(w*.06, h*.06, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  glow1_->setBlurRadius(w*.08);
  glow2_->setBlurRadius(w*.08);
  glow3_->setBlurRadius(w*.08);
  glow4_->setBlurRadius(w*.08);
  glow5_->setBlurRadius(w*.02);
  glow6_->setBlurRadius(w*.02);
  glow7_->setBlurRadius(w*.02);
  glow8_->setBlurRadius(w*.02);
  glow9_->setBlurRadius(w*.02);
  glow10_->setBlurRadius(w*.02);
  glow11_->setBlurRadius(w*.02);
  glow12_->setBlurRadius(w*.02);
}

void CameraWindow::paramEventCalb(const rcl_interfaces::msg::ParameterEvent::ConstSharedPtr event) {
  QSignalBlocker blocker1(flip_h_chb_);
  QSignalBlocker blocker2(flip_v_chb_);
  QSignalBlocker blocker3(vcam_spb_);
  QSignalBlocker blocker4(fps_spb_);
  QSignalBlocker blocker5(mode_cb_);
  QSignalBlocker blocker6(comp_chb_);
  QSignalBlocker blocker7(comp_cb_);
  QSignalBlocker blocker8(qual_spb_);

  if (event->node == pns_->get_namespace() + cam_node_name_) {
    for (auto param : event->changed_parameters) {
      if (param.name == "Hflip") {
        flip_h_chb_->setChecked(param_client_->get_parameter<bool>(param.name));
      } else if (param.name == "Vflip") {
        flip_v_chb_->setChecked(param_client_->get_parameter<bool>(param.name));
      } else if (param.name == "VCam_num") {
        vcam_spb_->setValue(param_client_->get_parameter<int>(param.name));
      } else if (param.name == "fps") {
        fps_spb_->setValue(param_client_->get_parameter<int>(param.name));
      } else if (param.name == "mode") {
        mode_cb_->setCurrentIndex(param_client_->get_parameter<int>(param.name));
      } else if (param.name == "Compression") {
        comp_chb_->setChecked(param_client_->get_parameter<bool>(param.name));
      } else if (param.name == "format") {
        comp_cb_->setCurrentIndex(param_client_->get_parameter<int>(param.name));
        switch (param_client_->get_parameter<int>(param.name)) {
          case 0:
            qual_spb_->setRange(0, 100);
            qual_spb_->setValue(param_client_->get_parameter<int>("JPEG_quality"));
            break;
          case 1:
            qual_spb_->setRange(0, 9);
            qual_spb_->setValue(param_client_->get_parameter<int>("PNG_quality"));
            break;
          case 2:
            qual_spb_->setRange(0, 101);
            qual_spb_->setValue(param_client_->get_parameter<int>("WEBP_quality"));
            break;
        }
      } else if (param.name == "JPEG_quality") {
        if (comp_cb_->currentIndex() == 0) qual_spb_->setValue(param_client_->get_parameter<int>(param.name));
      } else if (param.name == "PNG_quality") {
        if (comp_cb_->currentIndex() == 1) qual_spb_->setValue(param_client_->get_parameter<int>(param.name));
      } else if (param.name == "WEBP_quality") {
        if (comp_cb_->currentIndex() == 2) qual_spb_->setValue(param_client_->get_parameter<int>(param.name));
      }
    }
  }
}

void CameraWindow::onUpdate() {
  if (!rclcpp::ok()) {
    close();
    return;
  }

  rclcpp::spin_some(pns_);
}

void CameraWindow::camUpdate() {
  if (expanded_) {
    cam_lbl_->setPixmap(QPixmap::fromImage(frame_).scaled(cam_lbl_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    prev_fps_lbl_->setText(QString("Receive FPS: %1").arg(round(100.0 / t_.count())/100.0));
  }
}

void CameraWindow::paintEvent(QPaintEvent *) {

}

void CameraWindow::paramsGet() {
  auto list = param_client_->list_parameters({}, 10);
  param_list_ = param_client_->get_parameters(list.names);

  /*for (const auto &name : list.names) {
    RCLCPP_INFO(pns_->get_logger(), "Param: %s", name.c_str());
  }*/
}

rclcpp::Parameter CameraWindow::paramGet(string name) {
  for (const auto &param : param_list_) {
    if (param.get_name() == name) return param;
  }
  RCLCPP_ERROR(pns_->get_logger(), "Param %s not found.", name.c_str());
  return rclcpp::Parameter();
}

void CameraWindow::rawImgCalb(const sensor_msgs::msg::Image::ConstSharedPtr msg){
  t_i_ = clk::now();
  t_ = t_i_ - t_0_;
  QImage img(msg->data.data(), msg->width, msg->height, QImage::Format_RGB888);
  frame_ = img.rgbSwapped();
  t_0_ = t_i_;
}

void CameraWindow::compImgCalb(const sensor_msgs::msg::CompressedImage::ConstSharedPtr msg){
  t_i_ = clk::now();
  t_ = t_i_ - t_0_;
  cv::Mat raw_data(1, msg->data.size(), CV_8UC1, const_cast<unsigned char*>(msg->data.data()));
  cv::Mat frame = cv::imdecode(raw_data, cv::IMREAD_COLOR);
  QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
  frame_ = img.rgbSwapped();
  t_0_ = t_i_;
}

void CameraWindow::onFlipHChanged(int state) {
  param_setter_client_->set_parameters({rclcpp::Parameter("Hflip", state == Qt::Checked)});
}

void CameraWindow::onFlipVChanged(int state) {
  param_setter_client_->set_parameters({rclcpp::Parameter("Vflip", state == Qt::Checked)});
}

void CameraWindow::onVCamChanged() {
  param_setter_client_->set_parameters({rclcpp::Parameter("VCam_num", vcam_spb_->value())});
}

void CameraWindow::onFPSChanged() {
  param_setter_client_->set_parameters({rclcpp::Parameter("fps", fps_spb_->value())});
}

void CameraWindow::onModeChanged(int value) {
  param_setter_client_->set_parameters({rclcpp::Parameter("mode", value)});
}

void CameraWindow::onCompChanged(int state) {
  param_setter_client_->set_parameters({rclcpp::Parameter("Compression", state == Qt::Checked)});
}

void CameraWindow::onFormatChanged(int value) {
  param_setter_client_->set_parameters({rclcpp::Parameter("format", value)});
  switch (value) {
    case 0:
      qual_spb_->setRange(0, 100);
      qual_spb_->setValue(param_client_->get_parameter<int>("JPEG_quality"));
      break;
    case 1:
      qual_spb_->setRange(0, 9);
      qual_spb_->setValue(param_client_->get_parameter<int>("PNG_quality"));
      break;
    case 2:
      qual_spb_->setRange(0, 101);
      qual_spb_->setValue(param_client_->get_parameter<int>("WEBP_quality"));
      break;
  }
}

void CameraWindow::onQualChanged() {
  switch (comp_cb_->currentIndex()) {
    case 0:
      param_setter_client_->set_parameters({rclcpp::Parameter("JPEG_quality", qual_spb_->value())});
      break;
    case 1:
      param_setter_client_->set_parameters({rclcpp::Parameter("PNG_quality", qual_spb_->value())});
      break;
    case 2:
      param_setter_client_->set_parameters({rclcpp::Parameter("WEBP_quality", qual_spb_->value())});
      break;
  }
}

void CameraWindow::saveConfig(){
  cam_timer_->stop();

  QString path = QFileDialog::getSaveFileName(
    this,
    "Save File",
    QString::fromStdString(src_dir_+"/launch"),
    "XML (*.xml)"
  );

  cam_timer_->start();

  if (path.isEmpty()) {
    RCLCPP_ERROR(pns_->get_logger(), "Save path not valid.");
    return;
  }

  QFile file(QString::fromStdString(src_dir_+"/launch/camera_node_template.xml.in"));

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    RCLCPP_ERROR(pns_->get_logger(), "Error while reading template file.");
    return;
  }

  QString content = QTextStream(&file).readAll();

  file.close();

  paramsGet();
  content.replace("{{fps}}", QString::number(paramGet("fps").as_int()));
  content.replace("{{comp}}", paramGet("Compression").as_bool() ? "true" : "false");
  content.replace("{{Hflip}}", paramGet("Hflip").as_bool() ? "true" : "false");
  content.replace("{{Vflip}}", paramGet("Vflip").as_bool() ? "true" : "false");
  content.replace("{{format}}", QString::number(paramGet("format").as_int()));
  content.replace("{{prev}}", paramGet("preview").as_bool() ? "true" : "false");
  content.replace("{{board}}", QString::fromStdString(paramGet("board").as_string()));
  content.replace("{{mode}}", QString::number(paramGet("mode").as_int()));
  content.replace("{{vcam}}", QString::number(paramGet("VCam_num").as_int()));
  content.replace("{{jpeg}}", QString::number(paramGet("JPEG_quality").as_int()));
  content.replace("{{png}}", QString::number(paramGet("PNG_quality").as_int()));
  content.replace("{{webp}}", QString::number(paramGet("WEBP_quality").as_int()));

  QFile out(path);

  if (!out.open(QIODevice::WriteOnly | QIODevice::Text)){
    RCLCPP_ERROR(pns_->get_logger(), "Error while writing xml file.");
    return;
  } else {
    RCLCPP_INFO(pns_->get_logger(), "XML file wrote successfully.");
  }

  QTextStream(&out) << content;

  out.close();
}

}
