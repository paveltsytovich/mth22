

/*****
 
 Все ресурсы для этого проекта здесь:
 https://randomnerdtutorials.com/
 
*****/

// загружаем библиотеки ESP8266WiFi, PubSubClient и DHT:
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// убираем знаки комментария у одной из строчек ниже,
// соответствующей типу датчика DHT:
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

// меняем эти константы, чтобы ESP8266 мог подключиться к роутеру:
//const char* ssid = "CHALLENGE";
//const char* password = "5rK&m7!vrb14wa^nN(88";

const char* ssid = "CHALLENGE.GUEST";
const char* password = "d35_Vm%4qUVZqQj$";


// меняем переменную ниже на IP-адрес Raspberry Pi,
// чтобы она могла подключиться к брокеру MQTT:
const char* mqtt_server = "192.168.1.125";

// инициализируем espClient:
WiFiClient espClient;
PubSubClient client(espClient);

// подключаем светодиоды к соответствующим GPIO-контактам ESP8266:
//const int ledGPIO5 = 5;
//const int ledGPIO4 = 4;

// датчик DHT:
const int DHTPin = 2;

// инициализируем датчик DHT:
DHT dht(DHTPin, DHTTYPE);

// вспомогательные переменные для таймера:
long now = millis();
long lastMeasure = 0;

// не меняйте функцию ниже; она подключает ESP8266 к роутеру:
void setup_wifi() {
  delay(10);
  // начинаем с подключения к WiFi-сети:
  Serial.println();
  Serial.print("Connecting to ");  //  "Подключение к "
  Serial.println(ssid);
  WiFi.setPhyMode(WIFI_PHY_MODE_11G);
  //WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  //  "Подключение к WiFi выполнено – IP-адрес ESP8266: "
  Serial.println(WiFi.localIP());
}

// эта функция выполняется, когда какой-то девайс публикует сообщение
// в топик, на который подписан ESP8266; поменяйте функцию ниже
// в соответствии с логикой вашей программы – чтобы, когда девайс
// будет публиковать сообщение в топик, на который подписан ESP8266,
// ваша программа работала как нужно:
void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  //  "Сообщение прибыло в топик: "
  Serial.print(topic);
  Serial.print(". Message: ");  //  ". Сообщение: "
  String messageTemp;
 
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // если в вашем проекте не 2 светодиода, а больше,
  // добавьте ниже больше кода:

  // если топик home/office/esp1/gpio2 получил сообщение,
  // смотрим, что это за сообщение – «0» или «1»;
  // переключаем GPIO-контакт в соответствии с присланным значением:
  /*if(topic=="esp8266/4"){
      Serial.print("Changing GPIO 4 to ");
      // "Смена состояния GPIO-контакта 4 на "
      if(messageTemp == "1"){
        digitalWrite(ledGPIO4, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(ledGPIO4, LOW);
        Serial.print("Off");
      }
  }
  if(topic=="esp8266/5"){
      Serial.print("Changing GPIO 5 to ");
      // "Смена состояния GPIO-контакта 5 на "
      if(messageTemp == "1"){
        digitalWrite(ledGPIO5, HIGH);
        Serial.print("On");
      }
      else if(messageTemp == "0"){
        digitalWrite(ledGPIO5, LOW);
        Serial.print("Off");
      }*/
  }


// эта функция переподключает ESP8266 к MQTT-брокеру;
// измените эту функцию, если хотите, чтобы ESP8266 подписывался
// на большее количество топиков:
void reconnect() {
  // заново запускаем цикл, пока не подключимся:
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // "Попытка подключиться к MQTT-брокеру... "

    // Пытаемся подключиться:

          /*
     ЕСЛИ У ВАС ПРОБЛЕМЫ С ПОДКЛЮЧЕНИЕМ НЕСКОЛЬКИХ УСТРОЙСТВ
     К MQTT-БРОКЕРУ, ПОМЕНЯЙТЕ СТРОЧКУ НИЖЕ.

       *  Чтобы поменять ID чипа ESP8266, ему нужно дать
          уникальное название. Так оно выглядит сейчас:
       if (client.connect("ESP8266Client")) {

       *  Если вы хотите подключить к MQTT-брокеру
          дополнительные устройства, его можно назвать так:
       if (client.connect("ESPOffice")) {

       *  Для других ESP:
       if (client.connect("ESPGarage")) {

       Это должно решить проблему с подключением
       нескольких устройств к MQTT-брокеру:

       Также обратите внимание, что это название должно
       соответствовать тому, что будет указано ниже,
       в блоке loop().
          */

    if (client.connect("ESPDHTFirst")) {
      Serial.println("connected");  //  "подключен"
      // подписываемся или переподписываемся на топик;
      // можно подписаться не только на один, а на несколько топиков
      // (что касается конкретно этого примера, то это позволит
      // управлять большим количеством светодиодов):
      client.subscribe("esp8266/1");
      client.subscribe("esp8266/2");
    } else {
      Serial.print("failed, rc=");  //  "подключение не удалось"
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // "5 секунд до следующей попытки"
      // ждем 5 секунд перед тем, как попробовать снова:
      delay(5000);
    }
  }
}

