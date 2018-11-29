#pragma once

// DISPLAY CONSTANTS
// Store constants having to do with the display on the screen

const int MAX_NUM_FONTS = 10;

// Draw Enums
enum font_types
{
	FONT_DEFAULT,
	FONT_DEFAULT_BOLD,
	FONT_LARGE,
	FONT_LARGE_BOLD,
	FONT_SMALL,
	FONT_SMALL_BOLD
};

enum PRIMITIVE_TYPES
{
	PRIMITIVE_TYPE_RECT,
	PRIMITIVE_TYPE_LINE,
	PRIMITIVE_TYPE_POINT
};

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

const int TILE_SIZE = 32;
const int SPRITE_SIZE = 32;

const int DISPLAY_MAX_ZOOM = 64;
const int DISPLAY_MIN_ZOOM = 16;

const int MAX_NUM_PRIMITIVES = 500;
const int MAX_NUM_TEXT_PRINT = 500;
const int MAX_SPRITE_INSTRUCTIONS = 4000;
const int MAX_SPRITES_PER_MULTISPRITE = 400;

enum spritesheets
{
	SPRITESHEET_NONE,
	SPRITESHEET_BACKGROUND,
	SPRITESHEET_BASE,
	SPRITESHEET_BASE_OVERLAY,
	SPRITESHEET_MID_1,
	SPRITESHEET_MID_1_OVERLAY,
	SPRITESHEET_MID_2,
	SPRITESHEET_ENTITY,
	SPRITESHEET_ROOF,
	SPRITESHEET_ICON,
	SPRITESHEET_PROJECTILE
};

enum draw_types
{
	DRAW_TYPE_SPRITESHEET,
	DRAW_TYPE_PRIMITIVE
};

enum draw_layers
{
	DRAW_LAYER_BACKGROUND,
	DRAW_LAYER_BASE,
	DRAW_LAYER_MID,
	DRAW_LAYER_MID_OVERLAY
};

enum draw_layer_stats
{
	DRAW_LAYER_STATE_UNUSED,
	DRAW_LAYER_STATE_ATROPHIED,
	DRAW_LAYER_STATE_DONT_DRAW,
	DRAW_LAYER_STATE_DRAW_AGAIN
};

// MESSAGE BUS CONSTANTS
// Store constants having to do with the message bus
const int MAX_INPUT_MESSAGES = 10;
const int MAX_CHATTER_MESSAGES = 10;
const int MAX_SG_TILE_UPDATE_MESSAGES = 1000;
const int MAX_LENGTH_CUSTOM_MESSAGE = 100;
const int MAX_NUM_CUSTOM_MESSAGES = 1000;

// Custom Message Type Enums
enum Custom_Message_Type
{
	MESSAGE_TYPE_NULL,
	MESSAGE_TYPE_STAT_UPDATE_REQUEST,
	MESSAGE_TYPE_SG_TILE_UPDATE_NOTIFICATION,
	MESSAGE_TYPE_SG_ENTITY_CREATE,
	MESSAGE_TYPE_SG_TILE_UPDATE_REQUEST,
	MESSAGE_TYPE_SG_ENTITY_MOVEMENT,
	MESSAGE_TYPE_SG_PROJECTILE_MOVEMENT,
	MESSAGE_TYPE_CURSOR_STATUS_UPDATE,
	MESSAGE_TYPE_SET_ENTITY_RALLY_POINT,
	MESSAGE_TYPE_ENTITY_JOB_REQUEST,
	MESSAGE_TYPE_SG_CONTAINER_AVAILABLE,
	MESSAGE_TYPE_SG_INVENTORY_MANIFEST_UPDATE
};

enum Stat_Update_Request_Type
{
	STAT_ADJUST,
	STAT_UPDATE
};

