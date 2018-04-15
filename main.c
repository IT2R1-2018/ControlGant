#include "mbed.h"
#include "main.h"

DigitalOut myled(LED1); //creation d'une sortie associée à une LED

Serial uart2(PTE22,PTE23,115200);   //creation d'un port série associé au broche (Tx, Rx, bauds)

SPI spi_1(PTD2,PTD3,PTD1);     //bus spi (mosi, miso, clk)
Ticker action;  //Timer
accel capteur[9];   //structure associé à chaque capteur


/*  Definition de broches en sortie */

DigitalOut D4C1(PTC7);  //Auriculaire capteur proximal
DigitalOut D4C2(PTC0);  //Auriculaire capteur intermédiaire
DigitalOut D4C3(PTC3);  //Auriculaire capteur distal
DigitalOut D3C1(PTC4);
DigitalOut D3C2(PTC5);
DigitalOut D3C3(PTC6);
DigitalOut D2C1(PTC10);
DigitalOut D2C2(PTC11);
DigitalOut D2C3(PTC12); //Majeur

int v_moy,delta;    //v_moy=vitesse moyenne des deux moteurs    
                    //delta=différence de vitesse entre les deux moteurs

int main() {
    int i;
    bool present;
    
    spi_1.format(8,3);  //configuration spi
    spi_1.frequency(5000000);
    
    Init_Cs();
    
    for(i=0;i<9;i++)
    {
        
    Write_Register(i,0x20,0x67); //Configuration accelerometres
    }
    
    present=Capteur_present();
    
    if(present==true)
    {
    uart2.attach(&reception);   //fonction callback quand reception sur uart
    //Init_WiFi();
    while(1) {
    lecture();
    get_commande();
    printf("valeur moyenne moteurs: %6d  difference moteurs: %6d\n\r",v_moy,delta);
    wait(0.5);
    myled=0; //allume LED
    }
    }
    myled=1;
    return 0;
}

void Init_Cs()
{
    int i;
    capteur[0].CS = &D4C1;  //affectation des chip select
    capteur[1].CS = &D4C2;
    capteur[2].CS = &D4C3;
    capteur[3].CS = &D3C1;
    capteur[4].CS = &D3C2;
    capteur[5].CS = &D3C3;
    capteur[6].CS = &D2C1;
    capteur[7].CS = &D2C2;
    capteur[8].CS = &D2C3;
    for(i=0;i<9;i++) 
    {
        *(capteur[i].CS)=1;
    }
    
}

void Write_Register(int cap, int addr, int valeur){
        *(capteur[cap].CS)=0;   //Indique au capteur que l'on souhaite communiquer avec lui
        spi_1.write(addr);      //Ecriture de l'adresse du registre
        spi_1.write(valeur);    //Ecriture de la valeur
        *(capteur[cap].CS)=1;   //Fin de la communication
}

int Read_Register(int cap, int addr)
{
        int term;
        int addr_2;
        addr_2 = (1<<7)|addr;   //Le  indique que l'on souhaite lire une valeur
        *(capteur[cap].CS)=0;   //Indique au capteur que l'on souhaite communiquer avec lui
        spi_1.write(addr_2);    //Ecriture de l'adresse du registre
        term=spi_1.write(0xFF); //Lecture de la valeur du registre
        *(capteur[cap].CS)=1;   //Fin de la communication
        return term;
    
}

bool Capteur_present(void){
    char present;
    int i;
    bool result(true);  
    for(i=0;i<9;i++)                    //parcours tout les capteurs
    {
        present=Read_Register(i,0x0F);  //Lecture du registre 0x0F
        if(present!=0b01001001){        //Si capteur présent alors il renvoie cette valeur
            result=false;
        }
    }
    return result;                      //retourne un booléen
}

void Read_x (int cap)
{
        char acc_x[2];
        acc_x[0]=Read_Register(cap,0x28);           //Lecture du registre contenant l'octet de poid faible de la valeur du capteur
        acc_x[1]=Read_Register(cap,0x29);           //Lecture du registre contenant l'octet de poid fort de la valeur du capteur
        capteur[cap].x=((acc_x[1]<<8)|acc_x[0]);    //Mise en forme de la valeur totalev du capteur
}

void Read_y (int cap)
{
        char acc_y[2];
        acc_y[0]=Read_Register(cap,0x2A);
        acc_y[1]=Read_Register(cap,0x2B);
        capteur[cap].y=((acc_y[1]<<8)|acc_y[0]);
}

void Read_z (int cap)
{
        char acc_z[2];
        acc_z[0]=Read_Register(cap,0x2C);
        acc_z[1]=Read_Register(cap,0x2D);
        capteur[cap].z=((acc_z[1]<<8)|acc_z[0]);
}

void lecture()
{
    int i;
    for(i=0;i<9;i++)
    {
    Read_x(i); //Lecture des 3 valeurs des capteurs
    Read_y(i); 
    Read_z(i); 
    capteur[i].x=capteur[i].x*(100.0/64000.0);  //met en pourcentage les valeurs des capteurs
    capteur[i].y=capteur[i].y*(100.0/64000.0);
    capteur[i].z=capteur[i].z*(100.0/64000.0);
    }
}

void get_commande(void)
{
    if((capteur[6].y>75)&&(capteur[6].y<90))
    {
         delta=50-(capteur[6].y-75)*50/15;
    }
    else if((capteur[6].y>6)&&(capteur[6].y<25))
    {
         delta=-(capteur[6].y-6)*50/15;
    }
    else delta=0;
    if((capteur[6].x>75)&&(capteur[6].x<120))
    {
    v_moy=100-(capteur[6].x-75)*20/30;
    }
    else if((capteur[6].x>-10)&&(capteur[6].x<17))
    {
        v_moy=80-(capteur[6].x)*80/17;
    }
    else
    {
     v_moy=0;
     delta=0;
    }
    if(v_moy+delta>100)v_moy-=delta;    //saturation de la vitesse des moteurs à 100
    if(v_moy-delta>100)v_moy+=delta;
    if(v_moy+delta<0)v_moy-=delta;
    if(v_moy-delta<0)v_moy+=delta;
    
}

void reception(void)
{
    char buff[100];
    char rec;
    /*uart2.gets(buff,100);     //Utilisé dans le cas d'une transmission Wifi
    printf("%s",buff);*/
    
    rec=uart2.getc();
    if(rec==0)    uart2.printf("%d",(char)v_moy);
    else if(rec==1)    uart2.printf("%d",(char)delta);
}

void sendCommand(char * command, int tempo_s)
{
    uart2.printf("%s",command);
    wait(tempo_s);   
}
    
void Init_WiFi(void)
{
    // reset module
    sendCommand("AT+RST\r\n",7); 
    
    // disconnect from any Access Point
    sendCommand("AT+CWQAP\r\n",2); 
    
    sendCommand("AT+CWMODE=3\r\n",2);
    
  // configure as Station 
    sendCommand("AT+CWJAP=\"IT2R1\",\"It2r2018\"\r\n",7);
    
    sendCommand("AT+CIFSR\r\n",2);
    
    //Connect to YOUR Access Point
    sendCommand("AT+CIPSTART=\"TCP\",\"192.168.0.100\",333\r\n",7); 
    
}