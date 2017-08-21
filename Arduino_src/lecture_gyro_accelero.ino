#include <DueTimer.h>
#include <SPI.h>

const byte CTRL_REG1  = 0x20; // rq: CTRL_REG's permettent de contrôler le
const byte CTRL_REG2  = 0x21; // fonctionnement du périph.
const byte CTRL_REG3  = 0x22;
const byte CTRL_REG4  = 0x24;
const byte STATUS_REG = 0x27;

void Timer_irq()
{
    //byte STAT = readRegister(STATUS_REG);
    //if((bitRead(STAT,3)==0))//verif du statut du capteur non utilisé
    // if((bitRead(STAT,7)==1))//verif du statut du capteur non utilisé

    int  X_OUT_acc  = read_accelero_Register(0x29);
    int  Y_OUT_acc  = read_accelero_Register(0x2B);
    int  Z_OUT_acc  = read_accelero_Register(0x2D);
    int  X_OUT_gyro = read_Gyro_Register(0x29);
    int  Y_OUT_gyro = read_Gyro_Register(0x2B);
    int  Z_OUT_gyro = read_Gyro_Register(0x2D);

    char buff[20];
    sprintf(buff,"%02d%02d%02d",X_OUT_acc,Y_OUT_acc,Z_OUT_acc);
    //pour envoyer aussi le gyro remplacer la ligne d'avant par
    //sprintf(buff,"%02d%02d%02d%02d%02d%02d",X_OUT_acc,Y_OUT_acc,Z_OUT_acc,X_OUT_gyro,Y_OUT_gyro,Z_OUT_gyro);
    Serial.print(buff);
}

void setup(){
    Serial.begin(9600);// le nombre  correspond à la vitesse de transmission du signal en baud (nbre de symbol transmis par secondes) Pour a communication avec le pc
    SPI.begin();// permet d'initialiser la fonction et SPI dispositif de communication
    pinMode(53, OUTPUT);//gyro // configure le pin 53 en mode sortie pour la CS
    pinMode(52, OUTPUT); // accelero
    SPI.setBitOrder(MSBFIRST); // Ordre d’envoi des bits
    SPI.setDataMode(SPI_MODE3); // definition des modalités de fonctionnement du SPI ( cs actif à 0 ou 1, clock sur front montants ou descendants...)
    SPI.setClockDivider(8);
    //accelero
    write_accelero_Register(CTRL_REG3,0x80);
    write_accelero_Register(CTRL_REG2,0x00);
    write_accelero_Register(CTRL_REG1,0x47);//400Hz fréquence de sortie

    //gyro
    write_Gyro_Register(CTRL_REG4,0x00);
    write_Gyro_Register(CTRL_REG2,0x00);
    write_Gyro_Register(CTRL_REG1,0xAF);

    Timer4.attachInterrupt(Timer_irq);
    Timer4.setFrequency(10);//how many times per second // ten for T=50ms
    while(Serial.read()==-1);
    Timer4.start();

}

void loop(){
}



inline void write_Gyro_Register(byte reg, byte value)
{
    writeRegister(reg,value,53);
}

inline void write_accelero_Register(byte reg, byte value)
{
    writeRegister(reg,value,52);
}

inline byte read_Gyro_Register(byte reg)
{
    return ReadRegister(reg,53);
}

inline byte read_accelero_Register(byte reg)
{
    return ReadRegister(reg,52);
}

inline byte ReadRegister(byte reg,int pin) { //sert à lire le registre "reg" en paramètre
    byte result = 0;
    reg = reg | 0x80; //mode lecture
    reg = reg & 0xBF; //mode pas d'incrementation adresse
    digitalWrite(pin, LOW); // Selection du chip
    SPI.transfer(reg); // demande de lecture du registre
    result = SPI.transfer(0x00); // Lecture
    digitalWrite(pin, HIGH); // Désélectionne un bit
    return result;
}


inline void writeRegister(byte reg, byte value,int pin) {
    byte data[2]={reg,value};
    reg = reg & 0x00111111; // Spécifie le registre dans lequel on veut écrire
    digitalWrite(pin, LOW); // Met a 0 la CS su gyro pour activer le device
    SPI.transfer(data,2); //on envoie les données
    digitalWrite(pin, HIGH); // Deselect the device
}

