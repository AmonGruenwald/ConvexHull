#pragma once

#include <SFML/Graphics.hpp>
#include<thread>

#include "datastructures.h"

class Scene {
public:
	bool GoStepByStep = false;
	bool IsAnimating = false;

	Scene(sf::RenderWindow& window) :window(window) {};

	void Render() {
		handleInput();

		window.clear(sf::Color::White);
		for (auto p : DefaultPoints) {
			displayPoint(p, DefaultColor);
		}
		for (auto p : WorkingPoints) {
			displayPoint(p, WorkingColor);
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

		if (IsAnimating)
			std::this_thread::sleep_for(std::chrono::milliseconds(GoStepByStep ? stepByStepDelay : animationStepTime));
	}

	inline void AddDefaultPoint(Point point) { DefaultPoints.push_back(point); }
	inline void AddDefaultPoints(std::vector<Point> points) { DefaultPoints = points; }
	inline void ClearDefaultPoints() { DefaultPoints.clear(); }
	inline void AddErrorPoint(Point point) { ErrorPoints.push_back(point); }
	inline void AddErrorPoints(std::vector<Point> points) { ErrorPoints = points; }
	inline void ClearErrorPoints() { ErrorPoints.clear(); }
	inline void AddCorrectPoint(Point point) { CorrectPoints.push_back(point); }
	inline void AddCorrectPoints(std::vector<Point> points) { CorrectPoints = points; }
	inline void ClearCorrectPoints() { CorrectPoints.clear(); }
	inline void AddWorkingPoint(Point point) { WorkingPoints.push_back(point); }
	inline void AddWorkingPoints(std::vector<Point> points) { WorkingPoints = points; }
	inline void ClearWorkingPoints() { WorkingPoints.clear(); }

	inline void AddDefaultHull(Hull hull) { DefaultHulls.push_back(hull); }
	inline void ClearDefaultHulls() { DefaultHulls.clear(); }
	inline void AddCorrectHull(Hull hull) { CorrectHulls.push_back(hull); }
	inline void ClearCorrectHulls() { CorrectHulls.clear(); }
	inline void AddErrorHull(Hull hull) { ErrorHulls.push_back(hull); }
	inline void ClearErrorHulls() { ErrorHulls.clear(); }

	inline void AddDefaultLine(Line line) { DefaultLines.push_back(line); }
	inline void ClearDefaultLines() { DefaultLines.clear(); }
	inline void AddWorkingLine(Line line) { WorkingLines.push_back(line); }
	inline void ClearWorkingLines() { WorkingLines.clear(); }
	inline void AddSecondWorkingLine(Line line) { SecondWorkingLines.push_back(line); }
	inline void ClearSecondWorkingLines() { SecondWorkingLines.clear(); }
	inline void AddErrorLine(Line line) { ErrorLines.push_back(line); }
	inline void ClearErrorLines() { ErrorLines.clear(); }

	void ClearAll() {
		ClearDefaultPoints();
		ClearErrorPoints();
		ClearCorrectPoints();
		ClearWorkingPoints();
		ClearDefaultHulls();
		ClearCorrectHulls();
		ClearErrorHulls();
		ClearDefaultLines();
		ClearWorkingLines();
		ClearSecondWorkingLines();
		ClearErrorLines();
	}

	void ClearAllExtras() {
		ClearErrorPoints();
		ClearCorrectPoints();
		ClearWorkingPoints();
		ClearCorrectHulls();
		ClearErrorHulls();
		ClearWorkingLines();
		ClearSecondWorkingLines();
		ClearErrorLines();
	}

private:

	sf::RenderWindow& window;
	const int animationStepTime = 500;
	const int stepByStepDelay = 300;
	const float pointRadius = 3;

	const sf::Color DefaultColor = sf::Color::Black;
	const sf::Color ErrorColor = sf::Color::Red;
	const sf::Color CorrectColor = sf::Color::Green;
	const sf::Color WorkingColor = sf::Color::Blue;
	const sf::Color SecondaryWorkingColor = sf::Color::Cyan;

	std::vector<Point> DefaultPoints;
	std::vector<Point> ErrorPoints;
	std::vector<Point> CorrectPoints;
	std::vector<Point> WorkingPoints;
	std::vector<Hull> DefaultHulls;
	std::vector<Hull> CorrectHulls;
	std::vector<Hull> ErrorHulls;
	std::vector<Line> DefaultLines;
	std::vector<Line> WorkingLines;
	std::vector<Line> SecondWorkingLines;
	std::vector<Line> ErrorLines;

	void handleInput() {
		// Activate animation.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			GoStepByStep = false;
		// Deactivate animation -> enable step by step.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			GoStepByStep = true;
		// Fast fowward to end.
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
			IsAnimating = false;

		while (IsAnimating && GoStepByStep && !sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			// Activate animation.
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				GoStepByStep = false;
			// Deactivate animation -> enable step by step.
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				GoStepByStep = true;
			// Fast fowward to end.
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
				IsAnimating = false;
		}
	}

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