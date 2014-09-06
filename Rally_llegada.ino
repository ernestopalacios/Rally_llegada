//#include <Time.h>
#include <stdarg.h>
#include <LiquidCrystal.h>
#include <LCDKeypad.h>

// Para trabajar con el RTC
#include <Wire.h>
#include "RTClib.h"

#define PIN_PULSO_1HZ       2

RTC_DS1307 rtc;       // Crea el objeto para RTC
Ds1307SqwPinMode mode;

LCDKeypad lcd;


////////// VARIABLES ///////////
volatile int numero=0;
int hora=0;
int minuto=0;
int segundo=0;
volatile int hora_i=0;
volatile int minuto_i=0;
volatile int segundo_i=0;
volatile int mili_segundo_i=0;   // Aumentado variable milisegundo

// Segundos previos para saber en que momento cambia la interrupcion
int _prev_segundo;
int _prev_minuto;
int _prev_hora;

int mili_segundo=0;
int hora2=0;
int minuto2=0;
int segundo2=0;
volatile int bandera=0;
int bandera2=0;
int bandera3=0;
String str_hora;
String str_minuto;
String str_segundo;
String str_hora_i;
String str_minuto_i;
String str_segundo_i;
//String str_mili_segundo_i;

String str_numero;
String str_mili_segundo;
char mili_segundo_char[3];
char hora_char[3];
char minuto_char[3];
char segundo_char[3];
int boton=0;
int boton2=0;
int boton3=0;
int buffer_hora;
int buffer_minuto;
int buffer_segundo;
int i=0;
int j=0;
char val[200];
char encabezado[8];
int k=0;

//////////////////////////////

   int bandera_IGUALAR_GPS = 0;
   int bandera_IGUALAR_RTC = 0;


//////////////////////////////


