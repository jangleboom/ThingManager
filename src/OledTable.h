/*********************************************************
 * Table for oled Displays 
 * 
 * 
 * Currently tested with 
 * ESP8266 and 64x48 OLED
 *
 */

#ifndef OLED_TABLE_H
#define OLED_TABLE_H
#include "Adafruit_SSD1306_mini.h"

#define OK 0
#define ERROR_WRONG_COL -1
#define ERROR_WRONG_ROW -2
#define ERROR_TEXT_TO_LONG -3 // currently not used


class OledTable {

public:
/**
 * Constructor
 */
OledTable(Adafruit_SSD1306 *display, uint8_t rows, uint8_t cols, bool drawFrame = true) {
  _display = display;
  _rows = rows;
  _cols = cols;
  _drawFrame = drawFrame;
	}
 
	
/** 
*  init the table, call this function during the setup
*/

   
void init(){
   _colWidth  = _display->width() / _cols;
   _rowHeight = _display->height() / _rows;
 }

void drawLines() {
  clear();
  
  // draw the frame
  if (_drawFrame) {
    _display->drawRect(0, 0, _display->width(), _display->height(), WHITE);
  }

  // Draw rows
  for (uint8_t i=1; i < _rows; i++) {
    _display->drawLine(1, i*_rowHeight, _display->width()-1, i*_rowHeight, WHITE);
  }

  // Draw cols
  for (uint8_t i=1; i < _cols; i++) {
    _display->drawLine(i*_colWidth, 1, i*_colWidth, _display->height()-1, WHITE);
    
  }
  
  refresh();
}

int setText(uint8_t row, uint8_t col, String text , bool fitToFrame = true) {
  if(row < 0 || row > _rows){
    return ERROR_WRONG_ROW;
  }

  if(col < 0 || col > _cols){
    return ERROR_WRONG_COL;
  }

  int16_t  x1, y1;
  uint16_t w, h;
  _display->setTextSize(1);
  _display->setTextColor(WHITE);
  _display->getTextBounds(text, (_rowHeight*row)+3, (_colWidth *col) +3, &x1, &y1, &w, &h);
  
  if (fitToFrame) {
    if (_colWidth < w || _rowHeight < h) return  ERROR_TEXT_TO_LONG;
    //_display->setCursor((_rowHeight * row) + 3, (_colWidth * col) + 3);
    _display->setCursor((_colWidth * col) + 3, (_rowHeight * row) + 3);
    _display->print(text);
  } else {
    _display->print(text);
    //_display->drawString((_colWidth *(col))+3, (_rowHeight*(row)) ,text);
  }
  
  refresh();
  
  return OK;
}

void refresh(){
  _display->display();
}

void clear(){
  _display->clearDisplay();
  //drawLines();
}

private:
  uint8_t _rows;
  uint8_t _cols; 
  int _colWidth;
  int _rowHeight;
  bool _drawFrame;
  Adafruit_SSD1306 *_display;
  uint8_t  _bgColor = WHITE;
  uint8_t  _fgColor = BLACK;

};

#endif // ** !OLED_TABLE_H **
