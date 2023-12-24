#ifndef SERVER_CONTROLLER_HPP
#define SERVER_CONTROLLER_HPP

#include <cmath>
#include <memory>

#include "dto/query.hpp"
#include "dto/query_result.hpp"
#include "dto/song.hpp"
#include "engine/ISearchEngine.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

class search_controller : public oatpp::web::server::api::ApiController {
 public:
  explicit search_controller(
      OATPP_COMPONENT(
          std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper>,
          object_mapper),
      OATPP_COMPONENT(std::shared_ptr<ISearchEngine>, search_engine))
      : oatpp::web::server::api::ApiController(object_mapper),
        m_search_engine(search_engine) {}

  static std::shared_ptr<search_controller> createShared(
      OATPP_COMPONENT(
          std::shared_ptr<oatpp::parser::json::mapping::ObjectMapper>,
          object_mapper),
      OATPP_COMPONENT(std::shared_ptr<ISearchEngine>, search_engine)) {
    return std::make_shared<search_controller>(object_mapper, search_engine);
  }

  ADD_CORS(query)
  ENDPOINT("POST", "/query", query, BODY_DTO(Object<query_dto>, q_dto)) {
    std::string lyric_to_search = q_dto->lyric;
    auto result = m_search_engine->query(lyric_to_search);
    auto [start, end] =
        m_search_engine->get_page_range(q_dto->page_number, q_dto->page_size);

    auto out = query_result::createShared();
    out->songs = oatpp::Vector<Object<song_dto>>::createShared();
    std::transform(start, end, std::back_inserter(*out->songs),
                   [](const auto& song) {
                     auto songDto = song_dto::createShared();
                     songDto->title = song.get_title();
                     songDto->artist = song.get_artist();
                     songDto->album = song.get_album();
                     return songDto;
                   });
    out->num_results = static_cast<uint32_t>(result.size());
    out->total_pages = static_cast<uint32_t>(
        std::ceil(static_cast<double>(result.size()) / q_dto->page_size));
    return createDtoResponse(Status::CODE_200, out);
  }

 private:
  std::shared_ptr<ISearchEngine> m_search_engine;
};

#include OATPP_CODEGEN_END(ApiController)

#endif  // SERVER_CONTROLLER_HPP
