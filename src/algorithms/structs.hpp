#ifndef STRUCTS
#define STRUCTS

struct Point {
    int x = 0, y = 0;
    Point(int x_, int y_): x(x_), y(y_) {}
    Point(const Point& other): x(other.x), y(other.y) {}
    bool operator<(const Point& other) {
        if (x < other.x && y < other.y) {
            return true;
        }
        return false;
    }
    bool operator>=(const Point& other) {
        if (x >= other.x && y >= other.y) {
            return true;
        }
        return false;
    }
    Point& operator=(const Point& other) {
        x = other.x;
        y = other.y;
        return *this;
    }
};

struct Rectangle {
    Point lower_left_point, upper_right_point;
    Rectangle(int x1, int y1, int x2, int y2):
    lower_left_point(x1, y1), upper_right_point(x2, y2) {}
    Rectangle& operator=(const Rectangle& other) {
        lower_left_point = other.lower_left_point;
        upper_right_point = other.upper_right_point;
        return *this;
    }
};

#endif
