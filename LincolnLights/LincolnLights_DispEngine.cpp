#include <Arduino.h>
#include <Nextion.h>

// Use qsuba for smooth pixel colouring and qsubd for non-smooth pixel colouring
#define qsubd(x, b)  ((x>b)?b:0)                              // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Analog Unsigned subtraction macro. if result <0, then => 0


#include <FastLED.h>

#define NUM_LEDS 38
#define NUM_LEDS_DASH 100
#define NUM_LEDS_ROW2 100
#define NUM_LEDS_ROW3 61
#define NUM_LEDS_CNTR 100
#define NUM_LEDS_BACK 100
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB

CRGB dash[NUM_LEDS_DASH];
CRGB row2[NUM_LEDS_ROW2];
CRGB row3[NUM_LEDS_ROW3];
CRGB back[NUM_LEDS_BACK];
CRGB cntr[NUM_LEDS_CNTR];

// Set up a CHSV color
CHSV paleBlue( 160, 128, 255);

#define DASH_LED_PIN 5
#define ROW2_LED_PIN 18
#define ROW3_LED_PIN 19
#define BACK_LED_PIN 21
#define CNTR_LED_PIN 22
#define DATA_LED  2
// #define BRIGHTNESS  255
#define UPDATES_PER_SECOND 8
#define TEMPERATURE_1 Tungsten100W
#define TEMPERATURE_2 OvercastSky
#define FRAMES_PER_SECOND 1000
// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//
//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

//           UNIVERSAL CALLBACKS
void homeTabCB(void *ptr);

//           PAGE 0 "splash"
void fwVerDisplayCB(void *ptr);
//           PAGE 1 "home"
void home_ledsBtnCB(void *ptr);
void home_settingsBtnCB(void *ptr);
//           PAGE 2 "hsl"
void hsl_dashCB(void *ptr);
void hsl_row2CB(void *ptr);
void hsl_row3CB(void *ptr);
void hsl_backCB(void *ptr);
void hsl_cntrCB(void *ptr);
void hsl_hslTabCB(void *ptr);
void hsl_colorTabCB(void *ptr);
void hsl_cctTabCB(void *ptr);
void hsl_paletteTabCB(void *ptr);
void hsl_efxTabCB(void *ptr);
void hsl_onCB(void *ptr);
void hsl_offCB(void *ptr);
void hsl_huePlus1CB(void *ptr);
void hsl_huePlus5CB(void *ptr);
void hsl_hueMinus1CB(void *ptr);
void hsl_hueMinus5CB(void *ptr);
void hsl_satPlus5CB(void *ptr);
void hsl_satMinus5CB(void *ptr);
void hsl_lvlPlus5CB(void *ptr);
void hsl_lvlMinus5CB(void *ptr);
//           PAGE 3 "color"
void color_dashCB(void *ptr);
void color_row2CB(void *ptr);
void color_row3CB(void *ptr);
void color_backCB(void *ptr);
void color_cntrCB(void *ptr);
void color_hslTabCB(void *ptr);
void color_colorTabCB(void *ptr);
void color_cctTabCB(void *ptr);
void color_paletteTabCB(void *ptr);
void color_efxTabCB(void *ptr);
void color_hue0CB(void *ptr);
void color_hue16CB(void *ptr);
void color_hue32CB(void *ptr);
void color_hue56CB(void *ptr);
void color_hue80CB(void *ptr);
void color_hue96CB(void *ptr);
void color_hue112CB(void *ptr);
void color_hue128CB(void *ptr);
void color_hue144CB(void *ptr);
void color_hue160CB(void *ptr);
void color_hue176CB(void *ptr);
void color_hue192CB(void *ptr);
void color_hue234CB(void *ptr);
void color_onCB(void *ptr);
void color_offCB(void *ptr);
void color_lvlPlus5CB(void *ptr);
void color_lvlMinus5CB(void *ptr);

//           PAGE 4 "cct"
void cct_dashCB(void *ptr);
void cct_row2CB(void *ptr);
void cct_row3CB(void *ptr);
void cct_backCB(void *ptr);
void cct_cntrCB(void *ptr);
void cct_hslTabCB(void *ptr);
void cct_colorTabCB(void *ptr);
void cct_cctTabCB(void *ptr);
void cct_paletteTabCB(void *ptr);
void cct_efxTabCB(void *ptr);
void cct_warmCB(void *ptr);
void cct_neutCB(void *ptr);
void cct_coolCB(void *ptr);
void cct_bluSkyCB(void *ptr);
void cct_onCB(void *ptr);
void cct_offCB(void *ptr);
void cct_lvlPlus5CB(void *ptr);
void cct_lvlMinus5CB(void *ptr);

//           PAGE 5 "palette"
void palette_dashCB(void *ptr);
void palette_row2CB(void *ptr);
void palette_row3CB(void *ptr);
void palette_backCB(void *ptr);
void palette_cntrCB(void *ptr);
void palette_hslTabCB(void *ptr);
void palette_colorTabCB(void *ptr);
void palette_cctTabCB(void *ptr);
void palette_paletteTabCB(void *ptr);
void palette_efxTabCB(void *ptr);

//           PAGE 6 "efx"
void efx_dashCB(void *ptr);
void efx_row2CB(void *ptr);
void efx_row3CB(void *ptr);
void efx_backCB(void *ptr);
void efx_cntrCB(void *ptr);
void efx_hslTabCB(void *ptr);
void efx_colorTabCB(void *ptr);
void efx_cctTabCB(void *ptr);
void efx_paletteTabCB(void *ptr);
void efx_efxTabCB(void *ptr);

