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
	int dirX = random(2);
	int dirY = random(2);

	// X direction
	int lenX = random(0, 40);
	if (dirX == 0) {
		lenX *= -1;
	}
	Mouse.move(0, lenX);

	// Y direction
	int lenY = random(0, 40);
	if (dirY == 0) {
		lenY *= -1;
	}
	Mouse.move(lenY, 0);

	Serial.printf("Moving mouse: %d,%d\r\n", lenX, lenY);
}
