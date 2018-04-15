typedef struct accel accel;
struct accel{
  int x;    //valeurs des capteurs selon x, y, z
  int y;
  int z;
  DigitalOut *CS;  
};

/*
Init_Cs()
Association des chip selec des capteurs au broches de sortie
*/

/*
Write_register(int cap,int addr, int valeur)
Ecriture dans un registre
cap     -> numéro du capteur
addr    -> addresse du registre
valeur  -> valeur à écrire
*/

/*
Read_register(int cap,int addr)
Ecriture dans un registre
cap     -> numéro du capteur
addr    -> addresse du registre
*/

/*
bool Capteur_present(void);
vérification des capteurs
*/

/*
void lecture();
lecture des valeurs x, y, z des capteurs
*/

/*
void get_commande(void);
calcul des vitesses moteurs à envoyer
*/

/*
void reception(void);
reçoit les données de l'uart2
*/

/*
void Read_x (int cap);
void Read_y (int cap);
void Read_z (int cap);
lecture des registres contenant les valeurs x, y, z des capteurs
*/

/*
void Init_WiFi(void);
Initialisation de la wifi
*/

/*
void sendCommand(char * command, int tempo_ms);
envoi des commandes suivi d'une temporisation
*/

void Init_Cs();                            
void Write_Register(int cap,int addr, int valeur);    
int Read_Register(int cap, int addr);
bool Capteur_present(void);
void lecture();
void get_commande(void);
void reception(void);
void Read_x (int cap);
void Read_y (int cap);
void Read_z (int cap);
void Init_WiFi(void);
void sendCommand(char * command, int tempo_ms);