bool cctOn=0;
bool randomOn=0;
bool gReverseDirection = false;

int pos;
int col;
int level;
int gHue=0;
int ctp=0;

uint32_t updateDash;
uint32_t updateRow2;
uint32_t updateRow3;
uint32_t updateBack;
uint32_t updateCntr;

uint8_t hue=32;
uint8_t hueDash;
uint8_t hueRow2;
uint8_t hueRow3;
uint8_t hueBack;
uint8_t hueCntr;

uint8_t sat=180;
uint8_t satDash;
uint8_t satRow2;
uint8_t satRow3;
uint8_t satBack;
uint8_t satCntr;

uint8_t lvl=128;
uint8_t lvlDash;
uint8_t lvlRow2;
uint8_t lvlRow3;
uint8_t lvlBack;
uint8_t lvlCntr;

uint8_t ledGlobalLvl=128;

int colorTempProfile[]={
    Candle, Tungsten40W, Tungsten100W, Halogen, CarbonArc,
    HighNoonSun, DirectSunlight, OvercastSky, ClearBlueSky,
    WarmFluorescent, StandardFluorescent, CoolWhiteFluorescent,
    FullSpectrumFluorescent, GrowLightFluorescent, BlackLightFluorescent,
    MercuryVapor, SodiumVapor, MetalHalide, HighPressureSodium, UncorrectedTemperature
};

CRGBPalette16 currentPalette;                                 // Palette definitions
CRGBPalette16 targetPalette;
TBlendType currentBlending = LINEARBLEND;

//           PAGE 0 "splash"
NexText fwVerDisplay = NexText(0,3,"t0");
//           PAGE 1 "home"
NexPage home=NexPage(1,0,"home");
NexButton home_ledsBtn=NexButton(1,1,"b0");
NexButton home_settingsBtn=NexButton(1,3,"b1");
NexNumber home_satVal=NexNumber(1,8,"n0");
NexNumber home_lvlVal=NexNumber(1,9,"n1");
NexText home_colorEfxVal=NexText(1,10,"t4");
//           PAGE 2 "hsl"
NexPage hsl=NexPage(2,0,"hsl");
NexDSButton hsl_dash=NexDSButton(2,1,"bt5"); 
NexDSButton hsl_row2=NexDSButton(2,2,"bt6"); 
NexDSButton hsl_row3=NexDSButton(2,3,"bt7"); 
NexDSButton hsl_back=NexDSButton(2,4,"bt8"); 
NexDSButton hsl_cntr=NexDSButton(2,38,"bt0"); 
NexButton hsl_hslTab=NexButton(2,5,"b0");
NexButton hsl_colorTab=NexButton(2,6,"b1");
NexButton hsl_cctTab=NexButton(2,7,"b2");
NexButton hsl_paletteTab=NexButton(2,8,"b3");
NexButton hsl_efxTab=NexButton(2,9,"b4");
NexButton hsl_homeTab=NexButton(2,10,"b6");
NexButton hsl_on=NexButton(2,11,"b5");
NexButton hsl_off=NexButton(2,12,"b7");
NexButton hsl_huePlus1=NexButton(2,13,"b8");
NexButton hsl_huePlus5=NexButton(2,14,"b9");
NexButton hsl_hueMinus1=NexButton(2,15,"b10");
NexButton hsl_hueMinus5=NexButton(2,16,"b11");
NexButton hsl_satPlus5=NexButton(2,17,"b12");
NexButton hsl_satMinus5=NexButton(2,18,"b13");
NexButton hsl_lvlPlus5=NexButton(2,19,"b14");
NexButton hsl_lvlMinus5=NexButton(2,20,"b15");
NexText hsl_hueVal=NexText(2,22,"t1");
NexText hsl_satVal=NexText(2,24,"t3");
NexText hsl_lvlVal=NexText(2,26,"t5");

//           PAGE 3 "color"
NexPage color=NexPage(3,0,"color");
NexDSButton color_dash=NexDSButton(3,1,"bt5"); 
NexDSButton color_row2=NexDSButton(3,2,"bt6"); 
NexDSButton color_row3=NexDSButton(3,3,"bt7"); 
NexDSButton color_back=NexDSButton(3,4,"bt8"); 
NexDSButton color_cntr=NexDSButton(3,34,"bt0");
NexButton color_hslTab=NexButton(3,5,"b0");
NexButton color_colorTab=NexButton(3,6,"b1");
NexButton color_cctTab=NexButton(3,7,"b2");
NexButton color_paletteTab=NexButton(3,8,"b3");
NexButton color_efxTab=NexButton(3,9,"b4");
NexButton color_homeTab=NexButton(3,10,"b6");
NexButton color_hue0=NexButton(3,11,"b5");
NexButton color_hue16=NexButton(3,12,"b7");
NexButton color_hue32=NexButton(3,13,"b8");
NexButton color_hue56=NexButton(3,14,"b9");
NexButton color_on=NexButton(3,15,"b10");
NexButton color_hue80=NexButton(3,16,"b11");
NexButton color_hue96=NexButton(3,17,"b12");
NexButton color_hue112=NexButton(3,18,"b13");
NexButton color_hue128=NexButton(3,19,"b14");
NexButton color_hue144=NexButton(3,20,"b15");
NexButton color_hue160=NexButton(3,21,"b16");
NexButton color_hue176=NexButton(3,22,"b17");
NexButton color_hue192=NexButton(3,23,"b18");
NexButton color_hue234=NexButton(3,24,"b19");
NexButton color_off=NexButton(3,25,"b20");
NexButton color_lvlPlus5=NexButton(3,26,"b21");
NexButton color_lvlMinus5=NexButton(3,27,"b22");
NexText color_lvlVal=NexText(3,28,"t0");


