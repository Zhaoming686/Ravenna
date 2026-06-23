#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <cstdlib>
#include <ctime>
using namespace std;

static void usage(const char* p) {
    cerr << "Usage: " << p
         << " -in <input.graph> -out <output.graph> -L <label_num> [-seed <s>]\n";
}

int main(int argc, char** argv) {
    string in_path, out_path;
    int L = 0;
    unsigned seed = (unsigned)time(nullptr);

    for (int i = 1; i < argc; ++i) {
        string k = argv[i];
        if (i + 1 >= argc) { usage(argv[0]); return 1; }
        string v = argv[++i];
        if      (k == "-in")   in_path  = v;
        else if (k == "-out")  out_path = v;
        else if (k == "-L")    L        = stoi(v);
        else if (k == "-seed") seed     = (unsigned)stoul(v);
        else { usage(argv[0]); return 1; }
    }
    if (in_path.empty() || out_path.empty() || L <= 0) { usage(argv[0]); return 1; }

    ifstream fin(in_path);
    if (!fin) { cerr << "cannot open " << in_path << "\n"; return 1; }
    ofstream fout(out_path);
    if (!fout) { cerr << "cannot write " << out_path << "\n"; return 1; }

    mt19937 gen(seed);
    uniform_int_distribution<int> dis(0, L - 1);

    string line;
    size_t v_count = 0;
    while (getline(fin, line)) {
        if (line.empty()) { fout << "\n"; continue; }

        // Only process vertex lines 'v <id> <old_label> [degree]'
        if (line[0] == 'v') {
            istringstream iss(line);
            string tag; int id, old_label;
            iss >> tag >> id >> old_label;
            int deg = -1;
            iss >> deg;            // optional

            int new_label = dis(gen);
            fout << "v " << id << " " << new_label;
            if (deg >= 0) fout << " " << deg;
            fout << "\n";
            ++v_count;
        } else {
            // Output t / e / other lines unchanged
            fout << line << "\n";
        }
    }

    cerr << "Relabeled " << v_count << " vertices into [0, " << L - 1
         << "] -> " << out_path << "  (seed=" << seed << ")\n";
    return 0;
}