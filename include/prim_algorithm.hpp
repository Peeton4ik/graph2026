/**
 * @file prim_algorithm.hpp
 * @author Your Name
 *
 * Реализация алгоритма Прима для поиска минимального остовного дерева
 * за O(n^2).
 */

#ifndef INCLUDE_PRIM_ALGORITHM_HPP_
#define INCLUDE_PRIM_ALGORITHM_HPP_

#include <vector>
#include <limits>
#include <stdexcept>
#include <unordered_map>
#include "weighted_graph.hpp"

namespace graph {

/**
 * @brief Структура для хранения ребра минимального остовного дерева.
 */
struct PrimEdge {
  size_t from;    
  size_t to;      
  double weight;  
};

/**
 * @brief Алгоритм Прима для поиска минимального остовного дерева.
 *
 * @tparam Weight Тип веса ребра.
 * @param graph Взвешенный неориентированный граф.
 * @param start_vertex Начальная вершина для построения MST.
 * @return Вектор рёбер минимального остовного дерева.
 *
 * Функция реализует алгоритм Прима за O(n^2) для нахождения минимального
 * остовного дерева во взвешенном неориентированном графе.
 *
 * @throws std::invalid_argument Если граф пуст или начальная вершина
 *         отсутствует в графе.
 * @throws std::runtime_error Если граф несвязный.
 */
template<typename Weight>
std::vector<PrimEdge> PrimAlgorithm(
    const WeightedGraph<Weight>& graph,
    size_t start_vertex) {
  
  if (graph.NumVertices() == 0) {
    throw std::invalid_argument("Граф пуст");
  }

  if (!graph.HasVertex(start_vertex)) {
    throw std::invalid_argument("Начальная вершина отсутствует в графе");
  }

  size_t num_vertices = graph.NumVertices();
  

  std::vector<size_t> vertices;
  vertices.reserve(num_vertices);
  for (size_t vertex_id : graph.Vertices()) {
    vertices.push_back(vertex_id);
  }


  std::unordered_map<size_t, size_t> vertex_to_index;
  for (size_t i = 0; i < vertices.size(); ++i) {
    vertex_to_index[vertices[i]] = i;
  }

  
  std::vector<Weight> min_weight(num_vertices, 
                                  std::numeric_limits<Weight>::max());
  
 
  std::vector<int> parent(num_vertices, -1);
  

  std::vector<bool> in_mst(num_vertices, false);


  size_t start_index = vertex_to_index[start_vertex];
  min_weight[start_index] = Weight();

  std::vector<PrimEdge> mst_edges;
  mst_edges.reserve(num_vertices - 1);

  for (size_t count = 0; count < num_vertices; ++count) {

    size_t min_idx = num_vertices;
    Weight min_val = std::numeric_limits<Weight>::max();

    for (size_t i = 0; i < num_vertices; ++i) {
      if (!in_mst[i] && min_weight[i] < min_val) {
        min_val = min_weight[i];
        min_idx = i;
      }
    }

    if (min_idx == num_vertices) {
      throw std::runtime_error("Граф несвязный");
    }


    in_mst[min_idx] = true;


    if (parent[min_idx] != -1) {
      PrimEdge edge;
      edge.from = vertices[parent[min_idx]];
      edge.to = vertices[min_idx];
      edge.weight = static_cast<double>(min_weight[min_idx]);
      mst_edges.push_back(edge);
    }


    size_t current_vertex_id = vertices[min_idx];
    const auto& neighbors = graph.Edges(current_vertex_id);

    for (size_t neighbor_id : neighbors) {
      auto it = vertex_to_index.find(neighbor_id);
      if (it == vertex_to_index.end()) {
        continue;
      }
      
      size_t neighbor_idx = it->second;
      
      if (!in_mst[neighbor_idx]) {
        Weight edge_weight = graph.EdgeWeight(current_vertex_id, neighbor_id);
        
        if (edge_weight < min_weight[neighbor_idx]) {
          min_weight[neighbor_idx] = edge_weight;
          parent[neighbor_idx] = static_cast<int>(min_idx);
        }
      }
    }
  }

  return mst_edges;
}

}  // namespace graph

#endif  // INCLUDE_PRIM_ALGORITHM_HPP_
