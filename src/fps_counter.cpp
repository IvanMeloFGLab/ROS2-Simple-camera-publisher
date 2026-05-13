#include "cpp_camera/fps_counter.h"

namespace cpp_camera {

  FPSCounter::FPSCounter(long unsigned int ln_avg) : ln_avg_(ln_avg) {}

  FPSCounter::~FPSCounter() {}

  void FPSCounter::addFrameTime(double dt) {
    ts_.push_back(dt);
    sum_ += dt;

    if (ts_.size() > ln_avg_) {
      sum_ -= ts_.front();
      ts_.pop_front();
    }
  }

  double FPSCounter::getFPS() {
    return 1.0/(sum_/ts_.size());
  }

}
