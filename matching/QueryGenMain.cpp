#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "GenerateQuery.h"
#include "graph/graph.h"

using namespace std;

static void usage(const char* p) {
    cerr << "Usage: " << p
     << " -Ldir <path/to/L15> [-dataset <stem>]"
        " [-qprop dense|sparse] [-mind <MinDen>] [-seed <s>]\n"
     << "  -dataset: e.g. 'dblp_15' to process only data_graph/dblp_15.graph\n";
}

static bool ends_with(const string& s, const string& suf) {
    return s.size() >= suf.size() &&
           s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
}

// Recursive mkdir; do not report an error if it already exists
static void mkdir_p(const string& path) {
    if (path.empty()) return;
    for (size_t i = 1; i <= path.size(); ++i) {
        if (i == path.size() || path[i] == '/') {
            string sub = path.substr(0, i);
            if (!sub.empty()) mkdir(sub.c_str(), 0755);
        }
    }
}

int main(int argc, char** argv) {
    string Ldir, qprop = "dense";
    int MinDen = 3;
    string dataset;
    unsigned seed = (unsigned)time(nullptr);

    for (int i = 1; i < argc; ++i) {
        string k = argv[i];
        if (i + 1 >= argc) { usage(argv[0]); return 1; }
        string v = argv[++i];
        if      (k == "-Ldir")  Ldir   = v;
        else if (k == "-qprop") qprop  = v;
        else if (k == "-mind")  MinDen = stoi(v);
        else if (k == "-seed")  seed   = (unsigned)stoul(v);
        else if (k == "-dataset")  dataset = v;
        else { usage(argv[0]); return 1; }
    }
    if (Ldir.empty()) { usage(argv[0]); return 1; }

    srand(seed);

    string data_dir  = Ldir + "/data_graph";
    string query_dir = Ldir + "/query_graph";
    mkdir_p(query_dir);

    const bool sparse = (qprop == "sparse");
    const vector<int> sizes = {24};
    const int N = 1;

    DIR* dir = opendir(data_dir.c_str());
    if (!dir) {
        cerr << "cannot open " << data_dir << "\n";
        return 1;
    }

    while (dirent* ent = readdir(dir)) {
        string name = ent->d_name;
        if (name == "." || name == "..") continue;
        if (!ends_with(name, ".graph")) continue;
        if (!dataset.empty() && name != dataset + ".graph") continue;   // NEW

        string stem    = name.substr(0, name.size() - 6);   // dblp_15
        string in_path = data_dir + "/" + name;
        string out_dir = query_dir + "/" + stem;            // L15/query_graph/dblp_15
        mkdir_p(out_dir);

        Graph* data_graph = new Graph(true);
        data_graph->loadGraphFromFile(in_path);
        cout << "Loaded " << in_path
             << " |V|=" << data_graph->getVerticesCount()
             << " |E|=" << data_graph->getEdgesCount() << "\n";

        for (int QS : sizes) {
            int produced = 0, attempt = 0, maxAttempt = N * 200;
            while (produced < N && attempt < maxAttempt) {
                bool ok = sparse
                    ? GenerateQueryS((ui)QS, data_graph, (ui)MinDen,
                                     produced + 1, true, out_dir)
                    : GenerateQueryD((ui)QS, data_graph, (ui)MinDen,
                                     produced + 1, out_dir);
                if (ok) ++produced;
                ++attempt;
            }
            cout << "  size=" << QS << ": " << produced << "/" << N
                 << " in " << attempt << " attempts.\n";
        }
        delete data_graph;
    }
    closedir(dir);

    cout << "All done. seed=" << seed << "\n";
    return 0;
}