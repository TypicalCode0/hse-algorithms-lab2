#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <set>
#include <memory>
#include <functional>
#include <chrono>
#include <cmath>
#include "structs.hpp"

class BruteForceAlgorithm {
public:
    size_t get_number_intersections(std::vector<Rectangle> &rectangles, Point point) {
        size_t count = 0;
        for (auto rectangle: rectangles) {
            if (point >= rectangle.lower_left_point && 
                point < rectangle.upper_right_point) {
                ++count;
            }
        }
        return count;
    }
};
