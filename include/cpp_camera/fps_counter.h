#include <deque>
#include <numeric>

using std::deque;
using std::accumulate;

namespace cpp_camera {

class FPSCounter {
public:
  FPSCounter(long unsigned int ln_avg);
  ~FPSCounter();

  void addFrameTime(double dt);
  double getFPS();

private:
  long unsigned int ln_avg_;
  double fps_, sum_;

  deque<double> ts_;

};

}
