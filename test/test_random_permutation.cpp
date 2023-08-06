#include "lumos/core/common.h"
#include <pcg_random.hpp>
#include <random>
#include <spdlog/fmt/bundled/ranges.h>
#include <stack>
#include <vector>

class RandomRangePermutation {
public:
  RandomRangePermutation(int left, int right)
      : rng(pcg32(13337)), left(left), right(right) {
    reset();
  }

  void reset() { stack.emplace(left, right); }

  bool has_next() { return !stack.empty(); }

  int next() {
    auto [l, r] = stack.top();
    stack.pop();
    if (l == r) {
      return l;
    }
    std::uniform_int_distribution<int> uniform_dist(l, r);
    int m = uniform_dist(rng);
    if (l <= m - 1) {
      stack.emplace(l, m - 1);
    }
    if (m + 1 <= r) {
      stack.emplace(m + 1, r);
    }
    return m;
  }

private:
  std::stack<std::pair<int, int>> stack;
  pcg32 rng;
  int left;
  int right;
};

int main() {
  // Seed with a real random value, if available
  // pcg_extras::seed_seq_from<std::random_device> seed_source;

  // Make a random number engine
  pcg32 rng(13337);

  std::function<void(int, int, std::vector<int> &)> recursive =
      [&rng, &recursive](int l, int r, std::vector<int> &res) {
        if (l > r) {
          return;
        }
        // generate a value in range (l,r)
        std::uniform_int_distribution<int> uniform_dist(l, r);
        int m = uniform_dist(rng);
        res.emplace_back(m);
        recursive(l, m - 1, res);
        recursive(m + 1, r, res);
      };

  std::function<void(int, int, std::vector<int> &)> non_recursive =
      [&rng](int l, int r, std::vector<int> &res) {
        std::stack<std::pair<int, int>> stack;
        stack.emplace(l, r);
        while (!stack.empty()) {
          auto [left, right] = stack.top();
          stack.pop();
          if (left > right) {
            continue;
          }
          std::uniform_int_distribution<int> uniform_dist(left, right);
          int m = uniform_dist(rng);
          res.emplace_back(m);
          stack.emplace(left, m - 1);
          stack.emplace(m + 1, right);
        }
      };

  std::vector<int> res;
  res.reserve(16);
  recursive(0, 15, res);
  INFO(res);
  res.clear();
  non_recursive(0, 15, res);
  INFO(res);
  res.clear();
  RandomRangePermutation perm(0, 15);
  while (perm.has_next()) {
    res.emplace_back(perm.next());
  }
  INFO(res);
  return 0;
}