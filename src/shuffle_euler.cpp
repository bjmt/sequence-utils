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

  /* 1D vector<int> --> 2D vector<vector<int>>
   * The first layer elements are vertices, second layer are the edges.
   */

  vector<vector<int>> edgelist(nletsm1, vector<int>(alphlen));
  int counter{0};

  for (int i = 0; i < nletsm1; ++i) {

    edgelist[i].reserve(alphlen);

    for (int j = 0; j < alphlen; ++j) {
      edgelist[i][j] = let_counts[counter];
      ++counter;
    }

  }

  return edgelist;

}

vector<int> find_euler(vector<vector<int>> edgelist, int lasti, int nletsm1,
    default_random_engine gen, int alphlen, int k, vector<bool> empty_vertices,
    bool verbose) {

  int u;
  int nletsm2 = pow(alphlen, k - 2);
  int good_v{0}, counter{0};
  vector<bool> vertices(nletsm1, false);
  vector<int> last_letsi(nletsm1, 0);
  vector<int> next_let_i;
  next_let_i.reserve(nletsm1);

  /* The idea is to go through and make sure that every last letter for each
   * vertex makes it so that a walk with no dead-ends to the tree root is
   * possible. Of course, not all klets/vertices have to be connected in
   * edge graph; these don't need to be checked.
   */

  vertices[lasti] = true;  /* tree root */

  /* I don't think there's a formula for this, so just prepare these beforehand */
  for (int i = 0; i < nletsm1; ++i) {
    next_let_i.push_back(counter * alphlen);
    if (counter == nletsm2 - 1) counter = 0;
    else ++counter;
  }

  for (int i = 0; i < nletsm1; ++i) {
    if (empty_vertices[i]) vertices[i] = true;  /* ignore unconnected vertices */
    else ++good_v;
  }

  if (verbose) cerr << "    Total vertices to travel: " << good_v << endl;

  for (int i = 0; i < nletsm1; ++i) {

    u = i;

    while (!vertices[u]) {
      /* pick a random possible edge from the vertex */
      discrete_distribution<int> next_let(edgelist[u].begin(), edgelist[u].end());
      last_letsi[u] = next_let(gen);
      /* now follow the edge to the next vertex */
      if (k == 2)
        u = last_letsi[u];
      else
        u = next_let_i[u] + last_letsi[u];
    }

    u = i;

    /* after a sucessful walk, go back and prevent these vertices from being
     * checked again
     */
    while (!vertices[u]) {
      vertices[u] = true;
      if (k == 2)
        u = last_letsi[u];
      else
        u = next_let_i[u] + last_letsi[u];
    }

  }

  return last_letsi;

}

vector<vector<int>> fill_vertices(vector<vector<int>> edgelist,
    vector<int> last_letsi, int nletsm1, int alphlen, int lasti,
    default_random_engine gen, vector<bool> empty_vertices) {

  /* The incoming edgelist is just a set of counts for each letter. This
   * will actually create vectors of letter indices based on counts.
   */

  /* TODO: find a cheaper alternative */

  vector<vector<int>> edgelist2(nletsm1);
  int b;

  for (int i = 0; i < nletsm1; ++i) {

    if (empty_vertices[i]) continue;

    edgelist2[i].reserve(accumulate(edgelist2[i].begin(), edgelist2[i].end(), 0));

    for (int j = 0; j < alphlen; ++j) {

      b = edgelist[i][j];
      for (int h = 0; h < b; ++h) {
        edgelist2[i].push_back(j);
      }

    }

    shuffle(edgelist2[i].begin(), edgelist2[i].end(), gen);

    /* to ensure the walk is Eulerian, manually insert the last edges */
    if (i != lasti) edgelist2[i].push_back(last_letsi[i]);

  }

  return edgelist2;

}

