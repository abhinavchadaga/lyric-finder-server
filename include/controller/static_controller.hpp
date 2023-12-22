#ifndef STATIC_CONTROLLER_HPP
#define STATIC_CONTROLLER_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "static_files_manager/static_files_manager.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class static_controller : public oatpp::web::server::api::ApiController {
 public:
  explicit static_controller(
      OATPP_COMPONENT(
          std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper>,
          object_mapper),
      OATPP_COMPONENT(std::shared_ptr<static_files_manager>, fileManager))
      : oatpp::web::server::api::ApiController(object_mapper),
        m_file_manager(fileManager) {}

  static std::shared_ptr<static_controller> createShared(
      OATPP_COMPONENT(
          std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper>,
          object_mapper),
      OATPP_COMPONENT(std::shared_ptr<static_files_manager>, fileManager)) {
    return std::make_shared<static_controller>(object_mapper, fileManager);
  }

  ENDPOINT("GET", "/", root) {
    auto buffer = m_file_manager->get_file("index.html");
    OATPP_ASSERT_HTTP(buffer, Status::CODE_404, "File not found");
    auto response = createResponse(Status::CODE_200, buffer);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

  ENDPOINT("GET", "/*", static_files, REQUEST(std::shared_ptr<IncomingRequest>,
                                              request)) {
    auto path = request->getPathTail();
    auto buffer = m_file_manager->get_file(path);
    OATPP_ASSERT_HTTP(buffer, Status::CODE_404, "File not found");
    auto response = createResponse(Status::CODE_200, buffer);
    response->putHeader(Header::CONTENT_TYPE, static_files_manager::get_mime_type(path));
    return response;
  }

 private:
  std::shared_ptr<static_files_manager> m_file_manager;
};

#include OATPP_CODEGEN_END(ApiController)

#endif  // STATIC_CONTROLLER_HPP