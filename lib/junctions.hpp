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

class Junction {
public:
    bool is_left; // true if left junction
    uint8_t multiple_side_length;
    LongTightString single_side; // is a prefix if is_left,
                                 // otherwise is a suffix
    std::vector<LongTightString> multiple_side;

    void shift_junction(const int8_t); // movement <0 then single_side shortens
                            // movement >0 then single_side enlarges
    LongTigthString remove_multiple(const int8_t); // remove one of the prefixes/suffixes,
                                                   // that at position given by the parameter
    void add_multiple(const LongTigthString &);   // add a new prefix/suffix,
    
private:
    find_largest_common_substring(const LongTightString &);
};


#endif
