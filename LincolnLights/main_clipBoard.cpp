#include <Arduino.h>
#include <Nextion.h>
#include <FastLED.h>

//           PAGE 0 "splash"
void fwVerDisplayCB(void *ptr);
//           PAGE 1 "home"
void home_ledsBtnCB(void *ptr);
void home_settingsBtnCB(void *ptr);
//           PAGE 2 "hsl"
void hsl_dashCB(void *ptr);
void hsl_drvrCB(void *ptr);
void hsl_passCB(void *ptr);
void hsl_backCB(void *ptr);
void hsl_hslTabCB(void *ptr);
void hsl_colorTabCB(void *ptr);
void hsl_cctTabCB(void *ptr);
void hsl_paletteTabCB(void *ptr);
void hsl_efxTabCB(void *ptr);
void hsl_homeTabCB(void *ptr);
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
void color_drvrCB(void *ptr);
void color_passCB(void *ptr);
void color_backCB(void *ptr);
void color_hslTabCB(void *ptr);
void color_colorTabCB(void *ptr);
void color_cctTabCB(void *ptr);
void color_paletteTabCB(void *ptr);
void color_efxTabCB(void *ptr);
void color_homeTabCB(void *ptr);


uint32_t updateDash;
uint32_t updateDrvr;
uint32_t updatePass;
uint32_t updateBack;

uint8_t hue=0;
uint8_t sat=255;
uint8_t lvl=255;
uint8_t brightness=128;



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
NexDSButton hsl_drvr=NexDSButton(2,2,"bt6"); 
NexDSButton hsl_pass=NexDSButton(2,3,"bt7"); 
NexDSButton hsl_back=NexDSButton(2,4,"bt8"); 
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
NexPage color=NexPage(3,0,"hsl");
NexDSButton color_dash=NexDSButton(3,1,"bt5"); 
NexDSButton color_drvr=NexDSButton(3,2,"bt6"); 
NexDSButton color_pass=NexDSButton(3,3,"bt7"); 
NexDSButton color_back=NexDSButton(3,4,"bt8"); 
NexButton color_hslTab=NexButton(3,5,"b0");
NexButton color_colorTab=NexButton(3,6,"b1");
NexButton color_cctTab=NexButton(3,7,"b2");
NexButton color_paletteTab=NexButton(3,8,"b3");
NexButton color_efxTab=NexButton(3,9,"b4");
NexButton color_homeTab=NexButton(3,10,"b6");



char buffer[100] = {0};


NexTouch *nex_listen_list[] = 
{
    &fwVerDisplay,
    &home_ledsBtn,
    &home_settingsBtn, 
    &hsl_dash,
    &hsl_drvr,
    &hsl_pass,
    &hsl_back,
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
    NULL
};





//           PAGE 0 "splash"
void fwVerDisplayCB(void *ptr){
    dbSerialPrintln("fwVerDisplayPopCallback");
    fwVerDisplay.setText("WS2812 control f/w ver_0.1");
}
//           PAGE 1 "home"
void home_ledsBtnCB(void *ptr){
    hsl.show();
}

void home_settingsBtnCB(void *ptr){
    //settings.show();
}
//           PAGE 2 "hsl"
void hsl_dashCB(void *ptr){
    dbSerialPrintln("hsl_dashCB");
    hsl_dash.getValue(&updateDash);
}

void hsl_drvrCB(void *ptr){
    dbSerialPrintln("hsl_drvrCB");
    hsl_drvr.getValue(&updateDrvr);   
}

void hsl_passCB(void *ptr){;
    dbSerialPrintln("hsl_passCB");
    hsl_pass.getValue(&updatePass);   
}

void hsl_backCB(void *ptr){
    dbSerialPrintln("hsl_backCB");
    hsl_back.getValue(&updateBack); 
}

void hsl_hslTabCB(void *ptr){
    //dont use / not needed
}

void hsl_colorTabCB(void *ptr){
    //color.show();
}

void hsl_cctTabCB(void *ptr){
    //cct.show();
}

void hsl_paletteTabCB(void *ptr){
    //palette.show();
}

void hsl_efxTabCB(void *ptr){
    //efx.show;
}

void hsl_homeTabCB(void *ptr){
    //home.show();
}

