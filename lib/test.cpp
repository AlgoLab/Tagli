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
	TTTTTTTTATTTTTTTTTTTTTTTTTTTTTTCTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTGTTTTTTTTTATTTTTTT");

	
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
