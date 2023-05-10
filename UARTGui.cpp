#include "UARTGui.h"



#include "Serial.h");

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <intrin.h>
#include <bitset>

#define COMM_MSG 0x01;
#define ANSW_MSG 0x02;

using namespace std;


QString convertToQS(uint16_t i) {
    char hex_string[20];
    sprintf(hex_string, "%X", i); //convert number to hex
    QString qs(hex_string);
    return qs;
}


void SwapBytes(uint16_t& value) {
    value = (value >> 8) | (value << 8);
}

void SwapBytes(int16_t& value) {
    value = (value >> 8) | (value << 8);
}

void SwapBytes(uint32_t& value) {
    std::uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    value = (tmp << 16) | (tmp >> 16);
}

void SwapBytes(float& f) {
    uint32_t value = 0;
    memcpy(&value, &f, sizeof value);
    std::uint32_t tmp = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF);
    value = (tmp << 16) | (tmp >> 16);
    memcpy(&f, &value, sizeof f);
}

void SwapBytes(uint64_t& value) {
    value = ((value & 0x00000000FFFFFFFFull) << 32) | ((value & 0xFFFFFFFF00000000ull) >> 32);
    value = ((value & 0x0000FFFF0000FFFFull) << 16) | ((value & 0xFFFF0000FFFF0000ull) >> 16);
    value = ((value & 0x00FF00FF00FF00FFull) << 8) | ((value & 0xFF00FF00FF00FF00ull) >> 8);
}

void SwapBytes(double& d) {
    uint64_t value = 0;
    memcpy(&value, &d, sizeof value);
    value = ((value & 0x00000000FFFFFFFFull) << 32) | ((value & 0xFFFFFFFF00000000ull) >> 32);
    value = ((value & 0x0000FFFF0000FFFFull) << 16) | ((value & 0xFFFF0000FFFF0000ull) >> 16);
    value = ((value & 0x00FF00FF00FF00FFull) << 8) | ((value & 0xFF00FF00FF00FF00ull) >> 8);
    memcpy(&d, &value, sizeof d);
}


class Message {
public:
    uint8_t length;
    uint8_t header;
    uint8_t command;
    uint8_t* command_data;
    uint16_t crc = 0;
    uint8_t* buffer;
    LPCWSTR port;

    Message() {}
    ~Message() {}

    void create_msg() {
        buffer = new uint8_t[length];
        buffer[0] = length;
        buffer[1] = header;
        buffer[2] = command;
        for (int i = 0; i < length - 5; i++) {
            buffer[i + 3] = command_data[i];
        }
    }

    void calculate_crc() {
        uint32_t crc_length = length - 2;

        uint16_t data = 0;

        while (0 != crc_length--) {
            data = *(buffer++) << 8;
            for (uint32_t i = 0; i < 8; i++) {
                if ((data ^ crc) & 0x8000)
                    crc = (crc << 1) ^ 0x1021;
                else
                    crc <<= 1;

                data <<= 1;
            }
        }
        for (size_t i = 0; i < length - 2; i++)
        {
            buffer--;
        }
        uint16_t in = crc;
        uint8_t out[2];
        *(uint16_t*)&out = in;
        buffer[length - 2] = out[1];
        buffer[length - 1] = out[0];
    }

    void send() {
        Serial serial = Serial(port);
        serial.send(buffer, length);
    }
};

class BITCommand : public Message {                                             /////////                 BÝT
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;
        uint32_t bitSonucu;
        uint8_t KTSBitSonucu;
        uint32_t TAPABitSonucu;
        uint32_t rezerve1;
        uint32_t rezerve2;
        uint16_t crc;
    };

public:
    response_data_struct rds;
    BITCommand() {
        command = 0x11;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);
        ui.textBrowser->append("bitSonucu: ");
        SwapBytes(rds.bitSonucu);
        ui.textBrowser->append(bitset<32>(rds.bitSonucu));
        ui.textBrowser->append("KTSBitSonucu: ");
        ui.textBrowser->append(bitset<8>(rds.KTSBitSonucu));
        ui.textBrowser->append("TAPABitSonucu: ");
        SwapBytes(rds.TAPABitSonucu);
        ui.textBrowser->append(bitset<32>(rds.TAPABitSonucu));
        ui.textBrowser->append("Rezerve: ");
        SwapBytes(rds.rezerve1);
        ui.textBrowser->append(bitset<32>(rds.rezerve1));
        ui.textBrowser->append("Rezerve: ");
        SwapBytes(rds.rezerve2);
        ui.textBrowser->append(bitset<32>(rds.rezerve2));
        ui.textBrowser->append("crc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class ICommand : public Message {                                             /////////                 IComman
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;
        uint8_t seriNo[12];
        uint16_t crc;
    };
public:
    response_data_struct rds;
    ICommand() {
        command = 0x21;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("seriNo: ");
        for (size_t i = 0; i < sizeof rds.seriNo; i++)
        {
            ui.textBrowser->append(rds.seriNo[i]);
        }


        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};


class MCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;
        uint8_t islemSonucu;
        uint8_t modDurumu;
        uint16_t crc;
    };
    struct command_data_struct
    {
        uint8_t modeInfo = 0;
    };

    command_data_struct cds;

public:
    response_data_struct rds;
    MCommand() {
        command = 0x20;
        header = COMM_MSG;
        length = 6;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("islemSonucu: ");
        ui.textBrowser->append(hex << (int)rds.islemSonucu);
        ui.textBrowser->append("modDurumu: ");
        ui.textBrowser->append(hex << (int)rds.modDurumu);


        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class LCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;
        uint8_t uky_modu;
        float lp_range;
        double rezerve[3];
        double lp_pos_llh_t[3];
        double rezerve_2;
        double rezerve_3;
        uint8_t rezerve_4;
        uint8_t lp_proxfuze;
        float lp_fts_cond[4];
        float rezerve_5[3];
        double rezerve_6;
        uint16_t lp_proxfuze_dist;
        double rezerve_7;
        uint8_t rezerve_8[11];
        uint16_t crc;
    };
    struct command_data_struct {
        uint8_t uky_modu = 0;
        float lp_range = 0;
        double rezerve[3] = { 0 };
        double lp_pos_llh_t[3] = { 0 };
        double rezerve_2 = 0;
        double rezerve_3 = 0;
        uint8_t rezerve_4 = 0;
        uint8_t lp_proxfuze = 0;
        float lp_fts_cond[4] = { 0 };
        float rezerve_5[3] = { 0 };
        double rezerve_6 = 0;
        uint16_t lp_proxfuze_dist = 0;
        double rezerve_7 = 0;
        uint8_t rezerve_8[11] = { 0 };
    };

    command_data_struct cds;


