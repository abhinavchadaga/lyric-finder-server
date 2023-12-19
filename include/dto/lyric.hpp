//
// Created by abhinavchadaga on 12/18/23.
//

#ifndef SERVER_LYRIC_HPP
#define SERVER_LYRIC_HPP

#include "oatpp/core/Types.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class lyric_dto : public oatpp::DTO {
    DTO_INIT(lyric_dto, DTO)

    DTO_FIELD(String, lyric);
};

#include OATPP_CODEGEN_END(DTO)

#endif //SERVER_LYRIC_HPP
