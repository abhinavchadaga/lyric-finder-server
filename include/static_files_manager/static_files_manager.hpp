#ifndef STATIC_FILE_MANAGER_HPP
#define STATIC_FILE_MANAGER_HPP

#include <unordered_map>

#include "easylogging++.h"
#include "oatpp/core/Types.hpp"
#include "oatpp/core/concurrency/SpinLock.hpp"

class static_files_manager {
 public:
  explicit static_files_manager(oatpp::String& base_path);
  oatpp::String get_file(const oatpp::String& path);
  static oatpp::String get_mime_type(const oatpp::String& path);

 private:
  oatpp::String m_base_path;
  oatpp::concurrency::SpinLock m_lock;
  std::unordered_map<oatpp::String, oatpp::String> m_cache;
  el::Logger* m_logger;
};

#endif  // STATIC_FILE_MANAGER_HPP