//           PAGE 4 "cct"
NexPage cct=NexPage(4,0,"cct");
NexDSButton cct_dash=NexDSButton(4,1,"bt5");
NexDSButton cct_row2=NexDSButton(4,2,"bt6");
NexDSButton cct_row3=NexDSButton(4,3,"bt7");
NexDSButton cct_back=NexDSButton(4,4,"bt8");
NexDSButton cct_cntr=NexDSButton(4,23,"bt0");
NexDSButton cct_hslTab=NexDSButton(4,5,"b0");
NexButton cct_colorTab=NexButton(4,6,"b1");
NexButton cct_cctTab=NexButton(4,7,"b2");
NexButton cct_paletteTab=NexButton(4,8,"b3");
NexButton cct_efxTab=NexButton(4,9,"b4");
NexButton cct_homeTab=NexButton(4,10,"b6");
NexButton cct_warm=NexButton(4,11,"b5");
NexButton cct_neut=NexButton(4,12,"b7");
NexButton cct_cool=NexButton(4,14,"b9");
NexButton cct_bluSky=NexButton(3,15,"b10");
NexButton cct_on=NexButton(4,13,"b8");
NexButton cct_off=NexButton(4,21,"b11");
NexButton cct_lvlPlus5=NexButton(4,16,"b12");
NexButton cct_lvlMinus5=NexButton(4,17,"b13");
NexText cct_lvlVal=NexText(4,18,"t0");


//           PAGE 5 "palette"
NexPage palette=NexPage(5,0,"palette");
NexDSButton palette_dash=NexDSButton(5,1,"bt5");
NexDSButton palette_row2=NexDSButton(5,2,"bt6");
NexDSButton palette_row3=NexDSButton(5,3,"bt7");
NexDSButton palette_back=NexDSButton(5,4,"bt8");
NexDSButton palette_cntr=NexDSButton(5,28,"bt0");
NexDSButton palette_hslTab=NexDSButton(5,5,"b0");
NexButton palette_colorTab=NexButton(5,6,"b1");
NexButton palette_cctTab=NexButton(5,7,"b2");
NexButton palette_paletteTab=NexButton(5,8,"b3");
NexButton palette_efxTab=NexButton(5,9,"b4");
NexButton palette_homeTab=NexButton(5,10,"b6");

//           PAGE 6 "efx"
NexPage efx=NexPage(6,0,"efx");
NexDSButton efx_dash=NexDSButton(6,1,"bt5");
NexDSButton efx_row2=NexDSButton(6,2,"bt6");
NexDSButton efx_row3=NexDSButton(6,3,"bt7");
NexDSButton efx_back=NexDSButton(6,4,"bt8");
NexDSButton efx_cntr=NexDSButton(6,28,"bt0");
NexDSButton efx_hslTab=NexDSButton(6,5,"b0");
NexButton efx_colorTab=NexButton(6,6,"b1");
NexButton efx_cctTab=NexButton(6,7,"b2");
NexButton efx_paletteTab=NexButton(6,8,"b3");
NexButton efx_efxTab=NexButton(6,9,"b4");
NexButton efx_homeTab=NexButton(6,10,"b6");

//           PAGE 6 "efx"
NexPage settings=NexPage(7,0,"settings");
NexButton settings_homeBtn=NexButton(7,2,"b0");







char buffer[100] = {0};


NexTouch *nex_listen_list[] = 
{
    &fwVerDisplay,

    &home_ledsBtn,
    &home_settingsBtn, 

    &hsl_dash,
    &hsl_row2,
    &hsl_row3,
    &hsl_back,
    &hsl_cntr,
    &hsl_hslTab,
    &hsl_colorTab,
    &hsl_cctTab,
    &hsl_paletteTab,
    &hsl_efxTab,
    &hsl_homeTab,
    &hsl_on,
    &hsl_off,
    &hsl_huePlus1,
    &hsl_huePlus5,
    &hsl_hueMinus1,
    &hsl_hueMinus5,
    &hsl_satPlus5,
    &hsl_satMinus5,
    &hsl_lvlPlus5,
    &hsl_lvlMinus5,

    &color_dash,
    &color_row2,
    &color_row3,
    &color_back,
    &color_cntr,
    &color_hslTab,
    &color_colorTab,
    &color_cctTab,
    &color_paletteTab,
    &color_efxTab,
    &color_homeTab, 
    &color_hue0,
    &color_hue16,
    &color_hue32,
    &color_hue56,
    &color_on,
    &color_hue80,
    &color_hue96,
    &color_hue112,
    &color_hue128,
    &color_hue144,
    &color_hue160,
    &color_hue176,
    &color_hue192,
    &color_hue234,
    &color_off,
    &color_lvlPlus5,
    &color_lvlMinus5, 

    &cct_dash,
    &cct_row2,
    &cct_row3,
    &cct_back,
    &cct_cntr,
    &cct_hslTab,
    &cct_colorTab,
    &cct_cctTab,
    &cct_paletteTab,
    &cct_efxTab,
    &cct_homeTab,  
    &cct_warm,
    &cct_neut,
    &cct_cool,
    &cct_bluSky,
    &cct_on,
    &cct_off,
    &cct_lvlPlus5,
    &cct_lvlMinus5,   

    &palette_dash,
    &palette_row2,
    &palette_row3,
    &palette_back,
    &palette_cntr,
    &palette_hslTab,
    &palette_colorTab,
    &palette_cctTab,
    &palette_paletteTab,
    &palette_efxTab,
    &palette_homeTab,

    &efx_dash,
    &efx_row2,
    &efx_row3,
    &efx_back,
    &efx_cntr,
    &efx_hslTab,
    &efx_colorTab,
    &efx_cctTab,
    &efx_paletteTab,
    &efx_efxTab,
    &efx_homeTab,     

    &settings_homeBtn,  

    NULL
};


