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
OledTable(Adafruit_SSD1306 *display, uint8_t rows, uint8_t cols, bool drawFrame = true);
	
/** 
*  init the table, call this function during the setup
*/

   
void init(void);
void drawLines(void);
int setText(uint8_t row, uint8_t col, String text , bool fitToFrame = true); 
void refresh(void);
void clear(void);

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
