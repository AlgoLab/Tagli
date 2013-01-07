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


#include "junctions.hpp"

using namespace std;


void Junction::move_border(len_t movement) {
    if (is_left) {
        LongTightString move = single_side.pop(movement);
        for (auto it = multiple_side.begin(); it != multiple_side.end(); ++it)
            it->unshift(move);
    } else {
        LongTightString move = single_side.shift(movement);
        for (auto it = multiple_side.begin(); it != multiple_side.end(); ++it)
            it->push(move);
    }
}

void Junction::clean_multiple(const LongTightString & s) {
    for(auto it=multiple_side.begin(); it != multiple_side.end(); ++it)
        if (it->length() < s.length())
            if ((is_left && it->is_prefix(s)) || (!is_left && it->is_suffix(s)))
                multiple_side.erase(it);
}

void Junction::add_read(LongTightString read) {
    ROOT_TRACE("add_read: " << read.dump());
    ROOT_TRACE(this->dump());
    if (multiple_side_length == 0 && single_side.length() == 0) { //multiple_side_length == 0 added to speed up the most
                                                                  //common case
        // first time we visit the junction
        single_side = read;
        return;
    }
    ROOT_TRACE("add_read: " << read.dump());
    ROOT_TRACE(this->dump());
    len_t read_left = overlap(read, single_side);
    len_t read_right = overlap(single_side, read);
    ROOT_TRACE("add_read: " << read.dump());
    ROOT_TRACE(this->dump());
    // ROOT_TRACE("Single side: " << single_side);
    // ROOT_TRACE("Read       : " << read);
    Match read_prefix_substring = find_longest_prefix_substring(read, single_side);
    ROOT_TRACE("add_read: " << read.dump());
    ROOT_TRACE(this->dump());
    // Match read_suffix_substring = find_longest_suffix_substring(read, single_side);
    // Match read_substring_prefix = find_longest_prefix_substring(single_side, read);
    // Match read_substring_suffix = find_longest_suffix_substring(single_side, read);
    Match read_substring = find_largest_common_substring(read, single_side);
    ROOT_TRACE("read_prefix_substring: " << read_prefix_substring.dump() << ", read_substring: " << read_substring.dump());
    ROOT_TRACE("read_left: " << read_left << ", read_right: " << read_right << endl);

    // First check if read is contained in the single side
    if (read_prefix_substring.length == read.length()) {
        ROOT_TRACE("Discarded: read is contained in the single side");
        return;
    }
    // Then check if read and single_side share the fingerprint, otherwise discard the read
    //                       Sometimes we can even discard the old single_side.
    if (read_substring.length < KMER_LENGTH) {
        ROOT_TRACE("Discarded: overlap too short");
        return;
    }

    if (multiple_side_length > 1) {
        ROOT_TRACE("multiple_side_length > 1, actually " << multiple_side_length);

        // Since we have determined the *multiple* side,
        // the single_side is fixed and cannot be changed, but only extended
        // in the direction opposite to the junxction
        if (is_left and read_left > THRESHOLD_MERGE_SUBSTRINGS) {
            // extend to the left
            single_side.update(merge(read.sequence(), single_side.sequence(), read_left),
                               single_side.length()+read.length()-read_left);
            return;
        }
        if (!is_left and read_right > THRESHOLD_MERGE_SUBSTRINGS) {
            // extend to the right
            single_side.update(merge(single_side.sequence(), read.sequence(), read_right),
                               single_side.length()+read.length()-read_right);
            return;
        }

        // We can't extend the single side
        if (is_left) {
            if(read_right > THRESHOLD_MERGE_SUBSTRINGS) {
                // read fits the rightmost portion of single_side
                if(longest_left_overlap(read, multiple_side) < read.length() - read_right) {
                    // the read is not absorbed into any multiple side component, we must
                    // sprout a new component
                    LongTightString new_multiple = read.suffix(read.length()-read_right);
                    add_multiple(new_multiple);
                    // remove all components that are contained in the one that we have just added
                    clean_multiple(new_multiple);
                }
            }
            return;
        } else {
            if(read_left > THRESHOLD_MERGE_SUBSTRINGS) {
                // read fits the leftmost portion of single_side
                if(longest_right_overlap(read, multiple_side) < read.length() - read_left) {
                    // the read is not absorbed into any multiple side component, we must
                    // sprout a new component
                    LongTightString new_multiple = read.prefix(read.length()-read_left);
                    add_multiple(new_multiple);
                    // remove all components that are contained in the one that we have just added
                    clean_multiple(new_multiple);
                }
            }
            return;
        }
        /*
          We have already determined the multiple_side, but the current read has a match that
          does not reach the end of the single_side.
          Therefore we must shorten the single_side and add the cut part to all components of the
          multiple_side
        */
        if (is_left) {
            len_t offset = single_side.length() - (read_substring.begin1 + read_substring.length - 1) - 1;
            this->move_border(offset);
            add_multiple(read.prefix(read.length() - offset));
        } else {
            len_t offset = read_substring.begin1;
            this->move_border(offset);
            add_multiple(read.prefix(read.length() - offset));
        }

    }
    /* If we reach this point, we have not determined the multiple side portion of the junction.
       Hence there are two cases:
       1. we can extend the single side
       2. we determine the type of the junction (left or right) and a tentative split of the single
       side
    */
    ROOT_TRACE("multiple_side_length: 0");
    if (read_left > THRESHOLD_MERGE_SUBSTRINGS || read_right > THRESHOLD_MERGE_SUBSTRINGS) {
        // yes, we extend the single side
        LongTightStringSequence single_side_seq = single_side.sequence();
        LongTightStringSequence read_seq = read.sequence();
        if (read_left > read_right) {
            ROOT_TRACE("Extend single_side to the left");
            len_t new_length = single_side.length() + read.length() - read_left;
            if (new_length <= LONGTIGHTSTRING_LEN) {
                single_side.update(merge(read_seq, single_side_seq, read_left), new_length);
            }
        } else {
            ROOT_TRACE("Extend single_side to the right");
            len_t new_length=single_side.length() + read.length() - read_right;
            if (new_length <= LONGTIGHTSTRING_LEN) {
                single_side.update(merge(single_side_seq, read_seq, read_right), new_length);
            }
        }
        return;
    }
    /*
      It's time to split the single_side into the multiple_side and determine
      if it is a left or right junction.
    */
    // TODO
    ROOT_TRACE("Create multiple_side");
    ROOT_TRACE("read_substring: " << read_substring.dump());
    if (read_substring.begin1 == 0 || read_substring.begin2 == 0)
        is_left = true;
    else {
        if (read_substring.begin1 + read_substring.length < read.length() - 1 &&
            read_substring.begin2 + read_substring.length < single_side.length() - 1)
            // in this case the longest match between the read and the single side cannot be extended to be a junction
            return;
        is_left = false;
    }
    ROOT_TRACE("Determine junction type: " << is_left);
    if (is_left) {
        ROOT_TRACE("Cutting read at position length: " << read_substring.begin1+read_substring.length << "/" <<
                   read.length()-(read_substring.begin1+read_substring.length));
        add_multiple(read.suffix(read.length()-(read_substring.begin1+read_substring.length)));
        ROOT_TRACE("Cutting single_side at position: " << read_substring.begin2+read_substring.length << "/" <<
                   single_side.length()-(read_substring.begin2+read_substring.length));
        add_multiple(single_side.pop(single_side.length()-(read_substring.begin2+read_substring.length)));
        if (read_substring.begin1 > 0) {
            ROOT_TRACE("extend single_side");
            single_side.unshift(read.prefix(read_substring.begin1));
        }
    } else {
        ROOT_TRACE("Cutting read at position       : " << read_substring.begin1);
        add_multiple(read.prefix(read_substring.begin1));
        ROOT_TRACE("Cutting single_side at position: " << read_substring.begin2);
        add_multiple(single_side.shift(read_substring.begin2));
        if (read_substring.begin1 + read_substring.length < read.length() - 1)  {
            ROOT_TRACE("extend single_side");
            single_side.push(read.suffix(read.length() - (read_substring.begin1 + read_substring.length)));
        }
    }
}

void Junction::add_multiple(const LongTightString & s) {
    multiple_side.push_back(s);
    multiple_side_length++;
}
LongTightString Junction::remove_multiple(const int8_t pos) {
    LongTightString s = multiple_side[pos];
    multiple_side.erase(multiple_side.begin()+pos);
    multiple_side_length--;
    return s;
}
