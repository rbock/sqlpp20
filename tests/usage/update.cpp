/*
Copyright (c) 2017 - 2020, Roland Bock
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import <array>;
import <string>;
import sqlpp20;
#include <sqlpp20_test/mock_db.h>
#include <sqlpp20_test/tables/TabDepartment.h>
#include <sqlpp20_test/tables/TabEmpty.h>
#include <sqlpp20_test/tables/TabPerson.h>

using ::test::tabPerson;

int main() {
  auto db = ::sqlpp::test::mock_db{};
  auto id = std::size_t{};
  const auto hasNewName = false;

  // normal updates
  id = db(update(tabPerson).set(tabPerson.isManager = true).unconditionally());
  id = db(update(tabPerson)
              .set(tabPerson.isManager = true)
              .where(tabPerson.isManager == false));
  id = db(update(tabPerson)
              .set(tabPerson.isManager = true,
                   tabPerson.address = ::std::nullopt,
                   tabPerson.language = "C++")
              .where(tabPerson.name == "Herb"));
  id = db(update(tabPerson)
              .set(tabPerson.isManager = true,
                   tabPerson.address = ::std::optional<std::string_view>{},
                   tabPerson.language = "C++")
              .where(tabPerson.name == "Herb"));

  // update with optional assignment
  id = db(update(tabPerson)
              .set(tabPerson.isManager = true,
                   hasNewName ? std::make_optional(tabPerson.name = "New Name")
                              : std::nullopt)
              .unconditionally());

  // query concatenation
  id = db(update(tabPerson) << update_set(tabPerson.isManager = true)
                            << sqlpp::unconditionally());
  id = db(update(tabPerson) << update_set(tabPerson.isManager = true)
                            << where(tabPerson.isManager == false));
}
