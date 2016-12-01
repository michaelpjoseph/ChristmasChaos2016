// Mike Joseph - michaelpjoseph@gmail.com - 2016.Oct.26
//  2016 Arduino Christmas Light display 

//  1x - Arduino UNO Microcontroller
//  4x - 50 LED WS2811 Strips
//  1x - 10W 5V Power Supply


#include "FastLED.h"

#define NUMPIX 200  // Total LEDs.  200 is probably about max with this code on the UNO.  Memory has been almost a constant issue.  Need to go Mega if you want more Lights.
#define LEDPIN 3    // LED control pin

//PIX struct and array.  This is the working array that gets directly manipulated, then the strip_FlushAndDisplay() function flushes it to the fastled array and displays the current strip state.
struct PIX {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};
PIX PIXARR[NUMPIX];   // master array to be dumped to strand on each redraw
CRGB leds[NUMPIX];  //fastled display array.  Not directly manipulated except for in the scdisplay_rainbow() function


//strip_FlushAndDisplay() - dumps PIXARR to 'leds' fastled display array and then 'shows' current state of strip
void strip_FlushAndDisplay() {
  for (int16_t a = 0; a < NUMPIX; a++) {
    //strip.setPixelColor( a, PIXARR[a].R, PIXARR[a].G, PIXARR[a].B); // single strand operation
    leds[a] = CRGB(PIXARR[a].R, PIXARR[a].G, PIXARR[a].B);
  }
  FastLED.show();
}

//listPIXARR() - debug function to dump current PIXARR state to terminal
void listPIXARR() {  // DEBUG - dump pixel array
  strip_FlushAndDisplay();
  for (int16_t a = 0; a < NUMPIX; a++) {
    Serial.print(a); Serial.print(": "); Serial.print("\t"); Serial.print(PIXARR[a].R); Serial.print("\t"); Serial.print(PIXARR[a].G); Serial.print("\t"); Serial.println(PIXARR[a].B);
  }
}

//initPIXARR_rand() - initializes all LEDs to a random RGB color
void initPIXARR_rand() {
  for (int16_t a = 0; a < NUMPIX; a++) {
    PIXARR[a] = { rand() % 255, rand() % 255, rand() % 255 };
  }
}

//initPIXARR_null()  - sets all LEDs to off
void initPIXARR_null() {
  for (int16_t a = 0; a < NUMPIX; a++) {
    PIXARR[a] = { 0, 0, 0 };
  }
}


//rndSinglePIX_init() - returns a random single pixel skewed toward either R, G, or B to prevent overly white or pastel looking colors
PIX rndSinglePIX_init() {
  PIX initPix_buffer;
  initPix_buffer.R = 0;
  int16_t skew = rand() % 3;  // 0 - red | 1 - green | 2 - blue
  switch (skew) {
    case 0:
      initPix_buffer.R = 255;
      initPix_buffer.G = rand() % 200;
      initPix_buffer.B = rand() % 200;
      break;
    case 1:
      initPix_buffer.R = rand() % 200;
      initPix_buffer.G = 255;
      initPix_buffer.B = rand() % 200;
      break;
    case 2:
      initPix_buffer.R = rand() % 200;
      initPix_buffer.G = rand() % 200;
      initPix_buffer.B = 255;
      break;
  }
  return initPix_buffer;
}

//initPIXARR_snakes() - creates an evenly spaced number of 'snakes' of specified 'length'
void initPIXARR_snakes(int16_t length, int16_t snakes) {
  if ((snakes * length) >= NUMPIX) {  //check for crazy inputs and set fallback defaults
    snakes = 1;
    length = 10;
  }

  uint8_t maxbrightness = 255;
  uint8_t currbrightness = 0;
  int16_t section_increment = maxbrightness / length;
  int16_t section_seperator = NUMPIX / snakes;

  for (int16_t a = 0; a < NUMPIX; a++) {  // zero out array
    PIXARR[a] = { 0, 0, 0 };
  }

  for (int16_t snake_start = 0; snake_start < NUMPIX; snake_start += section_seperator) {
    currbrightness = 0;
    for (int16_t s = snake_start; s < (snake_start + length); s++) {
      currbrightness += section_increment;
      PIXARR[s] = { currbrightness, currbrightness, currbrightness };
    }
    PIXARR[snake_start + length] = { maxbrightness, maxbrightness, maxbrightness };
  }
}

