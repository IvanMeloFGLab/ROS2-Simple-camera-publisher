#include <deque>
#include <numeric>
#include <math.h>

using std::deque;
using std::accumulate;
using std::round;
using std::pow;

namespace cpp_camera {

class FPSCounter {
public:
  FPSCounter(long unsigned int ln_avg);
  ~FPSCounter();

  void addFrameTime(double dt);
  double getFPS();
  double getRoundedFPS(int decimals);

private:
  long unsigned int ln_avg_;
  double fps_, sum_;

  deque<double> ts_;

};

}
