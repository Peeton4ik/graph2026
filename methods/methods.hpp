/**
 * @file methods/methods.hpp
 * @author Your Name
 *
 * Заголовочный файл для методов сервера.
 */

#ifndef METHODS_METHODS_HPP_
#define METHODS_METHODS_HPP_

#include "nlohmann/json.hpp"

namespace graph {

int PrimAlgorithmMethod(const nlohmann::json& input,
                        nlohmann::json* output);

}  // namespace graph

#endif  // METHODS_METHODS_HPP_
