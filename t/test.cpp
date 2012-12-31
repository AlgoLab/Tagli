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
