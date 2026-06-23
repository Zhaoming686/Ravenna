#include <algorithm>
#include <iostream>
#include <vector>
// #include "utils/types.h"
// #include "utils/globals.h"
// #include "utils/utils.h"
#include "graph/graph.h"
#include "matching/MatchCover.h"
#include "matching/EvaluateQuery.h"

#define NOT_EXIST UINT_MAX
#define UNMATCHED UINT_MAX


struct MatCo::EvalState {
    EvaluateQuery::CheckpointRecorder ck;
    // and label_flag / label_val / label_cover_cnt
};


MatCo::MatCo(const Graph* query_graph, const Graph* data_graph, size_t max_num_results, size_t embedding_cnt, ui **candidates,
          ui *candidates_count, ui *order, int TimeL, std::unordered_map<LabelID, std::vector<ui>> labelToCandidates)

: query_(query_graph)
, data_(data_graph)
, max_num_results_(max_num_results)
, print_preprocessing_results_(false)// set true
, print_enumeration_results_(false)// set true
, homomorphism_(false)// set false，need to make sure again
, visited_(data_graph->getVerticesCount() , false)
, num_initial_results_(0ul)
, num_keyvertex_(0ul)
, mutiexp_depth_(0u)
, prune_depth_(0u)
, candidates_(candidates)
, candidates_count_(candidates_count)
, embedding_cnt_(embedding_cnt)
, labelToCandidates_(labelToCandidates)
, order_(order) {
    time_limit_ms_ = TimeL * 1000;
    eval_ = new EvalState();
}


void MatCo::Preprocessing()
{
    // GenerateMatchingOrder();
    //Use the external order to override  the initial match_order directly
    match_order.assign(order_, order_ + query_->getVerticesCount());
    // #ifdef CP2LE
    BuildCP2LEOrder();
    // #endif
    BuildAdjMatrix();
}

