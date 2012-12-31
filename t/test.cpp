/* Copyright (c) 2010, Michigan State University.
   All rights reserved.

   Originally released under BSD License
   Downloaded from https://github.com/ged-lab/khmer
*/
#include "globals.hpp"
#include "tightString.hpp"
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <assert.h>

using namespace std;


void test0 (std::string s) {
    LongTightString t(s);
    Fingerprint f = encode(s);
    std::string df = decode(f, s.length());
    cout << s << " " << s.length() << " " << f << " * " << df  <<  "*\n";
    assert (s == df);

}

void test0b (unsigned int l) {
//  cout << l << " 0=>" << decode(0, l)  << " " << decode(0, l).length() << "*\n";
    assert (l == decode(0, l).length());

}

void test2 (std::string s) {
    LongTightString t(s);
    cout << s << "*" ;
    cout << t.unimport() << "-"
         << s.length()  << "+" << t.unimport().length() << " ** ";
    cout << t.length();
    cout << "\n";
    cout << t.sequence().to_string() << "\n";
    assert (s == t.unimport());
}

void test3 (std::string s1, std::string s2, unsigned short int len) {
    TightString t1(s1);
    TightString t2(s2);
    uint16_t l = overlap(t1,t2);
    // cout << s1 << "," << s2 << "=" << len << "\n";
    // cout <<  l << "\n";
    assert(l == len);
}

void test4 (std::string s1) {
    LongTightString t1(s1);
    NucleotideBits t2 = t1.pop();
    cout << s1 << "-" << decodeNucleotide(t2) << "\n";
    // cout <<  l << "\n";
    assert(s1[0] == decodeNucleotide(t2));
}


void test5 (std::string s1) {
    Fingerprint f = encode(s1);
    Fingerprint rc = reverse_complement(f);
    std::string s2 = decode(rc, s1.length());
    cout << s1.length() << "=" << s2.length() << "\n";
    cout << s1 << "=" << hex <<  f << " RevCompl " << rc << "=" <<  s2 << "\n";
    // cout <<  l << "\n";
    assert(s1.compare(reverse_complement(s2)) == 0);
}


void test6 (std::string s1, std::string s2, len_t len) {
    LongTightString lts1(s1);
    LongTightString lts2(s2);
    len_t ov=overlap(lts1, lts2);
    cout << s1 << "/" << s2 << "=" << ov << "\n";
    assert(len == ov);
}


int main()
{
    assert (sizeof(Fingerprint)*8 == TAGLI_WORD_SIZE);
    // Otherwise we need more than 8 bits to store a position within a fingerprint

    test0("A");
    test0("C");
    test0("G");
    test0("T");
    test0("AA");
    test0("CC");
    test0("GG");
    test0("TT");
    test0("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test0("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
    test0("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
    test0("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");

    for(unsigned int i=0; i<= KMER_LENGTH; i++) {
        test0b(i);
    }

    cout << "Test2\n";
    test2("A");
    test2("C");
    test2("G");
    test2("T");
    test2("AA");
    test2("CC");
    test2("GG");
    test2("TA");
    test2("AT");
    test2("TT");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test2("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test2("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
    test2("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
    test2("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG");
    test2("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACTAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT");
    test2("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test2("GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test2("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAATAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test2("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
    test2("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
    test2("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
    test2("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
    test2("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
    test2("TTTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTG");

    cout << "TEST 3\n";
    test3("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGC", "GCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 2);
    test3("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 32);
    test3("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 0);
    test3("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 1);

    cout << "TEST 4\n";
    test4("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test4("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test4("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test4("CCCAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test4("CCCAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test4("CCCTAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test4("TTTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTGTTTTTTTTTATTTTTTT");

    cout << "TEST 5\n";
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGA");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAATA");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGC");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAATC");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGG");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAATG");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGT");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAATT");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test5("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test5("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test5("CCCAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    test5("CCCAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
    test5("CCCTAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

    cout << "TEST 6\n";
    test6("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGC", "GCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 2);
    test6("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 32);
    test6("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC", "TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 0);
    test6("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", "TCAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT", 1);

    /*
      cout << "Starting speed-test..." << std::endl;
      const std::string s= "TTTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTGTTTTTTTTTATTTTTTT";
      for (size_t i= 0; i<1E7; ++i) {
      LongTightString t(s);
      if (t.unimport() != s) {
      cout << "Encoding/decoding error!" << std::endl;
      }
      }
    */
    return 0;
}
