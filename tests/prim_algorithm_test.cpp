/**
 * @file tests/prim_algorithm_test.cpp
 * @author Peeton4ik
 *
 * Тесты для алгоритма Прима.
 */

#include <cmath>
#include <random>
#include <vector>

#include "test.hpp"
#include "test_core.hpp"

#include "../include/prim_algorithm.hpp"
#include "httplib.h"
#include "nlohmann/json.hpp"

namespace {

using graph::PrimAlgorithm;
using graph::PrimEdge;
using graph::WeightedGraph;

static void SimpleTest() {
  WeightedGraph<double> graph;

  graph.AddVertex(1);
  graph.AddVertex(2);
  graph.AddVertex(3);
  graph.AddVertex(4);

  graph.AddEdge(1, 2, 1.0);
  graph.AddEdge(1, 3, 4.0);
  graph.AddEdge(2, 3, 2.0);
  graph.AddEdge(2, 4, 6.0);
  graph.AddEdge(3, 4, 3.0);

  std::vector<PrimEdge> mst = PrimAlgorithm(graph, 1);

  REQUIRE_EQUAL(mst.size(), static_cast<size_t>(3));

  double total_weight = 0.0;
  for (const auto& edge : mst) {
    total_weight += edge.weight;
  }

  REQUIRE_CLOSE(total_weight, 6.0, 0.001);
}

static void LinearGraphTest() {
  WeightedGraph<double> graph;

  graph.AddVertex(1);
  graph.AddVertex(2);
  graph.AddVertex(3);
  graph.AddVertex(4);

  graph.AddEdge(1, 2, 5.0);
  graph.AddEdge(2, 3, 3.0);
  graph.AddEdge(3, 4, 7.0);

  std::vector<PrimEdge> mst = PrimAlgorithm(graph, 1);

  REQUIRE_EQUAL(mst.size(), static_cast<size_t>(3));

  double total_weight = 0.0;
  for (const auto& edge : mst) {
    total_weight += edge.weight;
  }

  REQUIRE_CLOSE(total_weight, 15.0, 0.001);
}

static void CompleteGraphTest() {
  WeightedGraph<double> graph;

  for (size_t i = 1; i <= 5; ++i) {
    graph.AddVertex(i);
  }

  graph.AddEdge(1, 2, 1.0);
  graph.AddEdge(1, 3, 2.0);
  graph.AddEdge(1, 4, 3.0);
  graph.AddEdge(1, 5, 4.0);
  graph.AddEdge(2, 3, 5.0);
  graph.AddEdge(2, 4, 6.0);
  graph.AddEdge(2, 5, 7.0);
  graph.AddEdge(3, 4, 8.0);
  graph.AddEdge(3, 5, 9.0);
  graph.AddEdge(4, 5, 10.0);

  std::vector<PrimEdge> mst = PrimAlgorithm(graph, 1);

  REQUIRE_EQUAL(mst.size(), static_cast<size_t>(4));

  double total_weight = 0.0;
  for (const auto& edge : mst) {
    total_weight += edge.weight;
  }

  REQUIRE_CLOSE(total_weight, 10.0, 0.001);
}

static void EmptyGraphTest() {
  WeightedGraph<double> graph;

  REQUIRE_THROW(PrimAlgorithm(graph, 0), std::invalid_argument);
}

static void InvalidStartVertexTest() {
  WeightedGraph<double> graph;
  graph.AddVertex(1);
  graph.AddVertex(2);

  REQUIRE_THROW(PrimAlgorithm(graph, 999), std::invalid_argument);
}

static void DisconnectedGraphTest() {
  WeightedGraph<double> graph;

  graph.AddVertex(1);
  graph.AddVertex(2);
  graph.AddVertex(3);
  graph.AddVertex(4);

  graph.AddEdge(1, 2, 1.0);
  graph.AddEdge(3, 4, 2.0);

  REQUIRE_THROW(PrimAlgorithm(graph, 1), std::runtime_error);
}

static void SingleVertexTest() {
  WeightedGraph<double> graph;
  graph.AddVertex(1);

  std::vector<PrimEdge> mst = PrimAlgorithm(graph, 1);

  REQUIRE_EQUAL(mst.size(), static_cast<size_t>(0));
}

static void RandomGraphTest(httplib::Client* cli) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<size_t> vertex_dist(1, 20);
  std::uniform_real_distribution<double> weight_dist(1.0, 100.0);

  size_t num_vertices = vertex_dist(gen);

  nlohmann::json request;
  nlohmann::json vertices = nlohmann::json::array();
  nlohmann::json edges = nlohmann::json::array();

  for (size_t i = 1; i <= num_vertices; ++i) {
    vertices.push_back(i);
  }

  for (size_t i = 1; i < num_vertices; ++i) {
    nlohmann::json edge;
    edge["from"] = i;
    edge["to"] = i + 1;
    edge["weight"] = weight_dist(gen);
    edges.push_back(edge);
  }

  std::uniform_int_distribution<size_t> edge_count_dist(0, num_vertices);
  size_t extra_edges = edge_count_dist(gen);

  for (size_t i = 0; i < extra_edges; ++i) {
    size_t from = vertex_dist(gen) % num_vertices + 1;
    size_t to = vertex_dist(gen) % num_vertices + 1;

    if (from != to) {
      nlohmann::json edge;
      edge["from"] = from;
      edge["to"] = to;
      edge["weight"] = weight_dist(gen);
      edges.push_back(edge);
    }
  }

  request["vertices"] = vertices;
  request["edges"] = edges;

  auto res =
      cli->Post("/PrimAlgorithm", request.dump(), "application/json");

  REQUIRE(res != nullptr);
  REQUIRE_EQUAL(res->status, 200);

  nlohmann::json response = nlohmann::json::parse(res->body);

  REQUIRE(!response.contains("error"));
  REQUIRE(response.contains("mst_edges"));
  REQUIRE(response.contains("total_weight"));
  REQUIRE(response.contains("num_edges"));

  size_t expected_edges = num_vertices - 1;
  REQUIRE_EQUAL(response["num_edges"].get<size_t>(), expected_edges);
}

}  // namespace

void TestPrimAlgorithm() {
  TestSuite suite("TestPrimAlgorithm");

  RUN_TEST(suite, SimpleTest);
  RUN_TEST(suite, LinearGraphTest);
  RUN_TEST(suite, CompleteGraphTest);
  RUN_TEST(suite, EmptyGraphTest);
  RUN_TEST(suite, InvalidStartVertexTest);
  RUN_TEST(suite, DisconnectedGraphTest);
  RUN_TEST(suite, SingleVertexTest);
}

void TestPrimAlgorithmRemote(httplib::Client* cli) {
  TestSuite suite("TestPrimAlgorithmRemote");

  RUN_TEST_REMOTE(suite, cli, RandomGraphTest);
}