void MatCo::BuildAdjMatrix(){
    
    //build adjacency matrix of query graph
    adjacency_matrix.reserve(query_->getVerticesCount());
    for(int i = 0; i<query_->getVerticesCount();i++){
        adjacency_matrix.emplace_back();
        adjacency_matrix[i].reserve(query_->getVerticesCount());
        adjacency_matrix[i].resize(query_->getVerticesCount(),false);
    }
    
    for(int i = 0; i<query_->getVerticesCount();i++){
        ui cnt = 0;
        const VertexID* nbrs = (const VertexID*)query_->getVertexNeighbors(i, cnt);
        for (ui k = 0; k < cnt; ++k) {
            VertexID j = nbrs[k];
            adjacency_matrix[i][j] = true;
            adjacency_matrix[j][i] = true;
        }
    }

    //compute prune_depth_, When search depth > prune_depth_, it means no local candidate space is null. 
    std::vector<bool> AllRN(query_->getVerticesCount(), false);
    for(uint i = 0; i < query_->getVerticesCount()-1; i++)// The following block computes prune_depth_ during preprocessing. When the search depth is greater than prune_depth_, no local candidate space is empty.
    {
        std::fill(AllRN.begin(),AllRN.end(),false);
        for(uint s = 0 ; s <= i ; s++)
        {
            uint uf = match_order[s];
            ui cnt = 0;
            const VertexID* nbrs = (const VertexID*)query_->getVertexNeighbors(uf, cnt);
            for (ui k = 0; k < cnt; ++k) {
                VertexID j = nbrs[k];
                AllRN[j] = true;
            }

        }
        bool flag = true;
        for(uint j = i +1 ; j < query_->getVerticesCount(); j++)
        {
            uint ub = match_order[j];
            if(!AllRN[ub]) {
                flag = false;
                break;
            }
        }
        if(flag) {
            prune_depth_ = i;
            break;
        }
    }
    if(print_preprocessing_results_) std::cout<<"prune_depth_: "<<prune_depth_<<std::endl;
}
void MatCo::BuildCP2LEOrder(){
    if(print_preprocessing_results_) std::cout<<"BuildCP2LEOrder...."<<std::endl;
    mutiexp_depth_ = UINT32_MAX;
    int sum = 0;
    for(int i = query_->getVerticesCount()-1; i>=0; i--){
        uint u = match_order[i];
        bool flag = true;
        for(int j = i+1; j<query_->getVerticesCount(); j++){
            uint v = match_order[j];
            if(query_->checkEdgeExistence(u,v)){
                flag = false;
                break;
            }
        }
        if(flag) {
            sum++;
            for(int j = i; j<query_->getVerticesCount()-1; j++){
                match_order[j] = match_order[j+1];
            }
            match_order[query_->getVerticesCount()-1] = u;
        }
    }
    mutiexp_depth_ = query_->getVerticesCount()-sum;
    std::reverse(match_order.begin() + mutiexp_depth_,  match_order.end());
    if(print_preprocessing_results_){
        std::cout<<"CP2LEOrder: "<<std::endl;
        for(int i =0 ; i<query_->getVerticesCount();i++){
            std::cout<<match_order[i]<<" ";
        }
        std::cout<<std::endl;
        std::cout<<"CP2LE_depth_:"<<mutiexp_depth_;
    }
    std::cout<<std::endl;
    
}
void MatCo::GenerateMatchingOrder()
{

    uint max_degree = 0u;
    uint max_degree_id = 0u;
    for (uint i = 0; i < query_->getVerticesCount(); i++)
    {
        if (query_->getVertexDegree(i) > max_degree)
        {
            max_degree = query_->getVertexDegree(i);
            max_degree_id=i;
        }
    }
    match_order.resize(query_->getVerticesCount());
    std::vector<bool> visited(query_->getVerticesCount(), false);
    match_order[0] = max_degree_id;
    visited[max_degree_id] = true;

    for (uint i = 1; i < query_->getVerticesCount(); ++i)
    {
        uint max_adjacent = 0;
        uint max_adjacent_u = NOT_EXIST;

        for (size_t j = 0; j < query_->getVerticesCount(); j++)
        {
            uint cur_adjacent = 0u;
            if (visited[j]) continue;

            ui cnt = 0;
            const VertexID* q_nbrs = (const VertexID*)query_->getVertexNeighbors(j, cnt);
            for (ui k = 0; k < cnt; ++k) {
                VertexID other = q_nbrs[k];
                if (visited[other])
                    cur_adjacent++;
            }

            if (!cur_adjacent) continue;

            if (
                max_adjacent_u == NOT_EXIST ||
                (cur_adjacent == max_adjacent &&
                    query_->getVertexDegree(j) > query_->getVertexDegree(max_adjacent_u)) ||
                cur_adjacent > max_adjacent
            ) {
                max_adjacent = cur_adjacent;
                max_adjacent_u = j;
            }
        }

        match_order[i] = max_adjacent_u;
        visited[max_adjacent_u] = true;
    }
    
    //print order
    if(print_preprocessing_results_){
        std::cout<<"origin match order: "<<std::endl;
        for(uint i = 0; i < query_->getVerticesCount(); i++)
        {
            std::cout<<match_order[i]<<" ";
        }
        std::cout<<std::endl;
    }
    
}

