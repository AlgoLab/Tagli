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
#include "prettyprint.hpp"

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
    vector<string> short_strings = {""};
    vector<string> long_strings = {""};
    vector< pair<string,string> > string_pairs;
    virtual void SetUp() {
        for (len_t len = 1; len <= 4; len++)
            for (Fingerprint f = 0; f < pow(4, len); f++)
                short_strings.push_back(decode(f, len));
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        long_strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        long_strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        long_strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        long_strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        long_strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        long_strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT");
        long_strings.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
        long_strings.push_back("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
        long_strings.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
        long_strings.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
        long_strings.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
        long_strings.push_back("TTTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTG");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        long_strings.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");

        strings.insert(strings.end(), kmers.begin(), kmers.end());
        strings.insert(strings.end(), short_strings.begin(), short_strings.end());
        strings.insert(strings.end(), long_strings.begin(), long_strings.end());

        string_pairs.push_back(make_pair("AAAAAAAAAAAA", "CAAAAAAAAAAA"));
        string_pairs.push_back(make_pair("AAAAAAAAAAAA", "AAAAAAAAAAAA"));
        string_pairs.push_back(make_pair("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
                                         "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"));
        string_pairs.push_back(make_pair("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT",
                                         "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC"));

        for (auto s1 : short_strings)
            for (auto s2 : short_strings)
                string_pairs.push_back(make_pair(s1, s2));
        for (auto s1 : long_strings)
            for (auto s2 : long_strings)
                string_pairs.push_back(make_pair(s1, s2));

    }

    len_t overlapHelper (std::string s1, std::string s2) {
        LongTightString t1(s1);
        LongTightString t2(s2);
        return overlap(t1,t2);
    }

    Match find_longest_common_substring_checker(std::string s1, std::string s2, bool start_anchored=false, bool end_anchored=false) {
        if (s1.size() == 0 || s2.size() == 0) return Match(0, 0, 0);
        len_t max_start = start_anchored ? 0 : s1.length()-1;
        len_t min_end = end_anchored ? s1.length()-1 : 0;
        Match result(0, 0, 0);

        for (len_t p1 = 0; p1 <= max_start; p1++) {
            for (int p2 = s1.length()-1; p2 >= p1 && p2 >= min_end; p2--) {
                len_t len = p2-p1+1;
                string needle = s1.substr(p1, len);
                size_t found = s2.find(needle);

                if (found!=string::npos && len > result.length) {
                    Match nr(p1, (len_t) found, len);
                    result = nr;
                }
            }
        }
        return result;
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
TEST_F(LongTightStringTest, pop) {
    for (auto x : strings) {
        if (x.length() == 0) continue;
        string orig_x = x;
        LongTightString lts(x);
        NucleotideBits z = lts.pop();
        string tested = lts.unimport();
        string ok = x.substr(0, x.length() - 1);
        EXPECT_EQ(ok.length(), tested.length());
        EXPECT_EQ(ok, tested) << orig_x;
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




TEST_F(TightStringTest, overlap) {
    EXPECT_EQ(2, overlapHelper("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGC", "GCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(32, overlapHelper("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(0, overlapHelper("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
    EXPECT_EQ(1, overlapHelper("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT"));
}

TEST_F(LongTightStringTest, prefix) {
    for (auto x : strings) {
        LongTightString t(x);
        for (len_t l = 0; l <= x.length(); ++l)
            EXPECT_EQ(x.substr(0, l), t.prefix(l).unimport());
    }
}

TEST_F(LongTightStringTest, find_longest_prefix_suffix_substring) {
    for (auto p : string_pairs) {
        LongTightString s1(p.first);
        LongTightString s2(p.second);
        EXPECT_TRUE(compare(s1.unimport(), s2.unimport(), find_longest_common_substring_checker(p.first, p.second, true, false),
                            find_longest_prefix_substring(s1, s2), true))
            << p.first << " " << p.second << " "
            << find_longest_common_substring_checker(p.first, p.second, true, false).dump()  << " "
            << find_longest_prefix_substring(s1, s2).dump();
        EXPECT_TRUE(compare(s1.unimport(), s2.unimport(), find_longest_common_substring_checker(p.first, p.second, false, true),
                            find_longest_suffix_substring(s1, s2), true))
            << p.first << " " << p.second << " "
            << find_longest_common_substring_checker(p.first, p.second, false, true).dump()  << " "
            << find_longest_suffix_substring(s1, s2).dump();
        EXPECT_TRUE(compare(s1.unimport(), s2.unimport(), find_longest_common_substring_checker(p.first, p.second, false, false),
                            find_largest_common_substring(s1, s2), true))
            << p.first << " " << p.second << " "
            << find_longest_common_substring_checker(p.first, p.second).dump()  << " "
            << find_largest_common_substring(s1, s2).dump();
    }
}



const len_t overlap(string s1, string s2) {
    for (len_t len = min(s1.length(), s2.length()); len>0; len--)
        if (s1.compare(s1.length()-len, len, s2, 0, len) == 0)
            return len;
    return 0;
}


TEST_F(LongTightStringTest, overlap) {
    for (auto p : string_pairs) {
        LongTightString s1(p.first);
        LongTightString s2(p.second);
        EXPECT_EQ(overlap(p.first, p.second), overlap(s1, s2)) << p.first << "/" << p.second << endl;
    }
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
