/**
 * @file prim_algorithm_method.cpp
 * @author Peeton4ik
 *
 * Серверная часть алгоритма Прима.
 */

#include <string>
#include <vector>

#include "methods.hpp"
#include "nlohmann/json.hpp"

#include "../include/prim_algorithm.hpp"

namespace graph {

int PrimAlgorithmMethod(const nlohmann::json& input,
                        nlohmann::json* output) {
  try {
    if (!input.contains("vertices") || !input.contains("edges")) {
      (*output)["error"] =
          "Отсутствуют обязательные поля: vertices или edges";
      return 1;
    }

    WeightedGraph<double> graph;

    const auto& vertices = input["vertices"];
    for (const auto& vertex : vertices) {
      graph.AddVertex(vertex.get<size_t>());
    }

    const auto& edges = input["edges"];
    for (const auto& edge : edges) {
      if (!edge.contains("from") || !edge.contains("to") ||
          !edge.contains("weight")) {
        (*output)["error"] =
            "Ребро должно содержать поля: from, to, weight";
        return 1;
      }

      size_t from = edge["from"].get<size_t>();
      size_t to = edge["to"].get<size_t>();
      double weight = edge["weight"].get<double>();

      graph.AddEdge(from, to, weight);
    }

    size_t start_vertex = 0;
    if (input.contains("start_vertex")) {
      start_vertex = input["start_vertex"].get<size_t>();
    } else if (!vertices.empty()) {
      start_vertex = vertices[0].get<size_t>();
    }

    std::vector<PrimEdge> mst_edges = PrimAlgorithm(graph, start_vertex);

    nlohmann::json result_edges = nlohmann::json::array();
    double total_weight = 0.0;

    for (const auto& edge : mst_edges) {
      nlohmann::json json_edge;
      json_edge["from"] = edge.from;
      json_edge["to"] = edge.to;
      json_edge["weight"] = edge.weight;
      result_edges.push_back(json_edge);
      total_weight += edge.weight;
    }

    (*output)["mst_edges"] = result_edges;
    (*output)["total_weight"] = total_weight;
    (*output)["num_edges"] = mst_edges.size();

    return 0;
  } catch (const std::exception& e) {
    (*output)["error"] = std::string("Ошибка: ") + e.what();
    return 1;
  }
}

}  // namespace graph
