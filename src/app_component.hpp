//
// Created by abhinavchadaga on 12/18/23.
//
#ifndef SERVER_APP_COMPONENT_HPP
#define SERVER_APP_COMPONENT_HPP

#include <memory>

#include "engine/ISearchEngine.hpp"
#ifdef USE_CUDA
#include "engine/search_engine_gpu.hpp"
#else
#include "engine/search_engine_cpu.hpp"
#endif
#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "static_files_manager/static_files_manager.hpp"

class app_component {
 public:
  app_component(bool use_gpu, size_t nthreads)
      : m_use_gpu(use_gpu), m_nthreads(nthreads) {}

 private:
  bool m_use_gpu;
  size_t m_nthreads;

  OATPP_CREATE_COMPONENT(
      std::shared_ptr<oatpp::network::ServerConnectionProvider>,
      serverConnectionProvider)
  ([] {
    return oatpp::network::tcp::server::ConnectionProvider::createShared(
        {"0.0.0.0", 8000, oatpp::network::Address::IP_4});
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>,
                         httpRouter)
  ([] { return oatpp::web::server::HttpRouter::createShared(); }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>,
                         serverConnectionHandler)
  ([] {
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);
    return oatpp::web::server::HttpConnectionHandler::createShared(router);
  }());

  OATPP_CREATE_COMPONENT(
      std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper>,
      apiObjectMapper)
  ([] { return oatpp::parser::json::mapping::ObjectMapper::createShared(); }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<ISearchEngine>, search_engine)
  ([this] {
    const char* const path_to_json = "../db/db.jsonl";
#ifdef USE_CUDA
    if (m_use_gpu) {
      return std::make_shared<search_engine_gpu>(path_to_json);
    } else {
      return std::make_shared<search_engine_cpu>(path_to_json, m_nthreads);
    }
#else
    return std::make_shared<search_engine_cpu>(path_to_json, m_nthreads);
#endif
  }());

  OATPP_CREATE_COMPONENT(std::shared_ptr<static_files_manager>, file_manager)
  ([] {
    oatpp::String path_to_static_files{"../dist/"};
    return std::make_shared<static_files_manager>(path_to_static_files);
  }());
};
#endif  // SERVER_APP_COMPONENT_HPP