//initPIXARR_sparse() - randomly inits pixels seperated by white space defined by 'spacing'
void initPIXARR_sparse(int16_t spacing) {
  initPIXARR_null();
  for (int16_t a = 0; a < NUMPIX; a++) {
    if ((a % spacing) == 0) {
      PIX buffer = rndSinglePIX_init();
      PIXARR[a] = buffer;
    }
  }
}


//scdisplay_rainbow() - cycles all LEDs simultaneously through all of the colors for the rainbow for a specified 'duration'
void scdisplay_rainbow(int16_t duration) {
  int16_t hue = 160;
  for (int i = 0; i < duration; i++) {
    for (int p = 0; p < NUMPIX; p++) {
      leds[p].setHSV(hue, 255, 255);
    }
    FastLED.show();
    delay(10);
    hue++;
  }
}
//scdisplay_PopInFadeOut() - self contained display - pops in random pixels, slowly fades them out, the puts it to sleep for a random interval.  Loops for 'duration'
void scdisplay_PopInFadeOut(int16_t duration) {
  initPIXARR_null();
  int16_t delaymax = 1000;
  int16_t curdelay[NUMPIX];
  for (uint8_t i = 0; i < NUMPIX; i++) {
    PIXARR[i].R = 0;
    PIXARR[i].G = 0;
    PIXARR[i].B = 0;
    curdelay[i] = rand() % delaymax;
  }
  for (int16_t a = 0; a <= duration; a++) {  //--- main display loop
    for (uint8_t i = 0; i < NUMPIX; i++) {
      if (PIXARR[i].R > 0 || PIXARR[i].G > 0 || PIXARR[i].B > 0) { // decay lit pixels
        if (PIXARR[i].R > 0) PIXARR[i].R--;
        if (PIXARR[i].G > 0) PIXARR[i].G--;
        if (PIXARR[i].B > 0) PIXARR[i].B--;
        //PIXARR[i] = PIXARR[i];
      } else if (curdelay[i] <= 0 && PIXARR[i].R <= 0 && PIXARR[i].G <= 0 && PIXARR[i].B <= 0) {  // time to re-initialize pixel
        PIXARR[i].R = rand() % 255;
        PIXARR[i].G = rand() % 255;
        PIXARR[i].B = rand() % 255;
        curdelay[i] = rand() % delaymax;
        //PIXARR[i] = PFPIXARR[i].color;
      } else {  // decrement delay - unlit pixel for now...
        curdelay[i]--;
      }
    }
    strip_FlushAndDisplay();
    delay(15);
  }
}


