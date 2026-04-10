// SPDX-FileCopyrightText: 2026 Christian von Elm <christian.von_elm@tu-dresden.de>
//
// SPDX-License-Identifier: MIT

#pragma once

// poor mans rust ?
#define CHECK_ASSIGN(var, stmt)                                                                    \
    auto var##_res = stmt;                                                                         \
    if (!var##_res.ok())                                                                           \
    {                                                                                              \
        return bowl::Unexpected(var##_res.unpack_error());                                         \
    }                                                                                              \
    auto var = var##_res.unpack_ok();
