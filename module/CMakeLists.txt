# Copyright (c) 2020, Roland Bock
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this
#    list of conditions and the following disclaimer in the documentation and/or
#    other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
# ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

SET(SQLPP20_MODULE_SRC "${CMAKE_CURRENT_SOURCE_DIR}/sqlpp20.cpp")
SET(SQLPP20_MODULE "${CMAKE_CURRENT_BINARY_DIR}/sqlpp20.pcm")
FILE(GLOB_RECURSE SQLPP20_MODULE_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/../include/sqlpp20/*.h")

add_custom_command(
    OUTPUT 
        ${SQLPP20_MODULE}
    COMMAND 
        /usr/local/llvm-trunk/bin/clang++ 
                -std=c++20 
                -stdlib=libc++ 
                -fmodules 
                ${SQLPP20_MODULE_SRC}
                -I"${CMAKE_CURRENT_SOURCE_DIR}/../include"
                -c 
                -Xclang -emit-module-interface 
                -o ${SQLPP20_MODULE}
    COMMENT 
        "Creating ${SQLPP20_MODULE}"
    DEPENDS
        ${SQLPP20_MODULE_SRC}
        ${SQLPP20_MODULE_DEPS}
)

add_custom_target(ModSqlpp20 
    DEPENDS 
        ${SQLPP20_MODULE}
    VERBATIM
)