void MatCo::InitialMatching()
{
    timeout_ = false;
    start_time_ = std::chrono::high_resolution_clock::now();

    // NEW for diversity
    qsiz = query_->getVerticesCount();
    label_flag = nullptr;
    EvaluateQuery::InitQueryLabelFlag(query_, label_flag);

    // for each label, group its candidates
    // EvaluateQuery::groupCandidatesByLabel(query_, qsiz, candidates_, candidates_count_, labelToCandidates);
    // EvaluateQuery::deduplicateLabelMap(labelToCandidates);

    // initialize label_val
    label_val = nullptr;
    label_target = nullptr;
    EvaluateQuery::InitLabelValByCandidates(query_, labelToCandidates_, label_val, label_target);

    EvaluateQuery::InitCheckpointRecorder(eval_->ck, time_limit_ms_ /*ms*/, interval_ms);
    next_t = 0;                // next time point to record, initially 0ms, then 100ms, 200ms, ...
    last_sat = 0.0;            // hard / step-function satisfaction
    last_cov_sat = 0.0;        // min-based coverage satisfaction
    last_avg_rel_cov = 0.0;    // average relative coverage rate

    labelsQuery = nullptr;     // label set
    labelsQuerySize = 0;       // label set size
    EvaluateQuery::BuildLabelsQuery(query_, query_->getVerticesCount(), labelsQuery, labelsQuerySize);

    L = query_->getLabelsCount();
    label_cover_cnt = new ui[L];// number of covered vertices for each label
    std::fill(label_cover_cnt, label_cover_cnt + L, 0);

    BuildCover();
}

void MatCo::BuildCover()
{
    uint max_degree_ = data_->getGraphMaxDegree();
    uint qsize_ = query_->getVerticesCount();
    //initialize local candidate space
    CandidateSets.reserve(qsize_);
    for (int i = 0; i < qsize_; ++i) {
        CandidateSets.emplace_back();
        CandidateSets[i].reserve(qsize_);
        for (int j = 0; j < qsize_; ++j) {
            CandidateSets[i].emplace_back();
            CandidateSets[i][j].reserve(max_degree_); // the candidate set size will not exceed the maximum degree of the data graph
        }
    }

    // Insert the root node's
    ui u0 = match_order[0];

    CandidateSets[0][0].assign(candidates_[u0], candidates_[u0] + candidates_count_[u0]);
    std::sort(CandidateSets[0][0].begin(), CandidateSets[0][0].end());
    CandidateSets[0][0].erase(std::unique(CandidateSets[0][0].begin(), CandidateSets[0][0].end()),
                              CandidateSets[0][0].end());


    //initialize flags of local candidate space. 
    //True means the elements of the candidate set are all key vertices.
    CandidateSetFlag.reserve(qsize_);
    for(int i = 0; i<qsize_;i++){
        CandidateSetFlag.emplace_back();
        CandidateSetFlag[i].resize(qsize_,false);
    }

    std::vector<uint> m(query_->getVerticesCount(), UNMATCHED);
    KeyVertexSet = std::vector<bool>(data_->getVerticesCount(), false); //initialize key vertex flags

    for (ui idx = 0; idx < CandidateSets[0][0].size(); ++idx) {
        ui v = CandidateSets[0][0][idx];

        m[u0] = v;
        visited_[v] = true;

        FindMatCo(1, m);

        visited_[v] = false;
        m[u0] = UNMATCHED;
    }

    // final checkpointing at TimeL
    EvaluateQuery::ComputeSatCovAndRelCov(last_sat, last_cov_sat, last_avg_rel_cov, labelsQuery, labelsQuerySize, label_flag, label_cover_cnt, label_target);
while (next_t <= time_limit_ms_) {
    EvaluateQuery::PushCheckpoint(eval_->ck, next_t, last_sat, last_cov_sat, last_avg_rel_cov, 0, 0, embedding_cnt_);
    next_t += interval_ms;
}

    std::cout << "\n--- Checkpoints ---\n";
    if (embedding_cnt_ == 0) {
        for (ui i = 0; i < eval_->ck.sz; ++i) {
            std::cout << "CHECK" << "\t"
  << eval_->ck.time_point[i] << "\t"
  << eval_->ck.sat_point[i] << "\t"
  << eval_->ck.cov_sat_point[i] << "\t"
  << eval_->ck.avg_rel_cov_point[i] << "\t"
  << eval_->ck.call_count_point[i] << "\t"
  << eval_->ck.prune_count_point[i] << "\t"
  << -1 << "\n";
        }
    }else {
        for (ui i = 0; i < eval_->ck.sz; ++i) {
            std::cout << "CHECK" << "\t"
  << eval_->ck.time_point[i] << "\t"
  << eval_->ck.sat_point[i] << "\t"
  << eval_->ck.cov_sat_point[i] << "\t"
  << eval_->ck.avg_rel_cov_point[i] << "\t"
  << eval_->ck.call_count_point[i] << "\t"
  << eval_->ck.prune_count_point[i] << "\t"
  << eval_->ck.embedding_cnt_point[i] << "\n";
        }
    }

}




