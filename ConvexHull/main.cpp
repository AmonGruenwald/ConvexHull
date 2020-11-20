#include <SFML/Graphics.hpp>
#include<iostream>
#include<chrono>
#include<thread>

#define POINTRADIUS 3
#define WINDOW_WIDTH 800
#define WINDOW_HEIGTH 600

struct Point {
public:
	float X;
	float Y;
	Point(float x, float y) :X(x), Y(y) {}
	Point() :X(0), Y(0) {}
};

struct Node {
	Node* clockwiseNext=nullptr;
	Node* counterclockNext=nullptr;
	Point point;
	Node(Point p):point(p){}
	Node(int x, int y):point(x,y){}
};

struct Hull {
	Node* left = nullptr;
	Node* right = nullptr;
	void pushBack(Node* n) {};
	void pushFront(Node* n) {};
};

typedef std::pair<Node*, Node*> Line;


//Setup functions
int handleParameters();
std::vector<Point> generatePoints(const std::string& filePath);

//Optimized calculation functions
Hull calculateHull(std::vector<Point>& points);
Hull merge(Hull left, Hull right);
Line findLowerTangentOfHulls(Hull left, Hull right);
Line findUpperTangentOfHulls(Hull left, Hull right);
bool isUpperTangentOfHull(Line tangent, Hull hull);

//Visual calculation functions
Hull calculateVisualHull(std::vector<Point>& points, sf::RenderWindow& window);
Hull mergeVisual(Hull left, Hull right, sf::RenderWindow& window);
Line visualFindLowerTangentOfHulls(Hull left, Hull right, sf::RenderWindow& window, Line upperTangent);
Line visualFindUpperTangentOfHulls(Hull left, Hull right, sf::RenderWindow& window);
bool visualIsUpperTangentOfHull(Line tangent, Hull hull, sf::RenderWindow& window);

//Utility functions
bool isPointLeftOfLine(Line line, Node* point);

//Display functions
void displayPoint(const Point& p, const sf::Color& color, sf::RenderWindow& window);
void displayPoints(const std::vector<Point>& points, const  sf::Color& color, sf::RenderWindow& window);
void displayHull(const Hull& hull, const sf::Color& color, sf::RenderWindow& window);
void displayLine(const Line& line, const sf::Color& color, sf::RenderWindow& window);



//variable that should be read from commandline arguments
bool visualMode = true;
std::string filePath = "";
int pointAmount = 10;
//colors
const sf::Color HullColor = sf::Color::Green;
const sf::Color PointColor = sf::Color::Black;
const sf::Color ErrorColor = sf::Color::Red;
const sf::Color CorrectColor = sf::Color::Green;
const sf::Color WorkingColor = sf::Color::Blue;
const sf::Color SecondaryWorkingColor = sf::Color::Cyan;

const int animationStepTime = 10;

std::vector<Point>points;

//main to test merge
/*
int main()
{
	Point a1(100, 100);
	Point a2(150, 50);
	Point a3(200, 100);
	Point a4(150, 150);


	points.push_back(a1);
	points.push_back(a2);
	points.push_back(a3);
	points.push_back(a4);

	auto aNode1 = new Node(a1);
	auto aNode2 = new Node(a2);
	auto aNode3 = new Node(a3);
	auto aNode4 = new Node(a4);

	aNode1->clockwiseNext = aNode2;
	aNode1->counterclockNext = aNode4;


	aNode2->clockwiseNext = aNode3;
	aNode2->counterclockNext = aNode1;

	aNode3->clockwiseNext = aNode4;
	aNode3->counterclockNext = aNode2;

	aNode4->clockwiseNext = aNode1;
	aNode4->counterclockNext = aNode3;

	Hull aHull;
	aHull.left = aNode1;
	aHull.right = aNode3;

	Point b1(300, 150);
	Point b2(350, 100);
	Point b3(400, 150);
	Point b4(350, 200);

	std::vector<Point> bPoints;

	points.push_back(b1);
	points.push_back(b2);
	points.push_back(b3);
	points.push_back(b4);

	auto bNode1 = new Node(b1);
	auto bNode2 = new Node(b2);
	auto bNode3 = new Node(b3);
	auto bNode4 = new Node(b4);

	bNode1->clockwiseNext = bNode2;
	bNode1->counterclockNext = bNode4;

	bNode2->clockwiseNext = bNode3;
	bNode2->counterclockNext = bNode1;

	bNode3->clockwiseNext = bNode4;
	bNode3->counterclockNext = bNode2;

	bNode4->clockwiseNext = bNode1;
	bNode4->counterclockNext = bNode3;

	Hull bHull;
	bHull.left = bNode1;
	bHull.right = bNode3;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Divide and Conquer");

	auto newHull = mergeVisual(aHull, bHull, window);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(newHull, HullColor, window);
		window.display();
	}
}
*/

