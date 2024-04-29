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
#include "src/algorithms/structs.hpp"

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


struct Node {
    int value;
    std::shared_ptr<Node> left, right;
    Node(int value, std::shared_ptr<Node> left, std::shared_ptr<Node> right):
         value(value), left(left), right(right) {}
    Node(): value(0), left(nullptr), right(nullptr) {}
};

class PersistentSegmentTree {
    std::vector<std::shared_ptr<Node>> versions;
    size_t count_nodes;

    void recursive_build(const std::vector<int> &values, int left_border, 
                         int right_border, std::shared_ptr<Node> &node) {
        if (right_border - left_border == 0) {
            node->value = 0;
            return;
        } else if (right_border - left_border == 1) {
            node->value = values[left_border];
            return;
        }
        int middle = (right_border + left_border) / 2;
        if (!node->left) {
            node->left = std::make_shared<Node>();
        }
        recursive_build(values, left_border, middle, node->left);
        if (!node->right) {
            node->right = std::make_shared<Node>();
        }
        recursive_build(values, middle, right_border, node->right);
    }

    auto recursive_add(int value, int left_border, int right_border, 
                       int curr_left_border, int curr_right_border, 
                       std::shared_ptr<Node> &node) {
        if (curr_left_border >= left_border && curr_right_border <= right_border) {
            return std::make_shared<Node>(node->value + value, node->left, node->right); 
        }
        if (curr_left_border >= right_border || curr_right_border <= left_border ||
            curr_right_border - curr_left_border <= 1) {
            return node;
        }
        int middle = (curr_left_border + curr_right_border) / 2;
        auto left_copy = recursive_add(value, left_border, right_border, 
                                       curr_left_border, middle, node->left);
        auto right_copy = recursive_add(value, left_border, right_border, 
                                        middle, curr_right_border, node->right);
        auto copy = std::make_shared<Node>(node->value, left_copy, right_copy);
        return copy;
    }

public:
    PersistentSegmentTree() = default;

    PersistentSegmentTree(const std::vector<int> &values): count_nodes(values.size()) {
        versions.push_back(std::make_shared<Node>());
        recursive_build(values, 0, values.size(), versions[0]);
    }

    void add(int value, int left_border, int right_border) {
        auto copy_from = versions[versions.size() - 1];
        auto new_version = recursive_add(value, left_border, right_border, 
                                         0, count_nodes, copy_from);
        versions.push_back(new_version);       
    }

    int get_sum(int index, int version) {
        auto curr_node = versions[version];
        int sum = 0, left_border = 0, right_border = count_nodes;
        do {
            sum += curr_node->value;
            int middle = (left_border + right_border) / 2;
            if (index < middle) {
                curr_node = curr_node->left;
                right_border = middle;
            } else {
                curr_node = curr_node->right;
                left_border = middle;
            }
        } while (curr_node);
        return sum;
    }
};

class PersistentSegmentTreeAlgorithm {
    PersistentSegmentTree tree;
    std::vector<int> sorted_x, sorted_y;
public:
    PersistentSegmentTreeAlgorithm(std::vector<Rectangle> &rectangles) {
        std::vector<std::tuple<int, int, int, int>> events;
        std::set<int> y_coords;
        for (auto r: rectangles) {
            int x1 = r.lower_left_point.x, y1 = r.lower_left_point.y;
            int x2 = r.upper_right_point.x, y2 = r.upper_right_point.y;
            events.push_back({x1, y1, y2, 1});
            events.push_back({x2, y1, y2, -1});
            y_coords.insert(y1);
            y_coords.insert(y2);
            sorted_x.push_back(x1);
            sorted_x.push_back(x2);
        }
        std::sort(events.begin(), events.end(), 
            [](std::tuple<int, int, int, int> a, std::tuple<int, int, int, int> b) {
                return std::get<0>(a) < std::get<0>(b);
            });
        std::sort(sorted_x.begin(), sorted_x.end());
        sorted_y = std::vector<int>(y_coords.begin(), y_coords.end());
        tree = PersistentSegmentTree(std::vector<int>(sorted_y.size(), 0));
        for (auto &event:events) {
            int left_border = (std::upper_bound(sorted_y.begin(), sorted_y.end(), std::get<1>(event)) - sorted_y.begin() - 1);
            int right_border = (std::upper_bound(sorted_y.begin(), sorted_y.end(), std::get<2>(event)) - sorted_y.begin() - 1);
            tree.add(std::get<3>(event), left_border, right_border);
        }
    }

