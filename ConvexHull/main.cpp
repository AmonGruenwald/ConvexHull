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

//Setup functions
int handleParameters();
std::vector<Point> generatePoints(const std::string& filePath);

//Optimized calculation functions
std::vector<Point> calculateHull(std::vector<Point> & points);

//Visual calculation functions
void startVisualCalculation();
//Debug step to test proof of concept
void addNextPointToHull();
void FinishVisualCalculation();
void VisualCalculationFinished();
void (*nextVisualCalculationStep)();

//Display functions
void displayPoint(const Point& p , const sf::Color& color,  sf::RenderWindow& window);
void displayPoints(const std::vector<Point>& points, const  sf::Color& color,  sf::RenderWindow& window);
void displayHull(const std::vector<Point>& points, const sf::Color& color,  sf::RenderWindow& window);
void displayLine(const Point& a, const Point& b, const sf::Color& color, sf::RenderWindow& window);



//variable that should be read from commandline arguments
bool visualMode = true;
std::string filePath = "";
int pointAmount = 4;

//colors
const sf::Color HullColor = sf::Color::Green;
const sf::Color PointColor = sf::Color::Black;
const sf::Color ErrorLineColor = sf::Color::Red;
const sf::Color WorkingLineColor = sf::Color::Blue;

//global geometry information variables (need to be global because of visual steps)
std::vector<Point> hull;
std::vector<Point> points; 
//only used by visual calculation:
int currentIndex = 0;
std::vector<Point> workingLine;
std::vector<Point> errorLine;

int main()
{
	int parameterExit = handleParameters();
	if (parameterExit != 0) {
		return parameterExit;
	} 
	points = generatePoints(filePath);


	bool firstLoop = true;
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

		if (visualMode) {
			if (firstLoop) {
				startVisualCalculation(); 
				firstLoop = false;
			}
			nextVisualCalculationStep();
			displayHull(hull, HullColor, window);
			if (!workingLine.empty()) {
				displayLine(workingLine[0],workingLine[1],WorkingLineColor, window);
			}
			if (!errorLine	.empty()) {
				displayLine(errorLine[0], errorLine[1], ErrorLineColor, window);
			}
			displayPoints(points, PointColor, window);
			window.display();
			//TODO: remove magic number
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		else {
			if (firstLoop) {
				firstLoop = false;
				auto start = std::chrono::high_resolution_clock::now();
				hull = calculateHull(points);
				auto end = std::chrono::high_resolution_clock::now();
				std::cout << "Time: " << (std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) << " microseconds" << std::endl;
				
			}
			displayHull(hull, HullColor, window);
			displayPoints(points, PointColor, window);
			window.display();
		}
	}
	return 0;
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
std::vector<Point> calculateHull(std::vector<Point> & points)
{
	//TODO: calculate Hull
	return points;
}
#pragma endregion

#pragma region Visual Hull Calculation
void startVisualCalculation()
{
	//TODO: define actual steps
	nextVisualCalculationStep = &addNextPointToHull;
}

//Debug step to test proof of concept
void addNextPointToHull() {
	if (currentIndex < points.size()) {
		errorLine.clear();
		workingLine.clear();
		hull.push_back(points[currentIndex]);
		if (currentIndex < points.size() - 1) {
			workingLine.push_back(points[currentIndex]);
			workingLine.push_back(points[currentIndex + 1]);
		}
		currentIndex++;
	}
	else {
		nextVisualCalculationStep = &FinishVisualCalculation;
	}
}


void FinishVisualCalculation() {
	errorLine.clear();
	workingLine.clear();
	nextVisualCalculationStep = &VisualCalculationFinished;
}
void VisualCalculationFinished() {}
#pragma endregion

#pragma region Display Functions
void displayPoint(const Point& p, const sf::Color& color, sf::RenderWindow& window) {
	sf::CircleShape point(POINTRADIUS);
	point.setFillColor(color);
	point.setPosition(p.X-POINTRADIUS, p.Y - POINTRADIUS);
	window.draw(point);
}
void displayPoints(const std::vector<Point>& points, const  sf::Color& color, sf::RenderWindow& window)
{
	for (auto p : points) {
		displayPoint(p, color, window);
	}
}
void displayHull(const std::vector<Point>& points, const sf::Color& color, sf::RenderWindow& window)
{
	for (int i = 0; i < points.size() - 1; i++) {
		displayLine(points[i], points[i + 1], color, window);
	}
	displayLine(points[points.size() - 1], points[0], color, window);
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