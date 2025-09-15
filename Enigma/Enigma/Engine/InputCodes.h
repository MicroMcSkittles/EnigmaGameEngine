#pragma once

// Key codes from GLFW

namespace Enigma {
	namespace Engine {
		namespace KeyCode {
			constexpr int KeySpace            = 32;
			constexpr int KeyApostrophe       = 39;  /* ' */
			constexpr int KeyComma			  = 44;  /* , */
			constexpr int KeyMinus            = 45;  /* - */
			constexpr int KeyPeriod           = 46;  /* . */
			constexpr int KeySlash            = 47;  /* / */
			constexpr int Key0                = 48;
			constexpr int Key1                = 49;
			constexpr int Key2                = 50;
			constexpr int Key3                = 51;
			constexpr int Key4                = 52;
			constexpr int Key5                = 53;
			constexpr int Key6                = 54;
			constexpr int Key7                = 55;
			constexpr int Key8                = 56;
			constexpr int Key9                = 57;
			constexpr int KeySemicolon        = 59;  /* ; */
			constexpr int KeyEqual            = 61;  /* = */
			constexpr int KeyA                = 65;
			constexpr int KeyB                = 66;
			constexpr int KeyC                = 67;
			constexpr int KeyD                = 68;
			constexpr int KeyE                = 69;
			constexpr int KeyF                = 70;
			constexpr int KeyG                = 71;
			constexpr int KeyH                = 72;
			constexpr int KeyI                = 73;
			constexpr int KeyJ                = 74;
			constexpr int KeyK                = 75;
			constexpr int KeyL                = 76;
			constexpr int KeyM                = 77;
			constexpr int KeyN                = 78;
			constexpr int KeyO                = 79;
			constexpr int KeyP                = 80;
			constexpr int KeyQ                = 81;
			constexpr int KeyR                = 82;
			constexpr int KeyS                = 83;
			constexpr int KeyT                = 84;
			constexpr int KeyU                = 85;
			constexpr int KeyV                = 86;
			constexpr int KeyW                = 87;
			constexpr int KeyX                = 88;
			constexpr int KeyY                = 89;
			constexpr int KeyZ                = 90;
			constexpr int KeyLeftBracket      = 91;  /* [ */
			constexpr int KeyBackslash        = 92;  /* \ */
			constexpr int KeyRightBracket     = 93;  /* ] */
			constexpr int KeyGraveAccent      = 96;  /* ` */
			constexpr int KeyWorld1			  = 161; /* non-US #1 */
			constexpr int KeyWorld2			  = 162; /* non-US #2 */

			/* Function keys */
			constexpr int KeyEscape           = 256;
			constexpr int KeyEnter            = 257;
			constexpr int KeyTab              = 258;
			constexpr int KeyBackspace        = 259;
			constexpr int KeyInsert           = 260;
			constexpr int KeyDelete           = 261;
			constexpr int KeyRight            = 262;
			constexpr int KeyLeft             = 263;
			constexpr int KeyDown             = 264;
			constexpr int KeyUp               = 265;
			constexpr int KeyPageUp           = 266;
			constexpr int KeyPageDown         = 267;
			constexpr int KeyHome             = 268;
			constexpr int KeyEnd              = 269;
			constexpr int KeyCapsLock         = 280;
			constexpr int KeyScrollLock       = 281;
			constexpr int KeyNumLock          = 282;
			constexpr int KeyPrintScreen      = 283;
			constexpr int KeyPause            = 284;
			constexpr int KeyF1               = 290;
			constexpr int KeyF2               = 291;
			constexpr int KeyF3               = 292;
			constexpr int KeyF4               = 293;
			constexpr int KeyF5               = 294;
			constexpr int KeyF6               = 295;
			constexpr int KeyF7               = 296;
			constexpr int KeyF8               = 297;
			constexpr int KeyF9               = 298;
			constexpr int KeyF10              = 299;
			constexpr int KeyF11              = 300;
			constexpr int KeyF12              = 301;
			constexpr int KeyF13              = 302;
			constexpr int KeyF14              = 303;
			constexpr int KeyF15              = 304;
			constexpr int KeyF16              = 305;
			constexpr int KeyF17              = 306;
			constexpr int KeyF18              = 307;
			constexpr int KeyF19              = 308;
			constexpr int KeyF20              = 309;
			constexpr int KeyF21              = 310;
			constexpr int KeyF22              = 311;
			constexpr int KeyF23              = 312;
			constexpr int KeyF24              = 313;
			constexpr int KeyF25              = 314;
			constexpr int KeyKP0              = 320;
			constexpr int KeyKP1              = 321;
			constexpr int KeyKP2              = 322;
			constexpr int KeyKP3              = 323;
			constexpr int KeyKP4              = 324;
			constexpr int KeyKP5              = 325;
			constexpr int KeyKP6              = 326;
			constexpr int KeyKP7              = 327;
			constexpr int KeyKP8              = 328;
			constexpr int KeyKP9              = 329;
			constexpr int KeyKPDecimal        = 330;
			constexpr int KeyKPDivide         = 331;
			constexpr int KeyKPMultiply       = 332;
			constexpr int KeyKPSubtract       = 333;
			constexpr int KeyKPAdd            = 334;
			constexpr int KeyKPEnter          = 335;
			constexpr int KeyKPEqual          = 336;
			constexpr int KeyLeftShift        = 340;
			constexpr int KeyLeftControl      = 341;
			constexpr int KeyLeftAlt          = 342;
			constexpr int KeyLeftSuper        = 343;
			constexpr int KeyRightShift       = 344;
			constexpr int KeyRightControl     = 345;
			constexpr int KeyRightAlt         = 346;
			constexpr int KeyRightSuper       = 347;
			constexpr int KeyMenu             = 348;
			constexpr int KeyLast             = KeyMenu;

			constexpr int MouseButton1        = 0;
			constexpr int MouseButton2        = 1;
			constexpr int MouseButton3        = 2;
			constexpr int MouseButton4        = 3;
			constexpr int MouseButton5        = 4;
			constexpr int MouseButton6        = 5;
			constexpr int MouseButton7        = 6;
			constexpr int MouseButton8        = 7;
			constexpr int MouseButtonLast     = MouseButton8;
			constexpr int MouseButtonLeft     = MouseButton1;
			constexpr int MouseButtonRight    = MouseButton2;
			constexpr int MouseButtonMiddle   = MouseButton3;

			constexpr int ShiftMask           = 1;
			constexpr int CtrlMask            = 2;
			constexpr int AltMask             = 4;

			constexpr int KeyRelease          = 0;
			constexpr int KeyPress            = 1;
			constexpr int KeyRepeat           = 2;
		}
	}
}