int main()
{
	int parameterExit = handleParameters();
	if (parameterExit != 0) {
		return parameterExit;
	}

	points = generatePoints(filePath);
	
	bool firstLoop = true; 
	Hull hull;

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Divide and Conquer");

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear(sf::Color::White);

		if (firstLoop) {
			firstLoop = false;
			if (visualMode) {
				hull = calculateVisualHull(points, window);
			}
			else {
				auto start = std::chrono::high_resolution_clock::now();
				hull = calculateHull(points);
				auto end = std::chrono::high_resolution_clock::now();
				std::cout << "Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) << " microseconds" << std::endl;
			}
		}
		displayHull(hull, HullColor, window);
		displayPoints(points, PointColor, window);
		window.display();
	}
}


#pragma region Setup Functions
int handleParameters()
{
	//TODO: handle commandline arguments
	return 0;
}
std::vector<Point> generatePoints(const std::string& filePath)
{
	//TODO: check if filepath is set and then read from file instead of generating random points
	std::vector<Point> points;
	for (int i = 0; i < pointAmount; i++) {
		//TODO: use better rand function
		Point point(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGTH);
		points.push_back(point);
	}
	return points;
}
#pragma endregion

#pragma region Optimized Hull Calculation
Hull calculateHull(std::vector<Point>& points)
{
	//TODO: calculate Hull
	Hull hull;
	return hull;
}

Hull merge(Hull left, Hull right) {
	Hull newHull;
	auto upperTangent = findUpperTangentOfHulls(left, right);
	auto lowerTangent = findLowerTangentOfHulls(left, right);

	newHull.left = left.left;
	newHull.right = right.right;

	//TODO: Delete Points that get removed
	upperTangent.first->counterclockNext = upperTangent.second;
	upperTangent.second->clockwiseNext = upperTangent.first;

	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;

	return newHull;
}

Line findLowerTangentOfHulls(Hull left, Hull right) {
	Line lowerTangent(right.left, left.right);
	bool isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
	bool isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);
	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {
		while (!isLowerTangentOfLeft) {
			lowerTangent.second = lowerTangent.second->clockwiseNext;
			isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
		}
		isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);

		while (!isLowerTangentOfRight) {
			lowerTangent.first = lowerTangent.first->counterclockNext;
			isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);
		}
		isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
	}
	return lowerTangent;
}

Line findUpperTangentOfHulls(Hull left, Hull right) {
	Line upperTangent(left.right, right.left);
	bool isUpperTangentOfLeft = isUpperTangentOfHull(upperTangent, left);
	bool isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {
		while (!isUpperTangentOfLeft) {
			upperTangent.first = upperTangent.first->counterclockNext;
			isUpperTangentOfLeft = isUpperTangentOfHull(upperTangent, left);
		}
		isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
		while (!isUpperTangentOfRight) {
			upperTangent.second = upperTangent.second->clockwiseNext;
			isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
		}
		isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, left);
	}
	return upperTangent;
}

bool isUpperTangentOfHull(Line tangent, Hull hull) {
	auto currentPoint = hull.left;
	do {
		if (isPointLeftOfLine(tangent, currentPoint)) {
			return false;
		}
		currentPoint = currentPoint->clockwiseNext;
	} while (currentPoint != hull.left);
	return true;
}
#pragma endregion

//TODO: refactor visual stuff hard
#pragma region Visual Hull Calculation
Hull calculateVisualHull(std::vector<Point>& points, sf::RenderWindow& window)
{
	//TODO: calculate Hull
	Hull hull;
	return hull;
}

