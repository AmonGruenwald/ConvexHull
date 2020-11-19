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

struct LinkedList {
	Node* head = nullptr;
	Node* tail = nullptr;
	Node* left = nullptr;
	Node* right = nullptr;
	void pushBack(Node* n) {};
	void pushFront(Node* n) {};
};

//Setup functions
int handleParameters();
std::vector<Point> generatePoints(const std::string& filePath);

//Optimized calculation functions
LinkedList calculateHull(std::vector<Point>& points);
LinkedList merge(LinkedList left, LinkedList right);
std::pair<Node*, Node*> findLowerTangentOfHulls(LinkedList left, LinkedList right);
std::pair<Node*, Node*> findUpperTangentOfHulls(LinkedList left, LinkedList right);
bool isUpperTangentOfHull(Point a, Point b, LinkedList hull);

//Visual calculation functions
LinkedList calculateVisualHull(std::vector<Point>& points, sf::RenderWindow& window);
LinkedList mergeVisual(LinkedList left, LinkedList right, sf::RenderWindow& window);
std::pair<Node*, Node*> visualFindLowerTangentOfHulls(LinkedList left, LinkedList right, sf::RenderWindow& window, std::pair<Node*, Node*> upperTangent);
std::pair<Node*, Node*> visualFindUpperTangentOfHulls(LinkedList left, LinkedList right, sf::RenderWindow& window);
bool visualIsUpperTangentOfHull(Point a, Point b, LinkedList hull, sf::RenderWindow& window);

//Utility functions
bool isPointLeftOfLine(Point a, Point b, Point p);

//Display functions
void displayPoint(const Point& p, const sf::Color& color, sf::RenderWindow& window);
void displayPoints(const std::vector<Point>& points, const  sf::Color& color, sf::RenderWindow& window);
void displayHull(const LinkedList& hull, const sf::Color& color, sf::RenderWindow& window);
void displayLine(const Point& a, const Point& b, const sf::Color& color, sf::RenderWindow& window);



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

const int animationStepTime = 1000;

std::vector<Point>points;

