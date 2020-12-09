#pragma once

#include <utility>

struct Point {
	float X;
	float Y;
	Point(float x, float y) :X(x), Y(y) {}
	Point() :X(0), Y(0) {}

	bool operator<(const Point& rhs) const
	{
		return X == rhs.X ? Y > rhs.Y : X < rhs.X;
	}

	bool operator==(const Point& rhs) const
	{
		return (X == rhs.X && Y == rhs.Y);
	}
};

struct Node {
	Node* clockwiseNext = nullptr;
	Node* counterclockNext = nullptr;
	Point point;
	Node(Point p) :point(p) {}
	Node(int x, int y) :point(x, y) {}
};

struct Hull {
	Node* left = nullptr;
	Node* right = nullptr;
};

typedef std::pair<Node*, Node*> Line;