/*
  Copyright (C) 2012-2012 Gianluca Della Vedova (http://gianluca.dellavedova.org)

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
#ifndef JUNCTIONS_HPP
#define JUNCTIONS_HPP

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <assert.h>
#include "tightString.hpp"
#include <vector>

#define THRESHOLD_MERGE_SUBSTRINGS KMER_LENGTH

class Junction {
public:
    bool is_left; // true if left junction
    uint8_t multiple_side_length;
    LongTightString single_side; // is a prefix if is_left,
                                 // otherwise is a suffix
    std::vector<LongTightString> multiple_side;

    void shift_junction(const int8_t shift_len); // movement <0 then single_side shortens
                            // movement >0 then single_side enlarges
    LongTightString remove_multiple(const int8_t); // remove one of the prefixes/suffixes,
                                                   // that at position given by the parameter
    void add_multiple(const LongTightString &);   // add a new prefix/suffix,

  Junction()
		:is_left(false), multiple_side_length(0), single_side(""), multiple_side()
  {};

	void dump() {
		std::cout << "Junction type: ";
		if (is_left)
			std::cout<< "left\n";
		else
			std::cout << "right\n";
		std::cout << "Single: " << single_side.unimport() << std::endl;
		std::cout << "Length (check): " << multiple_side.size();
		if (multiple_side.size()==multiple_side_length)
			std::cout << " (ok)\n";
		else
			std::cout << " (should be " << multiple_side_length << ")\n";
		for (unsigned int i=0; i<multiple_side.size(); ++i) {
			std::cout << i << ":" << multiple_side[i].unimport() << std::endl;
		}
	};
private:
    void find_largest_common_substring(const LongTightString &);
};

#endif
