#ifndef CZI_SHASTA_SIMPLE_BAYESIAN_CONSENSUS_CALLER_HPP
#define CZI_SHASTA_SIMPLE_BAYESIAN_CONSENSUS_CALLER_HPP

/*******************************************************************************

A SimpleBayesianConsensusCaller uses a simple Bayesian approach
to compute the "best" base and repeat count at a position of an alignment.

Based on initial work by Ryan Lorig-Roach at UCSC, the method works as follows.
Here, n is the true repeat count, m is the observed repeat count,
and mi the observed repeat counts in a set of reads.

- Once for a given sequencing technology, estimate conditional probabilities
P(m | n, base read) by mapping reads to portions of a reference
known not to contain variants.

- Use Bayes theorem to estimate

P(n | mi, base) proportional to P(n) times product over i P(m | n, base read)

Where P(n) is the prior probability of a homopolymer run of length n
and can be initially neglected.

Note that in the above, the base read at a given alignment position
must take into account which strand each read is on.

*******************************************************************************/

#include "ConsensusCaller.hpp"
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <limits>
#include <map>

using ChanZuckerberg::shasta::Consensus;
using std::ifstream;
using std::vector;
using std::array;
using std::string;
using std::pair;
using std::map;

namespace ChanZuckerberg {
    namespace shasta {
        class SimpleBayesianConsensusCaller;
    }
}


const double INF = std::numeric_limits<double>::infinity();;


// Given a set of observations (repeat, strand, base), predict the true repeat count
class ChanZuckerberg::shasta::SimpleBayesianConsensusCaller:
    public ChanZuckerberg::shasta::ConsensusCaller {
public:
    /// ----- Methods ----- ///

    // The constructor does not have any parameters.
    // All data is read from file SimpleBayesianConsensusCaller.csv
    // in the run directory. We will update the documentation accordingly.
    SimpleBayesianConsensusCaller();

    // Given a coverage object, return the most likely run length, and the normalized log likelihood vector for all run
    // lengths as a pair
    uint16_t predict_runlength(const Coverage &coverage, AlignedBase consensusBase, vector<double>& log_likelihood_y) const;

    AlignedBase predict_consensus_base(const Coverage& coverage) const;

    // This is the primary function of this class. Given a coverage object and consensus base, predict the true
    // run length of the aligned bases at a position
    virtual Consensus operator()(const Coverage&) const;

private:

    /// ---- Attributes ---- ///

    // The name specified under the field ">Name" in the configuration file
    string configuration_name;

    // Defined at initialization, this is the size of the probability matrix generated from the data
    uint16_t max_runlength;

    // Boolean flags for configuration
    bool ignore_non_consensus_base_repeats;
    bool predict_gap_runlengths;
    bool count_gaps_as_zeros;

    // p(X|Y) normalized for each Y, where X = observed and Y = True run length
    array<vector<vector<double> >, 4> probability_matrices;

    // priors p(Y) normalized for each Y, where X = observed and Y = True run length
    array<vector<double>, 2> priors;

    /// ----- Methods ----- ///

    // For parsing any character separated file format
    void split_as_double(string s, char separator_char, vector<double>& tokens);
    void split_as_string(string s, char separator_char, vector<string>& tokens);

    // Read each probability matrix from its file and store them in a vector (assuming decibel units, aka base 10)
    // Each delimited table in text should be preceded by a fasta-like header e.g.: ">A" for the base it corresponds to.
    // This converts each line to a vector of doubles, appending probability_matrices according to the matrix header.
    void load_configuration(ifstream& matrix_file);

    // Parsing functions broken out for readability
    void parse_name(ifstream& matrix_file, string& line);
    void parse_prior(ifstream& matrix_file, string& line, vector<string>& tokens);
    void parse_likelihood(ifstream& matrix_file, string& line, vector<string>& tokens);

    // For a given vector of likelihoods over each Y value, normalize by the maximum
    void normalize_likelihoods(vector<double>& x, double x_max) const;

    // Count the number of times each unique repeat was observed, to reduce redundancy in calculating log likelihoods
    void factor_repeats(array<map<uint16_t,uint16_t>,2>& factored_repeats, const Coverage& coverage) const;
    void factor_repeats(array<map<uint16_t,uint16_t>,2>& factored_repeats, const Coverage& coverage, AlignedBase consensus_base) const;

    // For debugging or exporting
    void print_priors(char separator);
    void print_probability_matrices(char separator=',');
    void print_log_likelihood_vector(vector<double>& log_likelihoods);
};

void testSimpleBayesianConsensusCaller();

#endif
