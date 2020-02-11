#include <silk/util/filepath.h>

constexpr char extension = '.';
constexpr char separator =
#ifdef __WIN32
  '\\'
#else
  '/'
#endif
  ;

constexpr const char* silk_vm_extension = ".sce";

std::string get_file_name(std::string_view path) {
  auto start = path.find_last_of(separator);
  auto end   = path.find_last_of(extension);

  return std::string {path.substr(start + 1, end)} + silk_vm_extension;
}