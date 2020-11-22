#include <SFML/Graphics.hpp>
#include<iostream>
#include<chrono>

#include "datastructures.h"
#include "scene.h"

#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGTH 600

// Delimiter that defines the position of a point in a file (e.g. 10,20).
#define DATA_DELIMITER ','

enum class ArgumentType { LOAD, POINT_AMOUNT, VISUAL_MODE, HELP };

std::map<std::string, ArgumentType> argumentMap{
	{"--load", ArgumentType::LOAD},
	{"--points", ArgumentType::POINT_AMOUNT},
	{"-p", ArgumentType::POINT_AMOUNT},
	{"--visual", ArgumentType::VISUAL_MODE},
	{"-v", ArgumentType::VISUAL_MODE},
	{"--help", ArgumentType::HELP},
};


#pragma region Function declarations
//Setup functions
void handleArguments(int argc, char* argv[]);
void showWrongArguments();
void showHelp();
std::vector<Point> generatePointsFromFile(const std::string& filePath);
std::vector<Point> generateRandomPoints(unsigned int amount);
std::vector<Point> generateRandomPointsOnCircle(unsigned int amount);

//Optimized calculation functions
Hull calculateHull(std::vector<Point>& points);
Hull generateSmallestHull(std::vector<Point>& points);
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
#pragma endregion

bool visualMode = false;
//std::string filePath = "../Testcases/TwoTriangles.txt";
std::string filePath = "";
int pointAmount = 100;


int main(int argc, char* argv[])
{
	handleArguments(argc, argv);

	std::vector<Point> points;
	if (filePath != "")
		points = generatePointsFromFile(filePath);
	else
	{
		points = generateRandomPoints(pointAmount);
		// points = generateRandomPointsOnCircle(pointAmount);
	}

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGTH), "Divide and Conquer");
	Scene scene(window);
	Hull hull;
	if (visualMode) {
		hull = calculateVisualHull(points, scene);
		scene.ClearAll();
	}
	else {
		auto start = std::chrono::high_resolution_clock::now();
		//TODO: Better sorting algorithm (quicksort?)
		// Sort points by their x coordinates.
		std::sort(points.begin(), points.end(), [](Point& a, Point& b) { return a.X < b.X; });
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

	//TODO: memory cleanup
}
#pragma region Argument Handling
void handleArguments(int argc, char* argv[])
{
	// Skip first argument (it's the .exe).
	for (unsigned int i = 1; i < argc; i++)
	{
		std::string argType = std::string(argv[i]);
		std::string argData = i + 1 < argc ? std::string(argv[i + 1]) : std::string();

		switch (argumentMap[argType])
		{
		case ArgumentType::LOAD:
			if (argData.empty())
				showWrongArguments();
			filePath = argData;
			i++;
			break;
		case ArgumentType::POINT_AMOUNT:
			if (argData.empty())
				showWrongArguments();
			pointAmount = std::stoi(argData);
			i++;
			break;
		case ArgumentType::VISUAL_MODE:
			visualMode = true;
			break;
		case ArgumentType::HELP:
			showHelp();
			std::exit(0);
			break;
		default:
			showWrongArguments();
			break;
		}
	}
}

void showWrongArguments()
{
	std::cerr << "Wrong arguments. Use arguments as following:" << std::endl;
	showHelp();
	std::exit(1);
}

void showHelp()
{
	std::cout << "--load <file> \t\t\t-> Filename to read from." << std::endl;
	std::cout << "--points [-p] <numberOfPoints> \t-> Number of random points to be generated." << std::endl;
	std::cout << "--visual [-v] \t\t\t-> Show visualization." << std::endl;
	std::cout << "--help \t\t\t\t-> Prints out this message." << std::endl;
}
#pragma endregion

