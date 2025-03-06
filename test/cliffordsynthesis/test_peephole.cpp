#include "Definitions.hpp"
#include "cliffordsynthesis/CliffordSynthesizer.hpp"
#include "cliffordsynthesis/Results.hpp"
#include "cliffordsynthesis/TargetMetric.hpp"
#include "ir/QuantumComputation.hpp"
#include "ir/operations/Control.hpp"
#include "qasm3/Importer.hpp"

#include <cstddef>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <plog/Severity.h>
#include <sstream>
#include <string>
#include <vector>

using namespace qc::literals;

namespace cs {

struct PeepTestConfiguration {
  std::string description;
  std::string initialCircuit;

  std::size_t expectedMinimalGates{};
  std::size_t expectedMinimalDepth{};
  std::string expectedOutputCircuit;
};

inline void from_json(const nlohmann::json& j, PeepTestConfiguration& test) {
  test.description = j.at("description").get<std::string>();
  test.initialCircuit = j.at("initial_circuit").get<std::string>();
  test.expectedMinimalGates = j.at("expected_minimal_gates").get<std::size_t>();
  test.expectedMinimalDepth = j.at("expected_minimal_depth").get<std::size_t>();
  test.expectedOutputCircuit = j.at("expected_output_circuit").get<std::string>();
}

namespace {
std::vector<PeepTestConfiguration> getTests(const std::string& path) {
  std::ifstream input(path);
  nlohmann::json j;
  input >> j;
  return j;
}
} // namespace


class PeepholeTest : public ::testing::TestWithParam<PeepTestConfiguration> {
  protected:
    void SetUp() override {
      test = GetParam();

      auto qc = qasm3::Importer::imports(test.initialCircuit);
      config = Configuration();
      config.heuristic = true;
      config.splitSize = 1;
      config.target = TargetMetric::Depth;
      synthesizer = CliffordSynthesizer(qc);
      synthesizer.synthesize(config);
      results = synthesizer.getResults();
    }

    Configuration config;
    CliffordSynthesizer synthesizer;
    Results results;
    PeepTestConfiguration test;
  
};

INSTANTIATE_TEST_SUITE_P(
    Circuits, PeepholeTest, testing::ValuesIn(getTests("circuits_peephole.json")),
    [](const testing::TestParamInfo<PeepholeTest::ParamType>& inf) {
      return inf.param.description;
    });

TEST_P(PeepholeTest, Gates) {
  config.target = TargetMetric::Gates;
  synthesizer.synthesize(config);
  results = synthesizer.getResults();

  EXPECT_EQ(results.getGates(), test.expectedMinimalGates);
}
} // namespace cs