//scdisplay_PIXVector_snake - self contained display - battles specified # of snakes equally spaced at either end of array.  snakes init on the left move right, right snakes move left.  collision detection flashes white.
//  right now left snakes are red, right are green for christmas theme.
void scdisplay_PIXVector_snake(int16_t length, int16_t snakes, int16_t duration) {
  initPIXARR_null();
  if ((snakes * length) >= NUMPIX) {  //check for crazy inputs and set fallback defaults
    snakes = 2;
    length = 5;
  }
  //cout << ".1.\n";


  int16_t maxbrightness = 255;
  int16_t currbrightness = 0;
  int16_t section_increment = maxbrightness / length;
  int16_t section_seperator = NUMPIX / snakes;


  // -- setup     10/12/2016 ->  can't move to global :/  has to be redefined in each scope
  struct s_PIXVECT {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    int16_t pos;
    int8_t dir;
    bool ishead;


    s_PIXVECT() {  // assign defaults
      R = 0;
      G = 0;
      B = 0;
      pos = 0;
      dir = 0;
      ishead = false;
    }
  };

  s_PIXVECT PIXVECT[snakes][length];


  //cout << ".2.\n";  // debug after object creation


  int16_t idx_L = 0; // snake placement indexes for even distribution
  int16_t idx_R = (NUMPIX - 1);
  bool fwddir = true; // bool-alternate direction vectors
  for (int16_t a = 0; a < snakes; a++) {
    currbrightness = 0;
    if (fwddir) {
      int16_t currposition = idx_L;
      for (int16_t b = 0; b < length; b++) {
        //cout << a << " " << b << "   forward\n";
        currbrightness += section_increment;
        PIXVECT[a][b].R = currbrightness;
        PIXVECT[a][b].G = 0;
        PIXVECT[a][b].B = 0;
        //        PIXVECT[a][b].R = a + 10;
        //        PIXVECT[a][b].G = a + 10;
        //        PIXVECT[a][b].B = a + 10;
        PIXVECT[a][b].pos = currposition;
        PIXVECT[a][b].dir = 1;
        currposition += 1;
      }
      idx_L += section_seperator;
    } else {
      int16_t currposition = idx_R;
      for (int16_t b = (length - 1) ; b >= 0; b--) {
        currbrightness += section_increment;
        PIXVECT[a][b].R = 0;
        PIXVECT[a][b].G = currbrightness;
        PIXVECT[a][b].B = 0;
        //        PIXVECT[a][b].R = a + 10;
        //        PIXVECT[a][b].G = a + 10;
        //        PIXVECT[a][b].B = a + 10;
        PIXVECT[a][b].pos = currposition;
        PIXVECT[a][b].dir = -1;
        currposition -= 1;
      }
      idx_R -= section_seperator;
    }
    fwddir = !fwddir;
  }

  /*
    //--- DEEP DEBUG START ---
    //  cout << ".3.\n"; // debug after snake placement
    for (int a = 0; a < snakes; a++) {
      //cout << "--- " << a << " ---\n";
      for (int b = 0; b < length; b++) {
        cout << PIXVECT[a][b].R << "\t" << PIXVECT[a][b].G << "\t" << PIXVECT[a][b].B << "\t" << PIXVECT[a][b].pos << "\t" << PIXVECT[a][b].dir << "\n";
      }
    }
    //system("pause");
    //cout << "--------------------------\n\n\n\n";
    //--- DEEP DEBUG END ---
  */

  // --- add iterative stuff here ---
  for (int16_t i = 0; i <= duration; i++) {
    //cout << "---- " << i << " ----\n";
    initPIXARR_null();  //blank out the array
    for (int16_t d = 0; d < snakes; d++) {  // draw the snakes to the array - outer object loop
      for (int16_t e = 0; e < length; e++) {  // inner element loop
        if (PIXARR[PIXVECT[d][e].pos].R != 0) {  // add collision detection here if PIXAR[pos].r / g / b > 0
          PIXARR[PIXVECT[d][e].pos] = { 255, 255, 255 };
        } else {  // no collission
          PIXARR[PIXVECT[d][e].pos] = {PIXVECT[d][e].R, PIXVECT[d][e].G, PIXVECT[d][e].B };
        }

      }
    }
    //listPIXARR();
    
    strip_FlushAndDisplay();
    delay(50);
    for (int16_t m = 0; m < snakes; m++) {  // move the snakes
      for (int16_t e = 0; e < length; e++) {  // inner element loop
        PIXVECT[m][e].pos += PIXVECT[m][e].dir;
        if (PIXVECT[m][e].pos > (NUMPIX - 1)) {
          PIXVECT[m][e].pos = 0;
          PIXVECT[m][e].dir = 1;
        }
        if (PIXVECT[m][e].pos < 0) {
          PIXVECT[m][e].pos = (NUMPIX - 1);
          PIXVECT[m][e].dir = -1;
        }
      }
    }
  }
}

//PIXARR_toWhite() - increment all pixels to solid white
void PIXARR_toWhite() {
  for (uint16_t w = 0; w < 300; w++) {
     for (uint16_t p = 0; p < NUMPIX; p++) {
        if (PIXARR[p].R < 255) PIXARR[p].R++;  
        if (PIXARR[p].G < 255) PIXARR[p].G++;  
        if (PIXARR[p].B < 255) PIXARR[p].B++;  
     }
     strip_FlushAndDisplay();
     delay(20);
  }
}

