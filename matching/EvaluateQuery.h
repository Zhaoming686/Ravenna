

#ifndef SUBGRAPHMATCHING_EVALUATEQUERY_H
#define SUBGRAPHMATCHING_EVALUATEQUERY_H

#include "graph/graph.h"
#include "utility/QFilter.h"
#include "utility/primitive/projection.h"
#include "utility/relation/catalog.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include <bitset>
#include <string>
enum topkVEnum
{
    VALUE_5 = 5,
    VALUE_10 = 10,
    VALUE_50 = 50,
    VALUE_100 = 100,
    VALUE_250 = 250,
    VALUE_500 = 500,
    VALUE_750 = 750,
    VALUE_1000 = 1000,
    VALUE_2500 = 2500,
    VALUE_5000 = 5000,
    VALUE_10000 = 10000
};

struct enumResult
{
    size_t embedding_cnt;
    ui candidate_true_count_sum;
    ui Can_embed;
    int topk[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    // ui topk;
    vector<set<ui>> candidate_true;
};

class EvaluateQuery
{
public:
    static enumResult
    DIVSMNN(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
            ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static void generateBNLM(const Graph *query_graph, ui *order, ui **&bn, ui *&bn_count);
    static enumResult
    LFTJDLSBASIC(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                 ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, size_t &valid_vtx_cnt, int TimeL, int FairT,
                 const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static bool nextCombination(std::vector<int> &indices, ui N, ui K);
    static void rankRandom(const Graph *query_graph, Edges ***edge_matrix, ui u, ui *&nodeId, ui **candidates, ui **valid_candidate_idx, ui *idx, ui valid_idx, ui *idx_count);
    static void pruneCandidatesIndexBySymmetryBreakingC(ui depth, ui *embedding, ui *order,
                                                        ui *candidates_count, ui **candidates,
                                                        ui *idx_count, ui **valid_candidates_idx,
                                                        const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult
    LFTJKOPTSQ(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
               ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, size_t &valid_vtx_cnt, int TimeL, int FairT,
               const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    static void allocateBufferLM(const Graph *data_graph, const Graph *query_graph, ui *candidates_count, ui *&idx,
                                 ui *&idx_count, ui *&embedding, ui *&idx_embedding, ui *&temp_buffer,
                                 ui **&valid_candidate_idx, bool *&visited_vertices);
    static enumResult
    DIVTOPK(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
            ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static void calculateCellAC(const Graph *query_graph, Edges ***edge_matrix,
                                ui *candidates_count, size_t **&candidatesHC, unordered_map<size_t, std::vector<VertexID>> *&idToValues, int count2, int i, int j);
    static enumResult exploreVEQStyle1(const Graph *data_graph, const Graph *query_graph, TreeNode *tree,
                                       Edges ***edge_matrix, ui **candidates, ui *candidates_count,
                                       size_t output_limit_num, size_t &call_count, int TimeL,
                                       const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &full_constraints);
    static void calculateCell(const Graph *query_graph, Edges ***edge_matrix,
                              ui *candidates_count, size_t **&candidatesHC, unordered_map<size_t, std::vector<VertexID>> *&idToValues, int count2, int i, int j);
    static void calculateCellFN(const Graph *query_graph, Edges ***edge_matrix,
                                ui *candidates_count, size_t **&candidatesHC, unordered_map<size_t, std::vector<VertexID>> *&idToValues, int count2, int i, int j, int ROQ[]);
    static void computeNEC_Adaptive(const Graph *query_graph, Edges ***edge_matrix, ui *candidates_count,
                                    ui **candidates, std::vector<std::vector<ui>> &vec_index,
                                    std::vector<std::vector<ui>> &vec_set, ui i, ui j, ui vec_count);

    static std::function<bool(std::pair<std::pair<VertexID, ui>, ui>, std::pair<std::pair<VertexID, ui>, ui>)>
        extendable_vertex_compare;

    typedef std::priority_queue<std::pair<std::pair<VertexID, ui>, ui>, std::vector<std::pair<std::pair<VertexID, ui>, ui>>,
                                decltype(EvaluateQuery::extendable_vertex_compare)>
        dpiso_min_pq;

    static size_t exploreGraph(const Graph *data_graph, const Graph *query_graph, Edges ***edge_matrix, ui **candidates,
                               ui *candidates_count, ui *order, ui *pivot, size_t output_limit_num, size_t &call_count,
                               const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &order_constraints = {});
    static enumResult LFTJDIV1(const Graph *data_graph, const Graph *query_graph, Edges ***edge_matrix, ui **candidates,
                               ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult LFTJ(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates, ui *candidates_count,
                           ui *order, size_t output_limit_num, size_t &call_count, size_t &valid_vtx_cnt, int TimeL, int FairT,
                           const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &order_constraints = {});
    static enumResult LFTJDIV(const Graph *data_graph, const Graph *query_graph, Edges ***edge_matrix, ui **candidates,
                              ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    static enumResult exploreVEQStyle(const Graph *data_graph, const Graph *query_graph, TreeNode *tree,
                                      Edges ***edge_matrix, ui **candidates, ui *candidates_count,
                                      size_t output_limit_num, size_t &call_count, int TimeL, ui *order,
                                      const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &full_constraints = {});
    static size_t exploreDPisoRecursiveStyle(const Graph *data_graph, const Graph *query_graph, TreeNode *tree,
                                             Edges ***edge_matrix, ui **candidates, ui *candidates_count,
                                             ui **weight_array, ui *order, size_t output_limit_num,
                                             size_t &call_count,
                                             const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &order_constraints = {});

    static void generateValidCandidateIndex(ui depth, ui *idx_embedding, ui *idx_count, ui **valid_candidate_index,
                                            Edges ***edge_matrix, ui **bn, ui *bn_cnt, ui *order,
                                            ui *&temp_buffer, ui **candidates, ui *candidates_count, const Graph *query_graph);
#if ENABLE_QFLITER == 1
    static BSRGraph ***qfliter_bsr_graph_;
    static int *temp_bsr_base1_;
    static int *temp_bsr_state1_;
    static int *temp_bsr_base2_;
    static int *temp_bsr_state2_;
#endif

    static void rankSimple(ui u, ui *&nodeId, ui **candidates, ui **valid_candidate_idx, ui *idx, ui valid_idx, ui *idx_count);
    static void rankLess(const Graph *query_graph, Edges ***edge_matrix, ui u, ui *&nodeId, ui **candidates, ui **valid_candidate_idx, ui *idx, ui valid_idx, ui *idx_count);
    static void rankMore(const Graph *query_graph, Edges ***edge_matrix, ui u, ui *&nodeId, ui **candidates, ui **valid_candidate_idx, ui *idx, ui valid_idx, ui *idx_count);
    static void rankFinal(const Graph *query_graph, Edges ***edge_matrix, ui u, ui *&nodeId, ui **candidates, ui **valid_candidate_idx, ui *idx, ui valid_idx, ui *idx_count);
    static void rankFinal1(const Graph *query_graph, Edges ***edge_matrix, ui u, ui *&nodeId, ui **candidates, ui **valid_candidate_idx, ui *idx, ui valid_idx, ui *idx_count);

#ifdef SPECTRUM
    static bool exit_;
#endif

#ifdef DISTRIBUTION
    static size_t *distribution_count_;
#endif

    // version with calculating Sat
    static enumResult MMDIV(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                            ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult LFTJDLSDIV(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                                 ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, size_t &valid_vtx_cnt, int TimeL, int FairT,
                                 const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult DIVSMSQDIV(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                                 ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult MatCoDIV(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                               ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult RavennaCheck(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                                   ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult RavennaForward(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                                     ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult Ravenna(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                              ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, int TimeL, int FairT, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static enumResult DSQLDIV(const Graph *data_graph, const Graph *query_graph, ui *&nodeId, Edges ***edge_matrix, ui **candidates,
                              ui *candidates_count, ui *order, size_t output_limit_num, size_t &call_count, size_t &valid_vtx_cnt, int TimeL, int FairT,
                              const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    // Sat computing
    static void computeDiversity(const Graph *query_graph, ui qsiz, ui **candidates, ui *candidates_count, ui *nodeId, std::unordered_map<LabelID, std::vector<ui>> labelToCandidates, const ui *label_target);
    static void groupCandidatesByLabel(const Graph *query_graph, ui qsiz, ui **candidates, ui *candidates_count, std::unordered_map<LabelID, std::vector<ui>> &labelToCandidates);
    static void groupCoveredVerticesByLabel(const Graph *query_graph, ui qsiz, ui **candidates, ui *candidates_count, ui *nodeId, std::unordered_map<LabelID, std::vector<ui>> &labelToAllEmbeddings);
    static void deduplicateLabelMap(std::unordered_map<LabelID, std::vector<ui>> &labelMap);
    static void recordAndPrintLabelCoverage(const std::unordered_map<LabelID, std::vector<ui>> &labelToCandidates, const std::unordered_map<LabelID, std::vector<ui>> &labelToAllEmbeddings, std::unordered_map<LabelID, std::pair<ui, double>> &labelStats);
    static void recordAndPrintAverageRelativeCoverage(const std::unordered_map<LabelID, std::vector<ui>> &labelToCandidates, const std::unordered_map<LabelID, std::vector<ui>> &labelToAllEmbeddings, const ui *label_target);

    // diversity check
    static void InitQueryLabelFlag(const Graph *query_graph, uint8_t *&label_flag);
    static void InitLabelValByCandidates(const Graph *query_graph, const std::unordered_map<LabelID, std::vector<ui>> &labelToCandidates, int *&label_val, ui *&label_target);
    static void UpdateLabelVal(const Graph *data_graph, ui Id, ui *nodeId, uint8_t *label_flag, int *label_val, ui *label_cover_cnt);
    static void UpdateLabelVal(const Graph *data_graph, ui Id, std::vector<bool> &KeyVertexSet, uint8_t *label_flag, int *label_val, ui *label_cover_cnt);
    static void generateValidCandidateIndexPrune(bool *assigned, ui *candidates_count, ui depth, ui *idx_embedding, ui &idx_count, ui *&valid_candidate_index, Edges ***edge_matrix, ui **bn, ui *bn_cnt, ui *order, ui *&temp_buffer);
    static void pruneCandidatesIndexBySymmetryBreakingCPrune(bool *assigned, ui depth, ui *embedding, ui *order, ui *candidates_count, ui **candidates, ui &idx_count, ui *&valid_candidates_idx, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);
    static void UpdateContinueFlags(const Graph *query_graph, bool &continueFlagBefore, bool &continueFlagAfter, ui cur_depth, ui max_depth, ui *order, const std::vector<ui> *VN, ui **valid_candidate_idx,
                                    ui *idx_count, ui **candidates, ui *candidates_count, ui *nodeId, uint8_t *label_flag, const Graph *data_graph, bool *assigned, ui *idx_embedding, ui *embedding,
                                    Edges ***edge_matrix, ui **bn, ui *bn_count, ui *&temp_buffer, ui *&valid_candidate_idx_temp, const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    // checkpoints
    struct CheckpointRecorder
    {
        ui cap = 0; // maximum number of checkpoint records
        ui sz = 0;  // how many records it currently stores

        int *time_point = nullptr;
        double *sat_point = nullptr;         // hard / step-function satisfaction
        double *cov_sat_point = nullptr;     // min-based coverage satisfaction
        double *avg_rel_cov_point = nullptr; // average relative coverage rate
        size_t *call_count_point = nullptr;
        ui *prune_count_point = nullptr;
        size_t *embedding_cnt_point = nullptr;
    };
    static void InitCheckpointRecorder(CheckpointRecorder &rec, int time_limit_ms, int interval_ms);
    static void PushCheckpoint(CheckpointRecorder &rec, int t_ms, double sat, double cov_sat, double avg_rel_cov, size_t call_count, ui prune_count, size_t embedding_cnt);
    static void DestroyCheckpointRecorder(CheckpointRecorder &rec);
    static void BuildLabelsQuery(const Graph *query_graph, ui qsiz, LabelID *&labelsQuery, ui &labelsQuerySize);
    static void DestroyLabelsQuery(LabelID *&labelsQuery);
    static void ComputeSatCovAndRelCov(double &last_sat, double &last_cov_sat, double &last_avg_rel_cov, const LabelID *labelsQuery, ui labelsQuerySize, const uint8_t *label_flag, const ui *label_cover_cnt, const ui *label_target);

    // forward equivalence set
    static void calculateCellAndSet(const Graph *query_graph, Edges ***edge_matrix, ui *candidates_count, size_t **&candidatesHC, std::unordered_map<size_t, std::vector<ui>> *&idToValues, int count2, int i, int j);
    static void calculateCellFNAndSet(const Graph *query_graph, Edges ***edge_matrix, ui *candidates_count, size_t **&candidatesHC, std::unordered_map<size_t, std::vector<ui>> *&idToValues, int count2, int i, int j, int ROQ[]);

    // prune and inclusion set
    static void pruneBySet(ui depth, VertexID u, ui **candidates, ui **valid_candidate_idx, ui *idx, ui *idx_count, ui *nodeId, const std::vector<ui> &prune_list, bool covered_only);
    static void markSubBySet(ui depth, VertexID u, ui **candidates, ui **valid_candidate_idx, uint8_t **valid_candidate_tag, ui *idx, ui *idx_count, ui *nodeId, const std::vector<ui> &prune_list);
    static void markSupBySet(ui depth, ui **valid_candidate_idx, uint8_t **valid_candidate_tag, ui *idx, ui *idx_count, const std::vector<ui> &prune_list);
    static void calculateCellFNAndSubset(const Graph *query_graph, Edges ***edge_matrix, ui *candidates_count, size_t **&candidatesHC, std::unordered_map<size_t, std::vector<ui>> *&idToValues, std::unordered_map<size_t, std::vector<ui>> *&idToSubValues, int count2, int i, int j, int ROQ[], std::unordered_map<size_t, std::vector<ui>> *&idToSupValues);
    static void calculateCellAndSubset(const Graph *query_graph, Edges ***edge_matrix, ui *candidates_count, size_t **&candidatesHC, std::unordered_map<size_t, std::vector<ui>> *&idToValues, std::unordered_map<size_t, std::vector<ui>> *&idToSubValues, int count2, int i, int j, std::unordered_map<size_t, std::vector<ui>> *&idToSupValues);

    // embedings
    struct Bucket
    {
        VertexID u;
        std::vector<ui> vertices;
    };
    struct EmbeddingRecord
    {
        std::vector<ui> embedding_by_q; // query vertex u -> data vertex embedding_by_q[u]
        std::vector<Bucket> vn_buckets;
        std::vector<Bucket> local_buckets;
        bool output_original = false;
    };
    static size_t outputEmbeddings(const std::vector<EmbeddingRecord> &embedding_records, ui qsiz, size_t output_limit_num = std::numeric_limits<size_t>::max(), bool print_embedding = true);
    static void GreedyConstructEmbedding(const Graph *data_graph, const Graph *query_graph, ui **candidates, ui *order, std::vector<ui> *VN, ui *embedding, ui *nodeId, uint8_t *label_flag, int *label_val, ui *label_cover_cnt, size_t &embedding_cnt, int cur_depth, bool has_equiv = false, VertexID conflict_v = 0, const std::vector<ui> *VNTempPtr = nullptr, bool print_embedding = true);
    static size_t buildBucketsAndMarksub(const Graph *data_graph, const Graph *query_graph, Edges ***edge_matrix, ui **candidates, ui *order, int *RQ, ui *base_embedding, ui *base_idx_embedding, std::vector<ui> *VN, ui **valid_candidate_idx, ui *idx_count, ui *idx, ui *nodeId, uint8_t *label_flag, int *label_val, ui *label_cover_cnt, int max_dep, std::vector<EmbeddingRecord> &embedding_records, std::vector<ui> *VSub = nullptr);

    // tau values
    static void SetTauValues(const std::vector<double> &tau_values, const Graph *data_graph);

private:
    static void generateBN(const Graph *query_graph, ui *order, ui *pivot, ui **&bn, ui *&bn_count);
    static void generateBN(const Graph *query_graph, ui *order, ui **&bn, ui *&bn_count);
    static void allocateBuffer(const Graph *query_graph, const Graph *data_graph, ui *candidates_count, ui *&idx,
                               ui *&idx_count, ui *&embedding, ui *&idx_embedding, ui *&temp_buffer,
                               ui **&valid_candidate_idx, bool *&visited_vertices);
    static void releaseBuffer(ui query_vertices_num, ui *idx, ui *idx_count, ui *embedding, ui *idx_embedding,
                              ui *temp_buffer, ui **valid_candidate_idx, bool *visited_vertices, ui **bn, ui *bn_count);

    static void generateValidCandidateIndex(const Graph *data_graph, ui depth, ui *embedding, ui *idx_embedding,
                                            ui *idx_count, ui **valid_candidate_index, Edges ***edge_matrix,
                                            bool *visited_vertices, ui **bn, ui *bn_cnt, ui *order, ui *pivot,
                                            ui **candidates);

    static void generateValidCandidateIndex(ui depth, ui *idx_embedding, ui *idx_count, ui **valid_candidate_index,
                                            Edges ***edge_matrix, ui **bn, ui *bn_cnt, ui *order, ui *&temp_buffer);

    static void generateValidCandidates(const Graph *data_graph, ui depth, ui *embedding, ui *idx_count, ui **valid_candidate,
                                        bool *visited_vertices, ui **bn, ui *bn_cnt, ui *order, ui **candidates, ui *candidates_count);

    static void generateValidCandidates(const Graph *query_graph, const Graph *data_graph, ui depth, ui *embedding,
                                        ui *idx_count, ui **valid_candidates_idx, bool *visited_vertices, ui **bn, ui *bn_cnt,
                                        ui *order, ui *pivot);

    static void generateValidCandidates(ui depth, ui *embedding, ui *idx_count, ui **valid_candidates, ui *order,
                                        ui *&temp_buffer, TreeNode *tree,
                                        std::vector<std::unordered_map<VertexID, std::vector<VertexID>>> &TE_Candidates,
                                        std::vector<std::vector<std::unordered_map<VertexID, std::vector<VertexID>>>> &NTE_Candidates);

    static void pruneCandidatesIndexBySymmetryBreaking(ui depth, ui *embedding, ui *order,
                                                       ui *candidates_count, ui **candidates,
                                                       ui *idx_count, ui **valid_candidates_idx,
                                                       const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    static void pruneCandidatesBySymmetryBreaking(ui depth, ui *embedding, ui *order,
                                                  ui *idx_count, ui **valid_candidates,
                                                  const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    static bool checkSingeVertexBySymmetryBreaking(ui depth, ui *embedding, ui *order, VertexID v,
                                                   const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    static bool checkSingeVertexBySymmetryBreaking(ui depth, ui *embedding, ui *order, bool *visited_u, VertexID v,
                                                   const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &full_constraints,
                                                   bool *voilated_symmetry_u, ui max_depth);

    static void updateExtendableVertex(ui *idx_embedding, ui *idx_count, ui **valid_candidate_index,
                                       Edges ***edge_matrix, ui *&temp_buffer, ui **weight_array,
                                       TreeNode *tree, VertexID mapped_vertex, ui *extendable,
                                       std::vector<dpiso_min_pq> &vec_rank_queue, const Graph *query_graph);

    static void restoreExtendableVertex(TreeNode *tree, VertexID unmapped_vertex, ui *extendable);

    static void generateValidCandidateIndex(VertexID vertex, ui *idx_embedding, ui *idx_count, ui *&valid_candidate_index,
                                            Edges ***edge_matrix, ui *bn, ui bn_cnt, ui *&temp_buffer);

    static void computeAncestor(const Graph *query_graph, TreeNode *tree, VertexID *order,
                                std::vector<std::bitset<MAXIMUM_QUERY_GRAPH_SIZE>> &ancestors);

    static void computeAncestor(const Graph *query_graph, ui **bn, ui *bn_cnt, VertexID *order,
                                std::vector<std::bitset<MAXIMUM_QUERY_GRAPH_SIZE>> &ancestors);

    static void computeAncestor(const Graph *query_graph, VertexID *order, std::vector<std::bitset<MAXIMUM_QUERY_GRAPH_SIZE>> &ancestors);

    static void AncestorWithSymmetryBreaking(const Graph *query_graph,
                                             std::vector<std::bitset<MAXIMUM_QUERY_GRAPH_SIZE>> &ancestors,
                                             const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &ordered_constraints);

    static void generateFeasibility(const Graph *graph, ui v, bool *matched, ui level, ui ***feasibility, ui **feasibility_count);
    static void generateFeasibility(const Graph *query_graph, const ui *order, ui ***feasibility, ui **feasibility_count);
    static bool isFeasibility(const Graph *data_graph, const Graph *query_graph, ui depth, ui cur_u, ui cur_v,
                              ui *embedding, ui *order, ui ***query_feasibility, ui **query_feasibility_count,
                              ui ***data_feasibility, ui **data_feasibility_count);

    static bool exploreVF3Backtrack(const Graph *data_graph, const Graph *query_graph, ui *order, ui *pivot,
                                    size_t output_limit_num, size_t &call_count, ui &embedding_cnt,
                                    ui depth, ui max_depth, ui *idx, ui *idx_count, ui *embedding,
                                    ui **valid_candidates, bool *visited_vertices,
                                    ui ***query_feasibility, ui **query_feasibility_count,
                                    ui ***data_feasibility, ui **data_feasibility_count,
                                    const std::unordered_map<VertexID, std::pair<std::set<VertexID>, std::set<VertexID>>> &order_constraints);

    static void RestoreValidCans(const Graph *query_graph, const Graph *data_graph, bool *visited_u,
                                 VertexID last_v, VertexID last_u,
                                 std::vector<std::unordered_map<VertexID, ui>> &valid_cans);
    static void computeNEC(const Graph *query_graph, ui **nec);
    static void computeNEC(const Graph *query_graph, Edges ***edge_matrix, ui *candidates_count, ui **candidates,
                           std::vector<std::vector<ui>> &vec_index,
                           std::vector<std::vector<ui>> &vec_set);
    static ui generateNextU(const Graph *data_graph, const Graph *query_graph, ui **candidates, ui *candidates_count, ui **valid_cans,
                            ui *valid_cans_count, ui *extendable, ui **nec, ui depth, ui *embedding,
                            Edges ***edge_matrix, bool *visited_vertices, bool *visited_u, ui *order,
                            TreeNode *tree);
    static void ComputeValidCans(const Graph *data_graph, const Graph *query_graph, ui **candidates, ui *candidates_count,
                                 ui **valid_cans, ui *valid_cans_count, ui *embedding, VertexID u, bool *visited_u);

    static void ComputeValidCans(const Graph *data_graph, const Graph *query_graph, ui **candidates, ui *candidates_count,
                                 ui **valid_cans, ui *valid_cans_count, ui *embedding, VertexID u, bool *visited_u, bool *visited_v);

    static size_t computeKSSEmbeddingNaive(ui shell_num, ui *shell, ui **valid_cans, ui *valid_cans_count, bool *visited_v);
    static size_t computeKSSEmbeddingOpt1(ui shell_num, ui *shell, ui **valid_cans, ui *valid_cans_count, bool *visited_v);
    static size_t computeKSSEmbeddingOpt2(ui shell_num, ui *shell, ui **valid_cans, ui *valid_cans_count);

    static size_t computeKSSEmbeddingNaiveImpl(ui depth, ui shell_num, ui *shell, ui **valid_cans, ui *valid_cans_count, bool *visited_v);
    static size_t computeKSSEmbeddingOpt1Impl(ui depth, ui shell_num, ui *shell, ui **valid_cans, ui *valid_cans_count,
                                              std::unordered_map<VertexID, ui> &counter,
                                              bool *visited_v);
    static size_t computeKSSEmbeddingOpt2Impl(ui depth, ui shell_num, ui *shell, ui **valid_cans, ui *valid_cans_count,
                                              std::unordered_map<VertexID, ui> &counter,
                                              std::set<VertexID> *used,
                                              std::map<std::set<VertexID>, size_t> *memory_table);

    static std::bitset<MAXIMUM_QUERY_GRAPH_SIZE> exploreDPisoBacktrack(ui max_depth, ui depth, VertexID mapped_vertex, TreeNode *tree, ui *idx_embedding,
                                                                       ui *embedding, std::unordered_map<VertexID, VertexID> &reverse_embedding,
                                                                       bool *visited_vertices, ui *idx_count, ui **valid_candidate_index,
                                                                       Edges ***edge_matrix,
                                                                       std::vector<std::bitset<MAXIMUM_QUERY_GRAPH_SIZE>> &ancestors,
                                                                       dpiso_min_pq rank_queue, ui **weight_array, ui *&temp_buffer, ui *extendable,
                                                                       ui **candidates, size_t &embedding_count, size_t &call_count,
                                                                       const Graph *query_graph);

    static void convertCans2Catalog(const Graph *query_graph, ui **candidates, Edges ***edge_matrix, catalog *storage);

    static void convert_to_encoded_relation(catalog *storage, ui *order);

    static void convert_to_encoded_relation(catalog *storage, uint32_t u, uint32_t v);

    static void convert_to_hash_relation(catalog *storage, uint32_t u, uint32_t v);

    static void convert_encoded_relation_to_sparse_bitmap(catalog *storage, ui *order);

    static void updateShell2Kernel(const Graph *query_graph, VertexID u, ui *shell2kernel, bool *kos, std::vector<VertexID> &update);

    static void restoreShell2Kernel(const Graph *query_graph, VertexID u, ui *shell2kernel, bool *kos);

    static void generateValidCandidateIndexSS(ui depth, ui *idx_embedding, ui *idx_count, ui **valid_candidate_index,
                                              Edges ***edge_matrix, ui **bn, ui *bn_cnt, ui *order,
                                              ui *&temp_buffer, ui CN, const Graph *data_graph, ui dataV, ui *embedding);
};

#endif
