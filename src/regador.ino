
/*
 pinos
 --------------------------------
 pino D6 perna azul led rgb     |
 pino D5 perna verde led rgb    |
 pino D4 perna vermelha led rgb |
 pino D7 rele                   |
 pino A0 sensor humidade        |
 --------------------------------
 */

// Leds
const int led_azul = 6; 
const int led_verde = 5; 
const int led_vermelho = 4;
const int rele = 7;

// Sensor
const int sensor_humidade = A0;
int humidade_valor = 0;
int humidade = 0;

// Variaveis
int ligar_rele = 40;    // porcentagen para ligar o rele
int desligar_rele = 80;   // porcenagem para desligar o rele
int estado_led = LOW;
unsigned long millis_anterior_rele = 0;
unsigned long millis_anterior_led = 0;
const long intervalo_led = 100;
const long intervalo_rele = 2000;

void setup() {
                                            
  Serial.begin(9600);
  
  // declara que que cada pino faz
  pinMode(led_azul, OUTPUT);
  pinMode(led_verde, OUTPUT);
  pinMode(led_vermelho, OUTPUT);
  pinMode(rele, OUTPUT);
  pinMode(sensor_humidade, INPUT);
}

void loop() {

  // Define o tempo atual
  unsigned long millis_atual = millis();

  // A diferença entre a última execução e a atual tem que ser maior ou igual ao intervalo do rele
  if (millis_atual - millis_anterior_rele >= intervalo_rele) {
        
    // Salva a última vez que foi execultada a leitura 
    millis_anterior_rele = millis_atual;
  
    // le o valor do sensor
    humidade_valor = analogRead(A0);
    
    // ele converte o valor analogico do sensor em uma porcentagem
    humidade = map(humidade_valor, 1023, 0, 0, 100);
    
    // exibe no monitor serial a humidade
    Serial.print("Humidade ");
    Serial.print(humidade);
    Serial.println("%  ");
    
    // ele ve se precisa regar
    if(humidade <= ligar_rele && humidade > 0 && digitalRead(rele) == LOW){
        
      digitalWrite(rele,HIGH);
      digitalWrite(led_azul,HIGH);
      digitalWrite(led_vermelho,LOW);
      digitalWrite(led_verde,LOW);   
  
    //ve se precisa desligar 
    } else if ((humidade >= desligar_rele || humidade == 0) && digitalRead(rele) == HIGH){
        
      digitalWrite(rele,LOW);
      digitalWrite(led_azul,LOW);      
    
    }
  }  
  
  
  // ele ve se o rele ta ligado se nao estiver acende um led
  if(humidade >= 60 && digitalRead(rele) == LOW && digitalRead(led_verde) == LOW) {
    
    digitalWrite(led_verde,HIGH);
    digitalWrite(led_vermelho,LOW);
    
  } else if (humidade < 60 && digitalRead(rele) == LOW && humidade > 0 && digitalRead(led_vermelho) == LOW){
    
    digitalWrite(led_vermelho,HIGH);
    digitalWrite(led_verde,LOW);
  
  } else if (humidade == 0){

    unsigned long millis_atual = millis();
      
    if (millis_atual - millis_anterior_led >= intervalo_led) {
        
      millis_anterior_led = millis_atual;   // save the last time you blinked the LED
      
      // if the LED is off turn it on and vice-versa:
      if (estado_led == LOW) {
            
          estado_led = HIGH;
              
      } else {
              
          estado_led = LOW;
        
        }
        
      digitalWrite(led_vermelho,estado_led);
          
    }
  }
}
