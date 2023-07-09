#include "Utils.hpp"
#include <cstdlib>

std::string Utils::randomString(std::uint32_t size) noexcept {
  static auto &alphanum = "_0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(size);

  while (size--) {
    tmp_s += alphanum[std::rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
}
