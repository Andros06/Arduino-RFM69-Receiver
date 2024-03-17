//Inkluderer bibliotek
#include <SPI.h>
#include <RH_RF95.h>

//Definerer pinner
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

// Definerer frekvens til radio (915Mhz)
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Definerer LED til 13
#define LED 13

void setup() 
{
   
  pinMode(LED, OUTPUT); //setter LED pin til output    
  pinMode(RFM95_RST, OUTPUT); //setter RFM95_RST reset pin til output
  digitalWrite(RFM95_RST, HIGH); //setter reset pin til høg, lar rf module boote

  //venter til seriel kommunikasjon mellom arduino og pc er satt opp, starter deretter seriel med 9600 bits per sekund
  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa RX Test!"); //kjøyre en liten test melding på seriel kommunikasjon
  
  // manuel resest
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // initialiserer rf95 kommunikasjon, printer status på initialisering
  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  // Printer frekvensen som blei satt tidligare i koden, dersom frekvens ikkje blir satt riktig printast feilmelding
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  //setter rf95 styrke nivå, setter til 23dBm. false deaktiverer "boost" pin til rf95
  rf95.setTxPower(23, false);
}

void loop()
{
  if (rf95.available()) //visst det kjem ei melding fra avsender
  {
    // konvertere meldinga   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      //blinker og seriel printer meldinga som blei sendt fra avsend med RSSI verdi
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      
      // Sender tilbakemelding til avsender
      uint8_t data[] = "And hello back to you";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);
    }
    else
    {
      //feilmelding dersom mottatt melding er gale
      Serial.println("Receive failed");
    }
  }
}