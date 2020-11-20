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

class Scene {
public:
	Scene(sf::RenderWindow& window) :window(window) {};
	
	void Render() {
		window.clear(sf::Color::White);
		for (auto p : DefaultPoints) {
			displayPoint(p, DefaultColor);
		}
		for (auto p : ErrorPoints) {
			displayPoint(p, ErrorColor);
		}
		for (auto p : CorrectPoints) {
			displayPoint(p, CorrectColor);
		}
		for (auto h : DefaultHulls) {
			displayHull(h, DefaultColor);
		}
		for (auto h : CorrectHulls) {
			displayHull(h, CorrectColor);
		}
		for (auto h : ErrorHulls) {
			displayHull(h, ErrorColor);
		}
		for (auto l : DefaultLines) {
			displayLine(l, DefaultColor);
		}
		for (auto l : WorkingLines) {
			displayLine(l, WorkingColor);
		}
		for (auto l : SecondWorkingLines) {
			displayLine(l, SecondaryWorkingColor);
		}
		for (auto l : ErrorLines) {
			displayLine(l, ErrorColor);
		}
		window.display();
		std::this_thread::sleep_for(std::chrono::milliseconds(animationStepTime));
	}

	void AddDefaultPoint(Point point) { DefaultPoints.push_back(point); }
	void AddDefaultPoints(std::vector<Point> points) { DefaultPoints=points; }
	void ClearDefaultPoints() { DefaultPoints.clear(); }
	void AddErrorPoint(Point point) { ErrorPoints.push_back(point); }
	void AddErrorPoints(std::vector<Point> points) { ErrorPoints = points; }
	void ClearErrorPoints() { ErrorPoints.clear(); }
	void AddCorrectPoint(Point point) { CorrectPoints.push_back(point); }
	void AddCorrectPoints(std::vector<Point> points) { CorrectPoints=points; }
	void ClearCorrectPoints() { CorrectPoints.clear(); }

	void AddDefaultHull(Hull hull) { DefaultHulls.push_back(hull); }
	void ClearDefaultHulls() { DefaultHulls.clear(); }
	void AddCorrectHull(Hull hull) { CorrectHulls.push_back(hull); }
	void ClearCorrectHulls() { CorrectHulls.clear(); }
	void AddErrorHull(Hull hull) { ErrorHulls.push_back(hull); }
	void ClearErrorHulls() { ErrorHulls.clear(); }
	void AddDefaultLine(Line line) { DefaultLines.push_back(line); }
	void ClearDefaultLine() { DefaultLines.clear(); }
	void AddWorkingLine(Line line) { WorkingLines.push_back(line); }
	void ClearWorkingLines() { WorkingLines.clear(); }
	void AddSecondWorkingLine(Line line) { SecondWorkingLines.push_back(line); }
	void ClearSecondWorkingLines() { SecondWorkingLines.clear(); }
	void AddErrorLine(Line line) { ErrorLines.push_back(line); }
	void ClearErrorLines() { ErrorLines.clear(); }

	void ClearAll() {
		ClearDefaultPoints();
		ClearErrorPoints();
		ClearCorrectPoints();
		ClearDefaultHulls();
		ClearCorrectHulls();
		ClearErrorHulls();
		ClearDefaultLine();
		ClearWorkingLines();
		ClearSecondWorkingLines();
		ClearErrorLines();
	}

private:

	sf::RenderWindow& window;
	const int animationStepTime = 1000;

	//colors
	const sf::Color DefaultColor = sf::Color::Black;
	const sf::Color ErrorColor = sf::Color::Red;
	const sf::Color CorrectColor = sf::Color::Green;
	const sf::Color WorkingColor = sf::Color::Blue;
	const sf::Color SecondaryWorkingColor = sf::Color::Cyan;

	std::vector<Point> DefaultPoints;
	std::vector<Point> ErrorPoints;
	std::vector<Point> CorrectPoints;
	std::vector<Hull> DefaultHulls;
	std::vector<Hull> CorrectHulls;
	std::vector<Hull> ErrorHulls;
	std::vector<Line> DefaultLines;
	std::vector<Line> WorkingLines;
	std::vector<Line> SecondWorkingLines;
	std::vector<Line> ErrorLines;



#pragma region Display Functions
	void displayPoint(const Point& p, const sf::Color& color) {
		sf::CircleShape point(POINTRADIUS);
		point.setFillColor(color);
		point.setPosition(p.X - POINTRADIUS, p.Y - POINTRADIUS);
		window.draw(point);
	}
	void displayHull(const Hull& hull, const sf::Color& color)
	{
		auto currentNode = hull.left;
		if (currentNode != nullptr) {
			do {
				displayLine(Line(currentNode, currentNode->clockwiseNext), color);
				currentNode = currentNode->clockwiseNext;
			} while (currentNode != hull.left);
		}
	}
	void displayLine(const Line& line, const sf::Color& color)
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
};



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
Hull calculateVisualHull(std::vector<Point>& points, Scene& scene);
Hull mergeVisual(Hull left, Hull right, Scene& scene);
Line visualFindLowerTangentOfHulls(Hull left, Hull right, Scene& scene);
Line visualFindUpperTangentOfHulls(Hull left, Hull right, Scene& scene);
bool visualIsUpperTangentOfHull(Line tangent, Hull hull, Scene& scene);

//Utility functions
bool isPointLeftOfLine(Line line, Node* point);

