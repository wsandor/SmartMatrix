/*
 * SmartMatrix Library - Scrolling Layer Class
 *
 * Copyright (c) 2015 Louis Beaudoin (Pixelmatix)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <string.h>

#define SCROLLING_BUFFER_ROW_SIZE   (this->localWidth / 8)
#define SCROLLING_BUFFER_SIZE       (SCROLLING_BUFFER_ROW_SIZE * this->localHeight)

template <typename RGB, unsigned int optionFlags>
SMLayerScrolling<RGB, optionFlags>::SMLayerScrolling(uint8_t * bitmap, uint16_t width, uint16_t height) {
    scrollingBitmap = bitmap;
	
	scrollingBackBitmap = (uint8_t *)malloc(width * (height / 8));
    memset(scrollingBackBitmap, 0x00, width * (height / 8));
	
	this->matrixWidth = width;
    this->matrixHeight = height;
    this->textcolor = rgb48(0xffff, 0xffff, 0xffff);
	this->backcolor = rgb48(0x0000, 0x0000, 0x0000);
	this->drawBackColor = false;
	this->xEnd = width;
	this->yEnd = height;
	changed = 0xFFFFFFFF;
}

template <typename RGB, unsigned int optionFlags>
SMLayerScrolling<RGB, optionFlags>::SMLayerScrolling(uint16_t width, uint16_t height) {
    scrollingBitmap = (uint8_t *)malloc(width * (height / 8));
#ifdef ESP32
    assert(scrollingBitmap != NULL);
#else
    this->assert(scrollingBitmap != NULL);
#endif
    memset(scrollingBitmap, 0x00, width * (height / 8));
	
	scrollingBackBitmap = (uint8_t *)malloc(width * (height / 8));
#ifdef ESP32
    assert(scrollingBackBitmap != NULL);
#else
    this->assert(scrollingBackBitmap != NULL);
#endif
    memset(scrollingBackBitmap, 0x00, width * (height / 8));
    this->matrixWidth = width;
    this->matrixHeight = height;
    this->textcolor = rgb48(0xffff, 0xffff, 0xffff);
	this->backcolor = rgb48(0x0000, 0x0000, 0x0000);
	this->drawBackColor = false;
	this->xEnd = width;
	this->yEnd = height;
	changed = 0xFFFFFFFF;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::begin(void) {
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::frameRefreshCallback(void) {
    updateScrollingText();
}

// returns true and copies color to xyPixel if pixel is opaque, returns false if not
template<typename RGB, unsigned int optionFlags> template <typename RGB_OUT>
bool SMLayerScrolling<RGB, optionFlags>::getPixel(uint16_t hardwareX, uint16_t hardwareY, RGB_OUT &xyPixel) {
    uint16_t localScreenX, localScreenY;

    // convert hardware x/y to the pixel in the local screen
    switch( this->rotation ) {
      case rotation0 :
        localScreenX = hardwareX;
        localScreenY = hardwareY;
        break;
      case rotation180 :
        localScreenX = (this->matrixWidth - 1) - hardwareX;
        localScreenY = (this->matrixHeight - 1) - hardwareY;
        break;
      case  rotation90 :
        localScreenX = hardwareY;
        localScreenY = (this->matrixWidth - 1) - hardwareX;
        break;
      case  rotation270 :
        localScreenX = (this->matrixHeight - 1) - hardwareY;
        localScreenY = hardwareX;
        break;
      default:
        // TODO: Should throw an error
        return false;
    };

    uint8_t bitmask = 0x80 >> (localScreenX % 8);

    if (scrollingBitmap[(localScreenY * SCROLLING_BUFFER_ROW_SIZE) + (localScreenX/8)] & bitmask) {
        xyPixel = textcolor;
        return true;
    }

    return false;
}

// returns true and copies color to xyPixel if pixel is opaque, returns false if not
template<typename RGB, unsigned int optionFlags> template <typename RGB_OUT>
bool SMLayerScrolling<RGB, optionFlags>::getBackPixel(uint16_t hardwareX, uint16_t hardwareY, RGB_OUT &xyPixel) {
    uint16_t localScreenX, localScreenY;

    // convert hardware x/y to the pixel in the local screen
    switch( this->rotation ) {
      case rotation0 :
        localScreenX = hardwareX;
        localScreenY = hardwareY;
        break;
      case rotation180 :
        localScreenX = (this->matrixWidth - 1) - hardwareX;
        localScreenY = (this->matrixHeight - 1) - hardwareY;
        break;
      case  rotation90 :
        localScreenX = hardwareY;
        localScreenY = (this->matrixWidth - 1) - hardwareX;
        break;
      case  rotation270 :
        localScreenX = (this->matrixHeight - 1) - hardwareY;
        localScreenY = hardwareX;
        break;
      default:
        // TODO: Should throw an error
        return false;
    };

    uint8_t bitmask = 0x80 >> (localScreenX % 8);

    if (scrollingBackBitmap[(localScreenY * SCROLLING_BUFFER_ROW_SIZE) + (localScreenX/8)] & bitmask) {
        xyPixel = backcolor;
        return true;
    }

    return false;
}

template<typename RGB, unsigned int optionFlags>
bool SMLayerScrolling<RGB, optionFlags>::getPixel(uint16_t hardwareX, uint16_t hardwareY) {
    uint16_t localScreenX, localScreenY;

    // convert hardware x/y to the pixel in the local screen
    switch( this->rotation ) {
      case rotation0 :
        localScreenX = hardwareX;
        localScreenY = hardwareY;
        break;
      case rotation180 :
        localScreenX = (this->matrixWidth - 1) - hardwareX;
        localScreenY = (this->matrixHeight - 1) - hardwareY;
        break;
      case  rotation90 :
        localScreenX = hardwareY;
        localScreenY = (this->matrixWidth - 1) - hardwareX;
        break;
      case  rotation270 :
        localScreenX = (this->matrixHeight - 1) - hardwareY;
        localScreenY = hardwareX;
        break;
      default:
        // TODO: Should throw an error
        return false;
    };

    uint8_t bitmask = 0x80 >> (localScreenX % 8);

    if (scrollingBitmap[(localScreenY * SCROLLING_BUFFER_ROW_SIZE) + (localScreenX/8)] & bitmask) {
        return true;
    }

    return false;
}


template<typename RGB, unsigned int optionFlags>
bool SMLayerScrolling<RGB, optionFlags>::getBackPixel(uint16_t hardwareX, uint16_t hardwareY) {
    uint16_t localScreenX, localScreenY;

    // convert hardware x/y to the pixel in the local screen
    switch( this->rotation ) {
      case rotation0 :
        localScreenX = hardwareX;
        localScreenY = hardwareY;
        break;
      case rotation180 :
        localScreenX = (this->matrixWidth - 1) - hardwareX;
        localScreenY = (this->matrixHeight - 1) - hardwareY;
        break;
      case  rotation90 :
        localScreenX = hardwareY;
        localScreenY = (this->matrixWidth - 1) - hardwareX;
        break;
      case  rotation270 :
        localScreenX = (this->matrixHeight - 1) - hardwareY;
        localScreenY = hardwareX;
        break;
      default:
        // TODO: Should throw an error
        return false;
    };

    uint8_t bitmask = 0x80 >> (localScreenX % 8);

    if (scrollingBackBitmap[(localScreenY * SCROLLING_BUFFER_ROW_SIZE) + (localScreenX/8)] & bitmask) {
        return true;
    }

    return false;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::fillRefreshRow(uint16_t hardwareY, rgb48 refreshRow[]) {
    rgb48 currentPixel, currentBackPixel;
    int i;

    if(this->ccEnabled) {
        colorCorrection(textcolor, currentPixel);
		colorCorrection(backcolor, currentBackPixel);
	} else {
        currentPixel = textcolor;
		currentBackPixel = backcolor;
	}
	

    for(i= xStart; i<xEnd/*this->matrixWidth*/; i++) {
        if (getPixel(i, hardwareY)) 
			refreshRow[i] = currentPixel;
	    else 
			if ((getBackPixel(i, hardwareY)) && drawBackColor)
				refreshRow[i] = currentBackPixel;	
    }
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::fillRefreshRow(uint16_t hardwareY, rgb24 refreshRow[]) {
    rgb24 currentPixel, currentBackPixel;
    int i;

    if(this->ccEnabled) {
        colorCorrection(textcolor, currentPixel);
		colorCorrection(backcolor, currentBackPixel);
	} else {
        currentPixel = textcolor;
		currentBackPixel = backcolor;
	}

    for(i= xStart; i<xEnd/*this->matrixWidth*/; i++) {
        if (getPixel(i, hardwareY)) 
			refreshRow[i] = currentPixel;
	    else 
			if ((getBackPixel(i, hardwareY)) && drawBackColor)
				refreshRow[i] = currentBackPixel;
    }
}

