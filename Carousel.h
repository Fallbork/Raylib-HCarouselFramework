#pragma once
#include <vector>
#include <raylib.h>

struct CarouselButton {
	int x;						// X position
	static int y;				// Global: Y position shared between all buttons
	static int w;				// Global: button width
	static int h;				// Global: button height
	int y_offset;				// Used to move the button up as a visual feedback
	bool selected;				// Is it the currently selected one?
	bool hovering;				// Is the mouse hovering it?
	Color color;				// Just for showcase :P

	CarouselButton(int x) : x(x), y_offset(0), selected(false), hovering(false), color{ 230,230,230,255 } {
		// Parameterized constructor
	};

	void OnSelect();			// Called whenever the button reaches the center of the screen
	void OnHover();				// Called whenever the mouse hovers over the button
	void OnLeave();				// Called whenever the mouse leaves the button
	void OnClick();				// Called whenever you click the button
	void OnDeselect();			// Called whenever the button leaves the center of the screen
};

class Carousel {
	std::vector<CarouselButton> buttons;	// Array of buttons
	int padding;							// The amount of pixels betwwen each button
	unsigned int length;					// Total length of the carousel
	float prev_mouseX;						// To calculate the variation in movement (X2 - X1)
	Camera2D camera;						// Used for Mode2D

	void Recalculate();						// Updates all variables

public:

	Carousel();								// Default constructor: generates a single button

	// Game-loop functions
	void ImGui();
	void HandleInput();
	void Update();
	void Draw();
};