void MatCo::FindMatCo(uint depth, std::vector<uint> m)
{
    // if (reach_time_limit) return;

    if (timeout_) return;

    auto now = std::chrono::high_resolution_clock::now();
    auto ens = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();

    // checkpointing, make sure we record at every 100ms
    if (next_t <= time_limit_ms_ && ens >= next_t) {
        EvaluateQuery::ComputeSatCovAndRelCov(last_sat, last_cov_sat, last_avg_rel_cov, labelsQuery, labelsQuerySize, label_flag, label_cover_cnt, label_target);
while (next_t <= time_limit_ms_ && ens >= next_t) {
    EvaluateQuery::PushCheckpoint(eval_->ck, next_t, last_sat, last_cov_sat, last_avg_rel_cov, 0, 0, embedding_cnt_);
    next_t += interval_ms;
}
    }

    if (ens > time_limit_ms_) {
        timeout_ = true;
        return;
    }

    if (num_initial_results_ >= max_num_results_) return; //max_num_results_ is set to ULONG_MAX by default 
    if(!ComputeCand(depth,m)) {
        // empty_set_num++; // exist empty candidate set, return
        return;
    }
    
    //prune by local candidate space
    // #ifdef FullCoverage
    if(depth>=prune_depth_&&FullCoveragePrune(depth,m)) return;
    // #endif

    //optimization with linear enumeration
    // #ifdef CP2LE
    if(depth == mutiexp_depth_){
        MutiExpansion(m);
        return;
    }
    // #endif

    // normal enumeration
    uint u = match_order[depth];
    //
    if(CandidateSets[depth][depth].size() == 0){// Isolated vertex
        // for (size_t i = 0; i < data_->getVerticesCount(); i++)
        // if (data_->getVertexLabel(i) != NOT_EXIST)
        // if (query_->getVertexLabel(u) == data_->getVertexLabel(i))
        // {
        //     CandidateSets[depth][depth].push_back(i);
        // }
        CandidateSets[depth][depth].assign(candidates_[u], candidates_[u] + candidates_count_[u]);
        std::sort(CandidateSets[depth][depth].begin(), CandidateSets[depth][depth].end());
        CandidateSets[depth][depth].erase(
            std::unique(CandidateSets[depth][depth].begin(), CandidateSets[depth][depth].end()),
            CandidateSets[depth][depth].end()
        );
    }
    if(CandidateSets[depth][depth].size() == 0) return;

    for(uint i =0 ; i< CandidateSets[depth][depth].size();i++){
        uint v = CandidateSets[depth][depth][i];
        if (!homomorphism_ && visited_[v]) continue;
        m[u] = v;
        visited_[v] = true;
        
        if (depth == query_->getVerticesCount() - 1)
        {   
            num_initial_results_++;
            embedding_cnt_++;
            for(auto j: m) {
                if(KeyVertexSet[j]) continue;
                else{
                    num_keyvertex_++;
                    // KeyVertexSet[j] = true;
                    EvaluateQuery::UpdateLabelVal(data_, j, KeyVertexSet, label_flag, label_val, label_cover_cnt);

                }
            }
            if(print_enumeration_results_) PrintMatch(m);
        }
        else
        {
            FindMatCo(depth + 1, m);
        }
        visited_[v] = false;
        m[u] = UNMATCHED;
        if (num_initial_results_ >= max_num_results_) return;
        // if (reach_time_limit) return;

        if (timeout_) return;

        auto now = std::chrono::high_resolution_clock::now();
        auto ens = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();

        // checkpointing, make sure we record at every 100ms
        if (next_t <= time_limit_ms_ && ens >= next_t) {
            EvaluateQuery::ComputeSatCovAndRelCov(last_sat, last_cov_sat, last_avg_rel_cov, labelsQuery, labelsQuerySize, label_flag, label_cover_cnt, label_target);
while (next_t <= time_limit_ms_ && ens >= next_t) {
    EvaluateQuery::PushCheckpoint(eval_->ck, next_t, last_sat, last_cov_sat, last_avg_rel_cov, 0, 0, embedding_cnt_);
    next_t += interval_ms;
}
        }

        if (ens > time_limit_ms_) {
            timeout_ = true;
            return;
        }
    }
}

