//
// Created by abhinavchadaga on 12/18/23.
//

#ifndef SERVER_SONG_LIST_HPP
#define SERVER_SONG_LIST_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "song.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class query_result : public oatpp::DTO {
  DTO_INIT(query_result, DTO)

  DTO_FIELD(Vector<Object<song_dto>>, songs);
  DTO_FIELD(UInt32, num_results);
  DTO_FIELD(UInt32, total_pages);
};

#include OATPP_CODEGEN_END(DTO)

#endif  // SERVER_SONG_LIST_HPP
