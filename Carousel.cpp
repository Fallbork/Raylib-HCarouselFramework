#include "Carousel.h"
#include <imgui.h>

#define IN_RANGE(low, x, high) (low <= x) && (x <= high)
#define CLAMP(low, x, high) (x < low ? low : x > high ? high : x)

using CB = CarouselButton;		// This is purely for convenience, really...

int CarouselButton::y = 0;		//Start on the center of the screen
int CarouselButton::h = 48;
int CarouselButton::w = 64;

void CarouselButton::OnSelect() {
	color = { 127, 0, 255, 255 };		// Set special color
	y_offset = -CB::h / 4;				// Offset the button up by 1/4th of its own height
}

void CarouselButton::OnHover() {
	color = { 255, 255, 255, 255 };		// Highlight
}

void CarouselButton::OnLeave() {
	color = { 230, 230, 230, 255 };		// Revert to original color
}

static float mid_point;

void CarouselButton::OnClick() {
	mid_point = x + w / 2.0f;				// Sketchy as hell
}

void CarouselButton::OnDeselect() {
	color = { 230, 230, 230, 255 };		// Revert to original color
	y_offset = 0;						// Offset the button back to its original height
}

void Carousel::Recalculate() {
	length = buttons.size() * (CB::w + padding) - padding;
	camera.target.x = CLAMP(0, camera.target.x, length);
	CB::y = CB::y + CB::h > GetScreenHeight() ? GetScreenHeight() - CB::h : CB::y;
	for (size_t i = 0; i < buttons.size(); i++) {
		buttons[i].x = i * (CB::w + padding);
	}
}

Carousel::Carousel() : camera({ { (float)GetScreenWidth() / 2, 0 }, { 0,0 }, 0.0f,  1.0f }), padding(3), length(0) {
	buttons.emplace_back(buttons.size() * (CB::w + padding));
	CB::y = GetScreenHeight() / 2;
	Recalculate();
}

void Carousel::ImGui() {
	using namespace ImGui;				// Convenience, once more...
	Begin("Carousel Options");
	if (Button("Add")) {
		buttons.emplace_back(buttons.size() * (CB::w + padding));
		Recalculate();
	};
	SameLine();
	if (Button("Remove")) {
		buttons.pop_back();
		Recalculate();
	};
	if (SliderInt("Padding", &padding, 1, 200)		||
		SliderInt("Button width", &CB::w, 10, 200)	||
		SliderInt("Button height", &CB::h, 10, 200)	||											// They all recalculate,
		SliderInt("Button y", &CB::y, 0, GetScreenHeight() - CB::h)) { Recalculate(); };		// so I chained them

	End();
}

void Carousel::HandleInput() {
	if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		const float mouse_offset = prev_mouseX - (GetMouseX() + camera.target.x - camera.offset.x);

		// Lock the offset camera between the length range
		camera.target.x = CLAMP(0, camera.target.x + mouse_offset, length);
	}
	for (CB& button : buttons) {
		if (IN_RANGE(camera.target.x - camera.offset.x - CB::w, button.x, camera.target.x + camera.offset.x) &&
			button.hovering && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
			button.OnClick();
			camera.target.x = mid_point;		// Sketchy as hell pt. 2: Electric Boogaloo :)))))))))))))000
		}
	}
	prev_mouseX = GetMouseX() + camera.target.x - camera.offset.x;
}

void Carousel::Update() {
	const Vector2 mouse_pos = { GetMouseX() + camera.target.x - camera.offset.x,
								GetMouseY() + camera.target.y - camera.offset.y };
	for (CB& button : buttons) {
		if (IN_RANGE(camera.target.x - camera.offset.x - CB::w, button.x, camera.target.x + camera.offset.x)) {
			if (IN_RANGE(button.x, camera.target.x, button.x + CB::w)) {
				if (!button.selected) {
					button.OnSelect();
					button.selected = true;
				}
				button.hovering = false;
			}
			else {
				if (button.selected) {
					button.OnDeselect();
					button.selected = false;
				}
				if (CheckCollisionPointRec(mouse_pos, { (float)button.x, (float)CB::y + button.y_offset, (float)CB::w, (float)CB::h })) {
					if (!button.hovering) {
						button.hovering = true;
						button.OnHover();
					}
				}
				else if (button.hovering) {
					button.OnLeave();
					button.hovering = false;
				}
			}
		}
	}
}

void Carousel::Draw() {
	BeginMode2D(camera);
	for(CB button : buttons) {
		if (IN_RANGE(camera.target.x - camera.offset.x - CB::w, button.x, camera.target.x + camera.offset.x)) {
			DrawRectangle(button.x, CB::y + button.y_offset, CB::w, CB::h, button.color);
		}
	}
	EndMode2D();
}