// scdisplay_PIXVector_linedots - not currently used - looks too similar to my battling snakes in practice
void scdisplay_PIXVector_linedots(int16_t duration) {
  Serial.println("linedots process");
  int16_t maxbrightness = 255;
  int16_t halfway = NUMPIX / 2;
  int16_t dotsep = 3;   // decrease for more dots, increase for less
  int16_t numdots = halfway / dotsep;

  // -- setup     10/12/2016 ->  can't move to global :/  has to be redefined in each scope
  struct s_PIXVECT {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    int16_t pos;
    int8_t dir;
    bool ishead;


    s_PIXVECT() {  // assign defaults
      R = 0;
      G = 0;
      B = 0;
      pos = 0;
      dir = 0;
      ishead = false;
    }
  };
  s_PIXVECT PIXVECT[NUMPIX];  // every pixel has a vector


  //cout << ".2.\n";  // debug after object creation

  // draw line portion on left side
  int16_t currbrightness = 0;
  int16_t currposition = 0;
  int16_t fadepixels = 3;
  int16_t section_increment = maxbrightness / fadepixels;
  for (int16_t a = 0; a < fadepixels; a++) {  // fade left line
    currbrightness += section_increment;
//    PIXVECT[a].R = currbrightness;
//    PIXVECT[a].G = currbrightness;
//    PIXVECT[a].B = currbrightness;
    PIXVECT[a].R = currbrightness;
    PIXVECT[a].G = 0;
    PIXVECT[a].B = 0; 
    PIXVECT[a].pos = currposition;
    PIXVECT[a].dir = 1;
    currposition += 1;
  }
  for (int16_t a = currposition; a < (halfway - fadepixels); a++) {  // bright center of left line
//    PIXVECT[a].R = maxbrightness;
//    PIXVECT[a].G = maxbrightness;
//    PIXVECT[a].B = maxbrightness;
    PIXVECT[a].R = currbrightness;
    PIXVECT[a].G = 0;
    PIXVECT[a].B = 0;
    PIXVECT[a].pos = currposition;
    PIXVECT[a].dir = 1;
    currposition += 1;
  }
  for (int16_t a = currposition; a < halfway; a++) {  // bright center of left line
    currbrightness -= section_increment;
    PIXVECT[a].R = currbrightness;
    PIXVECT[a].G = 0;
    PIXVECT[a].B = 0;
    //PIXVECT[a].G = currbrightness;
    //PIXVECT[a].B = currbrightness;
    PIXVECT[a].pos = currposition;
    PIXVECT[a].dir = 1;
    currposition += 1;
  }



  // draw dot portion
  for (int16_t a = (NUMPIX - 1) ; a >  halfway; a -= dotsep) {
    PIXVECT[a].R = 0;
    PIXVECT[a].G = maxbrightness;
    PIXVECT[a].B = 0;
    
//    PIXVECT[a].R = maxbrightness;
//    PIX VECT[a].G = maxbrightness;
//    PIXVECT[a].B = maxbrightness;
    PIXVECT[a].pos = a;
    PIXVECT[a].dir = -1;
  }


  /*
    //--- DEEP DEBUG START ---
    //  cout << ".3.\n"; // debug after snake placement
    for (int a = 0; a < NUMPIX; a++) {
      cout << a << ":\t" <<PIXVECT[a].R << "\t" << PIXVECT[a].G << "\t" << PIXVECT[a].B << "\t" << PIXVECT[a].pos << "\t" << PIXVECT[a].dir << "\n";
    }
    system("pause");
    //cout << "--------------------------\n\n\n\n";
    //--- DEEP DEBUG END ---
  */


  // --- add iterative stuff here ---
  for (int16_t i = 0; i <= duration; i++) {
    //system("cls");
    initPIXARR_null();  //blank out the array
    for (int16_t d = 0; d < NUMPIX; d++) {  // draw the snakes to the array - outer object loop
      if (PIXVECT[d].dir != 0) {
        if (PIXARR[PIXVECT[d].pos].R != 0) {  // add collision detection here if PIXAR[pos].r / g / b > 0
          PIXARR[PIXVECT[d].pos] = { 255, 255, 255 };
        } else {  // no collission
          PIXARR[PIXVECT[d].pos] = {PIXVECT[d].R, PIXVECT[d].G, PIXVECT[d].B };
        }
      }
    }
    //listPIXARR();
    strip_FlushAndDisplay();
    //system("sleep .005");
    delay(25);
    for (int16_t e = 0; e < NUMPIX; e++) {  // inner element loop
      if (PIXVECT[e].dir != 0) {
        PIXVECT[e].pos += PIXVECT[e].dir;
        if (PIXVECT[e].pos > (NUMPIX - 1)) {
          PIXVECT[e].pos = 0;
          PIXVECT[e].dir = 1;
        }
        if (PIXVECT[e].pos < 0) {
          PIXVECT[e].pos = (NUMPIX - 1);
          PIXVECT[e].dir = -1;
        }
      }
    }
  }
}
  
