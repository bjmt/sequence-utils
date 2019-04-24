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
#include <set>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <random>
#include "klets.hpp"
using namespace std;

vector<vector<int>> make_edgelist(vector<int> let_counts, int nletsm1, int alphlen) {

  vector<vector<int>> edgelist(nletsm1, vector<int>(alphlen));
  int counter {0};

  for (int i = 0; i < nletsm1; ++i) {

    for (int j = 0; j < alphlen; ++j) {
      edgelist[i][j] = let_counts[counter];
      ++counter;
    }

  }

  return edgelist;

}

vector<int> find_euler(vector<vector<int>> edgelist, int lasti, int nletsm1,
    default_random_engine gen, int alphlen, int k, vector<bool> empty_vertices) {

  vector<int> last_letsi;
  vector<bool> vertices;
  int u;

  for (int i = 0; i < nletsm1; ++i) {
    vertices.push_back(false);
    last_letsi.push_back(0);
  }
  cerr << "lasti=" << lasti << endl;
  cerr << "nletsm1=" << nletsm1 << endl;
  vertices[lasti] = true;  /* tree root */

  for (int i = 0; i < nletsm1; ++i) {
    if (empty_vertices[i]) vertices[i] = true;  /* ignore unconnected vertices */
    cerr << "vertices[" << i << "]: " << vertices[i] << endl;
  }

  for (int i = 0; i < nletsm1; ++i) {

    u = i;

    while (!vertices[u]) {
      cerr << u << "a ";
      discrete_distribution<int> next_let(edgelist[u].begin(), edgelist[u].end());
      last_letsi[u] = next_let(gen);
      u = pow(alphlen, k - 2) * (u % alphlen) + last_letsi[u];
    }
    cerr << endl;

    u = i;

    while (!vertices[u]) {
      cerr << u << "b ";
      vertices[u] = true;
      u = pow(alphlen, k - 2) * (u % alphlen) + last_letsi[u];
    }
    cerr << endl;

  }

  return last_letsi;

}

vector<vector<int>> fill_vertices(vector<vector<int>> edgelist,
    vector<int> last_letsi, int nletsm1, int alphlen, int lasti,
    default_random_engine gen) {

  vector<vector<int>> edgelist2(nletsm1);
  int b;

  for (int i = 0; i < nletsm1; ++i) {

    for (int j = 0; j < alphlen; ++j) {

      b = edgelist[i][j];
      for (int h = 0; h < b; ++h) {
        edgelist2[i].push_back(j);
      }

    }

    shuffle(edgelist2[i].begin(), edgelist2[i].end(), gen);

    if (i != lasti) edgelist2[i].push_back(last_letsi[i]);

  }

  return edgelist2;

}

string walk_euler(vector<vector<int>> edgelist, int seqlen, int k,
    vector<char> lets_uniq, default_random_engine gen, vector<int> last_letsi,
    string firstl, string lastl, int lasti) {

  vector<int> out_i, edgelist_counter;
  int alphlen = lets_uniq.size();
  int nletsm1 = edgelist.size();
  int current {0};
  string out;

  for (int i = 0; i < nletsm1; ++i) {
    edgelist_counter.push_back(0);
  }

  for (int i = 0; i < firstl.length(); ++i) {
    for (int j = 0; j < alphlen; ++j) {
      if (firstl.substr(i, 1)[0] == lets_uniq[j]) {
        out_i.push_back(j);
        break;
      }
    }
  }

  //-------------------------------------------//
  cerr << "init: ";                            //
  for (int i = 0; i < out_i.size(); ++i) {     //
    cerr << lets_uniq[out_i[i]];               //
  }                                            //
  cerr << endl;                                //
  string tmp;                                  //
  //-------------------------------------------//
  for (int i = k; i < seqlen; ++i) {

    current = 0;
    for (int j = k - 2; j >= 0; --j) {
      current += pow(alphlen, j) * out_i[i - k];
    }

  //------------------------------------------------------//
    tmp += lets_uniq[current];                            //
    cerr << "current: " << lets_uniq[current] << endl;    //
    cerr << "seq: " << tmp << " [i=" << i << "]" << endl; //
  //------------------------------------------------------//

    out_i.push_back(edgelist[current][edgelist_counter[current]]);
    ++edgelist_counter[current];

  //----------------------------------------------------------------//
    cerr << "next: " << lets_uniq[out_i[out_i.size() - 1]] << endl; //
  //----------------------------------------------------------------//

  }

  for (int i = 0; i < out_i.size(); ++i) {
    out += lets_uniq[out_i[i]];
  }

  //-------------------------------------------//
  cerr << "lastl: " << lastl << endl;          //
  //-------------------------------------------//
  out += lastl;
  //--------------------------------------------------------------//
  cerr << "final: " << out << "[" << out.length() << "]" << endl; //
  //--------------------------------------------------------------//

  return out;

}

string shuffle_euler(vector<char> letters, default_random_engine gen, int k,
    bool verbose) {

  int seqlen = letters.size();
  int alphlen, nlets, nletsm1;
  int lasti {-1};
  vector<int> let_counts, last_letsi;
  vector<char> lets_uniq;
  set<int> lets_set;
  vector<string> klets, kletsm1;
  vector<vector<int>> edgelist;
  string firstl, lastl, out;

  cerr << "char count: " << letters.size() << endl;

  for (int i = 0; i < k - 1; ++i) {
    firstl += letters[i];
  }
  for (int i = seqlen - k + 1; i < seqlen; ++i) {
    lastl += letters[i];
  }

  for (int i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());

  alphlen = lets_uniq.size();
  nlets = pow(alphlen, k);
  nletsm1 = pow(alphlen, k - 1);

  klets = make_klets(lets_uniq, k);
  let_counts = count_klets(letters, klets, lets_uniq, k, alphlen);
  kletsm1 = make_klets(lets_uniq, k - 1);

  for (int i = 0; i < nletsm1; ++i) {
    if (lastl.compare(kletsm1[i]) == 0) {
      lasti = i;
      break;
    }
  }

  edgelist = make_edgelist(let_counts, nletsm1, alphlen);

  int counter = 0;
  for (int i = 0; i < nletsm1; ++i) {
    for (int j = 0; j < alphlen; ++j) {
      cerr << klets[counter] << "  " << edgelist[i][j] << endl;
      ++counter;
    }
  }

  vector<bool> empty_vertices;
  for (int i = 0; i < nletsm1; ++i) {
    empty_vertices.push_back(true);
    for (int j = 0; j < alphlen; ++j) {
      if (edgelist[i][j] > 0) {
        empty_vertices[i] = false;
        break;
      }
    }
  }

  last_letsi = find_euler(edgelist, lasti, nletsm1, gen, alphlen, k, empty_vertices);

  cerr << "Last lets:" << endl;
  for (int i = 0; i < nletsm1; ++i) {
    cerr << kletsm1[i] << " " << lets_uniq[last_letsi[i]] << endl;
  }

  vector<vector<int>> edgelist2;
  for (int i = 0; i < last_letsi.size(); ++i) {
    if (i != lasti) --edgelist[i][last_letsi[i]];
  }
  edgelist2 = fill_vertices(edgelist, last_letsi, nletsm1, alphlen, lasti, gen);

  out = walk_euler(edgelist2, seqlen, k, lets_uniq, gen, last_letsi, firstl, lastl, lasti);

  return out;

}
