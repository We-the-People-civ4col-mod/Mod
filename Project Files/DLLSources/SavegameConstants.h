#pragma once

// savegame version as seen by the exe.
// lowering this by one will make the exe reject all existing savegames without crashing the game.
// exe supports negative savegame versions.
const int SAVEGAME_VERSION_EXE = 2;

// The savegame version is used to determine if and how to read the savegame
// WARNING: if a savegame fails to read because of it fails one of the following conditions, the game will quit.
// SAVEGAME_VERSION is always used when saving and the mod rejects any savegame using a higher version that this number.
// SAVEGAME_OLDEST_COMPATIBLE_VERSION is used to reject savegames, which uses an older verison than this.
const unsigned int SAVEGAME_VERSION = 2;
const unsigned int SAVEGAME_OLDEST_COMPATIBLE_VERSION = 1;

// The exe saves an array of building and unit infos. Adding one will cause the exe to read a different number of bytes.
// To get around this, make the exe believe we have way more entries than we have and as such adding one will not change the number seen by the exe.
// Changing either of those two numbers will render savegames incompatible.
const int SAVEGAME_EXE_FALSE_XML_BUILDINGS = 384;
const int SAVEGAME_EXE_FALSE_XML_UNITS = 384;