#pragma region Setup Functions
std::vector<Point> generatePointsFromFile(const std::string& filePath)
{
	// Generate points from file.
	std::vector<Point> points;
	if (filePath != "") {
		std::ifstream input;
		input.open(filePath);
		if (!input.is_open()) {
			std::cerr << "Could not open file. Please check the path." << std::endl;
			std::exit(1);
		}
		// Go through each line and get x and y coordinates.
		while (input.peek() != EOF) {
			std::string xAsString, yAsString;
			std::getline(input, xAsString, DATA_DELIMITER);
			std::getline(input, yAsString);
			points.push_back(Point(std::stof(xAsString), std::stof(yAsString)));
		}
	}
	return points;
}
std::vector<Point> generateRandomPoints(unsigned int amount)
{
	std::vector<Point> points;
	for (int i = 0; i < amount; i++) {
		//TODO: use better rand function
		Point point(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGTH);
		points.push_back(point);
	}
	return points;
}
std::vector<Point> generateRandomPointsOnCircle(unsigned int amount)
{
	std::vector<Point> points;
	float radius = 200;
	float periphery = M_PI * 2;
	float steps = periphery / amount;
	Point center = Point(WINDOW_WIDTH / 2, WINDOW_HEIGTH / 2);
	for (float angle = 0; angle <= periphery; angle += steps) {
		Point point(center.X + radius * cos(angle), center.Y + radius * sin(angle));
		points.push_back(point);
	}
	return points;
}
#pragma endregion

#pragma region Optimized Hull Calculation
Hull calculateHull(std::vector<Point>& points)
{
	unsigned int half = points.size() / 2;
	std::vector<Point> left(points.begin(), points.begin() + half);
	std::vector<Point> right(points.begin() + half, points.end());

	Hull leftHull;
	Hull rightHull;

	// Split until each vector has 3 or 2 values.
	if (left.size() > 3)
		leftHull = calculateHull(left);
	else
		leftHull = generateSmallestHull(left);

	if (right.size() > 3)
		rightHull = calculateHull(right);
	else
		rightHull = generateSmallestHull(right);

	Hull hull = merge(leftHull, rightHull);
	return hull;
}

Hull generateSmallestHull(std::vector<Point>& points)
{
	Hull hull = Hull();
	if (points.size() == 2) {
		Node* left = new Node(points[0]);
		Node* right = new Node(points[1]);
		left->clockwiseNext = right;
		left->counterclockNext = right;
		right->clockwiseNext = left;
		right->counterclockNext = left;
		hull.left = left;
		hull.right = right;
	}
	else {
		Node* left = new Node(points[0]);
		Node* middle = new Node(points[1]);
		Node* right = new Node(points[2]);

		// Smaller because we start drawing in the top-left corner!
		bool middleIsUp = middle->point.Y < right->point.Y;
		left->clockwiseNext = middleIsUp ? middle : right;
		left->counterclockNext = middleIsUp ? right : middle;
		middle->clockwiseNext = middleIsUp ? right : left;
		middle->counterclockNext = middleIsUp ? left : right;
		right->clockwiseNext = middleIsUp ? left : middle;
		right->counterclockNext = middleIsUp ? middle : left;

		hull.left = left;
		hull.right = right;
	}
	return hull;
}

Hull merge(Hull left, Hull right)
{
	//finding both tangents
	auto upperTangent = findUpperTangentOfHulls(left, right);
	auto lowerTangent = findLowerTangentOfHulls(left, right);

	//deleting nodes that are not connected to the new hull anymore
	auto currentNode = upperTangent.first->counterclockNext;
	while (currentNode != lowerTangent.second) {
		auto deleteNode = currentNode;
		currentNode = currentNode->counterclockNext;
		delete deleteNode;
	}
	currentNode = lowerTangent.first->counterclockNext;
	while (currentNode != upperTangent.second) {
		auto deleteNode = currentNode;
		currentNode = currentNode->counterclockNext;
		delete deleteNode;
	}

	//connecting points on both tangents to each other, so the hull is correctly connected
	upperTangent.first->counterclockNext = upperTangent.second;
	upperTangent.second->clockwiseNext = upperTangent.first;

	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;

	//creating a new hull and setting left and right to corresponding values of previous hulls, so we dont have to sort again
	Hull newHull;

	newHull.left = left.left;
	newHull.right = right.right;

	return newHull;
}

