#pragma once
#include "Enigma/Core/Types.h"

// Key codes from GLFW

namespace Enigma {
	namespace Engine {
		namespace KeyCode {
			constexpr i32 KeySpace            = 32;
			constexpr i32 KeyApostrophe       = 39;  /* ' */
			constexpr i32 KeyComma			  = 44;  /* , */
			constexpr i32 KeyMinus            = 45;  /* - */
			constexpr i32 KeyPeriod           = 46;  /* . */
			constexpr i32 KeySlash            = 47;  /* / */
			constexpr i32 Key0                = 48;
			constexpr i32 Key1                = 49;
			constexpr i32 Key2                = 50;
			constexpr i32 Key3                = 51;
			constexpr i32 Key4                = 52;
			constexpr i32 Key5                = 53;
			constexpr i32 Key6                = 54;
			constexpr i32 Key7                = 55;
			constexpr i32 Key8                = 56;
			constexpr i32 Key9                = 57;
			constexpr i32 KeySemicolon        = 59;  /* ; */
			constexpr i32 KeyEqual            = 61;  /* = */
			constexpr i32 KeyA                = 65;
			constexpr i32 KeyB                = 66;
			constexpr i32 KeyC                = 67;
			constexpr i32 KeyD                = 68;
			constexpr i32 KeyE                = 69;
			constexpr i32 KeyF                = 70;
			constexpr i32 KeyG                = 71;
			constexpr i32 KeyH                = 72;
			constexpr i32 KeyI                = 73;
			constexpr i32 KeyJ                = 74;
			constexpr i32 KeyK                = 75;
			constexpr i32 KeyL                = 76;
			constexpr i32 KeyM                = 77;
			constexpr i32 KeyN                = 78;
			constexpr i32 KeyO                = 79;
			constexpr i32 KeyP                = 80;
			constexpr i32 KeyQ                = 81;
			constexpr i32 KeyR                = 82;
			constexpr i32 KeyS                = 83;
			constexpr i32 KeyT                = 84;
			constexpr i32 KeyU                = 85;
			constexpr i32 KeyV                = 86;
			constexpr i32 KeyW                = 87;
			constexpr i32 KeyX                = 88;
			constexpr i32 KeyY                = 89;
			constexpr i32 KeyZ                = 90;
			constexpr i32 KeyLeftBracket      = 91;  /* [ */
			constexpr i32 KeyBackslash        = 92;  /* \ */
			constexpr i32 KeyRightBracket     = 93;  /* ] */
			constexpr i32 KeyGraveAccent      = 96;  /* ` */
			constexpr i32 KeyWorld1			  = 161; /* non-US #1 */
			constexpr i32 KeyWorld2			  = 162; /* non-US #2 */

			/* Function keys */
			constexpr i32 KeyEscape           = 256;
			constexpr i32 KeyEnter            = 257;
			constexpr i32 KeyTab              = 258;
			constexpr i32 KeyBackspace        = 259;
			constexpr i32 KeyInsert           = 260;
			constexpr i32 KeyDelete           = 261;
			constexpr i32 KeyRight            = 262;
			constexpr i32 KeyLeft             = 263;
			constexpr i32 KeyDown             = 264;
			constexpr i32 KeyUp               = 265;
			constexpr i32 KeyPageUp           = 266;
			constexpr i32 KeyPageDown         = 267;
			constexpr i32 KeyHome             = 268;
			constexpr i32 KeyEnd              = 269;
			constexpr i32 KeyCapsLock         = 280;
			constexpr i32 KeyScrollLock       = 281;
			constexpr i32 KeyNumLock          = 282;
			constexpr i32 KeyPrintScreen      = 283;
			constexpr i32 KeyPause            = 284;
			constexpr i32 KeyF1               = 290;
			constexpr i32 KeyF2               = 291;
			constexpr i32 KeyF3               = 292;
			constexpr i32 KeyF4               = 293;
			constexpr i32 KeyF5               = 294;
			constexpr i32 KeyF6               = 295;
			constexpr i32 KeyF7               = 296;
			constexpr i32 KeyF8               = 297;
			constexpr i32 KeyF9               = 298;
			constexpr i32 KeyF10              = 299;
			constexpr i32 KeyF11              = 300;
			constexpr i32 KeyF12              = 301;
			constexpr i32 KeyF13              = 302;
			constexpr i32 KeyF14              = 303;
			constexpr i32 KeyF15              = 304;
			constexpr i32 KeyF16              = 305;
			constexpr i32 KeyF17              = 306;
			constexpr i32 KeyF18              = 307;
			constexpr i32 KeyF19              = 308;
			constexpr i32 KeyF20              = 309;
			constexpr i32 KeyF21              = 310;
			constexpr i32 KeyF22              = 311;
			constexpr i32 KeyF23              = 312;
			constexpr i32 KeyF24              = 313;
			constexpr i32 KeyF25              = 314;
			constexpr i32 KeyKP0              = 320;
			constexpr i32 KeyKP1              = 321;
			constexpr i32 KeyKP2              = 322;
			constexpr i32 KeyKP3              = 323;
			constexpr i32 KeyKP4              = 324;
			constexpr i32 KeyKP5              = 325;
			constexpr i32 KeyKP6              = 326;
			constexpr i32 KeyKP7              = 327;
			constexpr i32 KeyKP8              = 328;
			constexpr i32 KeyKP9              = 329;
			constexpr i32 KeyKPDecimal        = 330;
			constexpr i32 KeyKPDivide         = 331;
			constexpr i32 KeyKPMultiply       = 332;
			constexpr i32 KeyKPSubtract       = 333;
			constexpr i32 KeyKPAdd            = 334;
			constexpr i32 KeyKPEnter          = 335;
			constexpr i32 KeyKPEqual          = 336;
			constexpr i32 KeyLeftShift        = 340;
			constexpr i32 KeyLeftControl      = 341;
			constexpr i32 KeyLeftAlt          = 342;
			constexpr i32 KeyLeftSuper        = 343;
			constexpr i32 KeyRightShift       = 344;
			constexpr i32 KeyRightControl     = 345;
			constexpr i32 KeyRightAlt         = 346;
			constexpr i32 KeyRightSuper       = 347;
			constexpr i32 KeyMenu             = 348;
			constexpr i32 KeyLast             = KeyMenu;
					  
			constexpr i32 MouseButton1        = 0;
			constexpr i32 MouseButton2        = 1;
			constexpr i32 MouseButton3        = 2;
			constexpr i32 MouseButton4        = 3;
			constexpr i32 MouseButton5        = 4;
			constexpr i32 MouseButton6        = 5;
			constexpr i32 MouseButton7        = 6;
			constexpr i32 MouseButton8        = 7;
			constexpr i32 MouseButtonLast     = MouseButton8;
			constexpr i32 MouseButtonLeft     = MouseButton1;
			constexpr i32 MouseButtonRight    = MouseButton2;
			constexpr i32 MouseButtonMiddle   = MouseButton3;
					  
			constexpr i32 ShiftMask           = 1;
			constexpr i32 CtrlMask            = 2;
			constexpr i32 AltMask             = 4;
					  
			constexpr i32 KeyRelease          = 0;
			constexpr i32 KeyPress            = 1;
			constexpr i32 KeyRepeat           = 2;
		}
	}
}