void bpm(){
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    back[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}


void juggle(){
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( dash, NUM_LEDS, 200);
  fadeToBlackBy( back, NUM_LEDS, 200);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    dash[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    back[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}


void Fire2012(){
// Array of temperature readings at each simulation cell
  static byte heat[NUM_LEDS_ROW3];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_LEDS_ROW3; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS_ROW3) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_LEDS_ROW3 - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_LEDS_ROW3; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS_ROW3-1) - j;
      } else {
        pixelnumber = j;
      }
      row3[pixelnumber] = color;
    }
}

void resetUpdateFlags(){
    updateDash=0;
    updateRow2=0;
    updateRow3=0;
    updateBack=0;
    updateCntr=0;
}

//           UNIVERSAL CALLBACKS 
void homeTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    home.show();
    memset(buffer, 0, sizeof(buffer));
    itoa(hueDash, buffer, 10);      
    home_colorEfxVal.setText(buffer);  
    delay(3);   
    uint16_t mSat=map(satDash,0,255,0,100);
    home_satVal.setValue(mSat); 
    delay(3);
    uint16_t mLvl=map(lvlDash,0,255,0,100);
    home_lvlVal.setValue(mLvl); 
    // delay(3);
    // home_colorEfxVal.Set_background_color_bco(hue);         
    resetUpdateFlags();
}


//           PAGE 0 "splash"
void fwVerDisplayCB(void *ptr){
    digitalWrite(DATA_LED,1);
    dbSerialPrintln("fwVerDisplayPopCallback");
    fwVerDisplay.setText("WS2812 control f/w ver_0.1");
}
//           PAGE 1 "home"
void home_ledsBtnCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl.show();
}

void home_settingsBtnCB(void *ptr){
    digitalWrite(DATA_LED,1);
    settings.show();
}
//           PAGE 2 "hsl"
void hsl_dashCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl_dash.getValue(&updateDash);
    dbSerialPrint("hsl_dashCB");
    dbSerialPrintln(updateDash);    
}

void hsl_row2CB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl_row2.getValue(&updateRow2);   
    dbSerialPrint("hsl_row2CB");
    dbSerialPrintln(updateRow2);    
}

void hsl_row3CB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl_row3.getValue(&updateRow3);   
    dbSerialPrint("hsl_row3CB");
    dbSerialPrintln(updateRow3);
}

void hsl_backCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl_back.getValue(&updateBack); 
    dbSerialPrint("hsl_backCB");
    dbSerialPrintln(updateBack);    
}

void hsl_cntrCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl_cntr.getValue(&updateCntr); 
    dbSerialPrint("hsl_cntrCB");
    dbSerialPrintln(updateCntr);    
}

void hsl_hslTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cctOn=0;
    //dont use / not needed
}

void hsl_colorTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    color.show();
    resetUpdateFlags();
}

void hsl_cctTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct.show();
    resetUpdateFlags();
}

void hsl_paletteTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette.show();
    resetUpdateFlags();
}

void hsl_efxTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx.show();
    resetUpdateFlags();
}

void hsl_onCB(void *ptr){
    digitalWrite(DATA_LED,1);
    ledGlobalLvl=128;
}

void hsl_offCB(void *ptr){
    digitalWrite(DATA_LED,1);
    ledGlobalLvl=0;
}

void hsl_huePlus1CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_huePlus1CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 1;
    if(number>255)number=255;
    // if(number<0)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer);    
}

void hsl_huePlus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_huePlus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>255)number=255;
    // if(number<0)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer); 
}

void hsl_hueMinus1CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_hueMinus1CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 1;
    if(number>255&&number<=65535)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer);   
}

void hsl_hueMinus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_hueMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>255&&number<=65535)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer);   
}

void hsl_satPlus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_satPlus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_satVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>100)number=100;
    // if(number<0)number=0;
    sat=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_satVal.setText(buffer);
}

void hsl_satMinus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_satMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_satVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>100&&number<=65535)number=0;
    sat=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_satVal.setText(buffer);
}

void hsl_lvlPlus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_lvlPlus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>100)number=100;
    // if(number<0)number=0;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_lvlVal.setText(buffer);
}

void hsl_lvlMinus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("hsl_lvlMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>100&&number<=65535)number=0;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_lvlVal.setText(buffer);
}


//           PAGE 3 "color"
void color_dashCB(void *ptr){
    digitalWrite(DATA_LED,1);
    color_dash.getValue(&updateDash);
    dbSerialPrint("color_dashCB");
    dbSerialPrintln(updateDash);  
}

void color_row2CB(void *ptr){
    digitalWrite(DATA_LED,1);
    color_row2.getValue(&updateRow2);
    dbSerialPrint("color_row2CB");
    dbSerialPrintln(updateRow2);         
}

