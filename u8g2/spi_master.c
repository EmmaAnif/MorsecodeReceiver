#include "../u8g2Headers/spi_master.h"
 //tweaked for PIC32MX170F256B, uses SPI1
         
/****************************************************************************
 Function
    SPI_Init
 Parameters
    void
 Returns
    void
 Description
    set up the SPI system for use. Set the clock phase and polarity, master mode
    clock source, baud rate, SS control, transfer width (8-bits)
 Notes
    don't forget to map the pins & set up the TRIS not only for the SPI pins
    but also for the DC (RB13) & Reset (RB12) pins 
 
****************************************************************************/
void SPI_Init(void){
    
    //disable analog function on all pins
    ANSELA = 0;
    ANSELB = 0;

    //map SDO, SCK and SS to the desired pins
    TRISAbits.TRISA0 = 0; //set SS/RA0 to output
    TRISAbits.TRISA1 = 0; //set SDO/RA1 to output
    TRISBbits.TRISB12 = 0; // set RST/RB112 to output
    TRISBbits.TRISB13 = 0; //set DC/RRB13 to output
    TRISBbits.TRISB14 = 0; // set SCK/RB14 to output

    RPA0R = 0b0011; //SS1
    RPA1R = 0b0011; //SDO1
    //SCK
    
    //stop and reset the SPI module
    SPI1CONbits.ON = 0; 
    SPI1CONbits.SIDL = 0; //continue in idle mode
    
    //clear the receive buffer, set enhanced buffer mode
    uint32_t clearBuff;
    while (SPI1STATbits.SPITBE = 0){
        clearBuff = SPI1BUF; //clear the Buffer Register
    }
    SPI1CONbits.ENHBUF = 1;
    
    //disable SDI bit
    SPI1CONbits.DISSDI = 1; 
    
    //do not sign extend FIFO
    SPI1CON2bits.SPISGNEXT = 0;

    //set BRG register using bit rate
    uint32_t bitRate = 1e6;
    uint32_t PbClk = 20e6; //pic32 clock rate in Hz
    SPI1BRG = (PbClk/(bitRate*2))-1;

    //clear the SPIROV bit
    SPI1STATbits.SPIROV = 0;

    //set bit width to 8bits
    SPI1CON2bits.AUDEN = 0;
    SPI1CONbits.MODE32 = 0;
    SPI1CONbits.MODE16 = 0;


    //set CKE to 0, CKP to 1
    SPI1CONbits.CKE = 0;
    SPI1CONbits.CKP = 1;

    //enable master mode, master mode slave select control
    SPI1CONbits.MSSEN = 1;
    SPI1CONbits.SMP = 0; //data sampled at middle
    SPI1CONbits.MSTEN = 1;

    //set active low slave select polarity 
    SPI1CONbits.FRMPOL = 0;

    //set PBclk as clock for SPI
    SPI1CONbits.MCLKSEL = 0;

    //Enable SPI operation
    SPI1CONbits.ON = 1;
    
}
/****************************************************************************
 Function
    SPI_Tx
 Parameters
   uint8_t data   the 8-bit value to be sent out through the SPI
 Returns
    void
 Description
    write the data to the SPIxBUF and then wait for it to go out (SPITBF)
 Notes
    don't forget to read the buffer after the transfer to prevent over-runs
 
****************************************************************************/
void SPI_Tx(uint8_t data){
    uint8_t readBUF;
    SPI1BUF = data;
    while (SPI1STATbits.SPITBF = 1){}
    readBUF = SPI1BUF;
}

/****************************************************************************
 Function
    SPI_TxBuffer
 Parameters
   uint8_t *buffer, a pointer to the buffer to be transmitted 
   uint8_t length   the number of bytes in the buffer to transmit
 Returns
    void
 Description
   loop through buffer calling SPI_Tx for each element in the buffer
 Notes

 
****************************************************************************/
void SPI_TxBuffer(uint8_t *buffer, uint8_t length){
    uint8_t pBuffer = *buffer;
    uint8_t i;
    for (i = 0; i < length; i++){
        SPI_Tx(pBuffer);
        pBuffer++;
    }
}