void setup(void)
{
   DateTime now;     // Objeto donde guardarla hora
   
   //***              INICIALIZACION DEL RTC            ***//
   Wire.begin();
   rtc.begin();
   delayMicroseconds(500);

   mode = SquareWave1HZ;
   delayMicroseconds(500);

   rtc.writeSqwPinMode(mode);

   if (! rtc.isrunning() ) 
   {
      Serial.println("RTC is NOT running!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
   }

   now = rtc.now();


   // HORA IGUALADA DESDE EL RTC EN ESTE INSTANTE!!!!
   hora = now.hour();
   minuto = now.minute();
   segundo = now.second();

   Serial.print(now.year(), DEC);
   Serial.print('/');
   Serial.print(now.month(), DEC);
   Serial.print('/');
   Serial.print(now.day(), DEC);
   Serial.print(' ');
   Serial.print(now.hour(), DEC);
   Serial.print(':');
   Serial.print(now.minute(), DEC);
   Serial.print(':');
   Serial.print(now.second(), DEC);
   Serial.println();


   lcd.begin(16, 2);
   lcd.clear();
   lcd.print("     TIEMPO");
   pinMode(3, INPUT);
   pinMode(11, INPUT);
   attachInterrupt(1, imprimir, RISING);
   Serial.begin(9600);
   

   // Abre la comunicacion con la impresora
   encabezado[0]=(1);encabezado[1]=('T');encabezado[2]=('Q');encabezado[3]=('-');encabezado[4]=('0');encabezado[5]=('0');encabezado[6]=('0');encabezado[7]=('0');

   // INTERRUPCION CADA SEGUNDO
   // El chip DS1307 dara un flanco positivo cada segundo exacto. Se usa este disparo para aumentar
   // la variable segundos y dibujar el tiempo en el dispolay.
   attachInterrupt(0, pulso, CHANGE); 
   
}

  
// Para aumentar los segundos con cada pulso del RTC   
void pulso(void){

   // El codigo dentro del IF se ejecuta cada segundo
   if( digitalRead(PIN_PULSO_1HZ) )
   {
       
      segundo++;

      if( segundo > 59){
         segundo = 0;
         minuto ++;

         if( minuto > 59 ){
            minuto = 0;
            hora++;

            if( hora > 23 ){
               hora = 0;
            }
         }
      }
   }
}


void imprimir (){
     if (boton2==1){
       bandera=1;
     }
     else{
       bandera=0;
     }

     // Toma las variables de Hora directamente 
     // desde las variables del Arduino.
     // Las cuales han sido igualdas deacuerdo al RTC/GPS
     hora_i=hora;
     minuto_i=minuto;
     segundo_i=segundo;
     mili_segundo_i = mili_segundo;
     
     
  }
  

void loop(void)
{
   
   //boton=digitalRead(3);
   boton2=digitalRead(11);
   //boton3=digitalRead(12);
   //
   // si el MILISEGUNDO esta en 100 ó si se ha actualizado el milisegundo
   if (mili_segundo>99 || _prev_segundo != segundo){
     
      mili_segundo=0;
      _prev_segundo =  segundo;
   }
   else{
      mili_segundo=mili_segundo+1;
      delay(5);
   }
   
   // SE IMPRIME O NO SE IMPRIME
   if(boton2==0 && bandera3==0){
      bandera=0;
      bandera3=1;
      lcd.setCursor(0,0);
      lcd.print("OFF");
   }
   else if(boton2==1 && bandera3==1){
     bandera3=0;
     lcd.setCursor(0,0);
     lcd.print("ON ");
   }
   
   
   // Se prepara los ASCII PARA IMPRIMIR
   if (bandera==1 && boton2==1){
     
     numero++;
     str_hora_i=String(hora_i);
     str_minuto_i=String(minuto_i);
     str_segundo_i=String(segundo_i);
     str_mili_segundo=String(mili_segundo_i);
     
     lcd.setCursor(3,0);
     if(hora_i<10){
       lcd.print("0"+str_hora_i);
     }
     else
       lcd.print(str_hora_i);
     lcd.setCursor(5,0);
     lcd.print(":");
     if (minuto_i<10){
       lcd.print("0"+str_minuto_i);
     }
     else
       lcd.print(str_minuto_i);
     lcd.setCursor(8,0);
     lcd.print(":");
     if (segundo_i<10){
       lcd.print("0"+str_segundo_i+"."+str_mili_segundo+"  ");
     }
     else
       lcd.print(str_segundo_i+"."+str_mili_segundo+"  ");
     
     Serial.print(encabezado);
     for(k=0;k<40;k++){
       delay(1);
     }
     //delay(40);
     Serial.print("LOJA");
     Serial.write(0);Serial.write(0);Serial.write(0);Serial.write(0);Serial.write(0);Serial.write(0);Serial.write(0);Serial.write(0);Serial.write(0);
     for(k=0;k<40;k++){
       delay(1);
     }
     //delay(40);
     Serial.write(13);
     for(k=0;k<40;k++){
       delay(1);
     }
     //delay(40);
//     Serial.print("FUNCIONO!\r\n");
//     delay(40);

     
     if (hora_i<10 && minuto_i<10 && segundo_i<10){
       Serial.print("T"+str_numero+": 0"+str_hora_i+":0"+str_minuto_i+":0"+str_segundo_i+"."+str_mili_segundo+"\r\n");
       
     }
     else if(hora_i<10 && minuto_i<10 && segundo_i>=10){
       Serial.print("T"+str_numero+": 0"+str_hora_i+":0"+str_minuto_i+":"+str_segundo_i+"."+str_mili_segundo+"\r\n");
       
     }
     else if(hora_i<10 && minuto_i>=10 && segundo_i>=10){
       Serial.print("T"+str_numero+": 0"+str_hora_i+":"+str_minuto_i+":"+str_segundo_i+"."+str_mili_segundo+"\r\n");
       
     }
     else if(hora_i<10 && minuto_i>=10 && segundo_i<10){
       Serial.print("T"+str_numero+": 0"+str_hora_i+":"+str_minuto_i+":0"+str_segundo_i+"."+str_mili_segundo+"\r\n");
      
     }
     if (hora_i>=10 && minuto_i<10 && segundo_i<10){
       Serial.print("T"+str_numero+": "+str_hora_i+":0"+str_minuto_i+":0"+str_segundo_i+"."+str_mili_segundo+"\r\n");
      
     }
     else if(hora_i>=10 && minuto_i<10 && segundo_i>=10){
       Serial.print("T"+str_numero+": "+str_hora_i+":0"+str_minuto_i+":"+str_segundo_i+"."+str_mili_segundo+"\r\n");
     
     }
     else if(hora_i>=10 && minuto_i>=10 && segundo_i>=10){
       Serial.print("T"+str_numero+": "+str_hora_i+":"+str_minuto_i+":"+str_segundo_i+"."+str_mili_segundo+"\r\n");
      
     }
     else if(hora_i>=10 && minuto_i>=10 && segundo_i<10){
       Serial.print("T"+str_numero+": "+str_hora_i+":"+str_minuto_i+":0"+str_segundo_i+"."+str_mili_segundo+"\r\n");
       
     } 
     for(k=0;k<30;k++){
       delay(1);
     }
     //delay(30);
     Serial.print("No.:\n");    
     for(k=0;k<40;k++){
       delay(1);
     }
     //delay(40); 
     Serial.print("\n");    
     for(k=0;k<40;k++){
       delay(1);
     }
     //delay(40); 
     Serial.print("\r");
     for(k=0;k<32;k++){
       delay(1);
     }
     //delay(32);
     
     bandera=0;
   }
//     delay(100);
//     Serial.print("\r");
//     Serial.print("\n");
     
   
  
 /////////// CONVERSIONES DE INT A CONST CHAR ////////
   str_hora=String(hora);
   str_hora.toCharArray(hora_char,3);
   str_minuto=String(minuto);
   str_minuto.toCharArray(minuto_char,3);
   str_segundo=String(segundo);
   str_segundo.toCharArray(segundo_char,3);
   str_mili_segundo=String(mili_segundo);
   str_mili_segundo.toCharArray(mili_segundo_char,3);
   str_numero=String(numero);
/////////////////////////////////////////////////////
  
  lcd.setCursor(0,1);
  if (hora<10 && minuto<10 && segundo<10){
      
       lcd.print("T: 0"+str_hora+":0"+str_minuto+":0"+str_segundo+"."+str_mili_segundo+"  ");
     }
     else if(hora<10 && minuto<10 && segundo>=10){
      
       lcd.print("T: 0"+str_hora+":0"+str_minuto+":"+str_segundo+"."+str_mili_segundo+"  ");
     }
     else if(hora<10 && minuto>=10 && segundo>=10){
      
       lcd.print("T: 0"+str_hora+":"+str_minuto+":"+str_segundo+"."+str_mili_segundo+"  ");
     }
     else if(hora<10 && minuto>=10 && segundo<10){
      
       lcd.print("T: 0"+str_hora+":"+str_minuto+":0"+str_segundo+"."+str_mili_segundo+"  ");
     }
     if (hora>=10 && minuto<10 && segundo<10){
      
       lcd.print("T: "+str_hora+":0"+str_minuto+":0"+str_segundo+"."+str_mili_segundo+"  ");
     }
     else if(hora>=10 && minuto<10 && segundo>=10){
       
       lcd.print("T: "+str_hora+":0"+str_minuto+":"+str_segundo+"."+str_mili_segundo+"  ");
     }
     else if(hora>=10 && minuto>=10 && segundo>=10){
       
       lcd.print("T: "+str_hora+":"+str_minuto+":"+str_segundo+"."+str_mili_segundo+"  ");
     }
     else if(hora>=10 && minuto>=10 && segundo<10){
       
       lcd.print("T: "+str_hora+":"+str_minuto+":0"+str_segundo+"."+str_mili_segundo+"  ");
     } 
  
  if( Serial.available()>0 ){       
      val[i]=Serial.read();  
      
      i++;
    }
    else{
      i=0;
    }
    
    ////////// IGUALAR EL RELOJ AL GPS /////////////////
//    if(val[0]=='J'){
//      Serial.print("llega la J");
//    }
    if (val[49]=='A' && bandera2==0){   
        buffer_hora=(int)(val[39]);
        hora2=((buffer_hora-48)*10+((int)(val[40])-48))-5;
        if(hora2<0){
          hora2=hora2+24;
        }
        else{
          hora2=hora2;
        }

        hora = hora2;

        buffer_minuto=(int)(val[41]);
        minuto2=(buffer_minuto-48)*10+((int)(val[42])-48);

        minuto = minuto2;

        buffer_segundo=(int)(val[43]);
        segundo2=((buffer_segundo-48)*10+((int)(val[44])-48));


        segundo = segundo2;

        //setTime(hora2,minuto2,segundo2,5,9,2014);
        mili_segundo=0;

        Serial.print("SE OBTUVO TRAMA GPS!!! \n");

        for (j=0;j<200;j++){
          val[j]=0;
        }     
        bandera2=1;
   }
  

}

