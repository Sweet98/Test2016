;;
;    Assembly defines and code to support uart stuff I wished I could do in C code
;;
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/db_uart.s $
;; $Revision: 1.1.2.11 $
;; $Date: 2011/02/25 10:09:51EST $
;;======================================================================================

	;INCLUDE   db_memap.s    ; Defines how we're allocating RAM for application code, stacks, etc.
	INCLUDE   db_hdw.s      ; Definitions of SA-11x0 registers and bit settings

	EXPORT Init_Uart1
	EXPORT Send_Startup_Message
	EXPORT Send_CRLF
	EXPORT Send_Address_in_r0
	EXPORT Send_RAM_Pass
	EXPORT SetBaudRateRegisters
	EXPORT GetBaudRateRegisters

	AREA UARTCFG, CODE, READONLY


;;
;;********************************************************************************
;; Init_Uart1
;;********************************************************************************
;;
Init_Uart1
;; try and set up uart 1 for 115200

;; thinking is this USBPLL is set for 96MHz by default.  I have a peripheral clock I would like to start
;; at 16MHz before I do my baud down calculation/prescale, so that implies setting PCDR to have PER_DIV1 be 5 so it divides by 6
	ldr  r0,=PCDR
	ldr  r1,=PCDR_SETTINGS
	str r1,[r0]              ;; should establish starting point clock input of 16MHz for uarts, timers and pwm

;; set up the divide for Ref_clk
;;
	ldr r0,=UFCR_1
	ldr r1,=UFCR_1_SETTINGS		;; sets refclk / to 1, which means clock into uart IS perclk1 above
	str r1,[r0]

	IF ((platform_current = "imx27") || (platform_current = "imx25"))
;;Future home for uart hardware external gate configuration, unique to our board...
	ELSE