void MatCo::MutiExpansion(std::vector<uint> m){

    std::vector<uint> label_same_index ;
    std::vector<bool> label_check(query_->getVerticesCount(),false);
    for(uint i = mutiexp_depth_;i<query_->getVerticesCount();i++){
        if(label_check[i]) continue;
        uint uf = match_order[i];
        label_check[i] = true;
        label_same_index.clear();
        label_same_index.push_back(i);
        //find the vertices with the same label in isolated vertices
        for(uint j = mutiexp_depth_;j<query_->getVerticesCount();j++){
            if(label_check[j]) continue;
            uint ub = match_order[j];
            if(query_->getVertexLabel(uf) == query_->getVertexLabel(ub)){
                label_same_index.push_back(j);
                label_check[j] = true;
            }  
        }
        bool flag = false;
        uint u_index = label_same_index[0];
        uint u = match_order[u_index];
        //DFS to find whether there is a feasible match limit by same label
        for(int s = 0 ; s <CandidateSets[mutiexp_depth_][u_index].size();s++){
            uint v = CandidateSets[mutiexp_depth_][u_index][s];
            if(visited_[v]) continue;
            visited_[v] = true;
            m[u] = v;
            if(MutiExpTest(1,label_same_index,m)) {
                visited_[v] = false;
                flag = true;
                break;
            }
            m[u] = UNMATCHED;
            visited_[v] = false;
        }
        if(!flag) return; //no feasible match
    }
    //Now, we can reduce Cartesian products into linear enumeration.
    CountRes(m);
    FlushFlag(mutiexp_depth_-1);
}
void MatCo::CountRes(std::vector<uint> m){
    //if current match m exists uncovered vertices, then it is a result.
    bool flag_all_cv = true ;
    for(uint i = 0 ; i<query_->getVerticesCount();i++){
        uint u = match_order[i];
        if(KeyVertexSet[m[u]]==false){
            // KeyVertexSet[m[u]] = true;
            EvaluateQuery::UpdateLabelVal(data_, m[u], KeyVertexSet, label_flag, label_val, label_cover_cnt);
            num_keyvertex_++;
            flag_all_cv = false ; 
        }
    }
    if(!flag_all_cv){
        num_initial_results_++;
        embedding_cnt_++;
        if(print_enumeration_results_) PrintMatch(m);
    }
    //Go through local candidate space of and use uncovered vertices to replace m[u]
    for(uint i = mutiexp_depth_;i<query_->getVerticesCount();i++){
        for(auto cand:CandidateSets[mutiexp_depth_][i]){
            if(KeyVertexSet[cand]==false){
                // KeyVertexSet[cand] = true;
                EvaluateQuery::UpdateLabelVal(data_, cand, KeyVertexSet, label_flag, label_val, label_cover_cnt);
                num_keyvertex_++;
                num_initial_results_++;
                embedding_cnt_++;
                if(print_enumeration_results_){
                    uint u = match_order [i] ; 
                    m[u] = cand ;
                    PrintMatch(m);
                }
            }
        }
    }
    for(uint i = mutiexp_depth_;i<query_->getVerticesCount();i++){
        uint u = match_order [i] ; 
        m[u] = UNMATCHED ;
    }
}

