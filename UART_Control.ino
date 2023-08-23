#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line displa
int LED_PIN = 4;

uint8_t  crcTable[256];

#define WIDTH  (8 * sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

void crcInit(void)
{
    uint8_t  remainder;
    for (int dividend = 0; dividend < 256; ++dividend)
    {
        remainder = dividend << (WIDTH - 8);
        for (uint8_t bit = 8; bit > 0; --bit)
        {
            if (remainder & TOPBIT)
            {
                remainder = (remainder << 1) ^ 0xD5;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
        crcTable[dividend] = remainder;
    }
}

uint8_t crcFast(uint8_t const message[], int nBytes)
{
    uint8_t data;
    uint8_t remainder = 0;
    for (int byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH - 8));
        remainder = crcTable[data] ^ (remainder << 8);
    }
    return (remainder);
}

typedef struct crsf_channels_t
{
    unsigned ch0 : 11;
    unsigned ch1 : 11;
    unsigned ch2 : 11;
    unsigned ch3 : 11;
    unsigned ch4 : 11;
    unsigned ch5 : 11;
    unsigned ch6 : 11;
    unsigned ch7 : 11;
    unsigned ch8 : 11;
    unsigned ch9 : 11;
    unsigned ch10 : 11;
    unsigned ch11 : 11;
    unsigned ch12 : 11;
    unsigned ch13 : 11;
    unsigned ch14 : 11;
    unsigned ch15 : 11;
} PACKED ;

String LeadBy(String aS, char aC, int aL) {
  String res = aS;
  while (res.length() < aL)
    res = aC + res;
  return res;
}

#define PackageMaxSize 64
byte BF[PackageMaxSize]; 
int BFix = 0;
uint8_t PackageLength = 0;
uint8_t PackageType = 0;
bool PackageIsReading = false;

bool PackageJustReaded = false;
byte BF_channels[PackageMaxSize]; 
crsf_channels_t *cr_channels = (crsf_channels_t *)BF_channels;
bool channels_are_readed = false;
long channels_read_millis = -1;

void setup() {
  crcInit();

  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial1.begin(500000);

  Serial.print("Started\n");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Started");
  lcd.setCursor(0, 1);

  Serial.print("Size of channels:" + String(sizeof(crsf_channels_t)) +"\n");

  delay(200);
}


void loop() {

  PackageJustReaded = false;
  if (Serial1.available()) {

    while (Serial1.available() > 0) {

      byte bb = Serial1.read();

      if (PackageIsReading) {
        BF[BFix++] = bb;
      }
      else {
        if (bb == 0xc8 || bb == 0xee || bb == 0xea || bb == 0xec) { // read package
          PackageType = bb;
          PackageLength = Serial1.read();
          if (PackageLength <= PackageMaxSize) {
            PackageIsReading = true;
            BFix = 0;
          }
        }
      }
      if (PackageLength <= BFix) { //  package readed >>
        PackageIsReading = false;
        PackageJustReaded = true;
        uint8_t crc8 = crcFast(BF, PackageLength-1);
        if (crc8 == BF[PackageLength-1]) // crc is ok >>
        {
          if (BF[0] == 0x16) {
            for (int ix = 1; ix < PackageLength; ix++)
              BF_channels[ix-1] = BF[ix];
            
            channels_read_millis = millis();
            channels_are_readed = true;
          }
        } // <<
      } // <<
    }

  }

  if (channels_are_readed) {
    lcd.setCursor(0, 0);
    lcd.print(LeadBy(String(cr_channels->ch0), ' ', 5));
    lcd.setCursor(5, 0);
    lcd.print(LeadBy(String(cr_channels->ch1), ' ', 5));
    lcd.setCursor(10, 0);
    lcd.print(LeadBy(String(cr_channels->ch2), ' ', 5));
    lcd.setCursor(15, 0);
    lcd.print(LeadBy(String(cr_channels->ch3), ' ', 5));
    lcd.setCursor(0, 1);
    lcd.print(LeadBy(String(cr_channels->ch4), ' ', 5));
    lcd.setCursor(5, 1);
    lcd.print(LeadBy(String(cr_channels->ch5), ' ', 5));
    lcd.setCursor(10, 1);
    lcd.print(LeadBy(String(cr_channels->ch6), ' ', 5));
    lcd.setCursor(15, 1);
    lcd.print(LeadBy(String(cr_channels->ch7), ' ', 5));
    lcd.setCursor(0, 2);
    lcd.print(LeadBy(String(cr_channels->ch8), ' ', 5));
    lcd.setCursor(5, 2);
    lcd.print(LeadBy(String(cr_channels->ch9), ' ', 5));
    lcd.setCursor(10, 2);
    lcd.print(LeadBy(String(cr_channels->ch10), ' ', 5));
    lcd.setCursor(15, 2);
    lcd.print(LeadBy(String(cr_channels->ch11), ' ', 5));
    lcd.setCursor(0, 3);
    lcd.print(LeadBy(String(cr_channels->ch12), ' ', 5));
    lcd.setCursor(5, 3);
    lcd.print(LeadBy(String(cr_channels->ch13), ' ', 5));
    lcd.setCursor(10, 3);
    lcd.print(LeadBy(String(cr_channels->ch14), ' ', 5));
    lcd.setCursor(15, 3);
    lcd.print(LeadBy(String(cr_channels->ch15), ' ', 5));
    channels_are_readed = false;
  }

}
