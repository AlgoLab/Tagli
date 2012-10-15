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
//	cout << l << " 0=>" << decode(0, l)  << " " << decode(0, l).length() << "*\n";
	assert (l == decode(0, l).length());

}

void test1 (std::string s) {
	TightString t(s);
	// Fingerprint f = t.fingerprint;
//	cout << s << " " << hex << f  << " " << t.unimport()  << "\n";
	assert (s == t.unimport());
}

void test2 (std::string s) {
	LongTightString t(s);
	cout << s << "*" ;
	cout << t.unimport() << "-"
                  << s.length()  << "+" << t.unimport().length() << " ** ";
	cout << t.length;
	cout << "\n";
	cout << t.sequence.to_string() << "\n";
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
	LongTightString t2 = t1.pop_first_character();
	cout << s1 << "-" << t2.unimport() << "\n";
	// cout <<  l << "\n";
	assert(s1.substr(1) == t2.unimport());
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

	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG");
	test1("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT");
	test1("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	test1("GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	test1("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	test1("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
	test1("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
	test1("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");

	cout << "Test2\n";
	test2("A");
	test2("C");
	test2("G");
	test2("T");
	test2("AA");
	test2("CC");
	test2("GG");
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


	cout << "Starting speed-test..." << std::endl;
	const std::string s= "TTTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTGTTTTTTTTTATTTTTTT";
	for (size_t i= 0; i<1E7; ++i) {
          LongTightString t(s);
          if (t.unimport() != s) {
                 cout << "Encoding/decoding error!" << std::endl;
          }
	}

	return 0;
}
