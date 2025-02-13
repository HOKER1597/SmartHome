int a;
int b;
float c;
char ch;
void setup() {
  Serial.begin(9600);
  b=0;
  // put your setup code here, to run once:

}

void loop() {
  a=5;
  b=b+a;
  c=(b/3.5)*1000;
  Serial.print(a);
  Serial.print("\t");
  Serial.print(b);
  Serial.print("\t");
  Serial.println(c);
  // put your main code here, to run repeatedly:

}
