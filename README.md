# Ravenna
Ravenna is a project for running label-oriented subgraph matching experiments on large graph datasets. It supports time-limited enumeration, configurable label utilization thresholds, and comparisons with several coverage-oriented baselines. This repository provides the implementation, build instructions, command-line options, and example scripts for reproducing Ravenna experiments.

## Code

We build our algorithm based on the PVLDB paper [Mix & Match: Subgraph Matching for Absolute Coverage](https://doi.org/10.14778/3773731.3773737) and its [code](https://github.com/constantinosskitsas/Mix-Match), as well as the SIGMOD 2024 paper [A Comprehensive Survey and Experimental Study of Subgraph Matching: Trends, Unbiasedness and Interaction](https://dl.acm.org/doi/pdf/10.1145/3639315) and its [code](https://github.com/RapidsAtHKUST/SubgraphMatching).

## Commands

|   Option  | Description                                                                                        | Example                                                                                                                                                           |
| :-------: | :------------------------------------------------------------------------------------------------- | :---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|    `-d`   | Path to the input data graph.                                                                      | `/datasets/original/data_graph/patents.graph`                                                                                                                     |
|    `-q`   | Path to the input query graph.                                                                     | `/datasets/original/query_graph/patents/query_G_12_1.graph`                                                                                                       |
| `-engine` | Supported algorithm.                                                                               | `Ravenna`                                                                                                                                                         |
|  `-time`  | Running time limit for enumeration, in seconds.                                                    | `2`                                                                                                                                                               |
|   `-tau`  | Utilization thresholds. This can be either a single value or a comma-separated list. | `0.2` means 20% for each label. `0.1,0.2,0.3` means 10% for label 0, 20% for label 1, and 30% for label 2. The list size should be equal to the number of labels. |

## Supported Algorithms

| Algorithm | Description                          | Execution Code |
| :-------: | :----------------------------------- | :------------- |
|  Ravenna  | Ravenna                              | `Ravenna`      |
|    M&M    | Mix & Match                          | `MMDIV`        |
|   M&M-I   | Mix & Match with Initialization      | `MMIDIV`       |
|    DSQL   | Diversified Subgraph Query           | `DSQLDIV`      |
|   DSQL+   | Optimized Diversified Subgraph Query | `DSQLPDIV`     |
|   MATCO   | MatCo                                | `MATCODIV`     |

For options for filtering method and ordering method, please refer to the original [Mix & Match code](https://github.com/constantinosskitsas/Mix-Match).

## Build

Create a build directory and compile the source code:

```zsh
mkdir build && cd build
cmake ..
make
```

## Execute

Run the executable with the following command:

```zsh
./SubgraphMatching.out -d data_graph -q query_graph -filter filtering_method -order ordering_method -engine supported_algorithm -time time_limit -tau thresholds
```

For detailed parameter settings, see `matching/matchingcommand.h`.

**Example 1:** Ravenna experiment setup using a 30% threshold for each label on the Dblp dataset with a 1 second time limit.

```zsh
./SubgraphMatching.out -d ../datasets/original/data_graph/dblp.graph -q ../datasets/original/query_graph/dblp/query_G_24_1.graph -filter VEQ -order CFL -engine Ravenna -time 1 -tau 0.3
```

**Example 2:** Ravenna experiment setup using different thresholds on the Dblp dataset with a 1 second time limit.

```zsh
./SubgraphMatching.out -d ../datasets/original/data_graph/dblp.graph -q ../datasets/original/query_graph/dblp/query_G_24_1.graph -filter VEQ -order CFL -engine Ravenna -time 1 -tau 0.1,0.2,0.3,0.1,0.2,0.3,0.1,0.2,0.3,0.1,0.2,0.3,0.1,0.2,0.3
```

## Datasets

The datasets used in the experiments of Ravenna are available on Zenodo: [https://doi.org/10.5281/zenodo.20819771](https://doi.org/10.5281/zenodo.20819771)

The archive `datasets.zip` contains public data graphs collected from existing open graph datasets and query graphs generated for subgraph matching experiments. The generated query graphs and benchmark organization are provided to support reproducible evaluation of Ravenna.

