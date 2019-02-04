#pragma once

#include "streetpass/StreetpassManager.hpp"

void displayImportMenu(Streetpass::StreetpassManager& sm);
void importMenu(Streetpass::StreetpassManager& sm);

void importBoxes(bool del);
void importBox(Streetpass::StreetpassManager& sm, u8 slotNum);
void importMessages(Streetpass::StreetpassManager& sm, u8 slotNum);