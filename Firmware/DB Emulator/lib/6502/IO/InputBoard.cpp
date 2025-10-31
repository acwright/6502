#include "InputBoard.h"

InputBoard::InputBoard() {}
uint8_t InputBoard::read(uint16_t address) { return 0x00; }
void InputBoard::write(uint16_t address, uint8_t value) {}
void InputBoard::reset() {}

void InputBoard::updateMouse(int mouseX, int mouseY, int mouseW, uint8_t mouseButtons) {
  // uint8_t mouseX    = _mouseX < 0 ? (abs(_mouseX) | 0b10000000) : _mouseX;  // If negative, set bit 7 (Left = -X)
  // uint8_t mouseY    = _mouseY < 0 ? (abs(_mouseY) | 0b10000000) : _mouseY;  // If negative, set bit 7 (Up = -Y)
  // uint8_t mouseW    = _mouseW < 0 ? (abs(_mouseW) | 0b10000000) : _mouseW;  // If negative, set bit 7 (Up = -W)
}