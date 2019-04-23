/*
 * Copyright (C) 2019 Benjamin Jean-Marie Tremblay
 *
 * This file is part of sequenceshuffler.
 *
 * sequenceshuffler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * sequenceshuffler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sequenceshuffler.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

vector<string> make_klets(vector<char> lets_uniq, int k) {

  int alphlen = lets_uniq.size();
  int nlets = pow(alphlen, k);
  int let_i, counter, step;
  vector<string> klets;
  for (int i = 0; i < nlets; ++i) {
    klets.push_back("");
  }

  /* perhaps a bit primitive, but it works */

  for (int i = k - 1; i >= 0; --i) {

    counter = 0;
    let_i = 0;
    step = pow(alphlen, i);

    while (counter < nlets) {

      for (int j = 0; j < step; ++j) {
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

vector<int> count_klets(vector<char> letters, vector<string> klets,
    vector<char> lets_uniq, int k, int alphlen, bool progress = false) {

  int seqlen = letters.size();
  int nlets = pow(alphlen, k);
  vector<int> intletters, let_counts;
  for (int i = 0; i < nlets; ++i) {
    let_counts.push_back(0);
  }

  for (int i = 0; i < seqlen; ++i) {
    for (int j = 0; j < alphlen; ++j) {
      if (letters[i] == lets_uniq[j]) {
        intletters.push_back(j);
        break;
      }
    }
  }

  int stop {seqlen - k};
  int i_percent;
  if (progress) cerr << "  0%";

  int l;
  int counter {0};
  for (int i = 0; i < seqlen - k + 1; ++i) {

    if (progress) {
      i_percent = i * 100 / stop;
      if (i_percent != ((i - 1) * 100) / stop)
        cerr << "\b\b\b\b" << setw(3) << i * 100 / stop << "%";
    }

    l = 0;
    for (int j = k - 1; j >= 0; --j) {
      l += pow(alphlen, j) * intletters[i + counter];
      ++counter;
    }
    ++let_counts[l];
    counter = 0;

  }

  if (progress) cerr << endl;

  return let_counts;

}
