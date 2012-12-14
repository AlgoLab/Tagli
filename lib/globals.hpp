/*
  Copyright 2007, 2008 Daniel Zerbino (zerbino@ebi.ac.uk)
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
#ifndef _GLOBALS_TAGLI_
#define _GLOBALS_TAGLI_

#ifndef NULL
#define NULL 0
#endif

#ifndef true
#define true 1
#define false 0
#endif

#define MAXLINE 5000

#define TAGLI_WORD_SIZE 64
#define KMER_LENGTH (TAGLI_WORD_SIZE / 2)
#endif
