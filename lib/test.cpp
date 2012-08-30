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


void output_line (std::string s) {
	TightString t(s);
	Fingerprint f = t.fingerprint;
	cout << s << " " << f  << " " << t.unimport()  << "\n";
	assert (s.compare(t.unimport()) == 0);
}

int main()
{
	
	vector<std::string> cases;
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAG");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAT");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACA");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACC");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACG");
	cases.push_back("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAACT");
	cases.push_back("CAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	cases.push_back("GAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	cases.push_back("TAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
	cases.push_back("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
	cases.push_back("GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG");
	cases.push_back("TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");

	for_each (cases.begin(), cases.end(), output_line);
	
  return 0;
}
