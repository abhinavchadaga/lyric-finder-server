//
// Created by abhinavchadaga on 12/18/23.
//

#ifndef SERVER_SONG_HPP
#define SERVER_SONG_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class song_dto : public oatpp::DTO {
  DTO_INIT(song_dto, DTO)

  DTO_FIELD(String, title);

  DTO_FIELD(String, album);

  DTO_FIELD(String, artist);
};

#include OATPP_CODEGEN_END(DTO)

#endif  // SERVER_SONG_HPP