// эта функция настраивает GPIO-контакты ESP8266
// на режим вывода данных, запускает последовательную коммуникацию
// на скорости 112500 бод, настраивает MQTT-брокер
// и задает функцию обратного вызова;
// функция обратного вызова служит для получения сообщений и,
// собственно, управления светодиодами:
void setup() {
  //dht.begin();
  //pinMode(ledGPIO4, OUTPUT);
  //pinMode(ledGPIO5, OUTPUT);
 
  Serial.begin(115200);
  setup_wifi();
  //client.setServer(mqtt_server, 1883);
  //client.setCallback(callback);
}

// для этого проекта в функции loop() ничего менять не нужно;
// в сущности, эта функция устанавливает соединение
// между ESP8266 и MQTT-брокером:
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())

     /*
     ЕСЛИ У ВАС ПРОБЛЕМЫ С ПОДКЛЮЧЕНИЕМ НЕСКОЛЬКИХ УСТРОЙСТВ
     К MQTT-БРОКЕРУ, ПОМЕНЯЙТЕ СТРОЧКУ НИЖЕ.

       *  Чтобы поменять ID чипа ESP8266, ему нужно дать
          уникальное название. Так оно выглядит сейчас:
       if (client.connect("ESP8266Client")) {

       *  Если вы хотите подключить к MQTT-брокеру
          несколько устройств, его можно назвать так:
       if (client.connect("ESPOffice")) {

       *  Для других ESP:
       if (client.connect("ESPGarage")) {

       Это должно решить проблему с подключением
       нескольких устройств к MQTT-брокеру:

       Также обратите внимание, что это название должно
       соответствовать тому, что будет указано ниже,
       в блоке loop().
     */

    client.connect("ESPDHTFirst");
   
  now = millis();
  // публикуем новые данные о температуре и влажности
  // каждые 10 секунд:
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
    // данные от датчика могут отставать на 2 секунды
    // (это очень медленный датчик)
    float h = dht.readHumidity();
    // считываем температуру в Цельсиях (по умолчанию):
    float t = dht.readTemperature();
    // считываем температуру в Фаренгейтах (isFahrenheit = true):
    float f = dht.readTemperature(true);

    // проверяем, получилось ли прочитать данные с датчика,
    // и если нет, то завершаем операцию и пробуем снова:
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      //  "Не удалось прочесть данные с датчика!"
      return;
    }

    // рассчитываем температуру в Цельсиях:
    float hic = dht.computeHeatIndex(t, h, false);
    static char temperatureTemp[7];
    dtostrf(hic, 6, 2, temperatureTemp);
   
    // если нужно рассчитать температуру в Фаренгейтах,
    // убираем знаки комментирования у строчек ниже:
    // float hif = dht.computeHeatIndex(f, h);
    // static char temperatureTemp[7];
    // dtostrf(hic, 6, 2, temperatureTemp);
   
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    // публикуем данные о температуре и влажности:
    client.publish("/esp8266/temperature", temperatureTemp);
    client.publish("/esp8266/humidity", humidityTemp);
   
    Serial.print("Humidity: ");  //  "Влажность: "
    Serial.print(h);
    Serial.print(" %\t Temperature: ");  //  " %\t Температура: "
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(f);
    Serial.print(" *F\t Heat index: ");  //  " *F\t Тепловой индекс: "
    Serial.print(hic);
    Serial.println(" *C ");
    // Serial.print(hif);
    // Serial.println(" *F");
  }
}