Line findLowerTangentOfHulls(Hull left, Hull right) {
	//because we still use the method to check if a line is the upper tangent of the hull, we have to switch the direction of the tangent
	Line lowerTangent(right.left, left.right);
	//checking if the line is already viable for both hull
	bool isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
	bool isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);
	//while it is not viable for both hulls continue trying different points
	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {
		//while it is not viable for the left hull
		while (!isLowerTangentOfLeft) {
			//move the point on the left hull on step further
			lowerTangent.second = lowerTangent.second->counterclockNext;
			//and check again
			isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
		}
		//is new line tangent of right hull
		isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);
		//if its not
		while (!isLowerTangentOfRight) {
			//continue moving point on right hull
			lowerTangent.first = lowerTangent.first->clockwiseNext;
			//and rechecking it until the line is viable
			isLowerTangentOfRight = isUpperTangentOfHull(lowerTangent, right);
		}
		//is new line still viable for the left hull
		isLowerTangentOfLeft = isUpperTangentOfHull(lowerTangent, left);
	}
	//if it is were done
	return lowerTangent;
}

Line findUpperTangentOfHulls(Hull left, Hull right) {
	Line upperTangent(left.right, right.left);
	//checking if the line is already viable for both hull
	bool isUpperTangentOfLeft = isUpperTangentOfHull(upperTangent, left);
	bool isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
	//while it is not viable for both hulls continue trying different points
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {
		//while it is not viable for the left hull
		while (!isUpperTangentOfLeft) {
			//move the point on the left hull on step further
			upperTangent.first = upperTangent.first->clockwiseNext;
			//and check again
			isUpperTangentOfLeft = isUpperTangentOfHull(upperTangent, left);
		}
		//is new line tangent of right hull
		isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
		//if its not
		while (!isUpperTangentOfRight) {
			//continue moving point on right hull
			upperTangent.second = upperTangent.second->counterclockNext;
			//and rechecking it until the line is viable
			isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, right);
		}
		//is new line still viable for the left hull
		isUpperTangentOfLeft = isUpperTangentOfHull(upperTangent, left);
	}
	//if it is were done
	return upperTangent;
}