//When backtracking, flush the flag of local candidate space
void MatCo::FlushFlag(uint flush_depth){
    uint u = match_order[flush_depth];
    for(uint i = flush_depth+1;i<query_->getVerticesCount();i++){
        uint ub = match_order[i];
        if(adjacency_matrix[u][ub]) continue;
        // the vertex,which will backtracking, is not adjacent to the local candidate space
        // so the flag of local candidate space will be true.(all elements of the candidate set are key vertices)
        else{
            CandidateSetFlag[flush_depth][i] = true;
        }
    }
}
void MatCo::PrintMatch(const std::vector<uint> &m){
    for(auto j: m)
    {
        std::cout << j << " ";
    }
    std::cout << std::endl;
}

void MatCo::PrintKeyVertexSet(){
    std::cout<<"Key vertices: ";
    for(uint i = 0 ; i<KeyVertexSet.size();i++){
        if(KeyVertexSet[i]) std::cout<<i<<" ";
    }
    std::cout<<std::endl;
}

bool MatCo::VerifyCorrectness(const std::string& kvPath) {
    std::ifstream inputFile(kvPath);
    if (!inputFile.is_open()) {
        std::cerr << "file path error: " << kvPath << std::endl;
        return false;
    }

    std::set<int> kvSet;
    for(auto i = 0; i < KeyVertexSet.size(); i++){
        if(KeyVertexSet[i]) kvSet.insert(i);
    }

    std::set<int> verifySet;
    std::string line;
    while (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        std::string number;
        
        while (std::getline(ss, number, ',')) {
            try {
                verifySet.insert(std::stoi(number));
            } catch (const std::invalid_argument&) {
                std::cerr << "Invalid: " << number << std::endl;
                return false;
            }
        }
    }

    //VerifyCorrectness
    return kvSet == verifySet;
}

//DFS to find whether there is a feasible match limit by same label
bool MatCo::MutiExpTest(int depth,const std::vector<uint> &label_same_index, std::vector<uint> &m) {
    // if(reach_time_limit) return false;

    if (timeout_) return false;

    auto now = std::chrono::high_resolution_clock::now();
    auto ens = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_).count();

    // checkpointing, make sure we record at every 100ms
    if (next_t <= time_limit_ms_ && ens >= next_t) {
        EvaluateQuery::ComputeSatCovAndRelCov(last_sat, last_cov_sat, last_avg_rel_cov, labelsQuery, labelsQuerySize, label_flag, label_cover_cnt, label_target);
while (next_t <= time_limit_ms_ && ens >= next_t) {
    EvaluateQuery::PushCheckpoint(eval_->ck, next_t, last_sat, last_cov_sat, last_avg_rel_cov, 0, 0, embedding_cnt_);
    next_t += interval_ms;
}
    }

    if (ens > time_limit_ms_) {
        timeout_ = true;
        return false;
    }

    if(depth==label_same_index.size()) return true;
    uint u_index = label_same_index[depth];
    uint u = match_order[u_index];
    for(int i = 0 ; i < CandidateSets[mutiexp_depth_][u_index].size();i++){
        uint v = CandidateSets[mutiexp_depth_][u_index][i];
        if(visited_[v]) continue;
        visited_[v]=true;
        m[u] = v;
        if(MutiExpTest(depth+1, label_same_index,m)) {
            visited_[v]=false;
            return true;
        }
        m[u] = UNMATCHED;
        visited_[v]=false;
    }
    return false;
    
}