template<typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setColor(const RGB & newColor) {
    textcolor = newColor;
}

template<typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setBackColor(const RGB & newColor) {
    backcolor = newColor;
}

template<typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::enableColorCorrection(bool enabled) {
    this->ccEnabled = sizeof(RGB) <= 3 ? enabled : false;
}

// stops the scrolling text on the next refresh
template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::stop(void) {
    // setup conditions for ending scrolling:
    // scrollcounter is next to zero
    scrollcounter = 1;
    // position text at the end of the cycle
    scrollPosition = scrollMin;
}

// returns 0 if stopped
// returns positive number indicating number of loops left if running
// returns -1 if continuously scrolling
template <typename RGB, unsigned int optionFlags>
int SMLayerScrolling<RGB, optionFlags>::getStatus(void) const {
    return scrollcounter;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setMinMax(void) {
   switch (scrollmode) {
    case wrapForward:
    case bounceForward:
    case bounceReverse:
    case wrapForwardFromLeft:
        scrollMin = -textWidth + xStart;
        scrollMax = /*this->localWidth -*/ xEnd;

        scrollPosition = scrollMax;

        if (scrollmode == bounceReverse)
            scrollPosition = scrollMin;
        else if(scrollmode == wrapForwardFromLeft)
            scrollPosition = fontLeftOffset;

        // TODO: handle special case - put content in fixed location if wider than window

        break;

    case stopped:
    case off:
        scrollMin = scrollMax = scrollPosition = 0;
        break;
    }

}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::start(const char inputtext[], int numScrolls) {
    int length = strlen((const char *)inputtext);
    if (length > textLayerMaxStringLength)
        length = textLayerMaxStringLength;
    strncpy(text, (const char *)inputtext, length);
    textlen = length;
    scrollcounter = numScrolls;

	textWidth = -1;
	for (int i = 0; i < textlen; i++) {
		textWidth+= getBitmapFontCharWidth(text[i], scrollFont);
	}
    //textWidth = (textlen * scrollFont->Width) - 1;

    setMinMax();
 }

//Updates the text that is currently scrolling to the new value
//Useful for a clock display where the time changes.
template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::update(const char inputtext[]){
    int length = strlen((const char *)inputtext);
    if (length > textLayerMaxStringLength)
        length = textLayerMaxStringLength;
    strncpy(text, (const char *)inputtext, length);
    textlen = length;
	textWidth = -1;
	for (int i = 0; i < textlen; i++) {
		textWidth+= getBitmapFontCharWidth(text[i], scrollFont);
	}
    //textWidth = (textlen * scrollFont->Width) - 1;

    setMinMax();
}

// called once per frame to update (virtual) bitmap
// function needs major efficiency improvments
template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::updateScrollingText(void) {
    bool resetScrolls = false;
    // return if not ready to update
    if (!scrollcounter || ++currentframe <= framesperscroll)
        return;

    currentframe = 0;

    switch (scrollmode) {
    case wrapForward:
    case wrapForwardFromLeft:
        scrollPosition--;
        if (scrollPosition <= scrollMin) {
            scrollPosition = scrollMax;
            if (scrollcounter > 0) scrollcounter--;
        }
        break;

    case bounceForward:
        scrollPosition--;
        if (scrollPosition <= scrollMin) {
            scrollmode = bounceReverse;
            if (scrollcounter > 0) scrollcounter--;
        }
        break;

    case bounceReverse:
        scrollPosition++;
        if (scrollPosition >= scrollMax) {
            scrollmode = bounceForward;
            if (scrollcounter > 0) scrollcounter--;
        }
        break;

    default:
    case stopped:
        scrollPosition = fontLeftOffset;
        resetScrolls = true;
        break;
    }

    // done scrolling - move text off screen and disable
    if (!scrollcounter) {
        resetScrolls = true;
    }

    // for now, fill the bitmap fresh with each update
    // TODO: reset only when necessary, and update just the pixels that need it
    resetScrolls = true;
    if (resetScrolls) {
        redrawScrollingText();
    }
}

// TODO: recompute stuff after changing mode, font, etc
template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setMode(ScrollMode mode) {
    scrollmode = mode;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setRefreshRate(uint8_t newRefreshRate) {
    this->refreshRate = newRefreshRate;
    framesperscroll = (this->refreshRate * 1.0) / pixelsPerSecond;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setSpeed(unsigned char pixels_per_second) {
    pixelsPerSecond = pixels_per_second;
    framesperscroll = (this->refreshRate * 1.0) / pixelsPerSecond;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setFont(fontChoices newFont) {
    scrollFont = fontLookup(newFont);
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setBitmapFont(const bitmap_font newFont) {
  scrollFont = (bitmap_font *)&newFont;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setOffsetFromTop(int offset) {
    fontTopOffset = offset + yStart;
    majorScrollFontChange = true;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setStartOffsetFromLeft(int offset) {
    fontLeftOffset = offset;
}

template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::setWindow(int x, int y, int w, int h) {
	fontTopOffset-= yStart;
    xStart = x;
	yStart = y;
	xEnd = x + w;
	yEnd = y + h;
	fontTopOffset+= y;
	majorScrollFontChange = true;
}

// if font size or position changed since the last call, redraw the whole frame
template <typename RGB, unsigned int optionFlags>
void SMLayerScrolling<RGB, optionFlags>::redrawScrollingText(void) {
    int j, k;
    int charPosition, textPosition;
    uint16_t charY0, charY1;


    //for (j = 0; j < this->localHeight; j++) {
	for (j = yStart; j < yEnd; j++) {

        // skip rows without text
        if (j < fontTopOffset || j >= fontTopOffset + scrollFont->Height)
            continue;

        // now in row with text
        // find the position of the first char
        charPosition = scrollPosition;
        textPosition = 0;

        // move to first character at least partially on screen
        //while (charPosition + scrollFont->Width < 0 ) {
		while (charPosition + getBitmapFontCharWidth(text[textPosition], scrollFont) < 0 ) {			
            //charPosition += scrollFont->Width;
			charPosition += getBitmapFontCharWidth(text[textPosition], scrollFont);
            textPosition++;
        }

        // find rows within character bitmap that will be drawn (0-font->height unless text is partially off screen)
        charY0 = j - fontTopOffset;

        if (yEnd/*this->localHeight*/ < fontTopOffset + scrollFont->Height) {
            charY1 = yEnd/*this->localHeight*/ - fontTopOffset;
        } else {
            charY1 = scrollFont->Height;
        }

        if(majorScrollFontChange) {
            // clear full refresh buffer before copying background over, size or position may have changed, can't just clear rows used by font
            memset(scrollingBitmap, 0x00, SCROLLING_BUFFER_SIZE);
			if (this->drawBackColor)
				memset(scrollingBackBitmap, 0x00, SCROLLING_BUFFER_SIZE);
            majorScrollFontChange = false;
        } else {
            // clear rows used by font before drawing on top
            for (k = 0; k < charY1 - charY0; k++) {
                memset(&scrollingBitmap[((j + k) * SCROLLING_BUFFER_ROW_SIZE)], 0x00, SCROLLING_BUFFER_ROW_SIZE);
				if (this->drawBackColor)
					memset(&scrollingBackBitmap[((j + k) * SCROLLING_BUFFER_ROW_SIZE)], 0x00, SCROLLING_BUFFER_ROW_SIZE);
			}
        }

        while (textPosition < textlen && charPosition < xEnd/*this->localWidth*/) {
            uint8_t tempBitmask;
			uint8_t tempBackBitmask;
			int charWidth;
			uint16_t idxBitmap; 
			charWidth = getBitmapFontCharWidth(text[textPosition], scrollFont);//   scrollFont->Widths[getBitmapFontLocation(text[textPosition], scrollFont)];
            // draw character from top to bottom
            for (k = charY0; k < charY1; k++) {
                uint8_t xChar;			
				for (xChar = 0; xChar < charWidth; xChar+=8) {
					if ((charPosition + xChar > -8) && (charPosition + xChar < xEnd/*this->localWidth*/)) {
						tempBitmask = getBitmapFontRowAtXY(text[textPosition], xChar, k, scrollFont);
						if (this->drawBackColor) {
							tempBackBitmask = 0xFF;
							if (charWidth - xChar < 8)
								tempBackBitmask = tempBackBitmask << (8 - (charWidth - xChar));
						}
						idxBitmap = (j + k - charY0) * SCROLLING_BUFFER_ROW_SIZE;
						if (charPosition + xChar < 0) {
							scrollingBitmap[idxBitmap] |= tempBitmask << -(charPosition + xChar);
							if (this->drawBackColor) {
								scrollingBackBitmap[idxBitmap] |= tempBackBitmask << -(charPosition + xChar);
							}
						} else {
							int w = charWidth - charPosition - xChar;
							//if ((w < 8) && (w > 0))
							//	tempBitmask = tempBitmask & 0xFF << (8 - w);
							scrollingBitmap[idxBitmap + ((charPosition + xChar)/8)] |= tempBitmask >> ((charPosition + xChar)%8);
							if (this->drawBackColor) {
								//if ((w < 8) && (w > 0))
								//	tempBackBitmask = tempBackBitmask & 0xFF << (8 - w);
								scrollingBackBitmap[idxBitmap + ((charPosition + xChar)/8)] |= tempBackBitmask >> ((charPosition + xChar)%8);
							}
							// do two writes if the shifted 8-bit wide bitmask is still on the screen
							if((charPosition + xChar + 8 < (((xEnd + 7) >> 3) << 3)/*this->localWidth*/) && (charPosition % 8)) {
							    w+= 8;
								//if ((w < 8) && (w > 0))
								//	tempBitmask = tempBitmask & 0xFF << (8 - w);
								scrollingBitmap[idxBitmap + ((charPosition + xChar)/8) + 1] |= tempBitmask << (8-((charPosition + xChar)%8));
								if (this->drawBackColor) {
									//if ((w < 8) && (w > 0))
									//	tempBackBitmask = tempBackBitmask & 0xFF << (8 - w);
									scrollingBackBitmap[idxBitmap + ((charPosition + xChar)/8) + 1] |= tempBackBitmask << (8-((charPosition + xChar)%8));
								}
							}
						}	
					}								
                }
		/*		if (this->drawBackColor) {
					for (xChar = 0; xChar < charWidth; xChar+=8) {
						if ((charPosition + xChar > -8) && (charPosition + xChar < xEnd)) {
							tempBitmask = 0xFF;
							if (charWidth - xChar < 8)
								tempBitmask = tempBitmask << (8 - (charWidth - xChar));
							//tempBitmask = 0xAA;
							idxBitmap = (j + k - charY0) * SCROLLING_BUFFER_ROW_SIZE;
							if (charPosition + xChar < 0) {
								scrollingBackBitmap[idxBitmap] |= tempBitmask << -(charPosition + xChar);
							} else {
								scrollingBackBitmap[idxBitmap + ((charPosition + xChar)/8)] |= tempBitmask >> ((charPosition + xChar)%8);
								// do two writes if the shifted 8-bit wide bitmask is still on the screen
								if(charPosition + xChar + 8 < xEnd && charPosition % 8)
									scrollingBackBitmap[idxBitmap + ((charPosition + xChar)/8) + 1] |= tempBitmask << (8-((charPosition + xChar)%8));
							}	
						}								
					}	
				}*/
            }

            // get set up for next character
            charPosition += charWidth; //scrollFont->Width;
            textPosition++;
        }

        j += (charY1 - charY0) - 1;
    }
	changed = 0xFFFFFFFF;
}

template <typename RGB, unsigned int optionFlags>
bool SMLayerScrolling<RGB, optionFlags>::getBitmapPixelAtXY(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *bitmap) {
    int cell = (y * ((width / 8) + 1)) + (x / 8);
	
	uint8_t mask = 0x80 >> (x % 8);
	return (mask & bitmap[cell]);
}