bool isUpperTangentOfHull(Line tangent, Hull hull) {
	auto currentPoint = hull.left;
	//iterating over the whole hull
	do {
		//if a point is left of the line, life cannot be upper tangent of whole whole
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


	//finding both tangents

	auto upperTangent = visualFindUpperTangentOfHulls(left, right, scene);
	scene.AddWorkingLine(upperTangent);
	scene.Render();

	auto lowerTangent = visualFindLowerTangentOfHulls(left, right, scene);
	scene.AddWorkingLine(lowerTangent);
	scene.Render();

	scene.ClearCorrectHulls();
	scene.AddErrorHull(left);
	scene.AddErrorHull(right);
	scene.Render();

	//deleting nodes that are not connected to the new hull anymore
	auto currentNode = upperTangent.first->counterclockNext;
	while (currentNode != lowerTangent.second) {
		auto deleteNode = currentNode;
		currentNode = currentNode->counterclockNext;
		delete deleteNode;
	}
	currentNode = lowerTangent.first->counterclockNext;
	while (currentNode != upperTangent.second) {
		auto deleteNode = currentNode;
		currentNode = currentNode->counterclockNext;
		delete deleteNode;
	}

	//connecting points on both tangents to each other, so the hull is correctly connected
	upperTangent.first->counterclockNext = upperTangent.second;
	upperTangent.second->clockwiseNext = upperTangent.first;

	lowerTangent.first->counterclockNext = lowerTangent.second;
	lowerTangent.second->clockwiseNext = lowerTangent.first;

	//creating a new hull and setting left and right to corresponding values of previous hulls, so we dont have to sort again
	Hull newHull;

	newHull.left = left.left;
	newHull.right = right.right;

	scene.ClearErrorHulls();
	scene.ClearWorkingLines();
	scene.AddCorrectHull(newHull);
	scene.Render();
	return newHull;
}

Line visualFindLowerTangentOfHulls(Hull left, Hull right, Scene& scene) {

	//because we still use the method to check if a line is the upper tangent of the hull, we have to switch the direction of the tangent
	Line lowerTangent(right.left, left.right);

	scene.ClearSecondWorkingLines();
	scene.AddSecondWorkingLine(lowerTangent);
	scene.Render();

	//checking if the line is already viable for both hull
	bool isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, scene);
	bool isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, scene);
	scene.Render();
	scene.ClearErrorPoints();
	scene.ClearCorrectPoints();
	//while it is not viable for both hulls continue trying different points
	while (!isLowerTangentOfLeft || !isLowerTangentOfRight) {
		//while it is not viable for the left hull
		while (!isLowerTangentOfLeft) {
			//move the point on the left hull on step further
			lowerTangent.second = lowerTangent.second->counterclockNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(lowerTangent);
			scene.Render();
			//and check again
			isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();
		}
		//is new line tangent of right hull
		isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();
		//if its not
		while (!isLowerTangentOfRight) {
			//continue moving point on right hull
			lowerTangent.first = lowerTangent.first->clockwiseNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(lowerTangent);
			scene.Render();
			//and rechecking it until the line is viable
			isLowerTangentOfRight = visualIsUpperTangentOfHull(lowerTangent, right, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();
		}
		//is new line still viable for the left hull
		isLowerTangentOfLeft = visualIsUpperTangentOfHull(lowerTangent, left, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();
	}

	scene.ClearSecondWorkingLines();
	//if it is were done
	return lowerTangent;
}

Line visualFindUpperTangentOfHulls(Hull left, Hull right, Scene& scene) {

	//starting with the line connecting the opposite extreme points of both hulls
	Line upperTangent(left.right, right.left);

	scene.ClearSecondWorkingLines();
	scene.AddSecondWorkingLine(upperTangent);
	scene.Render();

	//checking if the line is already viable for both hull
	bool isUpperTangentOfLeft = visualIsUpperTangentOfHull(upperTangent, left, scene);
	bool isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, scene);
	scene.Render();
	scene.ClearErrorPoints();
	scene.ClearCorrectPoints();
	//while it is not viable for both hulls continue trying different points
	while (!isUpperTangentOfLeft || !isUpperTangentOfRight) {
		//while it is not viable for the left hull
		while (!isUpperTangentOfLeft) {
			//move the point on the left hull on step further
			upperTangent.first = upperTangent.first->clockwiseNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(upperTangent);
			scene.Render();
			//and check again
			isUpperTangentOfLeft = visualIsUpperTangentOfHull(upperTangent, left, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();

		}

		//is new line tangent of right hull
		isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();
		//if its not
		while (!isUpperTangentOfRight) {
			//continue moving point on right hull
			upperTangent.second = upperTangent.second->counterclockNext;
			scene.ClearSecondWorkingLines();
			scene.AddSecondWorkingLine(upperTangent);
			scene.Render();
			//and rechecking it until the line is viable
			isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, right, scene);
			scene.Render();
			scene.ClearErrorPoints();
			scene.ClearCorrectPoints();
		}
		//is new line still viable for the left hull
		isUpperTangentOfRight = visualIsUpperTangentOfHull(upperTangent, left, scene);
		scene.Render();
		scene.ClearErrorPoints();
		scene.ClearCorrectPoints();
	}
	scene.ClearSecondWorkingLines();
	//if it is were done
	return upperTangent;
}

bool visualIsUpperTangentOfHull(Line tangent, Hull hull, Scene& scene) {

	auto currentPoint = hull.left;
	bool upper = true;
	//iterating over the whole hull
	do {
		//if a point is left of the line, life cannot be upper tangent of whole whole
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
	//if determinant is greater than zero, point is left of the line
	return ((b.X - a.X) * (p.Y - a.Y) - (b.Y - a.Y) * (p.X - a.X)) > 0;
}
#pragma endregion

