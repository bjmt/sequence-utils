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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <unistd.h>
#include <unordered_map>
#include "klets.hpp"
using namespace std;

void usage() {
  printf(
    "countlets v1.3  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                \n"
    "                                                                                \n"
    "Usage:  countlets [options] -i [filename] -o [filename]                         \n"
    "        echo [string] | countlets [options] > [filename]                        \n"
    "                                                                                \n"
    " -i <str>   Input filename. All white space will be removed. Alternatively, can \n"
    "            take string input from a pipe.                                      \n"
    " -o <str>   Output filename. Alternatively, prints to stdout. Output is in tsv  \n"
    "            format.                                                             \n"
    " -a <str>   A string containing all of the alphabet letters present in the      \n"
    "            sequence. This allows the program not to have to load the entire    \n"
    "            sequence into memory to find all of the unique letters.             \n"
    " -k <int>   K-let size. Defaults to 1.                                          \n"
    " -n         Don't print k-lets with counts of zero.                             \n"
    " -h         Show usage.                                                         \n"
  );
}

unordered_map<string, unsigned int> count_stream(istream &input, vector<string> klets,
    unsigned int k) {

  char l;

  string let;
  let.reserve(k + 1);

  unordered_map<string, unsigned int> counts;
  counts.reserve(klets.size());
  for (size_t i = 0; i < klets.size(); ++i) {
    counts[klets[i]] = 0;
  }

  while (let.length() < k) {
    input >> l;
    let += l;
  }

  while (input >> l) {
    ++counts[let];
    let += l;
    let.erase(0, 1);
  }

  ++counts[let];

  return counts;

}

int main(int argc, char **argv) {

  /* variables */

  int k{1};
  int opt;
  size_t alphlen;
  ifstream seqfile;
  ofstream outfile;
  bool has_file{false}, has_out{false}, has_alph{false}, nozero{false};
  set<unsigned int> lets_set;
  vector<char> lets_uniq;
  vector<string> klets;
  string alph;

  while ((opt = getopt(argc, argv, "i:k:o:a:nh")) != -1) {
    switch (opt) {

      case 'i': if (optarg) {
                  seqfile.open(optarg);
                  if (seqfile.bad()) {
                    cerr << "Error: file not found\n";
                    cerr << "Run countlets -h to see usage.\n";
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
                    cerr << "Error: could not create outfile\n";
                    cerr << "Run countlets -h to see usage.\n";
                    exit(EXIT_FAILURE);
                  }
                  has_out = true;
                }
                break;

      case 'n': nozero = true;
                break;

      case 'h': usage();
                return 0;

    }
  }

  if (k < 1) {
    cerr << "Error: k must be greater than 0\n";
    cerr << "Run countlets -h to see usage.\n";
    exit(EXIT_FAILURE);
  }

  if (!has_file) {
    if (isatty(STDIN_FILENO)) {
      cerr << "Error: missing input\n";
      cerr << "Run countlets -h to see usage.\n";
      exit(EXIT_FAILURE);
    }
  }

  /* read input */

  if (!has_alph) {

    /* this version loads the entire sequence into memory */

    vector<unsigned int> counts;
    vector<char> letters;
    size_t seqlen;
    char l;

    if (!has_file) {
      while (cin >> l) letters.push_back(l);
    } else {
      while (seqfile >> l) letters.push_back(l);
      seqfile.close();
    }

    /* make and count klets */

    seqlen = letters.size();

    for (size_t i = 0; i < seqlen; ++i) {
      lets_set.insert(letters[i]);
    }
    lets_uniq.assign(lets_set.begin(), lets_set.end());
    alphlen = lets_uniq.size();

    klets = make_klets(lets_uniq, k);
    counts = count_klets(letters, lets_uniq, k, alphlen);

    /* return */

    if (has_out) {
      for (size_t i = 0; i < klets.size(); ++i) {
        if (counts[i] > 0 || !nozero)
          outfile << klets[i] << '\t' << counts[i] << '\n';
      }
    } else {
      for (size_t i = 0; i < klets.size(); ++i) {
        if (counts[i] > 0 || !nozero)
          cout << klets[i] << '\t' << counts[i] << '\n';
      }
    }

  } else {

    /* this version only keeps k+1 characters in memory */

    unordered_map<string, unsigned int> counts;

    if (alph.length() < 1) {
      cerr << "Error: could not parse -a option" << '\n';
      cerr << "Run countlets -h to see usage." << '\n';
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < alph.length(); ++i) {
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

    /* return */

    if (counts.size() > klets.size()) {
      cerr << "Warning: foreign character(s) encountered" << '\n';
    }

    if (has_out) {
      for (size_t i = 0; i < klets.size(); ++i) {
        if (counts[klets[i]] > 0 || !nozero)
          outfile << klets[i] << '\t' << counts[klets[i]] << '\t' << '\n';
      }
    } else {
      for (size_t i = 0; i < klets.size(); ++i) {
        if (counts[klets[i]] > 0 || !nozero)
          cout << klets[i] << '\t' << counts[klets[i]] << '\t' << '\n';
      }
    }

  }

  return 0;

}
