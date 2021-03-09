/* Original by Julian Schroeter @ https://www.hackster.io/julianso/esp32-voice-streamer-52bd7e
 * Forked by MinePro120 */

#include <WiFi.h>
#include <driver/adc.h>

#define AUDIO_BUFFER_LENGTH 20000
#define SSID "YourSSID"    // Change me
#define PASSWD "YourPassword"    // Change me
#define PORT 6120    // IANA unassigned port

IPAddress host (192, 168, 1, 6);    // Change me, IP Address of the computer to stream the audio to
uint16_t audioBuffer[AUDIO_BUFFER_LENGTH];
bool transmitNow = false;
WiFiClient client;
hw_timer_t *timer = NULL;    // Timer


void IRAM_ATTR onTimer (void);
void deepSleep (int);
void handleNet (void);
void blink (void);

void setup (void)
{
  Serial.begin (115200);
  WiFi.mode (WIFI_STA);
  pinMode (LED_BUILTIN, OUTPUT);    // Status LED
  delay (500);
  digitalWrite (LED_BUILTIN, HIGH);
  handleNet ();
  adc1_config_width (ADC_WIDTH_12Bit);    // Configures the ADC
  adc1_config_channel_atten (ADC1_CHANNEL_0, ADC_ATTEN_0db);    // Connects the ADC 1 with channel 0 (GPIO 36)
  timer = timerBegin (0, 40, true);    // 40 Prescaler (for 16 kHz)
  timerAttachInterrupt (timer, &onTimer, true);    // Binds the handling function to the timer 
  timerAlarmWrite (timer, 125, true);
  timerAlarmEnable (timer);
}


void loop (void)
{
  handleNet ();
  blink ();
  if (transmitNow)    // Checks if the buffer is full
  {
    transmitNow = false;
    client.write ((const uint8_t *) audioBuffer, sizeof (audioBuffer));    // Sends the buffer to the host
  }
}


bool connected = false;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR onTimer (void)
{
  portENTER_CRITICAL_ISR (&timerMux);    // ISR start
  if (connected)
  {
    static uint32_t bufferPointer = 0;
    audioBuffer[bufferPointer++] = map (adc1_get_voltage (ADC1_CHANNEL_0), 0, 4096, 0, 65536);    // 12-bit -> 16-bit
    if (bufferPointer == AUDIO_BUFFER_LENGTH)    // When the buffer is full
    {
      bufferPointer = 0;
      transmitNow = true;    // Sets the value true so we know we can transmit now
    }
  }
  portEXIT_CRITICAL_ISR (&timerMux);    // ISR end
}


void deepSleep (int sec)    // Deep sleep in seconds
{
  Serial.print ("[INFO/ESP32] Going to sleep for ");
  Serial.print (sec);
  Serial.println (" seconds");
  Serial.flush ();
  esp_sleep_enable_timer_wakeup (sec * 1000000);
  esp_deep_sleep_start ();
}


void handleNet (void)
{
  static int sleepCounter = 0;
  static bool firstRun = true;
  while ((WiFi.status () != WL_CONNECTED) || !client.connected ())
  {
    connected = false;
    if (WiFi.status () != WL_CONNECTED)
    {
      Serial.print ("[INFO/WIFI] (Re)connecting to network");
      if (firstRun)
      {
        WiFi.begin (SSID, PASSWD);
        firstRun = false;
      }
      else
        WiFi.reconnect ();
      int timeoutCounter = 0;
      while ((WiFi.status () != WL_CONNECTED) && (timeoutCounter++ < 30))
      {
        delay (1000);
        Serial.print ('.');
      }
      if (timeoutCounter >= 30)
      {
        Serial.println ("\n[INFO/WIFI] Connection timeout");
        if (++sleepCounter >= 30)    // Sleeps 30min after 15min of inactivity
          deepSleep (1800);
      }
      else
      {
        Serial.print ("\n[INFO/WIFI] Connected with IP : ");
        Serial.print (WiFi.localIP ());
        Serial.print (", at ");
        Serial.print (WiFi.RSSI ());
        Serial.println (" dBm");
      }
    }
    else
    {
      //host = WiFi.gatewayIP ();    // Uncomment to use the gateway IP instead
      Serial.print ("[INFO/WIFI] (Re)connecting to host at ");
      Serial.println (host);
      int timeoutCounter = 0;
      while (!client.connect (host, PORT) && (timeoutCounter++ < 30))
      {
        delay (1000);
        Serial.print ('.');
      }
      if (timeoutCounter >= 30)
      {
        Serial.println ("\n[INFO/WIFI] Connection timeout");
        if (++sleepCounter >= 30)    // Sleeps 30min after 15min of inactivity
          deepSleep (1800);
      }
      else
      {
        Serial.println ("\n[INFO/WIFI] Connected to host");
        connected = true;
        sleepCounter = 0;
        return;
      }
    }
  }
}  


void blink (void)
{
  static unsigned long previousMillis = 0;
  static bool light = false;
  if ((millis () - previousMillis) >= 2500)
  { 
    light = true;   
    previousMillis = millis ();
    digitalWrite (LED_BUILTIN, LOW);
  }
  if (light)
  {
    if ((millis ()- previousMillis) >= 50)
    {
      digitalWrite (LED_BUILTIN, HIGH);
      light = false;
    }
  }
}