Hull mergeVisual(Hull left, Hull right, sf::RenderWindow& window) {
	
	
	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	Hull newHull;
	auto upperTangent = visualFindUpperTangentOfHulls(left, right, window);

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(upperTangent, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	auto lowerTangent = visualFindLowerTangentOfHulls(left, right, window, upperTangent);
	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(upperTangent, WorkingColor, window);
	displayLine(lowerTangent, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	newHull.left = left.left;
	newHull.right = right.right;

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, ErrorColor, window);
	displayHull(right, ErrorColor, window); 
	displayLine(upperTangent, WorkingColor, window);
	displayLine(lowerTangent, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	//TODO: Delete Points that get removed
	upperTangent.first->counterclockNext = upperTangent.second;
	upperTangent.second->clockwiseNext = upperTangent.first; 
	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(newHull, HullColor, window);	
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	return newHull;
}

Line visualFindLowerTangentOfHulls(Hull left, Hull right, sf::RenderWindow& window, Line upperTangent) {
	Line lowerTangent(right.left, left.right);

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(lowerTangent, SecondaryWorkingColor, window);
	displayLine(upperTangent, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	
	
	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(lowerTangent, SecondaryWorkingColor, window);
	displayLine(upperTangent, WorkingColor, window);
	bool isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, window);
	bool isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {

		while (!isLowerTangentOfLeft) {
			
			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(lowerTangent, SecondaryWorkingColor, window);
			displayLine(upperTangent, WorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
			lowerTangent.second = lowerTangent.second->clockwiseNext;

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(lowerTangent, SecondaryWorkingColor, window);
			displayLine(upperTangent, WorkingColor, window);
			isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		}
		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(lowerTangent, SecondaryWorkingColor, window);
		displayLine(upperTangent, WorkingColor, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(lowerTangent, SecondaryWorkingColor, window);
		displayLine(upperTangent, WorkingColor, window);
		isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		while (!isLowerTangentOfRight) {
			lowerTangent.first = lowerTangent.first->counterclockNext;
			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(lowerTangent, SecondaryWorkingColor, window);
			displayLine(upperTangent, WorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(lowerTangent, SecondaryWorkingColor, window);
			displayLine(upperTangent, WorkingColor, window);
			isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(lowerTangent, SecondaryWorkingColor, window);
		displayLine(upperTangent, WorkingColor, window);
		isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	}
	return lowerTangent;
}

Line visualFindUpperTangentOfHulls(Hull left, Hull right, sf::RenderWindow& window) {
	Line upperTangent(left.right, right.left);

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(upperTangent, SecondaryWorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));


	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(upperTangent, SecondaryWorkingColor, window);
	bool isUpperTangentOfLeft = visualIsUpperTangentOfHull(upperTangent, left, window);
	bool isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {
		while (!isUpperTangentOfLeft) {
			upperTangent.first = upperTangent.first->counterclockNext;
			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(upperTangent, SecondaryWorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));


			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(upperTangent, SecondaryWorkingColor, window);
			isUpperTangentOfLeft = visualIsUpperTangentOfHull(upperTangent, left, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(upperTangent, SecondaryWorkingColor, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(upperTangent, SecondaryWorkingColor, window);
		isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		while (!isUpperTangentOfRight) {
			upperTangent.second = upperTangent.second->clockwiseNext;

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(upperTangent, SecondaryWorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(upperTangent, SecondaryWorkingColor, window);
			isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

			
		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(upperTangent, SecondaryWorkingColor, window);
		isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, left, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	}
	return upperTangent;
}

bool visualIsUpperTangentOfHull(Line tangent, Hull hull, sf::RenderWindow& window) {
	auto currentPoint = hull.left;
	bool upper = true;
	do {
		if (isPointLeftOfLine(tangent, currentPoint)) {
			displayPoint(currentPoint->point, ErrorColor, window);
			upper = false;
		}
		else {
			displayPoint(currentPoint->point, CorrectColor, window);
		}
		currentPoint = currentPoint->clockwiseNext;
	} while (currentPoint != hull.left);
	return upper;
}

#pragma endregion

#pragma region Utility Functions

bool isPointLeftOfLine(Line line, Node* point) {
	Point a = line.first->point;
	Point b = line.second->point;
	Point p = point->point;
	return ((b.X - a.X) * (p.Y - a.Y) - (b.Y - a.Y) * (p.X - a.X))>0;
}

#pragma endregion

#pragma region Display Functions
void displayPoint(const Point& p, const sf::Color& color, sf::RenderWindow& window) {
	sf::CircleShape point(POINTRADIUS);
	point.setFillColor(color);
	point.setPosition(p.X - POINTRADIUS, p.Y - POINTRADIUS);
	window.draw(point);
}
void displayPoints(const std::vector<Point>& points, const  sf::Color& color, sf::RenderWindow& window)
{
	for (auto& p : points) {
		displayPoint(p, color, window);
	}
}
void displayHull(const Hull& hull, const sf::Color& color, sf::RenderWindow& window)
{
	auto currentNode = hull.left;
	if (currentNode != nullptr) {
		do {
			displayLine(Line(currentNode, currentNode->clockwiseNext), color, window);
			currentNode = currentNode->clockwiseNext;
		} while (currentNode != hull.left);
	}
}
void displayLine(const Line& line, const sf::Color& color, sf::RenderWindow& window)
{
	sf::Vertex lineDrawing[] =
	{
		sf::Vertex(sf::Vector2f(line.first->point.X, line.first->point.Y)),
		sf::Vertex(sf::Vector2f(line.second->point.X, line.second->point.Y))
	};
	lineDrawing->color = color;
	window.draw(lineDrawing, 2, sf::Lines);
}
#pragma endregion