//
// Created by abhinavchadaga on 12/18/23.
//

#ifndef SERVER_SONG_LIST_HPP
#define SERVER_SONG_LIST_HPP

#include <oatpp/core/Types.hpp>
#include <oatpp/core/macro/codegen.hpp>

#include "song.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class songs_list_dto : public oatpp::DTO {
  DTO_INIT(songs_list_dto, DTO)

  DTO_FIELD(Vector<Object<song_dto>>, songs);
};

#include OATPP_CODEGEN_END(DTO)

#endif//SERVER_SONG_LIST_HPP
