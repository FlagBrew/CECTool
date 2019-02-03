#include "delete.hpp"

void deleteBox(Streetpass::StreetpassManager& sm, u8 slotNum) {
    sm.DeleteBox(slotNum);
}

void deleteAllBoxes(Streetpass::StreetpassManager& sm) {
    sm.DeleteAllBoxes();
}
