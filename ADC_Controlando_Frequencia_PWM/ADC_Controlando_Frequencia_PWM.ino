#include <avr/io.h>
#include <stdlib.h>
#define F_CPU 16000000UL
#include <util/delay.h>

uint16_t adc_value;  //Variavel para armazenar o valor do ADC

uint8_t pwm = 0;  //Variavel para a normalizacao do ADC para PWM

void adc_init(void); //Funcao para inicializar o ADC

uint16_t read_adc(uint8_t channel);   	//Funcao para ler o ADC

void Setar_Frequencia_PWM(int prescaler); //Funcao para setar a frequencia do PWM

int main(void){
  
    DDRD = 0b11111111;     //setar PORTD como saida
    PORTD = 0x00;
    TCCR0A = 0b10100011; //Fast PWM de 8bits, limpar o OC0B no Compare Match, setar OC0B no BOTTOM
    TCNT0 = 0;           // Resetar o TCNT0
    OCR0A = 0;           // Valor inicial da saida PWM
    DDRB |= 0b00000001; //Configurando a porta PB0 como saida
    PORTB = 0b00000001; //nivel logico alto na PB0 para setar o sentido desejado do giro do motor

        adc_init();    //Configurar o ADC
          
	while(1){       //loop infinito

 			adc_value = read_adc(0);  //ler o ADC
 
                        pwm = ceil(adc_value * (4 / 1023.0)); //normalizacao da leitura do ADC para o intervalo de 
                                                              //0˜4 em vista que temos 5 tipos de prescaler
                                                              //para o fast PWM (1,8,64,256,1024)
                        Setar_Frequencia_PWM(pwm);      //Chamada da funcao para setar a frequencia do PWM
                        
                        OCR0A = 255;   //Full PWM
                        
 			_delay_ms(20);                	//delay de 500ms
	}
return 0;

}

void adc_init(void){

 ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));  //16Mhz/128 = 125Khz referencia de clock do ADC

 ADMUX |= (1<<REFS0);                			//referencia do Avcc (5V)

 ADCSRA |= (1<<ADEN);                			//ativar o ADC

 ADCSRA |= (1<<ADSC);                			//Fazer a primeira conversao pois ela e a mais lenta e assim quando
                                                       // eu chamar uma leitura do ADC tudo estara funcionando.

}

uint16_t read_adc(uint8_t channel){

 ADMUX &= 0xF0;                    	//limpar o canal de leitura

 ADMUX |= channel;                	//definir o canal do ADC

 ADCSRA |= (1<<ADSC);               //iniciar uma nova conversao

 while(ADCSRA & (1<<ADSC));         //espera ate que a conversao acabe

 return ADCW;                   	//retorna a leitura do ADC

}

void Setar_Frequencia_PWM(uint8_t prescaler) {
  byte mode;
    switch(prescaler) {
      case 0: mode = 0x01; break; //prescaler 1
      case 1: mode = 0x02; break; //prescaler 8
      case 2: mode = 0x03; break; //prescaler 64
      case 3: mode = 0x04; break; //prescaler 256
      case 4: mode = 0x05; break; //prescaler 1024
      default: return;
    }
    
    TCCR0B = TCCR0B & 0b11111000 | mode; //setar o prescaler selecionado

    
}
