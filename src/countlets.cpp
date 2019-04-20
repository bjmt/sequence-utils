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

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <unistd.h>
#include "shuffle_markov.hpp"
using namespace std;

void usage() {
  printf(
    "                                                                                \n"
    "Usage:  countlets -k [num] -i [filename] -o [filename]                          \n"
    "        countlets -k [num] -i [filename] > [filename]                           \n"
    "        cat [filename] | countlets -k [num] -o [filename]                       \n"
    "                                                                                \n"
    " -k <int>     K-let size. Defaults to 1.                                        \n"
    " -i <string>  Input filename. All white space will be removed. Alternatively,   \n"
    "              can take string input from a pipe.                                \n"
    " -o <string>  Output filename. Alternatively, prints to stdout.                 \n"
    "                                                                                \n"
  );
}

int main(int argc, char **argv) {

  int k {1};
  int opt, seqlen, alphlen;
  ifstream seqfile;
  ofstream outfile;
  bool has_file {false}, has_out {false};
  char l;
  set<int> lets_set;
  vector<char> letters, lets_uniq;
  vector<string> klets;
  vector<int> counts;

  while ((opt = getopt(argc, argv, "i:k:o:")) != -1) {
    switch (opt) {

      case 'i': if (optarg) {
                  seqfile.open(optarg);
                  if (seqfile.bad()) {
                    cerr << "Error: file not found" << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_file = true;
                }
                break;

      case 'k': if (optarg) k = atoi(optarg);
                break;

      case 'o': if (optarg) {
                  outfile.open(optarg);
                  if (outfile.bad()) {
                    cerr << "Error: could not create outfile" << endl;
                    exit(EXIT_FAILURE);
                  }
                  has_out = true;
                }
                break;

      default: usage();
               exit(EXIT_FAILURE);

    }
  }

  if (k < 1) {
    cerr << "Error: k must be greater than 0" << endl;
    exit(EXIT_FAILURE);
  }

  if (!has_file) {
    if (isatty(STDIN_FILENO)) {
      usage();
      exit(EXIT_FAILURE);
    }
    while (cin >> l) letters.push_back(l);
  } else {
    while (seqfile >> l) letters.push_back(l);
    seqfile.close();
  }

  seqlen = letters.size();

  for (int i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());
  alphlen = lets_uniq.size();

  klets = make_klets(lets_uniq, k);
  counts = count_klets(letters, klets, k, alphlen);

  if (has_out) {
    for (int i = 0; i < klets.size(); ++i) {
      outfile << klets[i] << "  " << counts[i] << endl;
    }
  } else {
    for (int i = 0; i < klets.size(); ++i) {
      cout << klets[i] << "  " << counts[i] << endl;
    }
  }

  return 0;

}