public:
    response_data_struct rds;
    LCommand() {
        command = 0x22;
        header = COMM_MSG;
        length = 0x85;
        command_data = new uint8_t[length - 5];
    }

    void bufferCommData() {
        SwapBytes(cds.lp_range);
        for (int i = 0; i < sizeof(cds.rezerve) / 8; i++) {
            SwapBytes(cds.rezerve[i]);
        }
        for (int i = 0; i < sizeof(cds.lp_pos_llh_t) / 8; i++) {
            SwapBytes(cds.lp_pos_llh_t[i]);
        }
        SwapBytes(cds.rezerve_2);
        SwapBytes(cds.rezerve_3);
        for (int i = 0; i < sizeof(cds.lp_fts_cond) / 4; i++) {
            SwapBytes(cds.lp_fts_cond[i]);
        }

        for (int i = 0; i < sizeof(cds.rezerve_5) / 4; i++) {
            SwapBytes(cds.rezerve_5[i]);
        }
        SwapBytes(cds.rezerve_6);
        SwapBytes(cds.lp_proxfuze_dist);
        SwapBytes(cds.rezerve_7);
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("uky_modu: ");
        ui.textBrowser->append(hex << (int)rds.uky_modu);
        ui.textBrowser->append("lp_range: ");
        SwapBytes(rds.lp_range);
        ui.textBrowser->append(rds.lp_range);
        ui.textBrowser->append("Rezerve:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.rezerve[i]);
            ui.textBrowser->append("\t" << rds.rezerve[i]);
        }
        ui.textBrowser->append("lp_pos_llh_t:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.lp_pos_llh_t[i]);
            ui.textBrowser->append("\t" << rds.lp_pos_llh_t[i]);
        }
        ui.textBrowser->append("rezerve_2: ");
        SwapBytes(rds.rezerve_2);
        ui.textBrowser->append(rds.rezerve_2);
        ui.textBrowser->append("rezerve_3: ");
        SwapBytes(rds.rezerve_3);
        ui.textBrowser->append(rds.rezerve_3);
        ui.textBrowser->append("rezerve_4: ");
        ui.textBrowser->append(hex << (int)rds.rezerve_4);
        ui.textBrowser->append("lp_proxfuze: ");
        ui.textBrowser->append(hex << (int)rds.lp_proxfuze);
        ui.textBrowser->append("lp_fts_cond:\n");
        for (size_t i = 0; i < 4; i++)
        {
            SwapBytes(rds.lp_fts_cond[i]);
            ui.textBrowser->append("\t" << rds.lp_fts_cond[i]);
        }
        ui.textBrowser->append("rezerve_5:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.rezerve_5[i]);
            ui.textBrowser->append("\t" << rds.rezerve_5[i]);
        }
        ui.textBrowser->append("rezerve_6: ");
        SwapBytes(rds.rezerve_6);
        ui.textBrowser->append(rds.rezerve_6);
        ui.textBrowser->append("lp_proxfuze_dist: ");
        SwapBytes(rds.lp_proxfuze_dist);
        printf("%u\n", (uint16_t)rds.lp_proxfuze_dist);
        ui.textBrowser->append("rezerve_7: ");
        SwapBytes(rds.rezerve_7);
        ui.textBrowser->append(rds.rezerve_7);
        ui.textBrowser->append("Rezerve_8: ");
        for (size_t i = 0; i < sizeof rds.rezerve_8; i++)
        {
            ui.textBrowser->append(rds.rezerve_8[i];
        }

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class GCCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;
        uint8_t OUT_GC_CK;
        uint16_t OUT_GC_Diag;
        uint8_t KKS1_UyduSayisi;
        uint8_t KKS2_UyduSayisi;
        uint8_t KKS3_UyduSayisi;
        uint8_t Rezerve1[5];
        double Enlem;
        double Boylam;
        double Yukseklik;
        float Dogu_Hizi;
        float Kuzey_Hizi;
        float Yukari_Hizi;
        double Enlem2;
        double Boylam2;
        double Yukseklik2;
        float Dogu_Hizi2;
        float Kuzey_Hizi2;
        float Yukari_Hizi2;
        double Enlem3;
        double Boylam3;
        double Yukseklik3;
        float Dogu_Hizi3;
        float Kuzey_Hizi3;
        float Yukari_Hizi3;
        uint8_t Rezerve2[83];
        uint16_t crc;
    };
public:
    response_data_struct rds;
    GCCommand() {
        command = 0x23;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("OUT_GC_CK: ");
        ui.textBrowser->append(hex << (int)rds.OUT_GC_CK);
        ui.textBrowser->append("OUT_GC_Diag: ");
        SwapBytes(rds.OUT_GC_Diag);
        ui.textBrowser->append(bitset<16>(rds.OUT_GC_Diag));
        ui.textBrowser->append("KKS1_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKS1_UyduSayisi);
        ui.textBrowser->append("KKS2_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKS2_UyduSayisi);
        ui.textBrowser->append("KKS3_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKS3_UyduSayisi);
        ui.textBrowser->append("Rezerve: ");
        for (size_t i = 0; i < 5; i++)
        {
            ui.textBrowser->append(rds.Rezerve1[i]);
        }
        ui.textBrowser->append("Enlem: ");
        SwapBytes(rds.Enlem);
        ui.textBrowser->append(rds.Enlem);
        ui.textBrowser->append("Boylam: ");
        SwapBytes(rds.Boylam);
        ui.textBrowser->append(rds.Boylam);
        ui.textBrowser->append("Yukseklik: ");
        SwapBytes(rds.Yukseklik);
        ui.textBrowser->append(rds.Yukseklik);
        ui.textBrowser->append("Dogu_Hizi: ");
        SwapBytes(rds.Dogu_Hizi);
        ui.textBrowser->append(rds.Dogu_Hizi);
        ui.textBrowser->append("Kuzey_Hizi: ");
        SwapBytes(rds.Kuzey_Hizi);
        ui.textBrowser->append(rds.Kuzey_Hizi);
        ui.textBrowser->append("Yukari_Hizi: ");
        SwapBytes(rds.Yukari_Hizi);
        ui.textBrowser->append(rds.Yukari_Hizi);
        ui.textBrowser->append("Enlem2: ");
        SwapBytes(rds.Enlem2);
        ui.textBrowser->append(rds.Enlem2);
        ui.textBrowser->append("Boylam2: ");
        SwapBytes(rds.Boylam2);
        ui.textBrowser->append(rds.Boylam2);
        ui.textBrowser->append("Yukseklik2: ");
        SwapBytes(rds.Yukseklik2);
        ui.textBrowser->append(rds.Yukseklik2);
        ui.textBrowser->append("Dogu_Hizi2: ");
        SwapBytes(rds.Dogu_Hizi2);
        ui.textBrowser->append(rds.Dogu_Hizi2);
        ui.textBrowser->append("Kuzey_Hizi2: ");
        SwapBytes(rds.Kuzey_Hizi2);
        ui.textBrowser->append(rds.Kuzey_Hizi2);
        ui.textBrowser->append("Yukari_Hizi2: ");
        SwapBytes(rds.Yukari_Hizi2);
        ui.textBrowser->append(rds.Yukari_Hizi2);
        ui.textBrowser->append("Enlem3: ");
        SwapBytes(rds.Enlem3);
        ui.textBrowser->append(rds.Enlem3);
        ui.textBrowser->append("Boylam3: ");
        SwapBytes(rds.Boylam3);
        ui.textBrowser->append(rds.Boylam3);
        ui.textBrowser->append("Yukseklik3: ");
        SwapBytes(rds.Yukseklik3);
        ui.textBrowser->append(rds.Yukseklik3);
        ui.textBrowser->append("Dogu_Hizi3: ");
        SwapBytes(rds.Dogu_Hizi3);
        ui.textBrowser->append(rds.Dogu_Hizi3);
        ui.textBrowser->append("Kuzey_Hizi3: ");
        SwapBytes(rds.Kuzey_Hizi3);
        ui.textBrowser->append(rds.Kuzey_Hizi3);
        ui.textBrowser->append("Yukari_Hizi3: ");
        SwapBytes(rds.Yukari_Hizi3);
        ui.textBrowser->append(rds.Yukari_Hizi3);




        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class FCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;
        uint8_t OUT_F_CK;
        uint32_t OUT_F_Diag;
        uint8_t tfrBasarim;
        float eulerF[3];
        uint8_t KKSUyduSayýsý1;
        uint8_t KKSUyduSayýsý2;
        uint8_t KKSUyduSayýsý3;
        uint8_t KTSMotorGerilimDurumu;
        uint16_t HariciVoltaj;
        uint16_t DahiliVoltaj;
        uint16_t KTSVoltaj;
        float WF[3];
        float AF[3];
        float VF[3];
        float XF[3];
        float XF_1[3];
        float XF_2[3];
        float XF_3[3];
        uint8_t Rezerve[47];
        uint16_t crc;



    };
    struct command_data_struct {
        uint16_t validityWord = 0;
        uint16_t timeTagFXR = 0;
        float latitudeHV = 0;
        float longitudeHV = 0;
        float baroAltitudeHV = 0;
        float velocityXHV = 0;
        float velocityEastHV = 0;
        float velocityDownHV = 0;
        int16_t rollAngleHV = 0;
        int16_t pitchAngleHV = 0;
        int16_t headingHV = 0;
        float wanderAngleHV = 0;
        int16_t velocityWindXHV = 0;
        int16_t velocityWindYHV = 0;
        int16_t velocityWindZHV = 0;
        uint16_t pressureStaticHV = 0;
        int16_t temperatureStaticHV = 0;
        int16_t trueAirSpeedHV = 0;
        int16_t positionDeltaXLAU = 0;
        int16_t positionDeltaYLAU = 0;
        int16_t positionDeltaZLAU = 0;
        float lauRollAngle = 0;
        float lauIncidence = 0;
        float lauMisalignment = 0;
    };

    command_data_struct cds;

public:
    response_data_struct rds;
    FCommand() {
        command = 0x24;
        header = COMM_MSG;
        length = 73;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        SwapBytes(cds.validityWord);
        SwapBytes(cds.timeTagFXR);
        SwapBytes(cds.latitudeHV);
        SwapBytes(cds.longitudeHV);
        SwapBytes(cds.baroAltitudeHV);
        SwapBytes(cds.velocityXHV);
        SwapBytes(cds.velocityXHV);
        SwapBytes(cds.velocityEastHV);
        SwapBytes(cds.velocityDownHV);
        SwapBytes(cds.rollAngleHV);
        SwapBytes(cds.pitchAngleHV);
        SwapBytes(cds.headingHV);
        SwapBytes(cds.wanderAngleHV);
        SwapBytes(cds.velocityWindXHV);
        SwapBytes(cds.velocityWindYHV);
        SwapBytes(cds.velocityWindZHV);
        SwapBytes(cds.pressureStaticHV);
        SwapBytes(cds.temperatureStaticHV);
        SwapBytes(cds.trueAirSpeedHV);
        SwapBytes(cds.positionDeltaXLAU);
        SwapBytes(cds.positionDeltaYLAU);
        SwapBytes(cds.positionDeltaZLAU);
        SwapBytes(cds.lauRollAngle);
        SwapBytes(cds.lauIncidence);
        SwapBytes(cds.lauMisalignment);
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("OUT_F_CK: ");
        ui.textBrowser->append(hex << (int)rds.OUT_F_CK);
        ui.textBrowser->append("OUT_F_Diag: ");
        SwapBytes(rds.OUT_F_Diag);
        ui.textBrowser->append(bitset<32>(rds.OUT_F_Diag));
        ui.textBrowser->append("tfrBasarim: ");
        ui.textBrowser->append(hex << (int)rds.tfrBasarim);
        ui.textBrowser->append("eulerF:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.eulerF[i]);
            ui.textBrowser->append("\t" << rds.eulerF[i]);
        }
        ui.textBrowser->append("KKS1_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKSUyduSayýsý1);
        ui.textBrowser->append("KKS2_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKSUyduSayýsý2);
        ui.textBrowser->append("KKS3_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKSUyduSayýsý3);
        ui.textBrowser->append("KTSMotorGerilimDurumu: ");
        ui.textBrowser->append(bitset<8>(rds.KTSMotorGerilimDurumu));
        ui.textBrowser->append("HariciVoltaj: ");
        SwapBytes(rds.HariciVoltaj);
        printf("%u\n", (uint16_t)rds.HariciVoltaj);
        ui.textBrowser->append("DahiliVoltaj: ");
        SwapBytes(rds.DahiliVoltaj);
        printf("%u\n", (uint16_t)rds.DahiliVoltaj);
        ui.textBrowser->append("KTSVoltaj: ");
        SwapBytes(rds.KTSVoltaj);
        printf("%u\n", (uint16_t)rds.KTSVoltaj);
        ui.textBrowser->append("WF:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.WF[i]);
            ui.textBrowser->append("\t" << rds.WF[i]);
        }
        ui.textBrowser->append("AF:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.AF[i]);
            ui.textBrowser->append("\t" << rds.AF[i]);
        }
        ui.textBrowser->append("VF:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.VF[i]);
            ui.textBrowser->append("\t" << rds.VF[i]);
        }
        ui.textBrowser->append("XF:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.XF[i]);
            ui.textBrowser->append("\t" << rds.XF[i]);
        }
        ui.textBrowser->append("XF_1:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.XF_1[i]);
            ui.textBrowser->append("\t" << rds.XF_1[i]);
        }
        ui.textBrowser->append("XF_2:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.XF_2[i]);
            ui.textBrowser->append("\t" << rds.XF_2[i]);
        }
        ui.textBrowser->append("XF_3:\n");
        for (size_t i = 0; i < 3; i++)
        {
            SwapBytes(rds.XF_3[i]);
            ui.textBrowser->append("\t" << rds.XF_3[i]);
        }

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class ACommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t KKSUyduSaysisi1;
        double LATITUDE1;
        double LONGITUDE1;
        double ALTITUDE1;
        double X_VEL1;
        double Y_VEL1;
        double Z_VEL1;
        float gyrX;
        float gyrY;
        float gyrZ;
        float accX;
        float accY;
        float accZ;
        float IA1;
        float IA2;
        float IA3;
        float IG1;
        float IG2;
        float IG3;
        uint8_t KKSUyduSaysisi2;
        double Vx;
        double Vy;
        double Vz;
        double LATITUDE2;
        double LONGITUDE2;
        double ALTITUDE2;
        uint8_t KKS3_UyduSayisi;
        double XKonum3;
        double YKonum3;
        double ZKonum3;
        double XHizi3;
        double YHizi3;
        double ZHizi3;

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint16_t Sure = 4;
    };

    command_data_struct cds;
public:
    response_data_struct rds;
    ACommand() {
        command = 0x26;
        header = COMM_MSG;
        length = 0x07;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        SwapBytes(cds.Sure);
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("KKS1_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKSUyduSaysisi1);

        ui.textBrowser->append("LATITUDE1: ");
        SwapBytes(rds.LATITUDE1);
        ui.textBrowser->append(rds.LATITUDE1);
        ui.textBrowser->append("LONGITUDE1: ");
        SwapBytes(rds.LONGITUDE1);
        ui.textBrowser->append(rds.LONGITUDE1);
        ui.textBrowser->append("ALTITUDE1: ");
        SwapBytes(rds.ALTITUDE1);
        ui.textBrowser->append(rds.ALTITUDE1);
        ui.textBrowser->append("X_VEL1: ");
        SwapBytes(rds.X_VEL1);
        ui.textBrowser->append(rds.X_VEL1);
        ui.textBrowser->append("Y_VEL1: ");
        SwapBytes(rds.Y_VEL1);
        ui.textBrowser->append(rds.Y_VEL1);
        ui.textBrowser->append("Z_VEL1: ");
        SwapBytes(rds.Z_VEL1);
        ui.textBrowser->append(rds.Z_VEL1);
        ui.textBrowser->append("gyrX: ");
        SwapBytes(rds.gyrX);
        ui.textBrowser->append(rds.gyrX);
        ui.textBrowser->append("gyrY: ");
        SwapBytes(rds.gyrY);
        ui.textBrowser->append(rds.gyrY);
        ui.textBrowser->append("gyrZ: ");
        SwapBytes(rds.gyrZ);
        ui.textBrowser->append(rds.gyrZ);
        ui.textBrowser->append("accX: ");
        SwapBytes(rds.accX);
        ui.textBrowser->append(rds.accX);
        ui.textBrowser->append("accY: ");
        SwapBytes(rds.accY);
        ui.textBrowser->append(rds.accY);
        ui.textBrowser->append("accZ: ");
        SwapBytes(rds.accZ);
        ui.textBrowser->append(rds.accZ);
        ui.textBrowser->append("IA1: ");
        SwapBytes(rds.IA1);
        ui.textBrowser->append(rds.IA1);
        ui.textBrowser->append("IA2: ");
        SwapBytes(rds.IA2);
        ui.textBrowser->append(rds.IA2);
        ui.textBrowser->append("IA3: ");
        SwapBytes(rds.IA3);
        ui.textBrowser->append(rds.IA3);
        ui.textBrowser->append("IG1: ");
        SwapBytes(rds.IG1);
        ui.textBrowser->append(rds.IG1);
        ui.textBrowser->append("IG2: ");
        SwapBytes(rds.IG2);
        ui.textBrowser->append(rds.IG2);
        ui.textBrowser->append("IG3: ");
        SwapBytes(rds.IG3);
        ui.textBrowser->append(rds.IG3);

        ui.textBrowser->append("KKS2_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKSUyduSaysisi2);

        ui.textBrowser->append("Vx: ");
        SwapBytes(rds.Vx);
        ui.textBrowser->append(rds.Vx);
        ui.textBrowser->append("Vy: ");
        SwapBytes(rds.Vy);
        ui.textBrowser->append(rds.Vy);
        ui.textBrowser->append("Vz: ");
        SwapBytes(rds.Vz);
        ui.textBrowser->append(rds.Vz);
        ui.textBrowser->append("LATITUDE2: ");
        SwapBytes(rds.LATITUDE2);
        ui.textBrowser->append(rds.LATITUDE2);
        ui.textBrowser->append("LONGITUDE2: ");
        SwapBytes(rds.LONGITUDE2);
        ui.textBrowser->append(rds.LONGITUDE2);
        ui.textBrowser->append("ALTITUDE2: ");
        SwapBytes(rds.ALTITUDE2);
        ui.textBrowser->append(rds.ALTITUDE2);

        ui.textBrowser->append("KKS3_UyduSayisi: ");
        printf("%u\n", (uint8_t)rds.KKS3_UyduSayisi);

        ui.textBrowser->append("XKonum3: ");
        SwapBytes(rds.XKonum3);
        ui.textBrowser->append(rds.XKonum3);
        ui.textBrowser->append("YKonum3: ");
        SwapBytes(rds.YKonum3);
        ui.textBrowser->append(rds.YKonum3);
        ui.textBrowser->append("ZKonum3: ");
        SwapBytes(rds.ZKonum3);
        ui.textBrowser->append(rds.ZKonum3);
        ui.textBrowser->append("XHizi3: ");
        SwapBytes(rds.XHizi3);
        ui.textBrowser->append(rds.XHizi3);
        ui.textBrowser->append("YHizi3: ");
        SwapBytes(rds.YHizi3);
        ui.textBrowser->append(rds.YHizi3);
        ui.textBrowser->append("ZHizi3: ");
        SwapBytes(rds.ZHizi3);
        ui.textBrowser->append(rds.ZHizi3);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class OCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t haberlesmeDurumu;
        uint8_t KTS1TakasCVerisi[34];
        uint8_t KTS2TakasCVerisi[34];
        uint8_t KTS3TakasCVerisi[34];
        uint8_t KTS4TakasCVerisi[34];

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint32_t CASCommand[4] = { 1, 2, 3, 4 };
    };

    command_data_struct cds;
public:
    response_data_struct rds;
    OCommand() {
        command = 0x34;
        header = COMM_MSG;
        length = 21;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        for (int i = 0; i < sizeof(cds.CASCommand) / 4; i++) {
            SwapBytes(cds.CASCommand[i]);
        }
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("haberlesmeDurumu: ");
        ui.textBrowser->append(bitset<8>(rds.haberlesmeDurumu));

        ui.textBrowser->append("KTS1TakasCVerisi: ");
        for (size_t i = 0; i < 34; i++)
        {
            ui.textBrowser->append(hex << (int)rds.KTS1TakasCVerisi[i]);
            ui.textBrowser->append(" ");
        }
        
        ui.textBrowser->append("KTS2TakasCVerisi: ");
        for (size_t i = 0; i < 34; i++)
        {
            ui.textBrowser->append(hex << (int)rds.KTS2TakasCVerisi[i]);
            ui.textBrowser->append(" ");
        }
        
        ui.textBrowser->append("KTS3TakasCVerisi: ");
        for (size_t i = 0; i < 34; i++)
        {
            ui.textBrowser->append(hex << (int)rds.KTS3TakasCVerisi[i]);
            ui.textBrowser->append(" ");
        }
        
        ui.textBrowser->append("KTS3TakasCVerisi: ");
        for (size_t i = 0; i < 34; i++)
        {
            ui.textBrowser->append(hex << (int)rds.KTS3TakasCVerisi[i]);
            ui.textBrowser->append(" ");
        }

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class OperationTime : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint32_t toplamGucVerilmeSayisi;
        uint32_t toplamGucVerilmeSuresi;
        uint32_t sonCalismaSuresi;

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint32_t sureAyar = 4;
    };

    command_data_struct cds;
public:
    response_data_struct rds;
    OperationTime() {
        command = 0x50;
        header = COMM_MSG;
        length = 0x09;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        SwapBytes(cds.sureAyar);
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("toplamGucVerilmeSayisi: ");
        printf("%u\n", (uint32_t)rds.toplamGucVerilmeSayisi);
        ui.textBrowser->append("toplamGucVerilmeSuresi: ");
        printf("%u\n", (uint32_t)rds.toplamGucVerilmeSuresi);
        ui.textBrowser->append("sonCalismaSuresi: ");
        printf("%u\n", (uint32_t)rds.sonCalismaSuresi);

        ui.textBrowser->append("crc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class Update : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t guncellemeDurum;
        uint8_t guncelVeriNo;
        uint8_t guncelVeriDegeri[8];

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint8_t islemIstegi = 0x0c;
        uint16_t veriTabaniNo = 0x0a0b;
        uint8_t veriDegeri[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    };

    command_data_struct cds;

public:
    response_data_struct rds;
    Update() {
        command = 0x16;
        header = COMM_MSG;
        length = 0x10;
        command_data = new uint8_t[length - 5];

    }
    void bufferCommData() {
        SwapBytes(cds.veriTabaniNo);
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("guncellemeDurum: ");
        ui.textBrowser->append(hex << (int)rds.guncellemeDurum);
        ui.textBrowser->append("guncelVeriNo: ");
        printf("%u\n", (uint16_t)rds.guncelVeriNo);
        ui.textBrowser->append("guncelVeriDegeri: ");
        for (size_t i = 0; i < 8; i++)
        {
            ui.textBrowser->append(hex << (int)rds.guncelVeriDegeri[i]);
            ui.textBrowser->append(" ");
        }

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class Version : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t versiyonFBY[23];
        uint8_t versiyonFBYAlgDB[23];
        uint8_t versiyonFBYConfigDBY[23];
        uint8_t versiyonFBB[23];
        uint8_t versiyonMosaicD[15];
        uint8_t versiyonMosaicY[15];
        uint8_t versiyonAntaresD[15];
        uint8_t versiyonAntaresY[15];
        uint8_t versiyonAOBD[15];
        uint8_t versiyonAOBY[15];
        uint8_t versiyonFMSKD[15];
        uint8_t versiyonFMSKY[15];

        uint16_t crc;
    };
public:
    response_data_struct rds;
    Version() {
        command = 0x13;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("versiyonFBY: ");
        for (size_t i = 0; i < 23; i++)
        {
            ui.textBrowser->append(rds.versiyonFBY[i]);
        }
        
        ui.textBrowser->append("versiyonFBYAlgDB: ");
        for (size_t i = 0; i < 23; i++)
        {
            ui.textBrowser->append(rds.versiyonFBYAlgDB[i]);
        }
        
        ui.textBrowser->append("versiyonFBYConfigDBY: ");
        for (size_t i = 0; i < 23; i++)
        {
            ui.textBrowser->append(rds.versiyonFBYConfigDBY[i]);
        }
        
        ui.textBrowser->append("versiyonFBB: ");
        for (size_t i = 0; i < 23; i++)
        {
            ui.textBrowser->append(rds.versiyonFBB[i]);
        }
        
        ui.textBrowser->append("versiyonMosaicD: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonMosaicD[i]);
        }
        
        ui.textBrowser->append("versiyonMosaicY: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonMosaicY[i]);
        }
        
        ui.textBrowser->append("versiyonAntaresD: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonAntaresD[i]);
        }
        
        ui.textBrowser->append("versiyonAntaresY: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonAntaresY[i]);
        }
        
        ui.textBrowser->append("versiyonAOBD: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonAOBD[i]);
        }
        
        ui.textBrowser->append("versiyonAOBY: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonAOBY[i]);
        }
        
        ui.textBrowser->append("versiyonFMSKD: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonFMSKD[i]);
        }
        
        ui.textBrowser->append("versiyonFMSKY: ");
        for (size_t i = 0; i < 15; i++)
        {
            ui.textBrowser->append(rds.versiyonFMSKY[i]);
        }
        


        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class VoltajSorgu : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint16_t Haricivolt;
        uint16_t Dahilivolt;
        uint16_t KKS1volt;
        uint16_t KKS2volt;
        uint16_t KTSvolt;
        uint16_t AOBvolt;
        uint16_t Yedekvolt;
        uint16_t GBvolt;
        uint16_t GBakim;
        uint16_t FMSKVolt;
        uint16_t FRENVolt;

        uint16_t crc;
    };
public:
    response_data_struct rds;
    VoltajSorgu() {
        command = 0x18;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("Haricivolt: ");
        SwapBytes(rds.Haricivolt);
        ui.textBrowser->append(convertToQS(rds.Haricivolt));
        ui.textBrowser->append("Dahilivolt: ");
        SwapBytes(rds.Dahilivolt);
        ui.textBrowser->append(rds.Dahilivolt);
        ui.textBrowser->append("KKS1volt: ");
        SwapBytes(rds.KKS1volt);
        ui.textBrowser->append(rds.KKS1volt);
        ui.textBrowser->append("KKS2volt: ");
        SwapBytes(rds.KKS2volt);
        ui.textBrowser->append(rds.KKS2volt);
        ui.textBrowser->append("KTSvolt: ");
        SwapBytes(rds.KTSvolt);
        ui.textBrowser->append(rds.KTSvolt);
        ui.textBrowser->append("AOBvolt: ");
        SwapBytes(rds.AOBvolt);
        ui.textBrowser->append(rds.AOBvolt);
        ui.textBrowser->append("Yedekvolt: ");
        SwapBytes(rds.Yedekvolt);
        ui.textBrowser->append(convertToQS(rds.Yedekvolt));
        ui.textBrowser->append("GBvolt: ");
        SwapBytes(rds.GBvolt);
        ui.textBrowser->append(rds.GBvolt);
        ui.textBrowser->append("GBakim: ");
        SwapBytes(rds.GBakim);
        ui.textBrowser->append(rds.GBakim);
        ui.textBrowser->append("FMSKVolt: ");
        SwapBytes(rds.FMSKVolt);
        ui.textBrowser->append(rds.FMSKVolt);
        ui.textBrowser->append("FRENVolt: ");
        SwapBytes(rds.FRENVolt);
        ui.textBrowser->append(rds.FRENVolt);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class Reset : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t islemSonucu;

        uint16_t crc;
    };
public:
    response_data_struct rds;
    Reset() {
        command = 0x12;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("islemSonucu: ");
        ui.textBrowser->append(hex << (int)rds.islemSonucu);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class PCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        float gyrX;
        float gyrY;
        float gyrZ;
        float accX;
        float accY;
        float accZ;
        float IA1;
        float IA2;
        float IA3;
        float IG1;
        float IG2;
        float IG3;

        uint16_t crc;
    };
public:
    response_data_struct rds;
    PCommand() {
        command = 0x31;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("gyrX: ");
        SwapBytes(rds.gyrX);
        ui.textBrowser->append(rds.gyrX);
        ui.textBrowser->append("gyrY: ");
        SwapBytes(rds.gyrY);
        ui.textBrowser->append(rds.gyrY);
        ui.textBrowser->append("gyrZ: ");
        SwapBytes(rds.gyrZ);
        ui.textBrowser->append(rds.gyrZ);
        ui.textBrowser->append("accX: ");
        SwapBytes(rds.accX);
        ui.textBrowser->append(rds.accX);
        ui.textBrowser->append("accY: ");
        SwapBytes(rds.accY);
        ui.textBrowser->append(rds.accY);
        ui.textBrowser->append("accZ: ");
        SwapBytes(rds.accZ);
        ui.textBrowser->append(rds.accZ);
        ui.textBrowser->append("IA1: ");
        SwapBytes(rds.IA1);
        ui.textBrowser->append(rds.IA1);
        ui.textBrowser->append("IA2: ");
        SwapBytes(rds.IA2);
        ui.textBrowser->append(rds.IA2);
        ui.textBrowser->append("IA3: ");
        SwapBytes(rds.IA3);
        ui.textBrowser->append(rds.IA3);
        ui.textBrowser->append("IG1: ");
        SwapBytes(rds.IG1);
        ui.textBrowser->append(rds.IG1);
        ui.textBrowser->append("IG2: ");
        SwapBytes(rds.IG2);
        ui.textBrowser->append(rds.IG2);
        ui.textBrowser->append("IG3: ");
        SwapBytes(rds.IG3);
        ui.textBrowser->append(rds.IG3);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class SCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t Sonuc;

        uint16_t crc;
    };
public:
    response_data_struct rds;
    SCommand() {
        command = 0x32;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("islemSonucu: ");
        ui.textBrowser->append(hex << (int)rds.Sonuc);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class FrenCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t islemSonucu;

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint8_t sureAyar = 4;
    };

    command_data_struct cds;
public:
    response_data_struct rds;
    FrenCommand() {
        command = 0x35;
        header = COMM_MSG;
        length = 0x06;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("islemSonucu: ");
        ui.textBrowser->append(hex << (int)rds.islemSonucu);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class VTReset : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t islemSonucu;

        uint16_t crc;
    };
public:
    response_data_struct rds;
    VTReset() {
        command = 0x66;
        header = COMM_MSG;
        length = 0x05;
        command_data = new uint8_t[length - 5];
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("islemSonucu: ");
        ui.textBrowser->append(hex << (int)rds.islemSonucu);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class TVTYazma : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint8_t islemIstegi = 0;
        uint16_t veriTabaniNo = 0;
        uint32_t veriTabaniDegeri = 0;
    };

    command_data_struct cds;

public:
    response_data_struct rds;
    TVTYazma() {
        command = 0x16;
        header = COMM_MSG;
        length = 12;
        command_data = new uint8_t[length - 5];

    }
    void bufferCommData() {
        SwapBytes(cds.veriTabaniNo);
        SwapBytes(cds.veriTabaniDegeri);
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);



        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class LGCommand : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t LimitValue[235];

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint8_t LimitValue[235] = { 0 };
    };

    command_data_struct cds;
public:
    response_data_struct rds;
    LGCommand() {
        command = 0x51;
        header = COMM_MSG;
        length = 240;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        for (size_t i = 0; i < 235; i++)
        {
            ui.textBrowser->append(rds.LimitValue[i]);
        }

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};

class PBITMessage : public Message {
#pragma pack(1)
    struct response_data_struct {
        uint8_t length;
        uint8_t header;
        uint8_t command;

        uint8_t aktivasyoDirenciOkumaDurumu;
        uint8_t direncOkumaRedSebebi;
        uint8_t aktivasyonDirenci;
        uint8_t gobekBagiDurumu;

        uint16_t crc;
    };
    struct command_data_struct
    {
        uint8_t PirotenikSorgu = 0;
    };

    command_data_struct cds;
public:
    response_data_struct rds;
    PBITMessage() {
        command = 0x52;
        header = COMM_MSG;
        length = 6;
        command_data = new uint8_t[length - 5];
    }
    void bufferCommData() {
        memcpy(command_data, (uint8_t*)&cds, sizeof(cds));
    }
    void print_rds(Ui::UARTGuiClass ui) {
        ui.textBrowser->append("length: ");
        ui.textBrowser->append(hex << (int)rds.length);
        ui.textBrowser->append("header: ");
        ui.textBrowser->append(hex << (int)rds.header);
        ui.textBrowser->append("command: ");
        ui.textBrowser->append(hex << (int)rds.command);

        ui.textBrowser->append("aktivasyoDirenciOkumaDurumu: ");
        ui.textBrowser->append(hex << (int)rds.aktivasyoDirenciOkumaDurumu);

        ui.textBrowser->append("haberlesmeDurumu: ");
        ui.textBrowser->append(bitset<8>(rds.direncOkumaRedSebebi));

        ui.textBrowser->append("guncelVeriNo: ");
        printf("%u\n", (uint8_t)rds.aktivasyonDirenci);

        ui.textBrowser->append("gobekBagiDurumu: ");
        ui.textBrowser->append(hex << (int)rds.gobekBagiDurumu);

        ui.textBrowser->append("\ncrc: ");
        SwapBytes(rds.crc);
        ui.textBrowser->append(hex << (int)rds.crc);
    }
};



UARTGui::UARTGui(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    
}

void UARTGui::on_pushButton_2_clicked() {


    ui.comboBox->setEnabled(false);

}

void UARTGui::on_pushButton_clicked() {
    

    Serial serial = Serial(port_input);



    switch (ui.comboBox->currentIndex())
    {
    case 0: {
        BITCommand b_msg = BITCommand();
        ui.textBrowser->append("Creating BITCommand...\n");
        b_msg.create_msg();
        b_msg.calculate_crc();
        ui.textBrowser->append("Sending BITCommand...\n");
        serial.send(b_msg.buffer, b_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&b_msg.rds, recv_msg, len);
        b_msg.print_rds(ui);
        break;
    }
    case 1: {
        ICommand i_msg = ICommand();
        ui.textBrowser->append("Creating ICommand...\n");
        i_msg.create_msg();
        i_msg.calculate_crc();
        ui.textBrowser->append("Sending ICommand...\n");
        serial.send(i_msg.buffer, i_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&i_msg.rds, recv_msg, len);
        i_msg.print_rds(ui);
        break;
    }

    case 2: {
        ui.textBrowser->append("Creating MCommand...\n");
        MCommand m_msg = MCommand();
        m_msg.bufferCommData();
        m_msg.create_msg();
        m_msg.calculate_crc();
        ui.textBrowser->append("Sending MCommand...\n");
        serial.send(m_msg.buffer, m_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&m_msg.rds, recv_msg, len);
        m_msg.print_rds(ui);
        break;
    }

    case 3: {
        ui.textBrowser->append("Creating LCommand...\n");
        LCommand l_msg = LCommand();
        l_msg.bufferCommData();
        l_msg.create_msg();
        l_msg.calculate_crc();
        ui.textBrowser->append("Sending LCommand...\n");
        serial.send(l_msg.buffer, l_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&l_msg.rds, recv_msg, len);
        l_msg.print_rds(ui);
        break;
    }

    case 4: {
        GCCommand gc_msg = GCCommand();
        ui.textBrowser->append("Creating GCCommand...\n");
        gc_msg.create_msg();
        gc_msg.calculate_crc();
        ui.textBrowser->append("Sending GCCommand...\n");
        serial.send(gc_msg.buffer, gc_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&gc_msg.rds, recv_msg, len);
        gc_msg.print_rds(ui);
        break;
    }

    case 5: {
        FCommand f_msg = FCommand();
        ui.textBrowser->append("Creating FCommand...\n");
        f_msg.bufferCommData();
        f_msg.create_msg();
        f_msg.calculate_crc();
        ui.textBrowser->append("Sending FCommand...\n");
        serial.send(f_msg.buffer, f_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&f_msg.rds, recv_msg, len);
        f_msg.print_rds(ui);
        break;
    }

    case 6: {
        ACommand a_msg = ACommand();
        ui.textBrowser->append("Creating ACommand...\n");
        a_msg.bufferCommData();
        a_msg.create_msg();
        a_msg.calculate_crc();
        ui.textBrowser->append("Sending ACommand...\n");
        serial.send(a_msg.buffer, a_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&a_msg.rds, recv_msg, len);
        a_msg.print_rds(ui);
        break;
    }

    case 7: {
        OCommand o_msg = OCommand();
        ui.textBrowser->append("Creating OCommand...\n");
        o_msg.bufferCommData();
        o_msg.create_msg();
        o_msg.calculate_crc();
        ui.textBrowser->append("Sending OCommand...\n");
        serial.send(o_msg.buffer, o_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&o_msg.rds, recv_msg, len);
        o_msg.print_rds(ui);
        break;
    }

    case 8: {
        OperationTime ot_msg = OperationTime();
        ui.textBrowser->append("Creating OperationTime Message...\n");
        ot_msg.bufferCommData();
        ot_msg.create_msg();
        ot_msg.calculate_crc();
        ui.textBrowser->append("Sending OperationTime Message...\n");
        serial.send(ot_msg.buffer, ot_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&ot_msg.rds, recv_msg, len);
        ot_msg.print_rds(ui);
        break;
    }

    case 9: {
        Update up_msg = Update();
        ui.textBrowser->append("Creating UpdateMsg...\n");
        up_msg.bufferCommData();
        up_msg.create_msg();
        up_msg.calculate_crc();
        ui.textBrowser->append("Sending UpdateMsg...\n");
        serial.send(up_msg.buffer, up_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&up_msg.rds, recv_msg, len);
        up_msg.print_rds(ui);
        break;
    }

    case 10: {
        Version v_msg = Version();
        ui.textBrowser->append("Creating VersionMsg...\n");
        v_msg.create_msg();
        v_msg.calculate_crc();
        ui.textBrowser->append("Sending VersionMsg...\n");
        serial.send(v_msg.buffer, v_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&v_msg.rds, recv_msg, len);
        v_msg.print_rds(ui);
        break;
    }

    case 11: {
        VoltajSorgu volt_msg = VoltajSorgu();
        ui.textBrowser->append("Creating VoltajSorgu...\n");
        volt_msg.create_msg();
        volt_msg.calculate_crc();
        ui.textBrowser->append("Sending VoltajSorgu...\n");
        serial.send(volt_msg.buffer, volt_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&volt_msg.rds, recv_msg, len);
        volt_msg.print_rds(ui);
        break;
    }

    case 12: {
        Reset r_msg = Reset();
        ui.textBrowser->append("Creating ResetCommand...\n");
        r_msg.create_msg();
        r_msg.calculate_crc();
        ui.textBrowser->append("Sending ResetCommand...\n");
        serial.send(r_msg.buffer, r_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&r_msg.rds, recv_msg, len);
        r_msg.print_rds(ui);
        break;
    }

    case 13: {
        PCommand p_msg = PCommand();
        ui.textBrowser->append("Creating PCommand...\n");
        p_msg.create_msg();
        p_msg.calculate_crc();
        serial.send(p_msg.buffer, p_msg.length);
        ui.textBrowser->append("Sending PCommand...\n");
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&p_msg.rds, recv_msg, len);
        p_msg.print_rds(ui);
        break;
    }

    case 14: {
        SCommand s_msg = SCommand();
        ui.textBrowser->append("Creating SCommand...\n");
        s_msg.create_msg();
        s_msg.calculate_crc();
        serial.send(s_msg.buffer, s_msg.length);
        ui.textBrowser->append("Sending SCommand...\n");
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&s_msg.rds, recv_msg, len);
        s_msg.print_rds(ui);
        break;
    }

    case 15: {
        FrenCommand fr_msg = FrenCommand();
        ui.textBrowser->append("Creating FrenCommand...\n");
        fr_msg.bufferCommData();
        fr_msg.create_msg();
        fr_msg.calculate_crc();
        ui.textBrowser->append("Sending FrenCommand...\n");
        serial.send(fr_msg.buffer, fr_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&fr_msg.rds, recv_msg, len);
        fr_msg.print_rds(ui);
        break;
    }

    case 16: {
        VTReset vtr_msg = VTReset();
        ui.textBrowser->append("Creating VTResetCommand...\n");
        vtr_msg.create_msg();
        vtr_msg.calculate_crc();
        ui.textBrowser->append("Sending VTResetCommand...\n");
        serial.send(vtr_msg.buffer, vtr_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&vtr_msg.rds, recv_msg, len);
        vtr_msg.print_rds(ui);
        break;
    }

    case 17: {
        TVTYazma tvt_msg = TVTYazma();
        ui.textBrowser->append("Creating TVTYazmaCommand...\n");
        tvt_msg.bufferCommData();
        tvt_msg.create_msg();
        tvt_msg.calculate_crc();
        ui.textBrowser->append("Sending TVTYazmaCommand...\n");
        serial.send(tvt_msg.buffer, tvt_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&tvt_msg.rds, recv_msg, len);
        tvt_msg.print_rds(ui);
        break;
    }

    case 18: {
        LGCommand lg_msg = LGCommand();
        ui.textBrowser->append("Creating LGCommand...\n");
        lg_msg.bufferCommData();
        lg_msg.create_msg();
        lg_msg.calculate_crc();
        ui.textBrowser->append("Sending LGCommand...\n");
        serial.send(lg_msg.buffer, lg_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&lg_msg.rds, recv_msg, len);
        lg_msg.print_rds(ui);
        break;
    }

    case 19: {
        PBITMessage pbit_msg = PBITMessage();
        ui.textBrowser->append("Creating PBITMessage...\n");
        pbit_msg.bufferCommData();
        pbit_msg.create_msg();
        pbit_msg.calculate_crc();
        ui.textBrowser->append("Sending PBITMessage...\n");
        serial.send(pbit_msg.buffer, pbit_msg.length);
        uint8_t* recv_msg = serial.receive();
        ui.textBrowser->append("Receiving...\n\n");
        ui.textBrowser->append("Response Message:\n");
        int len = recv_msg[0];
        if (len == 205) {
            ui.textBrowser->append("No response.\n");
            break;
        }
        memcpy(&pbit_msg.rds, recv_msg, len);
        pbit_msg.print_rds(ui);
        break;
    }

    default:
        break;
    }
}