bool MatCo::ComputeCand(uint depth,std::vector<uint> m){
    uint uf = match_order[depth-1]; // uf is the last matched query vertex
    uint vf = m[uf]; // vf is the matched vertex of uf

    ui cnt = 0;
    const VertexID* vf_nbrs = (const VertexID*)data_->getVertexNeighbors(vf, cnt);

    for(uint i = depth;i<query_->getVerticesCount();i++){
        uint ub = match_order[i];
        if(adjacency_matrix[uf][ub]){ // uf and ub are adjacent
            CandidateSets[depth][i].clear();
            //local candidate space is null
            // if(CandidateSets[depth-1][i].size() == 0){
            //     for (ui k = 0; k < cnt; ++k) {
            //         VertexID vf_nei = vf_nbrs[k];
            //         if (data_->getVertexLabel(vf_nei) != NOT_EXIST)
            //             if(data_->getVertexLabel(vf_nei) == query_->getVertexLabel(ub)){
            //                 CandidateSets[depth][i].push_back(vf_nei);
            //             }
            //     }
            // }
            // //local candidate space is not null, intersect with the neighbors of vf
            // else{
            //     // std::set_intersection(CandidateSets[depth-1][i].begin(),CandidateSets[depth-1][i].end(),vf_nbrs.begin(),vf_nbrs.end(),std::back_inserter(CandidateSets[depth][i]));
            //     std::set_intersection(CandidateSets[depth-1][i].begin(), CandidateSets[depth-1][i].end(),vf_nbrs, vf_nbrs + cnt, std::back_inserter(CandidateSets[depth][i]));
            // }
            // an empty previous layer means it has not been initialized yet, so initialize from the input candidates and sort now
            if (CandidateSets[depth-1][i].empty()) {
                CandidateSets[depth-1][i].assign(candidates_[ub], candidates_[ub] + candidates_count_[ub]);
                std::sort(CandidateSets[depth-1][i].begin(), CandidateSets[depth-1][i].end());
                CandidateSets[depth-1][i].erase(
                    std::unique(CandidateSets[depth-1][i].begin(), CandidateSets[depth-1][i].end()),
                    CandidateSets[depth-1][i].end()
                );
            }

            // Intersection: vf_nbrs must also be sorted (CSR adjacency is usually sorted)
            std::set_intersection(
                CandidateSets[depth-1][i].begin(), CandidateSets[depth-1][i].end(),
                vf_nbrs, vf_nbrs + cnt,
                std::back_inserter(CandidateSets[depth][i])
            );
            
            if(CandidateSets[depth][i].size() == 0){
                return false;
            }
        }
        else{ // uf and ub are not adjacent, the local candidate space need not modify
            // CandidateSets[depth][i] = CandidateSets[depth-1][i];
            CandidateSets[depth][i] = CandidateSets[depth-1][i]; // Still preserve the semantics that an uninitialized set remains empty
        }
    }
    return true;

}


bool MatCo::FullCoveragePrune(uint depth,const std::vector<uint> &m){

    //check if the current matched vertices are all key vertices
    for(uint i = 0 ; i< depth ; i++){
        uint u= match_order[i];
        if(!KeyVertexSet[m[u]]) {
            // flush the flag of local candidate space
            for(uint i = depth; i<query_->getVerticesCount(); i++){
                CandidateSetFlag[depth][i] = CandidateSetFlag[depth-1][i];
            }
            return false;
        }
    }
    //check if the local candidate space are all key vertices
    for(uint i = depth; i<query_->getVerticesCount(); i++){
        if(CandidateSets[depth][i].size()==0) {
            for(uint j = i; j<query_->getVerticesCount(); j++){
                        CandidateSetFlag[depth][j] = CandidateSetFlag[depth-1][j];
                    }
                    return false;
        }
        if(CandidateSetFlag[depth-1][i]){   
            CandidateSetFlag[depth][i] = true;
        }
        else{
            CandidateSetFlag[depth][i] = true;
            for(auto v: CandidateSets[depth][i]){
                if(!KeyVertexSet[v]) {
                    for(uint j = i; j<query_->getVerticesCount(); j++){
                        CandidateSetFlag[depth][j] = CandidateSetFlag[depth-1][j];
                    }
                    return false;
                }
            }
        }
    }
    //scuccessful prune and flush the flag of local candidate space
    FlushFlag(depth-1);  
    return true;
}