    int get_number_intersections(Point point) {
        int version = (std::upper_bound(sorted_x.begin(), sorted_x.end(), point.x) - sorted_x.begin());
        int index = (std::upper_bound(sorted_y.begin(), sorted_y.end(), point.y) - sorted_y.begin() - 1);
        return tree.get_sum(index, version);
    }
};

int main() {
    using clock = std::chrono::steady_clock;
    using nano = std::chrono::nanoseconds;
    int m = 100000;
    std::vector<std::vector<long double>> time_query(3);
    std::vector<std::vector<long double>> time_preprocessing(2);
    for (int n = 1; n < 4096; n <<= 1) {
        std::vector<Rectangle> rectangles;
        for (int i = 0; i < n * 2; i += 2) {
            rectangles.push_back({i*10, i*10,10*(2*n - i), 10*(2*n - i)});
        }
        BruteForceAlgorithm alg1;
        auto start = clock::now();
        MapAlgorithm alg2(rectangles);
        auto end = clock::now();
        time_preprocessing[0].push_back(std::chrono::duration_cast<nano>(end - start).count());

        start = clock::now();
        PersistentSegmentTreeAlgorithm alg3(rectangles);
        end = clock::now();
        time_preprocessing[1].push_back(std::chrono::duration_cast<nano>(end - start).count());

        start = clock::now();
        for (int i = 0; i < m; ++i) {
            int x = long(std::pow(2999 * i, 31)) % (20 * n);
            int y = long(std::pow(5323 * i, 31)) % (20 * n);
            int ans1 = alg1.get_number_intersections(rectangles, Point(x, y));
        }
        end = clock::now();
        time_query[0].push_back(std::chrono::duration_cast<nano>(end - start).count());

        start = clock::now();
        for (int i = 0; i < m; ++i) {
            int x = long(std::pow(2999 * i, 31)) % (20 * n);
            int y = long(std::pow(5323 * i, 31)) % (20 * n);
            int ans2 = alg2.get_number_intersections(Point(x, y));
        }
        end = clock::now();
        time_query[1].push_back(std::chrono::duration_cast<nano>(end - start).count());

        start = clock::now();
        for (int i = 0; i < m; ++i) {
            int x = long(std::pow(2999 * i, 31)) % (20 * n);
            int y = long(std::pow(5323 * i, 31)) % (20 * n);
            int ans3 = alg3.get_number_intersections(Point(x, y));
        }
        end = clock::now();
        time_query[2].push_back(std::chrono::duration_cast<nano>(end - start).count());
    }
    std::cout<<std::fixed<<std::setprecision(0);
    std::cout<<"preprocessing\nMapAlgorithm\tPersistentSegmentTreeAlgorithm\n";
    for (int i = 0; i < time_preprocessing[0].size(); ++i) {
        std::cout<<std::setw(12)<<time_preprocessing[0][i]<<"\t"<<time_preprocessing[1][i]<<"\n";
    }
    std::cout<<"\n\nqueries\nBruteForceAlgorithm\tMapAlgorithm\tPersistentSegmentTreeAlgorithm\n";
    for (int i = 0; i < time_query[0].size(); ++i) {    
        std::cout<<std::setw(19)<<time_query[0][i]<<"\t"<<std::setw(12)<<time_query[1][i]<<"\t"<<time_query[2][i]<<"\n";
    }
}
