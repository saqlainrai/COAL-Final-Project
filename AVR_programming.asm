.include "m328pdef.inc"
.include "macroDelay.inc"
.def A = r16
.def AH = r17
.org 0x00
	    ; I/O Pins Configuration
		SBI DDRB,5          ; Set PB5 pin for Output to LED
		CBI PORTB,5         ; LED OFF
		
		; ADC Configuration
		LDI A,0b11000111    ; [ADEN ADSC ADATE ADIF ADIE ADIE ADPS2 ADPS1 ADPS0]
		STS ADCSRA,A
		LDI A,0b01100000    ; [REFS1 REFS0 ADLAR – MUX3 MUX2 MUX1 MUX0]
		STS ADMUX,A         ; Select ADC0 (PC0) pin
		SBI PORTC,PC0       ; Enable Pull-up Resistor
	
	loop:
		LDS A,ADCSRA        ; Start Analog to Digital Conversion
		ORI A,(1<<ADSC)
		STS ADCSRA,A
	
	wait:
		LDS A,ADCSRA        ; wait for conversion to complete
		sbrc A,ADSC
		rjmp wait
		LDS A,ADCL          ; Must Read ADCL before ADCH
		LDS AH,ADCH
		delay 100           ; delay 100ms
		cpi AH,200          ; compare LDR reading with our desired threshold
		brsh LED_ON         ; jump if same or higher (AH >= 200)
		CBI PORTB,5         ; LED OFF
		rjmp loop
	
	LED_ON:
		SBI PORTB,5         ; LED ON
	rjmp loop