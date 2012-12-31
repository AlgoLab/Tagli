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

using namespace std;

class TightStringTest : public testing::Test {
protected:
    vector<string> kmers = {
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

};

class LongTightStringTest : public TightStringTest {
protected:
    vector<string> strings = {""};
    virtual void SetUp() {
        for (len_t len = 1; len <= 4; len++)
            for (Fingerprint f = 0; f < pow(4, len); f++)
                strings.push_back(decode(f, len));
        strings.insert(strings.end(), kmers.begin(), kmers.end());
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    }
};


TEST_F(TightStringTest, unimport) {
    for (auto x : kmers) {
        TightString t(x);
        EXPECT_EQ(x.length(), t.unimport().length());
        EXPECT_EQ(x, t.unimport());
    }
}

const vector<string> build_suffixes(string s) {
    vector<string> result;
    for(size_t len = s.length(); len > 0; --len) {
        result.push_back(s.substr(0, len));
    }
    return result;
}





TEST_F(LongTightStringTest, shift) {
    for (auto x : strings) {
        if (x.length() == 0) continue;
        LongTightString lts(x);
        NucleotideBits z = lts.shift();
        string tested = lts.unimport();
        string ok = x.substr(1, x.length() - 1);
        EXPECT_EQ(ok.length(), tested.length());
        EXPECT_EQ(ok, tested);
    }
}
/*
  TEST_F(LongTightStringTest, build_suffixes) {
  kmers.insert(kmers.end(), strings.begin(), strings.end());
  for (auto x : kmers) {
  LongTightString lts(x);
  vector<LongTightString> ltsvec = build_suffixes(lts);
  vector<string> suff_test;
  suff_test.resize(ltsvec.size());
  transform(ltsvec.begin(), ltsvec.end(), suff_test.begin(),
  [](LongTightString p) { return(p.unimport()); });
  vector<string> suff_ok = build_suffixes(x);
  EXPECT_EQ(suff_ok.size(), suff_test.size());
  EXPECT_TRUE(equal(suff_ok.begin(), suff_ok.end(), suff_test.begin()));
  }
  }
*/
