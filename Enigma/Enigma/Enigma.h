#pragma once

// Enigma Game Engine ===========
// By Noah Beauchamp
// Version 1 0 3
// Note:
// I'm jumping around a lot of random features I will get back to the editor eventualy
// 
// Added a new project to handle serialization.
// The engine doesn't need to know about differant file formats, you should be able to just
// pass in the raw data. To do this I made the EnigmaSerialization project that will handle all of the
// differant files, like textures, models, json, fonts, etc.
// 
// ==============================

#include "Core/Core.h"
#include "Core/Utilities/Logger.h"
#include "Core/System.h"
#include "Core/Event/Event.h"
#include "Core/Event/WindowEvent.h"
#include "Core/Process/Application.h"
#include "Core/Process/SubProcess.h"