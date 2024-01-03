.include "m328pdef.inc"
.include "macroDelay.inc"

.def A = r16
.def AH = r17
.def ThresholdAddr = r18

.org 0x00

    ldi r16, 0x44                ; Load lower 8 bits of baud rate value   ---> 115200
	sts UBRR0L, r16
	ldi r16, 0x08                ; Load upper 8 bits of baud rate value
	sts UBRR0H, r16
	ldi r16, (1 << TXEN0) | (1 << RXEN0)                ; Enable transmitter and receiver
	sts UCSR0B, r16
	ldi r16, (1 << UCSZ01) | (1 << UCSZ00)              ; Set frame format: 8 data bits, no parity, 1 stop bit
	sts UCSR0C, r16

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
		SBRC A,ADSC
		RJMP wait
		LDS A,ADCL          ; Must Read ADCL before ADCH
		LDS AH,ADCH
		delay 100           ; delay 100ms
		CP AH, r18          ; compare LDR reading with our desired threshold
		BRSH LED_ON         ; jump if same or higher (AH >= 200)
		CBI PORTB,5         ; LED OFF

		; Check if data is available in USART Data Register (UDR)
		lds r16, UCSR0A
		sbrs r16, RXC0
	    rjmp loop ; Keep checking if no data

		; Read received byte
		lds r18, UDR0

		; Do something with the received byte (e.g., store in memory)
		; sts 0x100, r17 ; Store in memory address 0x100

		rjmp loop
	
	LED_ON:
		SBI PORTB,5         ; LED ON
	rjmp loop