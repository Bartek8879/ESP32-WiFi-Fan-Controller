#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "YOUR_WI-FI_NAME";
const char* password = "YOUR_WI-FI_PASSWORD";

const int MOS = 22;
const int PWM = 25;
const int TACH = 27;

WebServer server(80);

// ---------------- RPM ----------------

volatile unsigned long impulsy = 0;
unsigned long rpm = 0;
unsigned long ostatniPomiarRPM = 0;

// ---------------- TIMER ----------------

bool timerAktywny = false;
unsigned long startTimera = 0;

// ---------------- PRZERWANIE TACH ----------------

void IRAM_ATTR liczImpuls()
{
    impulsy++;
}

// ---------------- STRONA WWW ----------------

void stronaGlowna()
{
    String html = "";

    html += "<html>";
    html += "<head>";
    html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    html += "</head>";

    html += "<body style='text-align:center;font-family:Arial;background:#5b8fc9;'>";

    html += "<div style='background:white;width:300px;margin:40px auto;padding:25px;border-radius:15px;'>";

    html += "<h1>ESP32 FAN</h1>";

    html += "<a href='/on'><button>WLACZ</button></a><br><br>";
    html += "<a href='/off'><button>WYLACZ</button></a><br><br>";
    html += "<a href='/minute'><button>WLACZ NA 1 MINUTE</button></a>";

    html += "<h3>PWM</h3>";

    html += "<input type='range' min='0' max='100' value='100' ";
    html += "oninput='ustawPWM(this.value)'>";

    html += "<p><span id='pwm'>100</span>%</p>";

    html += "<h3>RPM</h3>";
    html += "<p><span id='rpm'>0</span> RPM</p>";

    html += "</div>";

    html += "<script>";

    html += "function ustawPWM(x){";
    html += "document.getElementById('pwm').innerHTML=x;";
    html += "fetch('/pwm?value='+x);";
    html += "}";

    html += "setInterval(function(){";
    html += "fetch('/rpm')";
    html += ".then(r=>r.text())";
    html += ".then(x=>document.getElementById('rpm').innerHTML=x);";
    html += "},1000);";

    html += "</script>";

    html += "</body>";
    html += "</html>";

    server.send(200, "text/html", html);
}

// ---------------- ON ----------------

void wlacz()
{
    digitalWrite(MOS, HIGH);
    timerAktywny = false;

    server.sendHeader("Location", "/");
    server.send(303);
}

// ---------------- OFF ----------------

void wylacz()
{
    digitalWrite(MOS, LOW);
    timerAktywny = false;

    server.sendHeader("Location", "/");
    server.send(303);
}

// ---------------- TIMER 1 MINUTA ----------------

void wlaczNaMinute()
{
    digitalWrite(MOS, HIGH);

    startTimera = millis();
    timerAktywny = true;

    server.sendHeader("Location", "/");
    server.send(303);
}

// ---------------- PWM ----------------

void ustawPWM()
{
    int procent = server.arg("value").toInt();

    if(procent < 0)
        procent = 0;

    if(procent > 100)
        procent = 100;

    int pwm = map(procent, 0, 100, 0, 255);

    analogWrite(PWM, pwm);

    server.send(200, "text/plain", "OK");
}

// ---------------- RPM ----------------

void wyslijRPM()
{
    server.send(200, "text/plain", String(rpm));
}

// ---------------- SETUP ----------------

void setup()
{
    Serial.begin(115200);

    pinMode(MOS, OUTPUT);
    pinMode(PWM, OUTPUT);
    pinMode(TACH, INPUT_PULLUP);

    digitalWrite(MOS, LOW);
    analogWrite(PWM, 255);

    attachInterrupt(
        digitalPinToInterrupt(TACH),
        liczImpuls,
        FALLING
    );

    WiFi.begin(ssid, password);

    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("Laczenie...");
    }

    Serial.println("Polaczono");
    Serial.println(WiFi.localIP());

    server.on("/", stronaGlowna);
    server.on("/on", wlacz);
    server.on("/off", wylacz);
    server.on("/minute", wlaczNaMinute);
    server.on("/pwm", ustawPWM);
    server.on("/rpm", wyslijRPM);

    server.begin();
}

// ---------------- LOOP ----------------

void loop()
{
    server.handleClient();

    // timer 1 minuta
    if(timerAktywny && millis() - startTimera >= 60000)
    {
        digitalWrite(MOS, LOW);
        timerAktywny = false;
    }

    // pomiar RPM co sekunde
    if(millis() - ostatniPomiarRPM >= 1000)
    {
        ostatniPomiarRPM = millis();

        noInterrupts();

        unsigned long liczbaImpulsow = impulsy;
        impulsy = 0;

        interrupts();

        rpm = liczbaImpulsow * 30;
    }
}
