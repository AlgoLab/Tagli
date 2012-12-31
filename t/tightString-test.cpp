/*
  Copyright 2012-2012 Gianluca Della Vedova (http://gianluca.dellavedova.org)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, see http://www.gnu.org/licenses/
*/


#include "../lib/tightString.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>


std::string unimportTester (std::string s) {
    TightString t(s);
    return t.unimport();
}

TEST(TightString, unimport) {
    std::vector<std::string> items = {
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG",
        "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT",
        "CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
        "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC",
        "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG",
        "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT",
    };
    for (auto x : items) { EXPECT_EQ(x, unimportTester(x)); }
}
