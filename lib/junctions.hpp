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
#ifndef JUNCTIONS_HH
#define JUNCTIONS_HH

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <assert.h>
#include "tightString.hpp"


class Junction {
public:
    bool is_left; // true if left junction
    uint8_t multiple_side_length;
    LongTightString single_side; // is a prefix if is_left,
                                 // otherwise is a suffix
    LongTightString *multiple_side;

    shift_junction(int8_t); // movement <0 then single_side shortens
                            // movement >0 then single_side enlarges

    find_largest_common_substring(std::string *);
};

