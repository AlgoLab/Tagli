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
    len_t overlapHelper (std::string s1, std::string s2) {
        TightString t1(s1);
        TightString t2(s2);
        return overlap(t1,t2);
    }

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
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT");
        strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
        strings.push_back("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
        strings.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
        strings.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
        strings.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
        strings.push_back("TTTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTG");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
    }

    len_t overlapHelper (std::string s1, std::string s2) {
        LongTightString t1(s1);
        LongTightString t2(s2);
        return overlap(t1,t2);
    }
};


TEST_F(TightStringTest, unimport) {
    for (auto x : kmers) {
        TightString t(x);
        EXPECT_EQ(x.length(), t.unimport().length());
        EXPECT_EQ(x, t.unimport());
    }
}

TEST_F(LongTightStringTest, unimport) {
    for (auto x : strings) {
        LongTightString t(x);
        EXPECT_EQ(x.length(), t.unimport().length());
        EXPECT_EQ(x, t.unimport());
    }
}



const vector<string> build_suffixes(string s) {
    vector<string> result = { "" };
    for(size_t len = 1; len <= s.length(); ++len) {
        result.push_back(s.substr(s.length() - len, len));
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



TEST_F(LongTightStringTest, overlap) {
    EXPECT_EQ(1, overlapHelper("T", "T"));
    EXPECT_EQ(1, overlapHelper("A", "A"));
    EXPECT_EQ(1, overlapHelper("C", "C"));
    EXPECT_EQ(1, overlapHelper("G", "G"));
    EXPECT_EQ(0, overlapHelper("G", "C"));
    EXPECT_EQ(0, overlapHelper("C", "G"));
    EXPECT_EQ(2, overlapHelper("AT", "AT"));
    EXPECT_EQ(2, overlapHelper("AA", "AA"));
    EXPECT_EQ(0, overlapHelper("AG", "AC"));
    EXPECT_EQ(0, overlapHelper("AC", "AG"));
    EXPECT_EQ(0, overlapHelper("CT", "AT"));
    EXPECT_EQ(1, overlapHelper("CA", "AA"));
    EXPECT_EQ(0, overlapHelper("CG", "AC"));
    EXPECT_EQ(0, overlapHelper("CC", "AG"));
    EXPECT_EQ(1, overlapHelper("CT", "TC"));
    EXPECT_EQ(1, overlapHelper("CA", "AC"));
    EXPECT_EQ(0, overlapHelper("CG", "CC"));
    EXPECT_EQ(1, overlapHelper("CC", "CG"));
    EXPECT_EQ(2, overlapHelper("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGC", "GCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(32, overlapHelper("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(0, overlapHelper("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(1, overlapHelper("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(36, overlapHelper("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                                "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"));

}

TEST_F(TightStringTest, overlap) {
    EXPECT_EQ(2, overlapHelper("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGC", "GCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(32, overlapHelper("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(0, overlapHelper("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(1, overlapHelper("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
}