enum Custom_Message_Focus
{
	FOCUS_NULL,
	FOCUS_SPECIFC_OBJECT,
	FOCUS_RANGE,
	FOCUS_ALL
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

enum input_actions
{
	INPUT_NULL,
	INPUT_LEFT_MOUSE_CLICK,
	INPUT_LEFT_MOUSE_DRAG,
	INPUT_LEFT_MOUSE_UNCLICK,
	INPUT_RIGHT_MOUSE_CLICK,
	INPUT_RIGHT_MOUSE_DRAG,
	INPUT_RIGHT_MOUSE_UNCLICK
};

enum window_names
{
	WINDOW_NULL,
	WINDOW_PLAYER_DIAGNOSTIC,
	WINDOW_CREATE_STRUCTURE,
	WINDOW_SCREEN_BUTTONS,
	WINDOW_OBJECT_DIAGNOSTIC
};

enum panel_names
{
	PANEL_NULL,
	PANEL_MESSAGE_STREAM,
	PANEL_STRUCTURE_CREATE_WALL,
	PANEL_STRUCTURE_CREATE_FLOOR,
	PANEL_STRUCTURE_CREATE_MISC,
	PANEL_OBJECT_INVENTORY,
	PANEL_OBJECT_STATS,
	PANEL_OBJECT_JOBS,
	PANEL_OBJECT_PRODUCTION
};

enum button_actions
{
	BUTTON_ACTION_DO_NOTHING,
	BUTTON_ACTION_DRAG_WINDOW,
	BUTTON_ACTION_SWITCH_PANEL,
	BUTTON_ACTION_CLOSE_WINDOW
};

enum Action_Message_Category
{
	UI_ACTION_CATEGORY_NULL,
	UI_ACTION_CATEGORY_SUPPORTING,
	UI_ACTION_CATEGORY_MAIN
};

enum Main_Action_Message_Type
{
	UI_ACTION_MAIN_TYPE_NULL,
	UI_ACTION_MAIN_TYPE_CLICK_IN_WORLD
};

enum Supporting_Action_Message_Types
{
	UI_ACTION_SUPPORTING_TYPE_NULL,
	UI_ACTION_SUPPORTING_TYPE_SET_STRUCTURE_BUILD,
	UI_ACTION_SUPPORTING_TYPE_DELETE_STRUCTURE,
	UI_ACTION_SUPPORTING_TYPE_SET_RALLY_POINT,
	UI_ACTION_SUPPORTING_TYPE_SET_ASTEROID_MINE,
	UI_ACTION_SUPPORTING_TYPE_SET_BLUEPRINT_ID
};

// GAME LIBRARY CONSTANTS

const int MAX_NUM_TEMPLATES = 50;

// SCENE GRAPH CONSTANTS

const int WORLD_MAX_NUM_STRUCTURES = 1000;
const int WORLD_MAX_NUM_ENTITIES = 100;
const int WORLD_MAX_NUM_PROJECTILES = 1000;
const int WORLD_MAX_NUM_CONTAINERS = 100;
const int WORLD_MAX_NUM_SCAFFOLDS = 100;
const int WORLD_MAX_NUM_JOBS = 100;

// Each background object takes about 1 FPS off the total so adjust judiciously
const int WORLD_MAX_NUM_BACKGROUND_OBJECTS = 1000;

enum Assigned_Flag
{
	OBJECT_UNASSIGNED,
	OBJECT_ASSIGNED
};

enum Object_Types
{
	OBJECT_TYPE_NULL,
	OBJECT_TYPE_STRUCTURE,
	OBJECT_TYPE_SCAFFOLD,
	OBJECT_TYPE_ENTITY,
	OBJECT_TYPE_ITEM,
	OBJECT_TYPE_CONTAINER,
	OBJECT_TYPE_PROJECTILE,
	OBJECT_TYPE_ANY
};

enum Main_Object_Functions
{
	OBJECT_FUNCTION_DRAW,
	OBJECT_FUNCTION_UPDATE,
	OBJECT_FUNCTION_CHECK_FOR_MESSAGES
};

enum Tile_Layers
{
	TILE_LAYER_BACKGROUND,
	TILE_LAYER_BASE,
	TILE_LAYER_MID,
	TILE_LAYER_ROOF,
	TILE_LAYER_SCAFFOLD
};

enum Object_Components
{
	OBJECT_COMPONENT_STATS,
	OBJECT_COMPONENT_MOVEMENT,
	OBJECT_COMPONENT_ITEM,
	OBJECT_COMPONENT_RELATIONSHIP,
	OBJECT_COMPONENT_JOB
};

// AI Stats Enums

enum Structure_Types
{
	STRUCTURE_TYPE_NULL
};

enum Structure_Ids
{
	STRUCTURE_ID_NULL
};

enum Entity_ID
{
	ENTITY_ID_NULL
};

enum Entity_Genders
{
	ENTITY_MALE,
	ENTITY_FEMALE,
	ENTITY_NEUTRAL
};

const int MAX_STRUCTURE_STATS = 20;
const int MAX_ENTITY_STATS = 10;

enum Object_Stat_Enum
{
	STAT_UNIQ_ID,
	STAT_OBJECT_FACTION,
	STAT_OBJECT_HEALTH,
	STAT_STRUCTURE_NULL,
	STAT_STRUCTURE_BUILT_LEVEL,
	STAT_STRUCTURE_OXYGEN_LEVEL,
	STAT_STRUCTURE_POWERED,
	STAT_STRUCTURE_IMPASSABLE,
	STAT_ENTITY_HUNGER,
	STAT_ENTITY_TIREDNESS,
	STAT_ENTITY_OXYGEN,
	STAT_ENTITY_FEAR,
	STAT_ENTITY_SPEED,
	STAT_ENTITY_ENNUI,
	STAT_PROJECTILE_RANGE,
	STAT_CURRENT_X_TILE,
	STAT_CURRENT_Y_TILE
};

// Render Component Enums

enum Render_Component_Type
{
	RENDER_COMPONENT_NONE,
	RENDER_COMPONENT_BACKGROUND,
	RENDER_COMPONENT_SIMPLECLIP,
	RENDER_COMPONENT_MULTICLIP,
	RENDER_COMPONENT_ANIMATED_CLIP,
	RENDER_COMPONENT_ENTITY_SIMPLE,
	RENDER_COMPONENT_ENTITY_COMPLEX,
	RENDER_COMPONENT_POINT_RENDER
};

enum Multiclip_Type
{
	MULTICLIP_NONE,
	MULTI_CLIP_WALL,
	MULTI_CLIP_FLOOR
};

enum ENTITY_ANIMATIONS
{
	ANIM_WALK_DOWN,
	ANIM_WALK_UP,
	ANIM_WALK_LEFT,
	ANIM_WALK_RIGHT
};

enum Simple_Animations
{
	SIMPLE_ANIMATION_NULL,
	SIMPLE_ANIMATION_INCREMENT_UNTIL_COMPLETE,
	SIMPLE_ANIMATION_DECREMENT_UNTIL_COMPLETE,
	SIMPLE_ANIMATION_PAUSE,
	SIMPLE_ANIMATION_INCREMENT_AND_REPEAT,
	SIMPLE_ANIMATION_BACK_AND_FORTH
};

const int MAX_NUM_ANIMATIONS = 4;
const int MAX_NUM_COMPONENTS = 5;

// AI_JOB ENUMS

const int MAX_LENGTH_GOAL_INSTRUCTION = 20;
const int MAX_NUM_GOAL_INSTRUCTIONS = 20;
const int MAX_ENTITIES_PER_JOB = 20;

enum Job_Parser_Codes
{
	JOB_NULL,