void color_row3CB(void *ptr){
    digitalWrite(DATA_LED,1);
    color_row3.getValue(&updateRow3);   
    dbSerialPrint("color_row3CB");
    dbSerialPrintln(updateRow3);      
}

void color_backCB(void *ptr){
    digitalWrite(DATA_LED,1);
    dbSerialPrintln("color_backCB");
    color_back.getValue(&updateBack); 
}

void color_cntrCB(void *ptr){
    digitalWrite(DATA_LED,1);   
    dbSerialPrintln("color_cntrCB");
    color_cntr.getValue(&updateCntr);   
}

void color_hslTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl.show();
    resetUpdateFlags();
    cctOn=0;
}    

void color_colorTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cctOn=0;
    //dont use / not needed
}

void color_cctTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct.show();
    resetUpdateFlags();
    cctOn=1;
}

void color_paletteTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette.show();
    resetUpdateFlags();
    cctOn=0;
}

void color_efxTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx.show();
    resetUpdateFlags();
    cctOn=0;
}

void color_hue0CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue0CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue0.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue16CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue16CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue16.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue32CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue32CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue32.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue56CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue56CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue56.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

// void color_hue64CB(void *ptr){
//     digitalWrite(DATA_LED,1);
//     uint16_t number;    
//     dbSerialPrintln("color_hue64CB");
//     memset(buffer, 0, sizeof(buffer));
//     color_hue64.getText(buffer, sizeof(buffer));  
//     number = atoi(buffer);
//     hue=number;
// }

void color_hue80CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue80CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue80.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue96CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue96CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue96.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue112CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue112CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue112.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue128CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue128CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue128.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue144CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue144CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue144.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue160CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue160CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue160.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue176CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue176CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue176.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue192CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue192CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue192.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_hue234CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_hue234CB");
    memset(buffer, 0, sizeof(buffer));
    color_hue234.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    hue=number;
    sat=255;
}

void color_offCB(void *ptr){
    digitalWrite(DATA_LED,1);
    ledGlobalLvl=0;
}

void color_onCB(void *ptr){
    digitalWrite(DATA_LED,1);
    ledGlobalLvl=128;
}

void color_lvlPlus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_lvlPlus5CB");
    memset(buffer, 0, sizeof(buffer));
    color_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>100)number=100;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    color_lvlVal.setText(buffer);
}

void color_lvlMinus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("color_lvlMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    color_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>100&&number<=65535)number=0;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    color_lvlVal.setText(buffer);
}





//           PAGE 4 "cct"
void cct_dashCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct_dash.getValue(&updateDash);
    dbSerialPrint("cct_dashCB");
    dbSerialPrintln(updateDash);     
}

void cct_row2CB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct_row2.getValue(&updateRow2); 
    dbSerialPrint("cct_row2CB");
    dbSerialPrintln(updateRow2);         
    
}

void cct_row3CB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct_row3.getValue(&updateRow3);
    dbSerialPrintln("cct_row3CB");     
}

void cct_backCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct_back.getValue(&updateBack); 
    dbSerialPrintln("cct_backCB");  
}

void cct_cntrCB(void *ptr){
    digitalWrite(DATA_LED,1);   
    dbSerialPrintln("cct_cntrCB");
    cct_cntr.getValue(&updateCntr);    
}

void cct_hslTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl.show();
    resetUpdateFlags();
    cctOn=0;
}

void cct_colorTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    color.show();
    resetUpdateFlags();
    cctOn=0;
}

void cct_cctTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cctOn=1;
    //dont use / not needed
}

void cct_paletteTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette.show();
    resetUpdateFlags();
    cctOn=0;
}

void cct_efxTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx.show();
    resetUpdateFlags();
    cctOn=0;
}

void cct_warmCB(void *ptr){
    ctp=1;
}

void cct_neutCB(void *ptr){
    ctp=2;
}

void cct_coolCB(void *ptr){
    ctp=11;
}

void cct_bluSkyCB(void *ptr){
    ctp=14;
}




void cct_offCB(void *ptr){
    digitalWrite(DATA_LED,1);
    ledGlobalLvl=0;
}

void cct_onCB(void *ptr){
    digitalWrite(DATA_LED,1);
    ledGlobalLvl=128;
}

void cct_lvlPlus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("cct_lvlPlus5CB");
    memset(buffer, 0, sizeof(buffer));
    cct_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>100)number=100;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    cct_lvlVal.setText(buffer);
}

void cct_lvlMinus5CB(void *ptr){
    digitalWrite(DATA_LED,1);
    uint16_t number;    
    dbSerialPrintln("cct_lvlMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    cct_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>100&&number<=65535)number=0;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    cct_lvlVal.setText(buffer);
}






//           PAGE 5 "palette"
void palette_dashCB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette_dash.getValue(&updateDash);
    dbSerialPrint("cct_dashCB");
    dbSerialPrintln(updateDash);       
}

void palette_row2CB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette_row2.getValue(&updateRow2); 
    dbSerialPrint("palette_row2CB");
    dbSerialPrintln(updateRow2);       
    
}

void palette_row3CB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette_row3.getValue(&updateRow3);
    dbSerialPrint("palette_row3CB");
    dbSerialPrintln(updateRow3);     
}

void palette_backCB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette_back.getValue(&updateBack); 
    dbSerialPrint("palette_backCB");
    dbSerialPrintln(updateBack);      
}

void palette_cntrCB(void *ptr){
    digitalWrite(DATA_LED,1);   
    dbSerialPrintln("palette_cntrCB");
    palette_cntr.getValue(&updateCntr);    
}

