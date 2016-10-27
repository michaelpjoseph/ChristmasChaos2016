// Mike Joseph - michaelpjoseph@gmail.com - 2016.Oct.26
//  2016 Christmas Light display


#include <Adafruit_NeoPixel.h>
#define NUMPIX 50
#define LEDPIN 3



struct PIX {
  int R;
  int G;
  int B;
};
PIX PIXARR[NUMPIX];   // master array to be dumped to strand on each redraw

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIX, LEDPIN, NEO_GRB + NEO_KHZ800);

void strip_FlushAndDisplay() {
  for (int a = 0; a < NUMPIX; a++) {
    strip.setPixelColor( a, PIXARR[a].R, PIXARR[a].G, PIXARR[a].B);
  }
  strip.show();
}

void listPIXARR() {  // DEBUG - dump pixel array
  strip_FlushAndDisplay();
  for (int a = 0; a < NUMPIX; a++) {
    Serial.print(a); Serial.print(": "); Serial.print("\t"); Serial.print(PIXARR[a].R); Serial.print("\t"); Serial.print(PIXARR[a].G); Serial.print("\t"); Serial.println(PIXARR[a].B);
  }
}

void initPIXARR_rand() {
  for (int a = 0; a < NUMPIX; a++) {
    PIXARR[a] = { rand() % 255, rand() % 255, rand() % 255 };
  }
}

void initPIXARR_null() {
  for (int a = 0; a < NUMPIX; a++) {
    PIXARR[a] = { 0, 0, 0 };
  }
}


