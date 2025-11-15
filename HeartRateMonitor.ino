#include <LiquidCrystal.h>

#define PROCESSING_VISUALIZER 1
#define SERIAL_PLOTTER  2

int pulsePin = 0;
int blinkPin = 13;

volatile int BPM = 0;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;

volatile int rate[10];
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 512;
volatile int amp = 0;
volatile boolean firstBeat = true;
volatile boolean secondBeat = false;

static int outputType = 2;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
    pinMode(blinkPin, OUTPUT);
    Serial.begin(115200);
    interruptSetup();
    lcd.begin(20, 4);
    lcd.print("Initializing...");
    delay(2000);
    lcd.clear();
    lcd.print("BPM: 0");
}

void loop() {
    serialOutput();
    if (QS == true) {
        displayBPM();
        QS = false;
    }
    delay(20);
}

void displayBPM() {
    lcd.setCursor(5, 0);
    lcd.print("    ");
    lcd.setCursor(5, 0);
    lcd.print(BPM);
    lcd.setCursor(0, 2);
    if (BPM > 100) {
        lcd.print("High heart rate         ");
    } else if (BPM >= 60) {
        lcd.print("Normal heart rate      ");
    } else if (BPM < 60){
        lcd.print("Low heart rate      ");
    }
}

void serialOutput() {
    switch (outputType) {
        case PROCESSING_VISUALIZER:
            sendDataToSerial('S', Signal);
            break;
        case SERIAL_PLOTTER:
            Serial.println(BPM);
            break;
        default:
            break;
    }
}

void sendDataToSerial(char symbol, int data) {
    Serial.print(symbol);
    Serial.println(data);
}

void interruptSetup() {
    TCCR2A = 0x02;
    TCCR2B = 0x06;
    OCR2A = 0X7C;
    TIMSK2 = 0x02;
    sei();
}

ISR(TIMER2_COMPA_vect) {
    cli();
    Signal = analogRead(pulsePin);
    sampleCounter += 2;
    int N = sampleCounter - lastBeatTime;

    if(Signal < thresh && N > (IBI/5)*3) {
        if (Signal < T) T = Signal;
    }

    if(Signal > thresh && Signal > P) {
        P = Signal;
    }

    if (N > 250) {
        if ((Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3)) {
            Pulse = true;
            digitalWrite(blinkPin, HIGH);
            IBI = sampleCounter - lastBeatTime;
            lastBeatTime = sampleCounter;

            if (secondBeat) {
                secondBeat = false;
                for (int i = 0; i <= 9; i++) rate[i] = IBI;
            }

            if (firstBeat) {
                firstBeat = false;
                secondBeat = true;
                sei();
                return;
            }

            word runningTotal = 0;
            for (int i = 0; i <= 8; i++) {
                rate[i] = rate[i + 1];
                runningTotal += rate[i];
            }

            rate[9] = IBI;
            runningTotal += rate[9];
            runningTotal /= 10;
            BPM = 60000 / runningTotal;
            QS = true;
        }
    }

    if (Signal < thresh && Pulse == true) {
        digitalWrite(blinkPin, LOW);
        Pulse = false;
        amp = P - T;
        thresh = amp / 2 + T;
        P = thresh;
        T = thresh;
    }

    if (N > 2500) {
        BPM = 0;
        thresh = 530;
        P = 512;
        T = 512;
        lastBeatTime = sampleCounter;
        firstBeat = true;
        secondBeat = false;
    }
    sei();
}