//variable that should be read from commandline arguments
bool visualMode = true;
std::string filePath = "";
int pointAmount = 10;

//main to test merge
/*
int main()
{
	Point a1(100, 100);
	Point a2(150, 50);
	Point a3(200, 100);
	Point a4(150, 150);

	std::vector<Point> points; 
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

	Scene scene(window);
	
	scene.AddDefaultPoints(points);

	auto newHull = mergeVisual(aHull, bHull, scene);
	//auto newHull = merge(aHull, bHull);

	scene.ClearAll();
	scene.AddCorrectHull(newHull);
	scene.AddDefaultPoints(points);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		scene.Render();
	}
}
*/


int main()
{
	int parameterExit = handleParameters();
	if (parameterExit != 0) {
		return parameterExit;
	}

	std::vector<Point> points = generatePoints(filePath);
	
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Divide and Conquer");
	Scene scene(window);
	Hull hull;
	if (visualMode) {
		hull = calculateVisualHull(points, scene);
		scene.ClearAll();
	}
	else {
		auto start = std::chrono::high_resolution_clock::now();
		hull = calculateHull(points);
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << "Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) << " microseconds" << std::endl;
	}
	scene.AddDefaultPoints(points);
	scene.AddCorrectHull(hull);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear(sf::Color::White);

		scene.Render();
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
			lowerTangent.second = lowerTangent.second->counterclockNext;
			isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
		}
		isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);

		while (!isLowerTangentOfRight) {
			lowerTangent.first = lowerTangent.first->clockwiseNext;
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
			upperTangent.first = upperTangent.first->clockwiseNext;
			isUpperTangentOfLeft = isUpperTangentOfHull(upperTangent, left);
		}
		isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
		while (!isUpperTangentOfRight) {
			upperTangent.second = upperTangent.second->counterclockNext;
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

#pragma region Visual Hull Calculation
Hull calculateVisualHull(std::vector<Point>& points, Scene& scene)
{
	//TODO: calculate Hull
	Hull hull;
	return hull;
}

Hull mergeVisual(Hull left, Hull right, Scene& scene) {
	scene.AddCorrectHull(left);
	scene.AddCorrectHull(right);
	scene.Render();

	Hull newHull;
	auto upperTangent = visualFindUpperTangentOfHulls(left, right, scene);
	
	scene.AddWorkingLine(upperTangent);
	scene.Render();

	auto lowerTangent = visualFindLowerTangentOfHulls(left, right, scene);

	scene.AddWorkingLine(lowerTangent);
	scene.Render();

	newHull.left = left.left;
	newHull.right = right.right;

	scene.ClearCorrectHulls();
	scene.AddErrorHull(left);
	scene.AddErrorHull(right);
	scene.Render();

	//TODO: Delete Points that get removed
	upperTangent.first->counterclockNext = upperTangent.second;
	upperTangent.second->clockwiseNext = upperTangent.first; 
	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;

	scene.ClearErrorHulls();
	scene.ClearWorkingLines();
	scene.AddCorrectHull(newHull);
	scene.Render();
	return newHull;
}

Line visualFindLowerTangentOfHulls(Hull left, Hull right, Scene& scene) {
	Line lowerTangent(right.left, left.right);

	scene.ClearSecondWorkingLines();
	scene.AddSecondWorkingLine(lowerTangent);
	scene.Render();


	bool isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, scene);
	bool isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, scene);
	scene.Render();
	scene.ClearErrorPoints();
	scene.ClearCorrectPoints();

	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {
		while (!isLowerTangentOfLeft) {
			
			lowerTangent.second = lowerTangent.second->counterclockNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(lowerTangent);
			scene.Render();

			isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();
		}

		isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();

		while (!isLowerTangentOfRight) {
			lowerTangent.first = lowerTangent.first->clockwiseNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(lowerTangent);
			scene.Render();

			isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();
		}

		isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();
	}

	scene.ClearSecondWorkingLines();
	return lowerTangent;
}

Line visualFindUpperTangentOfHulls(Hull left, Hull right, Scene& scene) {
	Line upperTangent(left.right, right.left);

	scene.ClearSecondWorkingLines();
	scene.AddSecondWorkingLine(upperTangent);
	scene.Render();

	bool isUpperTangentOfLeft = visualIsUpperTangentOfHull(upperTangent, left, scene);
	bool isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, scene);
	scene.Render();
	scene.ClearErrorPoints();
	scene.ClearCorrectPoints();
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {
		while (!isUpperTangentOfLeft) {
			upperTangent.first = upperTangent.first->clockwiseNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(upperTangent);
			scene.Render();

			isUpperTangentOfLeft = visualIsUpperTangentOfHull(upperTangent, left, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();

		}

		isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();

		while (!isUpperTangentOfRight) {
			upperTangent.second = upperTangent.second->counterclockNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(upperTangent);
			scene.Render();

			isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();
		}
		isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, left, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();
	}

	scene.ClearSecondWorkingLines();
	return upperTangent;
}

bool visualIsUpperTangentOfHull(Line tangent, Hull hull, Scene& scene) {
	auto currentPoint = hull.left;
	bool upper = true;
	do {
		if (isPointLeftOfLine(tangent, currentPoint)) {
			scene.AddErrorPoint(currentPoint->point);
			upper = false;
		}
		else {
			scene.AddCorrectPoint(currentPoint->point);
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

