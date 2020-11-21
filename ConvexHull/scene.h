#pragma once

#include <SFML/Graphics.hpp>
#include<thread>

#include "datastructures.h"

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
	void AddDefaultPoints(std::vector<Point> points) { DefaultPoints = points; }
	void ClearDefaultPoints() { DefaultPoints.clear(); }
	void AddErrorPoint(Point point) { ErrorPoints.push_back(point); }
	void AddErrorPoints(std::vector<Point> points) { ErrorPoints = points; }
	void ClearErrorPoints() { ErrorPoints.clear(); }
	void AddCorrectPoint(Point point) { CorrectPoints.push_back(point); }
	void AddCorrectPoints(std::vector<Point> points) { CorrectPoints = points; }
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
	const float pointRadius = 3;

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
		sf::CircleShape point(pointRadius);
		point.setFillColor(color);
		point.setPosition(p.X - pointRadius, p.Y - pointRadius);
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