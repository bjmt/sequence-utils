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
    "countlets v1.1  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                \n"
    "                                                                                \n"
    "Usage:  countlets [options] -i [filename] -o [filename]                         \n"
    "        echo [string] | countlets [options] > [filename]                        \n"
    "                                                                                \n"
    " -i <str>   Input filename. All white space will be removed. Alternatively, can \n"
    "            take string input from a pipe.                                      \n"
    " -o <str>   Output filename. Alternatively, prints to stdout.                   \n"
    " -a <str>   A string containing all of the alphabet letters present in the      \n"
    "            sequence. This allows the program not to have to load the entire    \n"
    "            sequence into memory to find all of the unique letters. The downside\n"
    "            is that runtime increases much more with increasing k.              \n"
    " -k <int>   K-let size. Defaults to 1.                                          \n"
    " -h         Show usage.                                                         \n"
  );
}

vector<int> count_stream(istream &input, vector<string> klets, int k) {

  vector<int> counts(klets.size(), 0);
  char l;
  string let;
  let.reserve(k + 1);

  vector<string>::iterator let_i;

  while (input >> l) {
    let += l;

    if (let.length() >= k) {

      if (let.length() == k + 1) let = let.substr(1, k);

        let_i = lower_bound(klets.begin(), klets.end(), let);
        if (let_i == klets.end()) {
          cerr << "Error: found unknown letter [" << l << "]" << endl;
          exit(EXIT_FAILURE);
        }
        ++counts[let_i - klets.begin()];

    }

  }

  return counts;

}

int main(int argc, char **argv) {

  /* variables */

  int k{1};
  int opt, alphlen, alignlen;
  ifstream seqfile;
  ofstream outfile;
  bool has_file{false}, has_out{false}, has_alph{false};
  set<int> lets_set;
  vector<char> lets_uniq;
  vector<string> klets;
  vector<int> counts;
  string alph;

  while ((opt = getopt(argc, argv, "i:k:o:a:h")) != -1) {
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

      case 'a': if (optarg) alph = optarg;
                has_alph = true;
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

  if (!has_file) {
    if (isatty(STDIN_FILENO)) {
      usage();
      exit(EXIT_FAILURE);
    }
  }

  /* read input */

  if (!has_alph) {

    /* this version loads the entire sequence into memory */

    vector<char> letters;
    int seqlen;
    char l;

    if (!has_file) {
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
    counts = count_klets(letters, klets, lets_uniq, k, alphlen);

  } else {

    /* this version only keeps k+1 characters in memory */

    if (alph.length() < 1) {
      cerr << "Error: could not parse -a option" << endl;
      exit(EXIT_FAILURE);
    }

    for (int i = 0; i < alph.length(); ++i) {
      lets_set.insert(alph[i]);
    }
    lets_uniq.assign(lets_set.begin(), lets_set.end());
    alphlen = lets_uniq.size();

    klets = make_klets(lets_uniq, k);

    if (!has_file) {
      counts = count_stream(cin, klets, k);
    } else {
      counts = count_stream(seqfile, klets, k);
      seqfile.close();
    }

  }

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
