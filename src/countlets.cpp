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
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <unistd.h>
#include "klets.hpp"
using namespace std;

void usage() {
  printf(
    "countlets v1.0  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                \n"
    "                                                                                \n"
    "Usage:  countlets [options] -i [filename] -o [filename]                         \n"
    "        echo [string] | countlets [options] > [filename]                        \n"
    "                                                                                \n"
    " -i <str>   Input filename. All white space will be removed. Alternatively, can \n"
    "            take string input from a pipe.                                      \n"
    " -o <str>   Output filename. Alternatively, prints to stdout.                   \n"
    " -k <int>   K-let size. Defaults to 1.                                          \n"
    " -p         Show k-let counting progress.                                       \n"
    " -h         Show usage.                                                         \n"
  );
}

int main(int argc, char **argv) {

  /* variables */

  int k{1};
  int opt, seqlen, alphlen, alignlen;
  ifstream seqfile;
  ofstream outfile;
  bool has_file{false}, has_out{false}, progress{false};
  char l;
  set<int> lets_set;
  vector<char> letters, lets_uniq;
  vector<string> klets;
  vector<int> counts;

  while ((opt = getopt(argc, argv, "i:k:o:hp")) != -1) {
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

      case 'p': progress = true;
                break;

      case 'h': usage();
                return 0;

      default: usage();
               return 0;

    }
  }

  if (k < 1) {
    cerr << "Error: k must be greater than 0" << endl;
    exit(EXIT_FAILURE);
  }

  /* read input */

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

  /* make and count klets */

  seqlen = letters.size();

  for (int i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());
  alphlen = lets_uniq.size();

  klets = make_klets(lets_uniq, k);
  if (progress) cerr << "Counting " << k << "-lets: ";
  counts = count_klets(letters, klets, lets_uniq, k, alphlen, progress);
  if (progress) cerr << endl;

  /* return */

  alignlen = to_string(max_element(counts.begin(), counts.end())[0]).length();

  if (has_out) {
    for (int i = 0; i < klets.size(); ++i) {
      outfile << klets[i] << "  " << setw(alignlen) << counts[i] << endl;
    }
  } else {
    for (int i = 0; i < klets.size(); ++i) {
      cout << klets[i] << "  " << setw(alignlen) << counts[i] << endl;
    }
  }

  return 0;

}
