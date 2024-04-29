#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <memory>
#include <functional>
#include <chrono>
#include <cmath>
#include "structs.hpp"


class MapAlgorithm {
    std::set<int> x_coords, y_coords;
    std::unordered_map<int, int> value_to_id_x, value_to_id_y;
    std::vector<std::vector<int>> map;
public:
    MapAlgorithm(std::vector<Rectangle>& rectangles) {
        for (auto r: rectangles) {
            x_coords.insert(r.lower_left_point.x);
            y_coords.insert(r.lower_left_point.y);
            x_coords.insert(r.upper_right_point.x);
            y_coords.insert(r.upper_right_point.y);
        }

        int index = 0;
        for (auto it:x_coords) {
            value_to_id_x.insert({it, index++});
        }
        index = 0;
        for (auto it:y_coords) {
            value_to_id_y.insert({it, index++});
        }
        map = std::vector<std::vector<int>>{value_to_id_x.size(), std::vector<int>(value_to_id_y.size(), 0)};

        for (auto r:rectangles) {
            int max_i = value_to_id_x[r.upper_right_point.x];
            int max_j = value_to_id_y[r.upper_right_point.y];
            for (int i = value_to_id_x[r.lower_left_point.x]; i < max_i; ++i) {
                for (int j = value_to_id_y[r.lower_left_point.y]; j < max_j; ++j) {
                    ++map[i][j];
                }
            }
        }
    }
    size_t get_number_intersections(Point point) {
        int x = *(--x_coords.upper_bound(point.x));
        int y = *(--y_coords.upper_bound(point.y));
        if (x < *x_coords.begin() || x > *(--x_coords.end()) || 
            y < *y_coords.begin() || y > *(--y_coords.end())) {
            return 0;
        }
        return map[value_to_id_x[x]][value_to_id_y[y]];
    }
};
