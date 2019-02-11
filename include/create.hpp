#pragma once

#include <3ds/types.h>
#include <memory>
#include <string>

#include "common/STDirectory.hpp"
#include "streetpass/Box.hpp"
#include "streetpass/MBoxList.hpp"
#include "streetpass/StreetpassManager.hpp"

void displayCreateMenu(Streetpass::StreetpassManager& sm);
void createMenu(Streetpass::StreetpassManager& sm);

void createBoxFromBackup(Streetpass::StreetpassManager& sm, STDirectory& backupDirectory, u8 slotNum);

void displayBackupSlotSelection(Streetpass::StreetpassManager& sm, STDirectory& backupDirectory, u8 slotNum);