PIX rndSinglePIX_init() {
  PIX initPix_buffer;
  initPix_buffer.R = 0;
  int skew = rand() % 3;  // 0 - red | 1 - green | 2 - blue
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

void initPIXARR_snakes(int length, int snakes) {
  if ((snakes * length) >= NUMPIX) {  //check for crazy inputs and set fallback defaults
    snakes = 1;
    length = 10;
  }

  int maxbrightness = 255;
  int currbrightness = 0;
  int section_increment = maxbrightness / length;
  int section_seperator = NUMPIX / snakes;

  for (int a = 0; a < NUMPIX; a++) {  // zero out array
    PIXARR[a] = { 0, 0, 0 };
  }

  for (int snake_start = 0; snake_start < NUMPIX; snake_start += section_seperator) {
    currbrightness = 0;
    for (int s = snake_start; s < (snake_start + length); s++) {
      currbrightness += section_increment;
      PIXARR[s] = { currbrightness, currbrightness, currbrightness };
    }
    PIXARR[snake_start + length] = { maxbrightness, maxbrightness, maxbrightness };
  }
}

void initPIXARR_sparse(int spacing) {
  initPIXARR_null();
  for (int a = 0; a < NUMPIX; a++) {
    if ((a % spacing) == 0) {
      PIX buffer = rndSinglePIX_init();
      PIXARR[a] = buffer;
    }
  }
}


void scdisplay_PopInFadeOut(unsigned int duration) {
  initPIXARR_null();
  int delaymax = 300;
  struct t_PFPIXARR {
    PIX color;
    int delay;
  };
  t_PFPIXARR PFPIXARR[NUMPIX];
  for (int i = 0; i < NUMPIX; i++) {
    PFPIXARR[i].color.R = 0;
    PFPIXARR[i].color.G = 0;
    PFPIXARR[i].color.B = 0;
    PFPIXARR[i].delay = rand() % delaymax;
  }
  for (int a = 0; a <= duration; a++) {  //--- main display loop
    for (int i = 0; i < NUMPIX; i++) {
      if (PFPIXARR[i].color.R > 0 || PFPIXARR[i].color.G > 0 || PFPIXARR[i].color.B > 0) { // decay lit pixels
        if (PFPIXARR[i].color.R > 0) PFPIXARR[i].color.R--;
        if (PFPIXARR[i].color.G > 0) PFPIXARR[i].color.G--;
        if (PFPIXARR[i].color.B > 0) PFPIXARR[i].color.B--;
        PIXARR[i] = PFPIXARR[i].color;
      } else if (PFPIXARR[i].delay <= 0 && PFPIXARR[i].color.R <= 0 && PFPIXARR[i].color.G <= 0 && PFPIXARR[i].color.B <= 0) {  // time to re-initialize pixel
        PFPIXARR[i].color.R = rand() % 255;
        PFPIXARR[i].color.G = rand() % 255;
        PFPIXARR[i].color.B = rand() % 255;
        PFPIXARR[i].delay = rand() % delaymax;
        PIXARR[i] = PFPIXARR[i].color;
      } else {  // decrement delay - unlit pixel for now...
        PFPIXARR[i].delay--;
      }
    }
    strip_FlushAndDisplay();
    delay(15);
    //system("sleep .005");
    //system("cls");
  }
}

void scdisplay_PIXVector_snake(int length, int snakes, int duration) {
  if ((snakes * length) >= NUMPIX) {  //check for crazy inputs and set fallback defaults
    snakes = 2;
    length = 5;
  }
  //cout << ".1.\n";


  int maxbrightness = 255;
  int currbrightness = 0;
  int section_increment = maxbrightness / length;
  int section_seperator = NUMPIX / snakes;


  // -- setup     10/12/2016 ->  can't move to global :/  has to be redefined in each scope
  struct s_PIXVECT {
    int R;
    int G;
    int B;
    int pos;
    int dir;
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


  int idx_L = 0; // snake placement indexes for even distribution
  int idx_R = (NUMPIX - 1);
  bool fwddir = true; // bool-alternate direction vectors
  for (int a = 0; a < snakes; a++) {
    currbrightness = 0;
    if (fwddir) {
      int currposition = idx_L;
      for (int b = 0; b < length; b++) {
        //cout << a << " " << b << "   forward\n";
        currbrightness += section_increment;
        PIXVECT[a][b].R = currbrightness;
        PIXVECT[a][b].G = currbrightness;
        PIXVECT[a][b].B = currbrightness;
        //        PIXVECT[a][b].R = a + 10;
        //        PIXVECT[a][b].G = a + 10;
        //        PIXVECT[a][b].B = a + 10;
        PIXVECT[a][b].pos = currposition;
        PIXVECT[a][b].dir = 1;
        currposition += 1;
      }
      idx_L += section_seperator;
    } else {
      int currposition = idx_R;
      for (int b = (length - 1) ; b >= 0; b--) {
        currbrightness += section_increment;
        PIXVECT[a][b].R = currbrightness;
        PIXVECT[a][b].G = currbrightness;
        PIXVECT[a][b].B = currbrightness;
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
  for (int i = 0; i <= duration; i++) {
    //cout << "---- " << i << " ----\n";
    initPIXARR_null();  //blank out the array
    for (int d = 0; d < snakes; d++) {  // draw the snakes to the array - outer object loop
      for (int e = 0; e < length; e++) {  // inner element loop
        if (PIXARR[PIXVECT[d][e].pos].R != 0) {  // add collision detection here if PIXAR[pos].r / g / b > 0
          PIXARR[PIXVECT[d][e].pos] = { 555, 555, 555 };
        } else {  // no collission
          PIXARR[PIXVECT[d][e].pos] = {PIXVECT[d][e].R, PIXVECT[d][e].G, PIXVECT[d][e].B };
        }

      }
    }
    //listPIXARR();
    
    strip_FlushAndDisplay();
    delay(100);
    for (int m = 0; m < snakes; m++) {  // move the snakes
      for (int e = 0; e < length; e++) {  // inner element loop
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


void scdisplay_PIXVector_linedots(int duration) {
  int maxbrightness = 255;
  int halfway = NUMPIX / 2;
  int dotsep = 3;   // decrease for more dots, increase for less
  int numdots = halfway / dotsep;

  // -- setup     10/12/2016 ->  can't move to global :/  has to be redefined in each scope
  struct s_PIXVECT {
    int R;
    int G;
    int B;
    int pos;
    int dir;
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
  int currbrightness = 0;
  int currposition = 0;
  int fadepixels = 3;
  int section_increment = maxbrightness / fadepixels;
  for (int a = 0; a < fadepixels; a++) {  // fade left line
    currbrightness += section_increment;
    PIXVECT[a].R = currbrightness;
    PIXVECT[a].G = currbrightness;
    PIXVECT[a].B = currbrightness;
    PIXVECT[a].pos = currposition;
    PIXVECT[a].dir = 1;
    currposition += 1;
  }
  for (int a = currposition; a < (halfway - fadepixels); a++) {  // bright center of left line
    PIXVECT[a].R = maxbrightness;
    PIXVECT[a].G = maxbrightness;
    PIXVECT[a].B = maxbrightness;
    PIXVECT[a].pos = currposition;
    PIXVECT[a].dir = 1;
    currposition += 1;
  }
  for (int a = currposition; a < halfway; a++) {  // bright center of left line
    currbrightness -= section_increment;
    PIXVECT[a].R = currbrightness;
    PIXVECT[a].G = currbrightness;
    PIXVECT[a].B = currbrightness;
    PIXVECT[a].pos = currposition;
    PIXVECT[a].dir = 1;
    currposition += 1;
  }



  // draw dot portion
  for (int a = (NUMPIX - 1) ; a >  halfway; a -= dotsep) {
    PIXVECT[a].R = maxbrightness;
    PIXVECT[a].G = maxbrightness;
    PIXVECT[a].B = maxbrightness;
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
  for (int i = 0; i <= duration; i++) {
    //system("cls");
    initPIXARR_null();  //blank out the array
    for (int d = 0; d < NUMPIX; d++) {  // draw the snakes to the array - outer object loop
      if (PIXVECT[d].dir != 0) {
        if (PIXARR[PIXVECT[d].pos].R != 0) {  // add collision detection here if PIXAR[pos].r / g / b > 0
          PIXARR[PIXVECT[d].pos] = { 999, 999, 999 };
        } else {  // no collission
          PIXARR[PIXVECT[d].pos] = {PIXVECT[d].R, PIXVECT[d].G, PIXVECT[d].B };
        }
      }
    }
    //listPIXARR();
    strip_FlushAndDisplay();
    //system("sleep .005");
    delay(25);
    for (int e = 0; e < NUMPIX; e++) {  // inner element loop
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


void incrPIXARR() {  // increment all pixels 1 position
  PIX buffer = PIXARR[0], hold;
  PIXARR[0] = PIXARR[NUMPIX - 1];
  for (int a = 1; a < (NUMPIX); a++) {
    hold = PIXARR[a];
    PIXARR[a] = buffer;
    buffer = hold;
  }
}


void decrPIXARR() { // decrement all pixels 1 position
  PIX buffer = PIXARR[(NUMPIX - 1)], hold;
  PIXARR[(NUMPIX - 1)] = PIXARR[0];
  for (int a = (NUMPIX - 2); a >= 0; a--) {
    hold = PIXARR[a];
    PIXARR[a] = buffer;
    buffer = hold;
  }
}


void incrPIXARR_looper(int times) {
  for (int a = 0; a < times; a++) {
    incrPIXARR();
    //listPIXARR();
    strip_FlushAndDisplay();
    //system("sleep .005");
    delay(100);
    //system("cls");
  }
}


void decrPIXARR_looper(int times) {
  for (int a = 0; a < times; a++) {
    decrPIXARR();
    //listPIXARR();
    strip_FlushAndDisplay();
    delay(100);
    //system("cls");
  }
}


void setup() {
  strip.begin();
  Serial.begin(9600);
}

void loop() {
  scdisplay_PopInFadeOut(1000);
  incrPIXARR_looper(10);
  decrPIXARR_looper(50);
  initPIXARR_sparse(10);
  incrPIXARR_looper(10);
  decrPIXARR_looper(50);
  initPIXARR_sparse(5);
  incrPIXARR_looper(50);
  decrPIXARR_looper(10);
  scdisplay_PIXVector_linedots(200);
  scdisplay_PIXVector_snake(8 , 8 , 162);  // loop multiples of NUMPIX for smooth animation
  incrPIXARR_looper(75);
  decrPIXARR_looper(75);
  initPIXARR_rand();
  incrPIXARR_looper(25);
  decrPIXARR_looper(25);
  initPIXARR_rand();
  incrPIXARR_looper(25);
  decrPIXARR_looper(25);
  initPIXARR_snakes(2, 10);
  incrPIXARR_looper(50);
  initPIXARR_snakes(4, 5);
  incrPIXARR_looper(50);
  decrPIXARR_looper(25);
}





