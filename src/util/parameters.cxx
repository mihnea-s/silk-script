#include <algorithm>
#include <cstring>

#include <silk/common/error.h>
#include <silk/util/parameters.h>

auto Parameters::is_param(const char* arg) const -> bool {
  return strncmp(arg, "-", 1) == 0;
}

auto Parameters::is_of_param(const char* arg, Flag flag) const -> bool {
  auto begin = arg;
  auto end   = arg + std::strlen(arg);

  auto v1 = usage(flag).first;
  auto v2 = usage(flag).second;
  return std::equal(begin, end, v1.begin(), v1.end()) ||
         std::equal(begin, end, v2.begin(), v2.end());
}

auto Parameters::files() const -> const std::vector<std::string>& {
  return _files;
}

auto Parameters::flag(Flag flag) const -> bool {
  return _bits.test((size_t)flag);
}

auto Parameters::parse(const int argc, const char** argv) -> void {
  _bits.reset();

  const char** arg = argv + 1;
  const char** end = argv + argc;

  for (; arg < end; arg++) {
    if (!is_param(*arg)) {
      _files.push_back(*arg);
      continue;
    }

    for (size_t fl = (size_t)Flag::HELP; fl != (size_t)Flag::LAST; fl++) {
      if (is_of_param(*arg, (Flag)fl)) {
        _bits.set(fl);
        break;
      }

      print_error("invalid parameter '{}'", *arg);
    }
  }
}
