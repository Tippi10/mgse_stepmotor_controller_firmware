/*
 * GPIO_Map.h
 *
 * Created: 2019/4/4 上午 02:59:22
 *  Author: 王碩傑
 */ 


#ifndef GPIO_MAP_H_
#define GPIO_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif

	#define _PB0 DDRB,PORTB,PINB,0,-1
	#define _PB1 DDRB,PORTB,PINB,1,-1
	#define _PB2 DDRB,PORTB,PINB,2,2
	#define _PB3 DDRB,PORTB,PINB,3,-1
	#define _PB4 DDRB,PORTB,PINB,4,6
	#define _PB5 DDRB,PORTB,PINB,5,-1
	#define _PB6 DDRB,PORTB,PINB,6,-1
	#define _PB7 DDRB,PORTB,PINB,7,-1
		
	#define _PC0 DDRC,PORTC,PINC,0,-1
	#define _PC1 DDRC,PORTC,PINC,1,-1
	#define _PC2 DDRC,PORTC,PINC,2,-1
	#define _PC3 DDRC,PORTC,PINC,3,-1
	#define _PC4 DDRC,PORTC,PINC,4,3
	#define _PC5 DDRC,PORTC,PINC,5,4
	#define _PC6 DDRC,PORTC,PINC,6,5
	#define _PC7 DDRC,PORTC,PINC,7,7
	
	#define _PD0 DDRD,PORTD,PIND,0,-1
	#define _PD1 DDRD,PORTD,PIND,1,-1
	#define _PD2 DDRD,PORTD,PIND,2,-1
	#define _PD3 DDRD,PORTD,PIND,3,-1
	#define _PD4 DDRD,PORTD,PIND,4,-1
	#define _PD5 DDRD,PORTD,PIND,5,-1
	#define _PD6 DDRD,PORTD,PIND,6,0
	#define _PD7 DDRD,PORTD,PIND,7,1
	
	#define _PE0 DDRE,PORTE,PINE,0,-1
	#define _PE1 DDRE,PORTE,PINE,1,-1
	#define _PE2 DDRE,PORTE,PINE,2,-1
	
	#define LED _PD2
	
#ifdef __cplusplus
}
#endif // extern "C"

#endif /* GPIO_MAP_H_ */