# Copyright (c) 2017 - 2020, Roland Bock
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

add_library(sqlpp20_testing INTERFACE)

function(test_target name type)
  set(target sqlpp20_test_${type}_${name})
  add_executable(${target} ${name}.cpp)
  add_dependencies(${target} ModSqlpp20)
  target_link_libraries(${target} PRIVATE sqlpp20 sqlpp20_testing)
  target_compile_options(${target} PRIVATE -std=c++20 -fcolor-diagnostics -fmodules -fimplicit-modules -fimplicit-module-maps -fprebuilt-module-path=${CMAKE_BINARY_DIR}/module)
  add_test(${target} ${target})
endfunction()

target_include_directories(sqlpp20_testing INTERFACE
  $<BUILD_INTERFACE:${sqlpp20_SOURCE_DIR}/tests/include>
  )

add_subdirectory(usage)
add_subdirectory(serialize)
add_subdirectory(static_assert)
add_subdirectory(type_traits)

