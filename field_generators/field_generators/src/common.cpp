#include <sstream>
#include <iomanip>

#include "common.h"

namespace Common {
std::string int2String(int i, int width) {
  std::ostringstream ss;
  ss << std::setw(width) << std::setfill('0') << i;
  return ss.str();
}

void randomK(std::vector<int>& random_k, int k, int N) {
  std::vector<int> random_N(N);
  for (int i = 0; i < N; ++i)
    random_N[i] = i;

  for (int i = 0; i < k; ++i) {
    int idx = i + rand() % (N - i);
    std::swap(random_N[i], random_N[idx]);
  }

  random_k.assign(k, 0);
  for (int i = 0; i < k; ++i)
    random_k[i] = random_N[i];

  return;
}
}
