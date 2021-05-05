#include <LiquidCrystal.h>

unsigned int notes[] = {261, 293, 329, 349, 392, 440, 493, 523}; // Set frequency for C4, D4, E4, F4, G4, A4, B4, C5

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);

int switchMode = 6;
int buzzer = 7;
int analogVal;

int button = 0;
int prev_button = 0;

//vectori pentru a tine evidenta butoanelor apasate si a duratei
int recorded_button[200];
unsigned long recorded_time[200];

unsigned long start_time;
unsigned long note_time;

int time_index = 0;
int button_index = 0;

int led1 = 3;
int led2 = 4;
int led3 = 5;

int rstBtn = 2;

volatile int done = 0;
volatile int reseted = 0;

volatile byte resetNow = false;

void setup() {

	Serial.begin(9600);
  
	pinMode(switchMode, INPUT_PULLUP);
    pinMode(rstBtn, INPUT_PULLUP);
	pinMode(buzzer,OUTPUT);
	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(led3, OUTPUT);
  
  	attachInterrupt(digitalPinToInterrupt(rstBtn), stop_ISR, RISING);
  
	lcd.begin(16, 2); //16*2 LCD display
	lcd.print("Arduino Piano");

	delay(2000);
	lcd.clear();
}

void loop() 
{	

  	//If the toggle switch is set in recording mode
  	while (digitalRead(switchMode) == 1 && resetNow==false)   	
    {	
      	if(done == 0 || (done == 1 && reseted == 1))
        {
          	digitalWrite(led1, HIGH);
          	digitalWrite(led2, LOW);
          	digitalWrite(led3, LOW);
        }
      	else
        {
          	digitalWrite(led1, LOW);
          	digitalWrite(led2, HIGH);
          	digitalWrite(led3, LOW);
        }
    	lcd.setCursor(0, 0); 
      	
    	Detect_button();
    	Play_tone();
      	reseted=0;
      }
  
	//If the toggle switch is set in Playing mode
  	while (digitalRead(switchMode) == 0 && resetNow==false) {
      
  		lcd.clear();
  		lcd.setCursor(0, 0);  
      	lcd.print("Now Playing..");
      
      	digitalWrite(led1, LOW);
      	digitalWrite(led2, LOW);
      	digitalWrite(led3, HIGH);
      
		delay(1000);
      
  		for (int i = 0; i < button_index; i++)
  		{
    		if (recorded_button[i] == 0)
      			noTone(buzzer); 
    		else
      			tone(buzzer, notes[(recorded_button[i] - 1)]); 
          
        	delay((recorded_time[i+1])*5);
        }
        done = 1;

	}
}

//definim intreruperea
void stop_ISR() {
  	resetNow = true;  
  	lcd.clear();
  	digitalWrite(led2, HIGH);
    lcd.setCursor(0, 0); 
    lcd.print("Reseting..");
  
  
    for (int i = 0; i < button_index; i++)
    {
    	recorded_button[i] = 0;
    }
    for (int i = 0; i < time_index; i++)
    {
    	recorded_time[i] = 0;
    }
    
    time_index = 0;
    button_index = 0;
  	start_time = 0;
  	done = 0;
  	reseted = 1;
  	
  	noTone(buzzer);
  	resetNow=false;
  
  	digitalWrite(led2, LOW);

}

void Detect_button()
{
  	analogVal = analogRead(A0); 
	
  	//tinem minte butonul apasat ca buton anterior, un fel de debounce
  	prev_button = button; 

  	if (analogVal < 550)
    	button = 8;

  	if (analogVal < 500)
    	button = 7;

  	if (analogVal < 450)
    	button = 6;

  	if (analogVal < 400)
    	button = 5;

  	if (analogVal < 300)
    	button = 4;

  	if (analogVal < 250)
    	button = 3;

  	if (analogVal < 150)
    	button = 2;

  	if (analogVal < 100)
    	button = 1;

  	if (analogVal > 1000)
    	button = 0;

//adaugam butonul apasat intr un vector
  	if (button != prev_button && prev_button != 0 && done == 0 || (done == 1 && reseted == 1))
  	{
      	//butonul apasat
    	recorded_button[button_index] = prev_button; 
    	button_index++;

      	//luam si cazul cand nu se apasa nimic -> buton 0
    	recorded_button[button_index] = 0;
    	button_index++;
  	}
}

void Play_tone()
{

//adaugam duratele dintre apasarile butoanelor
  	if (button != prev_button && done == 0 || (done == 1 && reseted == 1))
  	{
    	lcd.clear(); 
      
      	//timpul de redare a unei note, inclusiv pauza -> buton 0
      	//timpul de finalizare nota anterioaza- timp prezent
    	note_time = millis() - start_time ;

    	recorded_time[time_index] = note_time;
    	time_index++;
    
    	start_time = millis();
  	}

  	if (button == 0)
  	{
    	noTone(buzzer);
    	lcd.print("0 -> Pause..");
  	}

  	if (button == 1)
  	{
    	tone(buzzer, notes[0],100);
    	lcd.print("1 -> NOTE_C4");
  	}

  	if (button == 2)
  	{
    	tone(buzzer, notes[1],100);
    	lcd.print("2 -> NOTE_D4");
  	}

  	if (button == 3)
  	{
    	tone(buzzer, notes[2],100);
    	lcd.print("3 -> NOTE_E4");
  	}

  	if (button == 4)
  	{
    	tone(buzzer, notes[3],100);
    	lcd.print("4 -> NOTE_F4");
  	}

  	if (button == 5)
  	{
    	tone(buzzer, notes[4],100);
    	lcd.print("5 -> NOTE_G4");
  	}

  	if (button == 6)
  	{
    	tone(buzzer, notes[5],100);
    	lcd.print("6 -> NOTE_A4");
  	}

  	if (button == 7)
  	{
    	tone(buzzer, notes[6],100);
    	lcd.print("7 -> NOTE_B4");
  	}

  	if (button == 8)
  	{
    	tone(buzzer, notes[7],100);
    	lcd.print("8 -> NOTE_C5");
  	}
}