;; so now I have to set up the uart hardware external gates on the board...
;;
	ldr r0,=DDIR_C
	ldr r1,[r0]
	orr r1,r1,#BIT11    ;; txd output
	str r1,[r0]         ;; make txd an output (doesn't seem necessary per block diagram, but NOTE on page 26-3 seems to indicate this may be a good idea)

	ldr r0,=GIUS_C
	ldr r1,=GPR_C

	ldr r3,[r0]    ;; first deal with GIUS_C
	and r3,r3,#NUART1PINS
	str r3,[r0]    ;; clear bits 9 through 12

	ldr r3,[r1]    ;; then similarly with GPR_C
	and r3,r3,#NUART1PINS
	str r3,[r1]

	ENDIF	;;end hardware gate configuration

;; at this point the 'alternate function' concept for the 4 uart1 pins should be set
	ldr r0,=UCR1_1
	ldr r1,=UCR1_1_SETTINGS
	str r1,[r0],#4    			;; uart clk enable and uart enable

								;;UCR2_1
	ldr r1,=UCR2_1_SETTINGS  	;; no software reset?! 6
	str r1,[r0],#4				;; tx and rx enabled, reset the state machines and away we go ... 8 bit data

								;;UCR3_1
	ldr r1,=UCR3_1_SETTINGS
	str r1,[r0],#4

								;;UCR4_1 clear out the 16 MHz reference clock?!
	ldr r1,=UCR4_1_SETTINGS		;; for now, just assure reset state... debugger does something?!
	str r1,[r0]

;; ok let's try baud rate
;; The equation is [(baud)*16]/ref_frequency = num/denom
;; where num-1 = ubir
;; denom-1 = ubmr
;;	
	ldr r0,=UBIR_1_SETTINGS
	ldr r1,=UBMR_1_SETTINGS
	b SetBaudRateRegisters	;; NOTE the branch causes the return from SetBaudRateRegisters
							;; to return from here as well

;;Set up test register
;;in the iMX25 this register allows the UART to run while in debug mode this is not needed when running normaly
;; other processors use this register for other test purposes
;	IF platform_current = "imx25"
;	ldr r0,=UTS_1
;	ldr r1,=UTS_1_SETTINGS
;	str r1,[r0]
;	ENDIF	;;end set up clocks platform_current = "iMX25"


;;********************************************************************************
;;end Init_Uart1
;;********************************************************************************

	AREA UARTCFG2, CODE, READONLY

;;
;; pass in nibble to be translated to hex in r0, and send it
;; DON'T TOUCH r3 or r12 , calling functions in this file use them!!!!
;; No tested sdram or stack at this point to be able to save off registers
;;
send_nibble
     	ldr r1,=UTXDn_1     	;;uart 1 address in r0
    	and r0,r0,#0x0f     	;; mask off the nibble
     	add r0,r0,#0x30		;;make an ascii number for 0 to 9;; first I need to know is it from 0 to 9
     	cmp r0,#0x3a        	;; compare with 9  ( (0-9) - 10 should be cs)
     	addge r0,r0,#0x7	;;make a cap letter for A to F
     	str r0, [r1]            ;;send the HEX equiv of the nibble
     	bx lr			;;return




;;
;; pass in data to be sent in r0, and output each nibble as HEX character, then send 2 spaces
;; DON'T TOUCH r3 or r12 in other called functions, This function saves them and calls other functions in this file !!!!
;; No tested sdram or stack at this point to be able to save off registers
;;
;;
;; pass in data to be sent in r0, and output each nibble as HEX character, then send CR/LF
;; DON'T TOUCH r3 or r12 in other called functions, This function saves them and calls other functions in this file !!!!
;; No tested sdram or stack at this point to be able to save off registers
;;
Send_Address_in_r0
	mov	r3,r0			;;Lets save the info passed on r0 to r3
	mov	r12,lr	 		;; remember pc in r12

	bl	WaitToAllSentInUartOneTransBuffer

	mov	r0,r3, LSR #28  	;; save MS nibble in r5 Byte in r1
	bl	send_nibble
	mov	r0,r3, LSR #24
	bl	send_nibble
	mov	r0,r3, LSR #20
	bl	send_nibble
	mov	r0,r3,LSR #16
	bl	send_nibble
	mov	r0,r3,LSR #12
	bl	send_nibble
	mov	r0,r3,LSR #8
	bl	send_nibble
	mov	r0,r3,LSR #4
	bl	send_nibble
	mov	r0,r3
	bl	send_nibble

	bl	Send_CRLF

	bx 	r12			;;return



	IF MEMORY_DEBUG			;;set in Db_hdw.s
;;
;; Send a start up message
;; No tested sdram or stack at this point to be able to save off registers
;;
Send_Startup_Message
	mov	r12,lr	 		;; remember pc in r12

	bl	WaitToAllSentInUartOneTransBuffer

	ldr r0,=UTXDn_1

	;;send string : D-BALL-
	;;send string : System
	ldr r1,=0x53        ;;'S'
	str r1,[r0]         ;; send
	ldr r1,=0x79        ;;'y'
	str r1,[r0]         ;; send
	ldr r1,=0x73        ;;'s'
	str r1,[r0]         ;; send
	ldr r1,=0x74        ;;'t'
	str r1,[r0]         ;; send
	ldr r1,=0x65        ;;'e'
	str r1,[r0]         ;; send
	ldr r1,=0x6D        ;;'m'
	str r1,[r0]         ;; send
	ldr r1,=0x2d        ;;'-'
	str r1,[r0]         ;; send

	;; Send string : Startup [CR][LF]
	ldr r1,=0x53			;;'S'
	str r1,[r0]		;;send
	ldr r1,=0x74            ;;'t'
	str r1,[r0]		;;send
	ldr r1,=0x61            ;;'a'
	str r1,[r0]		;;send
	ldr r1,=0x72            ;;'r'
	str r1,[r0]		;;send
	ldr r1,=0x74            ;;'t'
	str r1,[r0]		;;send
	ldr r1,=0x75            ;;'u'
	str r1,[r0]		;;send
	ldr r1,=0x70            ;;'p'
	str r1,[r0]		;;send

	bl Send_CRLF

	bx r12		;;return



;;
;; Send out a string for ram test pass
;; No tested sdram or stack at this point to be able to save off registers
;;
Send_RAM_Pass
	mov	r12,lr	 		;; remember pc in r12

	bl	WaitToAllSentInUartOneTransBuffer

     	ldr r0,=UTXDn_1

	;; Send string: RAMok[CR][LF]
     	ldr r1,=0x52        	;;'R'
	str r1,[r0]         	;; send it
     	ldr r1,=0x41        	;;'A'
	str r1,[r0]         	;; send it
     	ldr r1,=0x4d        	;;'M'
	str r1,[r0]         	;; send it
     	ldr r1,=0x6f        	;;'o'
	str r1,[r0]         	;; send it
     	ldr r1,=0x6b        	;;'k'
	str r1,[r0]         	;; send it

	bl Send_CRLF

	mov pc,r12		;;return



	ELSE			;;else lets use a stub function
Send_Startup_Message
Send_RAM_Pass
	bx lr 		;;return
	ENDIF			;;end if ram test debug

;;
;; Send a cr lf
;; DON'T TOUCH r3 or r12 , calling functions in this file use them!!!!
;; No tested sdram or stack at this point to be able to save off registers
;;
Send_CRLF
     ldr r0,=UTXDn_1
     ldr r1,=0x0d		;;CR
     str r1,[r0]
     ldr r1,=0x0a		;;LF
     str r1,[r0]
     bx  lr			;;return



;;
;; check and wait for UART1 transmit buffer is empty
;; DON'T TOUCH r3 or r12 , calling functions in this file use them!!!!
;; No tested sdram or stack at this point to be able to save off registers
;;
WaitToAllSentInUartOneTransBuffer
	ldr	r2,=USR2_1
full_loop
	ldr	r0,[r2]
	tst	r0,#USR2_1_BIT_14_TRANS_BUF_EMPTY_MASK
	beq	full_loop		;;wait for all sent in transmit buffer
	bx lr			;;return


	AREA UARTCFG1, CODE, READONLY

;;
;; try and set up uart 1 for the caller specified baud rate
;;
SetBaudRateRegisters

	ldr r2,=UBIR_1
	str r0,[r2]

	ldr r2,=UBMR_1
	str r1,[r2]
	bx lr


;;
;; try to get for uart 1 for the baud rate set up
;;
GetBaudRateRegisters

	ldr r2,=UBIR_1
	ldr	r2,[r2]
	str r2,[r0]

	ldr r2,=UBMR_1
	ldr	r2,[r2]
	str r2,[r1]
	bx lr
	END