// increments all pixels by one place for clockwise rotating effect
void incrPIXARR() {  // increment all pixels 1 position
  PIX buffer = PIXARR[0], hold;
  PIXARR[0] = PIXARR[NUMPIX - 1];
  for (int16_t a = 1; a < (NUMPIX); a++) {
    hold = PIXARR[a];
    PIXARR[a] = buffer;
    buffer = hold;
  }
}

// decrements all pixels by one place for counterclockwise rotating effect
void decrPIXARR() { // decrement all pixels 1 position
  PIX buffer = PIXARR[(NUMPIX - 1)], hold;
  PIXARR[(NUMPIX - 1)] = PIXARR[0];
  for (int16_t a = (NUMPIX - 2); a >= 0; a--) {
    hold = PIXARR[a];
    PIXARR[a] = buffer;
    buffer = hold;
  }
}

// loop incrPIXARR function 'times' times
void incrPIXARR_looper(int16_t times) {
  for (int16_t a = 0; a < times; a++) {
    incrPIXARR();
    //listPIXARR();
    strip_FlushAndDisplay();
    //system("sleep .005");
    delay(100);
    //system("cls");
  }
}

// loop decrPIXARR function 'times' times
void decrPIXARR_looper(int16_t times) {
  for (int16_t a = 0; a < times; a++) {
    decrPIXARR();
    strip_FlushAndDisplay();
    delay(100);
  }
}


// program setup
void setup() {
  FastLED.addLeds<WS2811, 3, RGB>(leds, NUMPIX);
  FastLED.addLeds<WS2811, 4, RGB>(leds, NUMPIX);
  Serial.begin(9600);
  Serial.println("startup");
  delay(5000);
}

// -----------  main application loop
void loop() {
/*
//  DEBUGGING FADE / RAINBOW LOOP 
  PIXARR_toWhite();
  initPIXARR_null();
  PIXARR_toWhite();
  scdisplay_rainbow(1600);
  initPIXARR_null();
  PIXARR_toWhite();
  scdisplay_rainbow(1600);
  initPIXARR_null();
  PIXARR_toWhite();
  scdisplay_rainbow(1600);
  initPIXARR_null();
  PIXARR_toWhite();
  scdisplay_rainbow(1600);
  initPIXARR_null();
*/ 
  scdisplay_PIXVector_snake(8 , 8 , 800);  // loop multiples of NUMPIX for smooth animation
  scdisplay_PIXVector_snake(4 , 12 , 400);  // loop multiples of NUMPIX for smooth animation
  scdisplay_PIXVector_snake(2 , 25 , 400);  // loop multiples of NUMPIX for smooth animation
  
  scdisplay_PopInFadeOut(1000);
  incrPIXARR_looper(10);
  decrPIXARR_looper(50);
  
  initPIXARR_sparse(10);
  incrPIXARR_looper(50);
  decrPIXARR_looper(100);
  
  initPIXARR_sparse(5);
  incrPIXARR_looper(50);
  decrPIXARR_looper(100);
  incrPIXARR_looper(75);
  
  initPIXARR_rand();
  incrPIXARR_looper(150);
  
  initPIXARR_snakes(2, 10);
  incrPIXARR_looper(50);
  decrPIXARR_looper(50);
  
  initPIXARR_snakes(4, 5);
  incrPIXARR_looper(50);
  decrPIXARR_looper(50);

  PIXARR_toWhite();
  scdisplay_rainbow(1600);
}





