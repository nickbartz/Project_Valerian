#pragma once

// DISPLAY CONSTANTS
// Store constants having to do with the display on the screen
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int MAX_NUM_PRIMITIVES = 100;
const int MAX_NUM_TEXT_PRINT = 100;


// MESSAGE BUS CONSTANTS
// Store constants having to do with the message bus
const int MAX_INPUT_MESSAGES = 100;

// Message type enums
enum message_types
{
	MESSAGE_TYPE_INPUT
};

// UI CONSTANTS
const int UI_MAX_INPUT_MESSAGES = 100;
const int UI_MAX_CONSOLE_MESSAGES = 200;
const int UI_CONSOLE_MESSAGE_LIFETIME = 100;
const int UI_MAX_WINDOW_HEADERS = 5;
const int UI_MAX_CONSOLE_PANELS = 10;
const int UI_MAX_NUM_PANEL_COMPONENTS = 10;

enum mouse_buttons
{
	MOUSE_LEFT,
	MOUSE_MIDDLE,
	MOUSE_RIGHT
};

enum window_names
{
	WINDOW_NULL,
	WINDOW_PLAYER_DIAGNOSTIC
};

enum panel_names
{
	PANEL_NULL,
	PANEL_WORLD_INVENTORY,
	PANEL_MESSAGE_STREAM
};

enum button_actions
{
	BUTTON_ACTION_DO_NOTHING,
	BUTTON_ACTION_DRAG_WINDOW,
	BUTTON_ACTION_SWITCH_PANEL,
	BUTTON_ACTION_CLOSE_WINDOW
};

// DRAW CONFIGS
// Draw Constants
const int MAX_NUM_FONTS = 10;

// Draw Enums
enum font_types
{
	FONT_DEFAULT,
	FONT_LARGE,
	FONT_LARGE_BOLD,
	FONT_SMALL
};