#define redPin D6
#define greenPin D7
#define bluePin D8

void setup() {
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
}
void loop() {

    delay(1000);  

}
void setColor(int redValue, int greenValue, int blueValue) {
    //analogWrite(redPin, redValue);
    //analogWrite(greenPin, greenValue);
    //analogWrite(bluePin, blueValue);
    digitalWrite(D6,LOW);
    digitalWrite(D7,LOW);
    digitalWrite(D8,LOW);
}
