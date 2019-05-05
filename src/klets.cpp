/*
 * Copyright (C) 2019 Benjamin Jean-Marie Tremblay
 *
 * This file is part of sequence-utils.
 *
 * sequence-utils is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sequence-utils is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sequence-utils.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
using namespace std;

vector<string> make_klets(vector<char> lets_uniq, unsigned int k) {

  size_t alphlen = lets_uniq.size();
  unsigned int nlets = pow(alphlen, k);
  unsigned int let_i, counter, step;
  vector<string> klets(nlets, "");

  /* perhaps a bit primitive, but it works */

  for (int i = k - 1; i >= 0; --i) {

    counter = 0;
    let_i = 0;
    step = pow(alphlen, i);

    while (counter < nlets) {

      for (unsigned int j = 0; j < step; ++j) {
        klets[counter] += lets_uniq[let_i];
        ++counter;
      }

      if (let_i == alphlen - 1)
        let_i = 0;
      else
        ++let_i;

    }

  }

  return klets;

}

vector<unsigned int> count_klets(vector<char> letters, vector<char> lets_uniq,
    unsigned int k, size_t alphlen) {

  /* Scales very well with increasing k, but requires having the entire
   * sequence in memory.
   */

  size_t seqlen = letters.size();
  unsigned int nlets = pow(alphlen, k);
  vector<unsigned int> intletters;
  vector<unsigned int> let_counts(nlets, 0);
  intletters.reserve(seqlen);

  for (size_t i = 0; i < seqlen; ++i) {

    for (size_t j = 0; j < alphlen; ++j) {
      if (letters[i] == lets_uniq[j]) {
        intletters.push_back(j);
        break;
      }
    }

  }

  unsigned int l;
  unsigned int counter;
  for (size_t i = 0; i < seqlen - k + 1; ++i) {

    l = 0; counter = 0;
    for (int j = k - 1; j >= 0; --j) {
      l += pow(alphlen, j) * intletters[i + counter];
      ++counter;
    }
    ++let_counts[l];

  }

  return let_counts;

}