vector<int> walk_euler(vector<vector<int>> edgelist, int seqlen, int k,
    vector<char> lets_uniq, default_random_engine gen, vector<int> last_letsi,
    string firstl, int lasti) {

  vector<int> out_i;
  int alphlen = lets_uniq.size();
  int nletsm1 = edgelist.size();
  int current{0};
  int n = firstl.length();
  vector<int> edgelist_counter(nletsm1, 0);
  out_i.reserve(seqlen);

  /* initialize shuffled sequence with starting vertex */
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < alphlen; ++j) {
      if (firstl.substr(i, 1)[0] == lets_uniq[j]) {
        out_i.push_back(j);
        break;
      }
    }
  }

  /* walk */

  for (int i = n - 1; i < seqlen - 1; ++i) {

    /* find out which vertex we are sitting on */
    current = 0;
    for (int j = n - 1; j >= 0; --j) {
      current += pow(alphlen, j) * (out_i[i - j] % alphlen);
    }

    /* select a random availabe edge */
    out_i.push_back(edgelist[current][edgelist_counter[current]]);
    ++edgelist_counter[current];

  }

  return out_i;

}

string shuffle_euler(vector<char> letters, default_random_engine gen, int k,
    bool verbose) {

  int seqlen = letters.size();
  int alphlen, nlets, nletsm1;
  int lasti{0};
  vector<int> let_counts, last_letsi, out_i;
  vector<char> lets_uniq;
  set<int> lets_set;
  vector<vector<int>> edgelist;
  string firstl, out;

  /* the first and last letters remain unchanged; these are special vertices
   * which only have a single directed edge to them
   */
  for (int i = 0; i < k - 1; ++i) {
    firstl += letters[i];
  }

  for (int i = 0; i < seqlen; ++i) {
    lets_set.insert(letters[i]);
  }
  lets_uniq.assign(lets_set.begin(), lets_set.end());

  alphlen = lets_uniq.size();
  nlets = pow(alphlen, k);
  nletsm1 = pow(alphlen, k - 1);

  let_counts = count_klets(letters, lets_uniq, k, alphlen);

  for (int i = k - 2; i >= 0; --i) {
    for (int j = 0; j < alphlen; ++j) {
      if (letters[seqlen - 1 - i] == lets_uniq[j]) {
        lasti += pow(alphlen, i) * j;
        continue;
      }
    }
  }

  /* edgelist with letter counts */
  edgelist = make_edgelist(let_counts, nletsm1, alphlen);

  /* check for unconnected vertices; ignore these when searching for a new
   * Eulerian path
   */
  vector<bool> empty_vertices;
  empty_vertices.reserve(nletsm1);
  for (int i = 0; i < nletsm1; ++i) {
    empty_vertices.push_back(true);
    for (int j = 0; j < alphlen; ++j) {
      if (edgelist[i][j] > 0) {
        empty_vertices[i] = false;
        break;
      }
    }
  }


  if (verbose) cerr << "  Finding a random Eulerian path" << endl;

  /* find a new Eulerian path */
  last_letsi = find_euler(edgelist, lasti, nletsm1, gen, alphlen, k,
      empty_vertices, verbose);

  /* delete last edges from edge pool */
  vector<vector<int>> edgelist2;
  for (int i = 0; i < last_letsi.size(); ++i) {
    if (i != lasti) --edgelist[i][last_letsi[i]];
  }

  if (verbose) cerr << "  Generating random edges" << endl;

  /* generate edge indices + shuffle */
  edgelist2 = fill_vertices(edgelist, last_letsi, nletsm1, alphlen, lasti, gen,
      empty_vertices);

  if (verbose) cerr << "  Walking new Eulerian path" << endl;

  /* walk new Eulerian path */
  out_i = walk_euler(edgelist2, seqlen, k, lets_uniq, gen, last_letsi, firstl,
      lasti);

  /* indices --> letters */
  out.reserve(out_i.size());
  for (int i = 0; i < out_i.size(); ++i) {
    out += lets_uniq[out_i[i]];
  }

  return out;

}
