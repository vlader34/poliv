#include<LiquidCrystal.h>
#define rs 10
#define en 11
#define d4_lcd 6
#define d5_lcd 7
#define d6_lcd 8
#define d7_lcd 9
#define in1_rele 4
#define in2_rele 5
#define buttonSelect 3
#define interruptNumber 1

/*глобальные переменные обработчика перерываний */
  unsigned long timeinterrup=0;                /**/
  unsigned long timeStart=0;                   /**/
  unsigned long timeFinish=0;                  /**/
  boolean flagOnPress=false;
  boolean flagUnPress=false; 
  unsigned long timeClickButton=0;             /**/
/*------------------------------------------------*/

 

LiquidCrystal lcd(rs, en, d4_lcd, d5_lcd, d6_lcd, d7_lcd);

void pressButton(){  //обработчик прерывания
        if ( digitalRead(buttonSelect)==HIGH){ //прозошло нажатие  
              timeStart=millis();
              flagOnPress=true; 
              flagUnPress=false;  //сбрасываем флаг ненажатой кнопки HIGH
               Serial.println("обработчик прерываний кнопка нажата"); 
         }
         if ( digitalRead(buttonSelect)==LOW && flagOnPress==true){ //прозошло отжатие 
        Serial.println("обработчик прерываний кнопка отжата"); 
              timeFinish=millis(); 
              //flagOnPress=false;       
               flagUnPress=true; 
         }
}

unsigned long allTimePress(){ // возвращает последнее время нажатия кнопки  
          if (flagOnPress && flagUnPress) { // если сработало нажатие кнопки затем сработал отпуск кнопки  (оба флага истина) можем считать время нажатия кнопки 
            //flagOnPress=false; // сбросим флаг нажатой кнопки
            timeClickButton=timeFinish-timeStart;
             if (timeClickButton>2500){timeClickButton=0;}
            return timeClickButton;
          }
          return 0;
}

void setup() {
   lcd.begin(16,2);
   Serial.begin(9600);
   pinMode(in1_rele,OUTPUT);digitalWrite(in1_rele,HIGH); //выключаем реле 
   pinMode(in2_rele,OUTPUT); digitalWrite(in2_rele,HIGH);
   pinMode(buttonSelect,INPUT_PULLUP); // подтягиваем внутренний резистор на buttonSelect  выходе на +5В, на входе будет всегда значение +1
   attachInterrupt(interruptNumber,pressButton,CHANGE); //прерывание срабатывает при любом изменении сигнала 

}
void lcdSubMenu(boolean  carusel){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("vuberite podmenu");
    if (carusel){
    lcd.setCursor(0,1);  lcd.print("set period:");
    }
    if (!carusel){
   lcd.setCursor(0,1);  lcd.print("set reletime:");
    }
}
void loop() {
  if (allTimePress()>1300) { // если долгое нажатие  вызываем функциональное подменю с режимом выбора меню. короткое нажатие меняет пункты подменю
    Serial.print("Зашли в режим выбора подменю allTimePress()= ");  Serial.println(allTimePress());
    unsigned long timeStartSubmenu=millis();
    boolean carusel=false;
    lcdSubMenu(true);
     /*нужно хотя бы одно нажатие после входа в подменю, чтобы новое время нажатия отличалось от времени нажатия под которым мы вошли */  
    while ((millis()-timeStartSubmenu)<25000){    // пока мы не выбрали меню и время выбора подменю 15 сек не вышло м не выйдем из цикла while  
      while( flagUnPress==true){ // пока не нажата кнопка ждем// при нажатии flagUnPress=false
        digitalRead(buttonSelect);
        //Serial.println("первый while Кнопка не нажата. Ждем пока мы ее нажмем");
        //  Serial.print("  flagUnPress=");    Serial.println(flagUnPress);
      } 
      Serial.println("Кнопка нажата. Ждем пока отпустим"); 
      while( digitalRead(buttonSelect)==HIGH){ // пока кнопка нажата ждем  сли отпустим кнопку то LOW 
       digitalRead(buttonSelect); //Serial.print("Второй while ");Serial.println(digitalRead(buttonSelect)); 
      } 
      Serial.println("Кнопка нажата и отжата. Используем полученное время нажатия кнопки"); 
       // полученное время нажатия используем
       if (allTimePress()>35 && allTimePress()<1500){ //короткое  нажатие 
        Serial.println("короткое нажатие сменили выбранное подменю:  ");     
         if (carusel==false){carusel=true;} else {carusel=false;};
          Serial.print(" На Входе короткого нажатия  carusel=");    Serial.println(carusel);  
        if (carusel==true){
             Serial.print(" выбирали первое подменю carusel=");    Serial.println(carusel);
            lcdSubMenu(carusel); //1 пункт меню при carusel true 
        }
           if (carusel==false){
           Serial.print(" выбирали второе  подменю carusel=");    Serial.println(carusel);
           lcdSubMenu(carusel); //2ой пункт  меню активен
           }
           Serial.print(" На выходе из короткого нажатия  carusel=");    Serial.println(carusel);  
       } //если короткое нажатие
       Serial.print("выбрали подменю для  установки  carusel="); Serial.println(carusel);
    
       if (allTimePress()>1000){ //долгое нажатие 
           Serial.println("Долгое нажатие в подменю ставим время");
          if (carusel){ //если был выбран первый пункт меню делаем вызов функции установки времени периода включения 
              lcd.clear();  lcd.setCursor(0,0);  lcd.print("selected 1");
              Serial.println("мы выбрали первое подменю:  ");
              delay(3000);
              Serial.println("Мы выполнили работу возвращаем в экран вид подменю");
              lcdSubMenu(carusel);
           }
          if (carusel==false){ // если был вызван второй пунк меню то делаем вызов функции устрановки времени работы насосов
             lcd.clear(); lcd.setCursor(0,0);  lcd.print("selected 2");
             Serial.println("мы выбрали второе  подменю:  ");
             delay(3000);
             Serial.println("Мы выполнили работу возвращаем в экран вид подменю");
             lcdSubMenu(carusel);  
           }
      }
    } //end while 15 сек  
  } //if (allTimePress()>1300) выходим из вызванного подменю;
   // Serial.println("Вышли из режима выбора подменю: В основной экран");
    lcd.clear(); //зачищаем после себя экран
    lcd.setCursor(0,0);
    lcd.print("Osnovnoe menu");
  } // закончен вызов основного подменю
 
  //Serial.println(allTimePress()); delay(450);

  

