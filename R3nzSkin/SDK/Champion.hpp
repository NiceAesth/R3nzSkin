#pragma once

#include "AString.hpp"
#include "Pad.hpp"
#include "RiotArray.hpp"
#include "Skin.hpp"

class Champion {
  PAD(0x8)
  AString champion_name;
  PAD(0xD0)
  RiotArray<Skin> skins;
};
