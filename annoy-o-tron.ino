///////////////////////////////////////////////////////////////////////////////
// ESP32-S2 - USB Annoy-O-Tron
//
// Setup your ESP32-S2 to act as a USB keyboard/mouse and randomly fire events
// like:
//
// Toggling CAPSLOCK, CTRL + ALT + DEL, Right Click, ALT + TAB,
// Random Mouse movement, Random Arrow key, etc.
//
// Then plug your ESP into an unsuspecting victim's PC and wait for them to
// slowly go mad. Probabilities of events firing can be tweaked below in the
// loop() function.
///////////////////////////////////////////////////////////////////////////////

#include "USB.h"
#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

// https://www.arduino.cc/reference/en/libraries/mouse/
// https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
USBHIDMouse Mouse;
USBHIDKeyboard Keyboard;

// Startup state (can be toggle by pressing D0)
bool enabled      = true;
// How often to fire an 'event' in milliseconds
uint32_t interval = 10000;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Time of the last action
uint32_t last_action = 0;
byte lastButtonState = HIGH;

void setup() {
	Serial.begin(115200);
	delay(1000);

	// Enable pin 0 as a button
	pinMode(0, INPUT_PULLUP);
	// Setup the on board LED for "enable" light
	pinMode(15, OUTPUT);

	// initialize mouse/keyboard control:
	Mouse.begin();
	Keyboard.begin();
	USB.begin();
}

///////////////////////////////////////////////////////////////////////////////

void loop() {
	// Check to see if the button was pressed to enable/disable annoy-o-tron
	check_enable_toggle();

	// Are we due for another action
	bool past_interval = millis() - last_action > interval;

	if (past_interval) {

		// Check to see if we need to randomly fire an event or not.
		//
		// random(10)  has a 1/10  chance of firing
		// random(100) has a 1/100 chance of firing
		// random(500) has a 1/500 chance of firing
		// etc.
		if (!enabled) {
			Serial.println("Disabled");
		} else if (random(10) == 1) {
			rand_mouse_move();
		} else if (random(50) == 1) {
			random_arrow_key();
		} else if (random(100) == 1) {
			mouse_right_click();
		} else if (random(200) == 1) {
			toggle_capslock();
		} else if (random(500) == 1) {
			send_ctrl_alt_del();
		} else if (random(100) == 1) {
			send_alt_tab();
		} else if (random(1000) == 1) {
			send_screen_lock();
		} else {
			Serial.println("No action");
		}

		last_action = millis();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void mouse_right_click() {
	Serial.println("Mouse right click");

	Mouse.click(MOUSE_RIGHT);
}

void random_arrow_key() {
	int rand = random(4);

	if (rand == 0) {
		Keyboard.write(KEY_UP_ARROW);
		Serial.println("Up arrow");
	} else if (rand == 1) {
		Keyboard.write(KEY_DOWN_ARROW);
		Serial.println("Down arrow");
	} else if (rand == 2) {
		Keyboard.write(KEY_LEFT_ARROW);
		Serial.println("Left arrow");
	} else if (rand == 3) {
		Keyboard.write(KEY_RIGHT_ARROW);
		Serial.println("Right arrow");
	}
}

void send_screen_lock() {
	Serial.println("Screen lock: WIN + l");

	// Windows key
	Keyboard.press(KEY_LEFT_GUI);
	// Lowercase L
	Keyboard.press('l');
	Keyboard.releaseAll();
}

void toggle_capslock() {
	Serial.println("Toggling capslock");

	Keyboard.write(KEY_CAPS_LOCK);
}

// https://github.com/espressif/arduino-esp32/blob/master/libraries/USB/src/USBHIDKeyboard.h#L49
void send_ctrl_alt_del() {
	Serial.println("CTRL + ALT + DEL");

	Keyboard.press(KEY_LEFT_CTRL);
	Keyboard.press(KEY_LEFT_ALT);
	Keyboard.press(KEY_DELETE);
	Keyboard.releaseAll();
}

void send_alt_tab() {
	Serial.println("ALT + TAB");

	Keyboard.press(KEY_LEFT_ALT);
	Keyboard.press(KEY_TAB);
	Keyboard.releaseAll();
}

void check_enable_toggle() {
	int x = digitalRead(0);

	// Make sure we're not double tapping... we add a minor
	// threshold before we fire again
	static uint32_t last_press = 0;
	if (millis() - last_press < 75) {
		return;
	}

	// If the button has changed since last time we checked
	if (x != lastButtonState) {
		lastButtonState = x;
		last_press      = millis();

		// Button down
		if (x == LOW) {
			enabled = !enabled;

			if (enabled) {
				Serial.println("Enabling annoy-o-tron");
			} else {
				Serial.println("Disabling annoy-o-tron");
			}
		// Button up
		} else {
			//Serial.println("Button up");
		}
	}

	// Set the light to match the enabled state
	digitalWrite(15, enabled);
}

void rand_mouse_move() {
	// Delay between each mouse move... if we move on average 60 times
	// this works out to just about 1/10th of a second for the whole
	// move. Set this to 0 to be instantaneous
	uint8_t my_delay = 2;

	// Calculate how far we want to move the mouse in each direction
	int16_t x_dest = random(80) + 20;
	int16_t y_dest = random(80) + 20;
	float slope     = y_dest / (float)x_dest;

	//Serial.printf("0,0 to %d,%d = slope %0.3f\n", x_dest, y_dest, slope);

	// Previous x,y coordinates
	int16_t prev_x = 0;
	int16_t prev_y = 0;

	// Is the move up or left
	bool up   = random(2);
	bool left = random(2);

	// We calculate each X-coordinate and move the mouse accordingly
	// essentially one x-pixel at a time
	for (int i = 0; i < x_dest; i++) {
		float x = i;
		float y = slope * x;

		// If it's left or up we flip the sign to negative
		if (left) { x *= -1; }
		if (up)   { y *= -1; }

		// Calculate the difference in units this mini-move should be
		int8_t diff_x = int(x) - prev_x;
		int8_t diff_y = int(y) - prev_y;

		if (prev_x || prev_y) {
			Mouse.move(diff_x, diff_y);
			delay(my_delay);
		}

		// Store the x,y coordinates for next time
		prev_x = int(x);
		prev_y = int(y);
	}

	if (left) { x_dest *= -1; }
	if (up)   { y_dest *= -1; }

	Serial.printf("Moved mouse: %d,%d\r\n", x_dest, y_dest);

	return;
}