void palette_hslTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl.show();
    resetUpdateFlags();
    cctOn=0;
}

void palette_colorTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    color.show();
    resetUpdateFlags();
    cctOn=0;
}

void palette_cctTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct.show();
    resetUpdateFlags();
    cctOn=1;
}

void palette_paletteTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cctOn=0;
    //dont use / not needed
}

void palette_efxTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx.show();
    resetUpdateFlags();
    cctOn=0;
}

//           PAGE 6 "efx"
void efx_dashCB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx_dash.getValue(&updateDash);
    dbSerialPrint("efx_dashCB");
    dbSerialPrintln(updateDash);    
}

void efx_row2CB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx_row2.getValue(&updateRow2); 
    dbSerialPrint("efx_row2CB");
    dbSerialPrintln(updateRow2);     

}

void efx_row3CB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx_row3.getValue(&updateRow3);
    dbSerialPrint("efx_row3CB");
    dbSerialPrintln(updateRow3);     

}

void efx_backCB(void *ptr){
    digitalWrite(DATA_LED,1);
    efx_back.getValue(&updateBack); 
    dbSerialPrint("efx_backCB");
    dbSerialPrintln(updateBack);      
}

void efx_cntrCB(void *ptr){
    digitalWrite(DATA_LED,1);   
    dbSerialPrintln("efx_cntrCB");
    efx_cntr.getValue(&updateCntr);    
}

void efx_hslTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    hsl.show();
    resetUpdateFlags();
    cctOn=0;
}

void efx_colorTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    color.show();
    resetUpdateFlags();
    cctOn=0;
}

void efx_cctTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    cct.show();
    resetUpdateFlags();
    cctOn=1;
}

void efx_paletteTabCB(void *ptr){
    digitalWrite(DATA_LED,1);
    palette.show();
    resetUpdateFlags();
    cctOn=0;
}

void efx_efxTabCB(void *ptr){
    cctOn=0;
    //dont use / not needed
}