	// JOB PARSER COMMANDS
	JOB_INST_START = 45678,		// TELLS THE PARSER TO START
	JOB_INST_NEW_GOAL,			// STARTS CREATING A NEW GOAL
	JOB_INST_END_GOAL,			// ADDS THE CURRENT GOAL TO THE GOAL ARRAY
	JOB_INST_END,				// TELLS PARSER TO STOP

	// JOB TYPES
	JOB_TYPE_MINING,
	JOB_TYPE_CONTAINER_PICKUP,

	// ACTION MAIN TYPES
	ACTION_NONE,
	ACTION_CREATE,
	ACTION_DESTROY,
	ACTION_EDIT_INTERNAL,
	ACTION_EDIT_EXTERNAL,
	ACTION_ASSESS_INTERNAL,
	ACTION_ASSESS_EXTERNAL,
	ACTION_TRANSFER,
	ACTION_GOAL_TRAVERSAL,
	ACTION_CLOSE_OUT_GOAL_SET,

	// ACTION SUBTYPES
	A_AE_NULL,
	A_AE_OXYGENATE,
	A_AE_CHECK_SIMPLE_DISTANCE,
	A_AE_CHECK_OBJECT_STAT,
	A_AE_CHECK_FOR_ENTITIES_IN_RADIUS,
	A_AE_CHECK_IF_OBJECT_HAS_ITEM,
	A_AI_CHECK_JOB_FOR_MORE_GOALSETS,
	A_EI_OPEN_DOOR,
	A_EI_STOP_MOVING,
	A_EI_WAIT,
	A_EI_SET_WAIT_TIMER,
	A_EI_CLOSE_DOOR,
	A_EI_PAUSE_FOR_FURTHER_INSTRUCTIONS,
	A_EI_SET_TARGET_COORD_TO_OBJECT,
	A_EE_TOGGLE_MINE_OBJECT,
	A_EE_FIRE_MINING_LASER_AT_STRUCTURE,
	A_AT_TAKE_ALL_INVENTORY_FROM_OBJECT,
	A_AT_TAKE_ITEM_FROM_OBJECT,
	A_AT_GIVE_ITEM_TO_OBJECT,
	A_EE_ADJUST_OBJECT_STAT,
	A_C_CREATE_MINING_JOB,

	// COMPARISON FUNCTIONS
	LOWER_THAN_OR_EQUAL_TO,
	HIGHER_THAN_OR_EQUAL_TO

};


// AI Inventory Constants

const int MAX_NUM_INVENTORY_SLOTS = 40;
const int MAX_NUM_EQUIPMENT_SLOTS = 10;
const int MAX_NUM_ITEM_BLUEPRINTS = 10;
const int MAX_ITEMS_PER_BLUEPRINT = 5;

enum Equipment_slots
{
	EQUIPMENT_SLOT_NULL,
	EQUIPMENT_SLOT_SPACESUIT
};

enum Blueprint_Types
{
	BLUEPRINT_NULL,
	BLUEPRINT_INVENTORY,
	BLUEPRINT_SCAFFOLD,
	BLUEPRINT_PRODUCTION
};


// AI Movement 

const int SEARCH_WIDTH = 100;
const int SEARCH_HEIGHT = 100;

// AI Relationship

enum object_relationship
{
	OBJECT_RELATIONSHIP_NULL,
	OBJECT_RELATIONSHIP_PROJECTILE
};