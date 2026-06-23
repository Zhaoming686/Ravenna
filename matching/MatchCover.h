//
// Created by cbr406 on 2/10/26.
//

// #ifndef SUBGRAPHMATCHING_MATCHCOVER_H
// #define SUBGRAPHMATCHING_MATCHCOVER_H

#include <vector>
#include <chrono>
// #include "utils/types.h"
#include "graph/graph.h"
#include <unordered_map>
// #include "utils/timer.hpp"
#include <unordered_set>
#include <set>
#include <fstream>
#include <sstream>

class MatCo
{
private:
    // from matching
    const Graph* query_;
    const Graph* data_;

    const size_t max_num_results_;// config
    const bool print_preprocessing_results_;
    const bool print_enumeration_results_;
    const bool homomorphism_;

    ui **candidates_;
    ui *candidates_count_;
    ui *order_;

    // time limit
    int time_limit_ms_; // <=0 means no limit
    std::chrono::high_resolution_clock::time_point start_time_;
    bool timeout_ = false;

    // NEW for diversity
    int qsiz;
    uint8_t* label_flag;
    // for each label, group its candidates
    std::unordered_map<LabelID, std::vector<ui>> labelToCandidates_;
    // initialize label_val
    int* label_val;
    ui* label_target = nullptr;
    // checkpoint for time
    // every 100ms
    const int interval_ms = 1000;

    struct EvalState;     // forward declaration
    EvalState* eval_ = nullptr;

    int next_t;                // next time point to record, initially 0ms, then 100ms, 200ms, ...
    double last_sat;
    double last_cov_sat;
    double last_avg_rel_cov;

    LabelID* labelsQuery;//label set
    ui labelsQuerySize;//label set size

    ui L;
    ui* label_cover_cnt;// number of covered vertices for each label
    size_t embedding_cnt_;

    std::vector<bool> visited_;// execution info
    size_t num_initial_results_;
    size_t num_keyvertex_;

    // original
    std::vector<std::vector<std::vector<uint>>> CandidateSets;
    std::vector<std::vector<bool>> CandidateSetFlag;
    std::vector<std::vector<bool>> adjacency_matrix ;
    std::vector<bool> KeyVertexSet;
    std::vector<uint> match_order;
    uint mutiexp_depth_;
    uint prune_depth_;

public:
    MatCo(const Graph* query_graph, const Graph* data_graph, size_t max_num_results, size_t embedding_cnt, ui **candidates,
          ui *candidates_count, ui *order, int TimeL, std::unordered_map<LabelID, std::vector<ui>> labelToCandidates);

    ~MatCo() = default;

    const std::vector<bool>& GetKeyVertexSet() const { return KeyVertexSet; }
    bool IsTimeout() const { return timeout_; }

    //brief Generate matching order ,isolate vertices and adj matrix
    void Preprocessing();
    void InitialMatching();

    // ===== Getters from the original matching code (used by main) =====
    void GetNumInitialResults(size_t &num_initial_results) { num_initial_results = num_initial_results_; }
    void GetNumKeyVertex(size_t &num_keyvertex) { num_keyvertex = num_keyvertex_; }


    void PrintKeyVertexSet();

    bool VerifyCorrectness(const std::string &kv_path);



private:
    void GenerateMatchingOrder();

    void BuildCP2LEOrder();

    //brief build adjacency matrix and calculate prune_depth_.
    void BuildAdjMatrix();

    void BuildCover();

    bool ComputeCand(uint depth,std::vector<uint> m);

    // This function checks if the current partial match satisfies the pruning condition.
    bool FullCoveragePrune(const uint depth,const std::vector<uint> &m);

    //Optimization with Linear Enumeration
    void MutiExpansion(std::vector<uint> m);

    //DFS to find whether there is a feasible match limit by same label
    bool MutiExpTest(int depth,const std::vector<uint> &label_same_index, std::vector<uint> &m);

    void CountRes(std::vector<uint> m);

    void FlushFlag(uint flush_depth);

    void PrintMatch(const std::vector<uint> &m);

    void FindMatCo(uint depth, std::vector<uint> m);


};

// #endif //SUBGRAPHMATCHING_MATCHCOVER_H