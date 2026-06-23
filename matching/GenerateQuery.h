#ifndef SUBGRAPHMATCHING_GENERATEQUERY_H
#define SUBGRAPHMATCHING_GENERATEQUERY_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <cmath>

#include "graph/graph.h"

using namespace std;

bool GenerateQueryD(ui QS, const Graph *data_graph, ui MinDen,
                    int dd, const string& out_dir);

bool GenerateQueryS(ui QS, const Graph *data_graph, ui MinDen,
                    int dd, bool stop, const string& out_dir);

void randomRemoval(int **a, int *countS, int NE, ui QS);

#endif