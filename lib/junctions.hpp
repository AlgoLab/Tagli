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
#include <log.hpp>


#define THRESHOLD_MERGE_SUBSTRINGS KMER_LENGTH

class Junction {
public:
    bool is_left; // true if left junction
    uint8_t multiple_side_length;
    LongTightString single_side; // is a prefix if is_left,
                                 // otherwise is a suffix
    std::vector<LongTightString> multiple_side;

    Junction()
        :is_left(false), multiple_side_length(0), single_side(""), multiple_side()
        {};


    void shift_junction(const int8_t shift_len); // movement <0 then single_side shortens
    // movement >0 then single_side enlarges

    // remove one of the prefixes/suffixes,
    // that at position given by the parameter
    LongTightString remove_multiple(const int8_t pos);

    // add a new prefix/suffix,
    void add_multiple(const LongTightString & s);

    // Check if s obsoletes some components of the multiple_side
    void clean_multiple(const LongTightString & s);

    void move_border(len_t);
    void add_read(LongTightString read);

    std::string dump() {
        std::stringstream result;
        result << "Junction type: ";
        if (is_left)
            result<< "left,";
        else
            result << "right,";
        result << "Single: " << single_side.unimport() << ",";
        result << " Length: " << single_side.length();
        result << " Multiple side components: " << multiple_side.size();
        if (multiple_side.size()==multiple_side_length)
            result << " status: ok";
        else
            result << " status: should be " << multiple_side_length;
        result << std::endl;
        for (unsigned int i=0; i<multiple_side.size(); ++i) {
            result << i << ":" << multiple_side[i].unimport() << std::endl;
        }
        return result.str();
    };
    std::string simpledump() {
        std::stringstream result;
        result << single_side.unimport() << ",";
        result << multiple_side.size() << ",";
        for (unsigned int i=0; i<multiple_side.size(); ++i) {
            result << multiple_side[i].unimport() << ",";
        }
        if (is_left)
            result << "1";
        else
            result << "0";
        return result.str();
    };
// private:
//     void find_largest_common_substring(const LongTightString &);
};

#endif