void plasma() {                                                 // This is the heart of this program. Sure is short. . . and fast.

  int thisPhase = beatsin8(6,-64,64);                           // Setting phase change for a couple of waves.
  int thatPhase = beatsin8(7,-64,64);

  for (int k=0; k<NUM_LEDS_ROW2; k++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:

    int colorIndex = cubicwave8((k*23)+thisPhase)/2 + cos8((k*15)+thatPhase)/2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
    int thisBright = qsuba(colorIndex, beatsin8(7,0,96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..

    row2[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);  // Let's now add the foreground colour.
    row3[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending); 
  }

  for (int L=0; L<NUM_LEDS_DASH; L++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:

    int colorIndex1 = cubicwave8((L*23)+thisPhase)/2 + cos8((L*15)+thatPhase)/2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
    int thisBright1 = qsuba(colorIndex1, beatsin8(7,0,96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..

    dash[L] = ColorFromPalette(currentPalette, colorIndex1, thisBright1, currentBlending);  // Let's now add the foreground colour.
    back[L] = ColorFromPalette(currentPalette, colorIndex1, thisBright1, currentBlending); 
  }  

} 





void setup(void){
    pinMode(DATA_LED,OUTPUT);

    nexInit();
//           PAGE 0 "splash"
    fwVerDisplay.attachPush(fwVerDisplayCB,&fwVerDisplay);
//           PAGE 1 "home"    
    home_ledsBtn.attachPush(home_ledsBtnCB,&home_ledsBtn);
    home_settingsBtn.attachPush(home_settingsBtnCB,&home_settingsBtn);
//           PAGE 2 "hsl"    
    hsl_dash.attachPush(hsl_dashCB,&hsl_dash);
    hsl_row2.attachPush(hsl_row2CB,&hsl_row2);
    hsl_row3.attachPush(hsl_row3CB,&hsl_row3);
    hsl_back.attachPush(hsl_backCB,&hsl_back);
    hsl_cntr.attachPush(hsl_cntrCB,&hsl_cntr);
    hsl_hslTab.attachPush(hsl_hslTabCB,&hsl_hslTab);
    hsl_colorTab.attachPush(hsl_colorTabCB,&hsl_colorTab);
    hsl_cctTab.attachPush(hsl_cctTabCB,&hsl_cctTab);
    hsl_paletteTab.attachPush(hsl_paletteTabCB,&hsl_paletteTab);
    hsl_efxTab.attachPush(hsl_efxTabCB,&hsl_efxTab);
    hsl_homeTab.attachPush(homeTabCB,&hsl_homeTab);
    hsl_on.attachPush(hsl_onCB,&hsl_on);
    hsl_off.attachPush(hsl_offCB,&hsl_off);
    hsl_huePlus1.attachPush(hsl_huePlus1CB,&hsl_huePlus1);
    hsl_huePlus5.attachPush(hsl_huePlus5CB,&hsl_huePlus5);
    hsl_hueMinus1.attachPush(hsl_hueMinus1CB,&hsl_hueMinus1);
    hsl_hueMinus5.attachPush(hsl_hueMinus5CB,&hsl_hueMinus5);
    hsl_satPlus5.attachPush(hsl_satPlus5CB,&hsl_satPlus5);
    hsl_satMinus5.attachPush(hsl_satMinus5CB,&hsl_satMinus5);
    hsl_lvlPlus5.attachPush(hsl_lvlPlus5CB,&hsl_lvlPlus5);
    hsl_lvlMinus5.attachPush(hsl_lvlMinus5CB,&hsl_lvlMinus5);
//           PAGE 3 "color"
    color_dash.attachPush(color_dashCB,&color_dash);
    color_row2.attachPush(color_row2CB,&color_row2);
    color_row3.attachPush(color_row3CB,&color_row3);
    color_back.attachPush(color_backCB,&color_back);
    color_cntr.attachPush(color_cntrCB,&color_cntr);
    color_hslTab.attachPush(color_hslTabCB,&color_hslTab);
    color_colorTab.attachPush(color_colorTabCB,&color_colorTab);
    color_cctTab.attachPush(color_cctTabCB,&color_cctTab);
    color_paletteTab.attachPush(color_paletteTabCB,&color_paletteTab);
    color_efxTab.attachPush(color_efxTabCB,&color_efxTab);
    color_homeTab.attachPush(homeTabCB,&color_homeTab);
    color_hue0.attachPush(color_hue0CB,&color_hue0);
    color_hue16.attachPush(color_hue16CB,&color_hue16);
    color_hue32.attachPush(color_hue32CB,&color_hue32);
    color_hue56.attachPush(color_hue56CB,&color_hue56);
    color_on.attachPush(color_onCB,&color_on);
    color_hue80.attachPush(color_hue80CB,&color_hue80);
    color_hue96.attachPush(color_hue96CB,&color_hue96);
    color_hue112.attachPush(color_hue112CB,&color_hue112);
    color_hue128.attachPush(color_hue128CB,&color_hue128);
    color_hue144.attachPush(color_hue144CB,&color_hue144);
    color_hue160.attachPush(color_hue160CB,&color_hue160);
    color_hue176.attachPush(color_hue176CB,&color_hue176);
    color_hue192.attachPush(color_hue192CB,&color_hue192);
    color_hue234.attachPush(color_hue234CB,&color_hue234);
    color_off.attachPush(color_offCB,&color_off);
    color_lvlPlus5.attachPush(color_lvlPlus5CB,&color_lvlPlus5);
    color_lvlMinus5.attachPush(color_lvlMinus5CB,&color_lvlMinus5);

//           PAGE 4 "cct"
    cct_dash.attachPush(cct_dashCB,&cct_dash);
    cct_row2.attachPush(cct_row2CB,&cct_row2);
    cct_row3.attachPush(cct_row3CB,&cct_row3);
    cct_back.attachPush(cct_backCB,&cct_back);
    cct_cntr.attachPush(cct_cntrCB,&cct_cntr);
    cct_hslTab.attachPush(cct_hslTabCB,&cct_hslTab);
    cct_colorTab.attachPush(cct_colorTabCB,&cct_colorTab);
    cct_cctTab.attachPush(cct_cctTabCB,&cct_cctTab);
    cct_paletteTab.attachPush(cct_paletteTabCB,&cct_paletteTab);
    cct_efxTab.attachPush(cct_efxTabCB,&cct_efxTab);
    cct_homeTab.attachPush(homeTabCB,&cct_homeTab);

    cct_warm.attachPush(cct_warmCB,&cct_warm);
    cct_neut.attachPush(cct_neutCB,&cct_neut);
    cct_cool.attachPush(cct_coolCB,&cct_cool);
    cct_bluSky.attachPush(cct_bluSkyCB,&cct_bluSky);
    cct_on.attachPush(cct_onCB,&cct_on);
    cct_off.attachPush(cct_offCB,&cct_off);
    cct_lvlPlus5.attachPush(cct_lvlPlus5CB,&cct_lvlPlus5);
    cct_lvlMinus5.attachPush(cct_lvlMinus5CB,&cct_lvlMinus5);   


//           PAGE 5 "palette"
    palette_dash.attachPush(palette_dashCB,&palette_dash);
    palette_row2.attachPush(palette_row2CB,&palette_row2);
    palette_row3.attachPush(palette_row3CB,&palette_row3);
    palette_back.attachPush(palette_backCB,&palette_back);
    palette_cntr.attachPush(palette_cntrCB,&palette_cntr);
    palette_hslTab.attachPush(palette_hslTabCB,&palette_hslTab);
    palette_colorTab.attachPush(palette_colorTabCB,&palette_colorTab);
    palette_cctTab.attachPush(palette_cctTabCB,&palette_cctTab);
    palette_paletteTab.attachPush(palette_paletteTabCB,&palette_paletteTab);
    palette_efxTab.attachPush(palette_efxTabCB,&palette_efxTab);
    palette_homeTab.attachPush(homeTabCB,&palette_homeTab);


//           PAGE 6 "efx"
    efx_dash.attachPush(efx_dashCB,&efx_dash);
    efx_row2.attachPush(efx_row2CB,&efx_row2);
    efx_row3.attachPush(efx_row3CB,&efx_row3);
    efx_back.attachPush(efx_backCB,&efx_back);
    efx_cntr.attachPush(efx_cntrCB,&efx_cntr);
    efx_hslTab.attachPush(efx_hslTabCB,&efx_hslTab);
    efx_colorTab.attachPush(efx_colorTabCB,&efx_colorTab);
    efx_cctTab.attachPush(efx_cctTabCB,&efx_cctTab);
    efx_paletteTab.attachPush(efx_paletteTabCB,&efx_paletteTab);
    efx_efxTab.attachPush(efx_efxTabCB,&efx_efxTab);
    efx_homeTab.attachPush(homeTabCB,&efx_homeTab);

//           PAGE 7 "settings"
    settings_homeBtn.attachPush(homeTabCB,&settings_homeBtn);




    delay( 3000 ); // power-up safety delay
    // It's important to set the color correction for your LED strip here,
    // so that colors can be more accurately rendered through the 'temperature' profiles
    FastLED.addLeds<CHIPSET, DASH_LED_PIN, COLOR_ORDER>(dash, NUM_LEDS_DASH).setCorrection( TypicalSMD5050 );
    FastLED.addLeds<CHIPSET, ROW2_LED_PIN, COLOR_ORDER>(row2, NUM_LEDS_ROW2).setCorrection( TypicalSMD5050 );
    FastLED.addLeds<CHIPSET, ROW3_LED_PIN, COLOR_ORDER>(row3, NUM_LEDS_ROW3).setCorrection( TypicalSMD5050 );
    FastLED.addLeds<CHIPSET, BACK_LED_PIN, COLOR_ORDER>(back, NUM_LEDS_BACK).setCorrection( TypicalSMD5050 );
    FastLED.addLeds<CHIPSET, CNTR_LED_PIN, COLOR_ORDER>(cntr, NUM_LEDS_CNTR).setCorrection( TypicalSMD5050 );
    currentPalette = OceanColors_p;
    FastLED.setTemperature(DirectSunlight);
    FastLED.setBrightness(ledGlobalLvl);    
    FastLED.clear(1); 

    // for(uint16_t i=65530;i<255;i++){
    //     int x=i%256;
    //     dbSerialPrint("i=");
    //     dbSerialPrint(i);
    //     dbSerialPrint("  ||  x=");
    //     dbSerialPrintln(x);
    // }
    dbSerialPrintln("setup done");
}


void loop(void){
        if(gHue==255)gHue=0;
        pos=beatsin8(15,1,29);
    col=beatsin8(5,0,255);
    level=beatsin8(30,0,255);
    if(updateDash){
        if(cctOn){
            fill_solid(dash,NUM_LEDS_DASH,colorTempProfile[ctp]);
        }else{
            hueDash=hue;satDash=sat;lvlDash=lvl;
            fill_solid(dash,NUM_LEDS_DASH,CHSV(hueDash,satDash,lvlDash));
        }          
    }      
    if(updateRow2){
        if(cctOn){
            fill_solid(row2,NUM_LEDS_ROW2,colorTempProfile[ctp]);
        }else{
            hueRow2=hue;satRow2=sat;lvlRow2=lvl;
            fill_solid(row2,NUM_LEDS_ROW2,CHSV(hueRow2,satRow2,lvlRow2));
        }            
    }    
    if(updateRow3){
        if(cctOn){
            fill_solid(row3,NUM_LEDS_ROW3,colorTempProfile[ctp]);
        }else{
            hueRow3=hue;satRow3=sat;lvlRow3=lvl;
            fill_solid(row3,NUM_LEDS_ROW3,CHSV(hueRow3,satRow3,lvlRow3));
        }                
    }    
    if(updateBack){
        if(cctOn){
            fill_solid(back,NUM_LEDS_BACK,colorTempProfile[ctp]);
        }else{
            hueBack=hue;satBack=sat;lvlBack=lvl;
            fill_solid(back,NUM_LEDS_BACK,CHSV(hueBack,satBack,lvlBack));
        }            
    }    
    if(updateCntr){
        if(cctOn){
            fill_solid(cntr,NUM_LEDS_CNTR,colorTempProfile[ctp]);
        }else{
            hueCntr=hue;satCntr=sat;lvlCntr=lvl;
            fill_solid(cntr,NUM_LEDS_CNTR,CHSV(hueCntr,satCntr,lvlCntr));
        }
    }
    // nscale8(row2,NUM_LEDS_ROW2,5);
    // EVERY_N_MILLISECONDS(500){
    //   randomOn=!randomOn;
    // }  

//   EVERY_N_MILLISECONDS(50) {                                  // FastLED based non-blocking delay to update/display the sequence.
//     plasma();
//   }

//   EVERY_N_MILLISECONDS(1000) {
//     dbSerialPrintln(LEDS.getFPS());                            // Optional check of our fps.
//   }

//   EVERY_N_MILLISECONDS(100) {
//     uint8_t maxChanges = 24; 
//     nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
//   }


//   EVERY_N_SECONDS(5) {                                 // Change the target palette to a random one every 5 seconds.
//     uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
//     targetPalette = CRGBPalette16(CHSV(baseC+random8(32), 192, random8(128,255)), CHSV(baseC+random8(32), 255, random8(128,255)), CHSV(baseC+random8(32), 192, random8(128,255)), CHSV(baseC+random8(32), 255, random8(128,255)));
//   }

    // fadeToBlackBy(row2,NUM_LEDS_ROW2,15);
    // row2[29-pos]=CHSV(col,255,255);
    // row2[30+pos]=CHSV(col,255,255);
    // if(randomOn==1){ 
    //   row2[29]=CHSV(0,255,level);
    //   row2[30]=CHSV(0,255,level);
    // }else{
    //   row2[29]=CRGB::Black;
    //   row2[30]=CRGB::Black;        
    // }
    // bpm();
    // juggle();
    // Fire2012();
    FastLED.setBrightness(ledGlobalLvl);
    FastLED.show();
    FastLED.delay(1000 / FRAMES_PER_SECOND);
    digitalWrite(DATA_LED,0);
    nexLoop(nex_listen_list);
    EVERY_N_MILLISECONDS(20){
        gHue++;
    } 
}/////    LOOP END


