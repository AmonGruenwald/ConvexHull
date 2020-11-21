#include <SFML/Graphics.hpp>
#include<iostream>
#include<chrono>

#include "datastructures.h"
#include "scene.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGTH 600

#pragma region Function declarations
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
#pragma endregion

//TODO: variables should be read from commandline arguments
bool visualMode = true;
std::string filePath = "";
int pointAmount = 10;


//main to test merge, remove before handing in
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

	//TODO: memory cleanup
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
		isUpperTangentOfRight = isUpperTangentOfHull(upperTangent, left);
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
	return ((b.X - a.X) * (p.Y - a.Y) - (b.Y - a.Y) * (p.X - a.X))>0;
}
#pragma endregion

