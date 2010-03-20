// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#if defined HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef __GENRE_H__
#include "genre.h"
#endif

#ifdef SORT_RUNTIME
#include <stdlib.h>
#endif  // SORT_RUNTIME
#include <string.h>

#include "id3/globals.h"

const char ** genre_table = ID3_v1_genre_description;

const int genre_count = ID3_NR_OF_V1_GENRES;


#ifdef SORT_RUNTIME
int genre_map[genre_count];
#else
int genre_map[] = {123, 34, 74, 73, 99, 40, 20, 26, 145, 90, 116, 41, 135, 85, 96, 138, 89, 0, 107, 132, 65, 88, 104, 102, 97, 136, 61, 141, 1, 32, 112, 128, 57, 140, 2, 139, 58, 3, 125, 50, 22, 4, 55, 127, 122, 120, 98, 52, 48, 124, 25, 54, 84, 80, 81, 115, 119, 5, 30, 36, 59, 126, 38, 49, 91, 6, 79, 129, 137, 7, 35, 100, 131, 19, 33, 46, 47, 146, 8, 29, 63, 86, 71, 45, 142, 9, 77, 82, 64, 133, 10, 66, 39, 11, 103, 12, 75, 134, 13, 53, 62, 109, 117, 23, 108, 92, 67, 93, 43, 121, 14, 15, 68, 16, 76, 87, 118, 17, 78, 143, 114, 110, 69, 21, 111, 95, 105, 42, 37, 24, 56, 44, 101, 83, 94, 106, 147, 113, 18, 51, 130, 60, 70, 31, 144, 72, 27, 28};
#endif  // SORT_RUNTIME

int GetGenreCount()
{
    return genre_count;
}

const char *GetGenreFromNum(int genre_id)
{
    if ((genre_id >= 0) && (genre_id < GetGenreCount()))
        return genre_table[genre_id];
    else
        return "Unknown";
}

int GetNumFromGenre(const char *genre)
{
    int a=0;
    int b=GetGenreCount()-1;
    int i;

    while(a <= b) {
        const int i = (a+b)/2;
        const int genre_id = genre_map[i];
        const int cmp = strcmp(GetGenreFromNum(genre_id), genre);

        if (cmp == 0)
            return genre_id;
        else if (cmp < 0)
            a = i+1;
        else
            b = i-1;
    }

    return 255;
}

#ifdef SORT_RUNTIME
int CompareGenres(const void *a, const void *b)
{
    int g1, g2;
    g1 = *((int*)a);
    g2 = *((int*)b);

    return strcmp(GetGenreFromNum(g1), GetGenreFromNum(g2));
}

void InitGenres()
{
    int i;
    for (i=0; i<GetGenreCount(); ++i)
        genre_map[i] = i;

    qsort(genre_map, GetGenreCount(), sizeof(int), &CompareGenres);
}
#endif  // SORT_RUNTIME