//main to test merge
/*
int main()
{
	Point a1(100, 100);
	Point a2(150, 150);
	Point a3(200, 100);
	Point a4(150, 50);


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

	LinkedList aHull;
	aHull.left = aNode1;
	aHull.right = aNode3;

	Point b1(300, 150);
	Point b2(350, 200);
	Point b3(400, 150);
	Point b4(350, 100);

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

	LinkedList bHull;
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
	LinkedList hull;

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
LinkedList calculateHull(std::vector<Point>& points)
{
	//TODO: calculate Hull
	LinkedList hull;
	return hull;
}

LinkedList merge(LinkedList left, LinkedList right) {
	LinkedList newHull;
	auto upperTangent = findUpperTangentOfHulls(left, right);
	auto lowerTangent = findLowerTangentOfHulls(left, right);

	newHull.left = left.left;
	newHull.right = right.right;

	//TODO: Delete Points that get removed
	upperTangent.first->clockwiseNext = upperTangent.second;
	upperTangent.second->counterclockNext = upperTangent.first;

	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;

	return newHull;
}

std::pair<Node*, Node*> findLowerTangentOfHulls(LinkedList left, LinkedList right) {
	auto a = left.right;
	auto b = right.left;

	bool isLowerTangentOfLeft = isUpperTangentOfHull(b->point, a->point, left);
	bool isLowerTangentOfRight = isUpperTangentOfHull(b->point, a->point, right);
	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {
		while (!isLowerTangentOfLeft) {
			a = a->clockwiseNext;
			isLowerTangentOfLeft = isUpperTangentOfHull(b->point, a->point, left);
		}
		isLowerTangentOfRight = isUpperTangentOfHull(b->point, a->point, right);

		while (!isLowerTangentOfRight) {
			b = b->counterclockNext;
			isLowerTangentOfRight = isUpperTangentOfHull(b->point, a->point, right);
		}
		isLowerTangentOfLeft = isUpperTangentOfHull(b->point, a->point, left);
	}
	return std::pair<Node*, Node*>(a, b);
}

std::pair<Node*, Node*> findUpperTangentOfHulls(LinkedList left, LinkedList right) {
	auto a = left.right;
	auto b = right.left;

	bool isUpperTangentOfLeft = isUpperTangentOfHull(a->point, b->point, left);
	bool isUpperTangentOfRight = isUpperTangentOfHull(a->point, b->point, right);
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {
		while (!isUpperTangentOfLeft) {
			a = a->counterclockNext;
			isUpperTangentOfLeft = isUpperTangentOfHull(a->point, b->point, left);
		}
		isUpperTangentOfRight = isUpperTangentOfHull(a->point, b->point, right);
		while (!isUpperTangentOfRight) {
			b = b->clockwiseNext;
			isUpperTangentOfRight = isUpperTangentOfHull(a->point, b->point, right);
		}
		isUpperTangentOfRight = isUpperTangentOfHull(a->point, b->point, left);
	}
	return std::pair<Node*, Node*>(a, b);
}

bool isUpperTangentOfHull(Point a, Point b, LinkedList hull) {
	auto currentPoint = hull.left;
	do {
		if (isPointLeftOfLine(a, b, currentPoint->point)) {
			return false;
		}
		currentPoint = currentPoint->clockwiseNext;
	} while (currentPoint != hull.left);
	return true;
}
#pragma endregion

//TODO: refactor visual stuff hard
#pragma region Visual Hull Calculation
LinkedList calculateVisualHull(std::vector<Point>& points, sf::RenderWindow& window)
{
	//TODO: calculate Hull
	LinkedList hull;
	return hull;
}

LinkedList mergeVisual(LinkedList left, LinkedList right, sf::RenderWindow& window) {
	
	
	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	LinkedList newHull;
	auto upperTangent = visualFindUpperTangentOfHulls(left, right, window);

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	auto lowerTangent = visualFindLowerTangentOfHulls(left, right, window, upperTangent);

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
	displayLine(lowerTangent.first->point, lowerTangent.second->point, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	newHull.left = left.left;
	newHull.right = right.right;

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, ErrorColor, window);
	displayHull(right, ErrorColor, window); 
	displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
	displayLine(lowerTangent.first->point, lowerTangent.second->point, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	//TODO: Delete Points that get removed
	upperTangent.first->clockwiseNext = upperTangent.second;
	upperTangent.second->counterclockNext = upperTangent.first;
	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;


	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(newHull, HullColor, window);	
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	return newHull;
}

std::pair<Node*, Node*> visualFindLowerTangentOfHulls(LinkedList left, LinkedList right, sf::RenderWindow& window, std::pair<Node*, Node*> upperTangent) {
	auto a = left.right;
	auto b = right.left;

	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(a->point, b->point, SecondaryWorkingColor, window);
	displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	
	
	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(a->point, b->point, SecondaryWorkingColor, window);
	displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
	bool isLowerTangentOfLeft = visualIsUpperTangentOfHull(b->point, a->point, left, window);
	bool isLowerTangentOfRight = visualIsUpperTangentOfHull(b->point, a->point, right, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {

		while (!isLowerTangentOfLeft) {
			
			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
			a = a->clockwiseNext;


			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
			isLowerTangentOfLeft = visualIsUpperTangentOfHull(b->point, a->point, left, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		}
		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(a->point, b->point, SecondaryWorkingColor, window);
		displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(a->point, b->point, SecondaryWorkingColor, window);
		displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
		isLowerTangentOfRight = visualIsUpperTangentOfHull(b->point, a->point, right, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		while (!isLowerTangentOfRight) {
			b = b->counterclockNext;

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
			isLowerTangentOfRight = visualIsUpperTangentOfHull(b->point, a->point, right, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(a->point, b->point, SecondaryWorkingColor, window);
		displayLine(upperTangent.first->point, upperTangent.second->point, WorkingColor, window);
		isLowerTangentOfLeft = visualIsUpperTangentOfHull(b->point, a->point, left, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	}
	return std::pair<Node*, Node*>(a, b);
}

std::pair<Node*, Node*> visualFindUpperTangentOfHulls(LinkedList left, LinkedList right, sf::RenderWindow& window) {
	auto a = left.right;
	auto b = right.left;


	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(a->point, b->point, SecondaryWorkingColor, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));


	window.clear(sf::Color::White);
	displayPoints(points, PointColor, window);
	displayHull(left, HullColor, window);
	displayHull(right, HullColor, window);
	displayLine(a->point, b->point, SecondaryWorkingColor, window);
	bool isUpperTangentOfLeft = visualIsUpperTangentOfHull(a->point, b->point, left, window);
	bool isUpperTangentOfRight = visualIsUpperTangentOfHull(a->point, b->point, right, window);
	window.display();
	std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {

		

		while (!isUpperTangentOfLeft) {
			a = a->counterclockNext;
			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));


			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			isUpperTangentOfLeft = visualIsUpperTangentOfHull(a->point, b->point, left, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(a->point, b->point, SecondaryWorkingColor, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(a->point, b->point, SecondaryWorkingColor, window);
		isUpperTangentOfRight = visualIsUpperTangentOfHull(a->point, b->point, right, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

		while (!isUpperTangentOfRight) {
			b = b->clockwiseNext;

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

			window.clear(sf::Color::White);
			displayPoints(points, PointColor, window);
			displayHull(left, HullColor, window);
			displayHull(right, HullColor, window);
			displayLine(a->point, b->point, SecondaryWorkingColor, window);
			isUpperTangentOfRight = visualIsUpperTangentOfHull(a->point, b->point, right, window);
			window.display();
			std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));

			
		}

		window.clear(sf::Color::White);
		displayPoints(points, PointColor, window);
		displayHull(left, HullColor, window);
		displayHull(right, HullColor, window);
		displayLine(a->point, b->point, SecondaryWorkingColor, window);
		isUpperTangentOfRight = visualIsUpperTangentOfHull(a->point, b->point, left, window);
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	}
	return std::pair<Node*, Node*>(a, b);
}

bool visualIsUpperTangentOfHull(Point a, Point b, LinkedList hull, sf::RenderWindow& window) {
	auto currentPoint = hull.left;
	bool upper = true;
	do {
		if (isPointLeftOfLine(a, b, currentPoint->point)) {
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

bool isPointLeftOfLine(Point a, Point b, Point p) {
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
void displayHull(const LinkedList& hull, const sf::Color& color, sf::RenderWindow& window)
{
	auto currentNode = hull.left;
	if (currentNode != nullptr) {
		do {
			displayLine(currentNode->point, currentNode->clockwiseNext->point, color, window);
			currentNode = currentNode->clockwiseNext;
		} while (currentNode != hull.left);
	}
}
void displayLine(const Point& a, const Point& b, const sf::Color& color, sf::RenderWindow& window)
{
	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(a.X, a.Y)),
		sf::Vertex(sf::Vector2f(b.X, b.Y))
	};
	line->color = color;
	window.draw(line, 2, sf::Lines);
}
#pragma endregion