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
#include <fstream>
#include <unistd.h>
#include <string>
using namespace std;

void usage() {
  printf(
    "countfa v1.2  Copyright (C) 2019  Benjamin Jean-Marie Tremblay                  \n"
    "                                                                                \n"
    "Usage:  coutfa -i [filename]                                                    \n"
    "        cat [filename] | coutfa                                                 \n"
    "                                                                                \n"
    " -i <str>    Input filename. File must be fasta-formatted. Alternatively, takes \n"
    "             input from a pipe.                                                 \n"
    " -h          Print usage and exit.                                              \n"
  );
}

void do_countfa(istream &input) {

  bool at_name{false};
  unsigned long counter{0};
  char l;

  while (input.get(l)) {

    if (l == '>') {
      if (counter > 0) cout << counter << '\n';
      at_name = true;
      counter = 0;
    }

    if (l == '\n' && at_name) {
      at_name = false;
      cout << '\n';
    }

    if (at_name) cout << l;
    else if (l != ' ' && l != '\n') ++counter;

  }

  if (!at_name && counter > 0) cout << counter << '\n';

  return;

}

int main(int argc, char **argv) {

  int opt;
  bool has_file{false};
  ifstream seqfile;

  while ((opt = getopt(argc, argv, "i:h")) != -1) {
    switch (opt) {
      case 'i': if (optarg) {
                  seqfile.open(optarg);
                  if (seqfile.bad()) {
                    cerr << "Error: file not found\n";
                    cerr << "Run countfa -h to see usage.\n";
                    exit(EXIT_FAILURE);
                  }
                  has_file = true;
                }
                break;
      case 'h': usage();
                return 0;
    }
  }

  if (!has_file) {
    if (isatty(STDIN_FILENO)) {
      cerr << "Error: missing input\n";
      cerr << "Run countfa -h to see usage.\n";
      exit(EXIT_FAILURE);
    }
    do_countfa(cin);
  } else {
    do_countfa(seqfile);
  }

  return 0;

}