void hsl_onCB(void *ptr){
    //FastLED.setBrightness(brightness);
}

void hsl_offCB(void *ptr){
    //FastLED.setBrightness(0);
}

void hsl_huePlus1CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_huePlus1CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 1;
    if(number>255)number=255;
    if(number<0)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer);    
}

void hsl_huePlus5CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_huePlus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>255)number=255;
    if(number<0)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer); 
}

void hsl_hueMinus1CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_hueMinus1CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 1;
    if(number>255)number=255;
    if(number<0)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer);   
}

void hsl_hueMinus5CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_hueMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_hueVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>255)number=255;
    if(number<0)number=0;
    hue=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_hueVal.setText(buffer);   
}

void hsl_satPlus5CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_satPlus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_satVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>100)number=100;
    if(number<0)number=0;
    sat=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_satVal.setText(buffer);
}

void hsl_satMinus5CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_satMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_satVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>100)number=100;
    if(number<0)number=0;
    sat=number;
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_satVal.setText(buffer);
}

void hsl_lvlPlus5CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_lvlPlus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number += 5;
    if(number>100)number=100;
    if(number<0)number=0;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_lvlVal.setText(buffer);
}

void hsl_lvlMinus5CB(void *ptr){
    uint16_t number;    
    dbSerialPrintln("hsl_lvlMinus5CB");
    memset(buffer, 0, sizeof(buffer));
    hsl_lvlVal.getText(buffer, sizeof(buffer));  
    number = atoi(buffer);
    number -= 5;
    if(number>100)number=100;
    if(number<0)number=0;
    lvl=map(number,0,100,0,255);
    memset(buffer, 0, sizeof(buffer));
    itoa(number, buffer, 10);  
    hsl_lvlVal.setText(buffer);
}


//           PAGE 3 "color"
void color_dashCB(void *ptr){
    dbSerialPrintln("color_dashCB");
    color_dash.getValue(&updateDash);
}

void color_drvrCB(void *ptr){
    dbSerialPrintln("color_drvrCB");
    color_drvr.getValue(&updateDrvr);   
}

void color_passCB(void *ptr){;
    dbSerialPrintln("color_passCB");
    color_pass.getValue(&updatePass);   
}

void color_backCB(void *ptr){
    dbSerialPrintln("color_backCB");
    color_back.getValue(&updateBack); 
}

void color_hslTabCB(void *ptr){
    //hsl.show();
}    

void color_colorTabCB(void *ptr){
    //dont use / not needed
}

void color_cctTabCB(void *ptr){
    //cct.show();
}

void color_paletteTabCB(void *ptr){
    //palette.show();
}

void color_efxTabCB(void *ptr){
    //efx.show;
}

void color_homeTabCB(void *ptr){
    //home.show();
}


void setup(void){

    nexInit();
//           PAGE 0 "splash"
    fwVerDisplay.attachPush(fwVerDisplayCB,&fwVerDisplay);
//           PAGE 1 "home"    
    home_ledsBtn.attachPush(home_ledsBtnCB,&home_ledsBtn);
    home_settingsBtn.attachPush(home_settingsBtnCB,&home_settingsBtn);
//           PAGE 2 "hsl"    
    hsl_dash.attachPush(hsl_dashCB,&hsl_dash);
    hsl_drvr.attachPush(hsl_drvrCB,&hsl_drvr);
    hsl_pass.attachPush(hsl_passCB,&hsl_pass);
    hsl_back.attachPush(hsl_backCB,&hsl_back);
    hsl_hslTab.attachPush(hsl_hslTabCB,&hsl_hslTab);
    hsl_colorTab.attachPush(hsl_colorTabCB,&hsl_colorTab);
    hsl_cctTab.attachPush(hsl_cctTabCB,&hsl_cctTab);
    hsl_paletteTab.attachPush(hsl_paletteTabCB,&hsl_paletteTab);
    hsl_efxTab.attachPush(hsl_efxTabCB,&hsl_efxTab);
    hsl_homeTab.attachPush(hsl_homeTabCB,&hsl_homeTab);
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




    dbSerialPrintln("setup done");
}


void loop(void){
    //FastLED.setBrightness(brightness);
    //FastLED.Show();
    nexLoop(nex_listen_list);
}/////    LOOP END


