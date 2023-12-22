//
// Created by Abhinav Chadaga on 12/22/23.
//
#include "static_files_manager/static_files_manager.hpp"

static_files_manager::static_files_manager(oatpp::String& base_path)
    : m_base_path(base_path),
      m_logger(el::Loggers::getLogger("static-files-manager")) {}

oatpp::String static_files_manager::get_file(const oatpp::String& path) {
  if (!path) {
    return nullptr;
  }

  std::lock_guard<oatpp::concurrency::SpinLock> lock(m_lock);
  if (!m_cache.contains(path)) {
    auto file_path = m_base_path + path;
    auto file = oatpp::String::loadFromFile(file_path->c_str());
    if (file) {
      LOG(INFO) << "Loaded file " << path->c_str() << " from disk";
      m_cache[path] = file;
    } else {
      return nullptr;
    }
  } else {
    LOG(INFO) << "Loaded file " << path->c_str() << " from cache";
  }
  return m_cache[path];
}

oatpp::String static_files_manager::get_mime_type(const oatpp::String& path) {
  if (path->ends_with(".js")) {
    return "application/javascript";
  } else if (path->ends_with(".css")) {
    return "text/css";
  } else if (path->ends_with(".ico")) {
    return "image/x-icon";
  } else {
    return nullptr;
  }
}