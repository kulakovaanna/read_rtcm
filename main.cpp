#include <iostream>
#include <fstream>
#include <bitset>
#include <iterator>
#include <vector>
#include <cmath>

#define pi 3.1415926535897932
#define GM 398600500000000      //GM = 3.986005*10^14 [m^3 /s^2]
#define omega_e 0.00007292115   //value of the Earth’s rotation rate
#define RTCM3PREAMB 211

using namespace std;

typedef struct  RTCM1019_STRUCTURE {
    /* header | 24  bits */
    int32_t preamble;    //      |  8  bits
    //6 zeroes           //      |  6  bits
    int32_t length;      //      | 10  bits

    /* data message | 488  bits */
    int32_t type;        // DF002 | 12  bits | u
    int32_t satellite_id;// DF009 |  6  bits | u
    int32_t week;        // DF076 | 10  bits | u
    int32_t sv_accuracy; // DF077 |  4  bits | u
    int32_t code_on_l2;  // DF078 |  2  bits | u
    double idot;         // DF079 | 14  bits | s
    int32_t iode;        // DF071 |  8  bits | u
    double t_oc;         // DF081 | 16  bits | u
    double a_f2;         // DF082 |  8  bits | s
    double a_f1;         // DF083 | 16  bits | s
    double a_f0;         // DF084 | 22  bits | s
    int32_t idoc;        // DF085 | 10  bits | u
    double c_rs;         // DF086 | 16  bits | s
    double delta_n;      // DF087 | 16  bits | s
    double M0;           // DF088 | 32  bits | s
    double c_uc;         // DF089 | 16  bits | s
    double e;            // DF090 | 32  bits | u
    double c_us;         // DF091 | 16  bits | s
    double A_sqrt;       // DF092 | 32  bits | u
    double t_oe;         // DF093 | 16  bits | u | время действия эфемирид
    double c_ic;         // DF094 | 16  bits | s
    double OMEGA0;       // DF095 | 16  bits | s
    double c_is;         // DF096 | 16  bits | s
    double i0;           // DF097 | 32  bits | s
    double c_rc;         // DF098 | 16  bits | s
    double omega;        // DF099 | 32  bits | s
    double OMEGADOT;     // DF100 | 24  bits | s
    double t_GD;         // DF101 |  8  bits | s
    int32_t sv_health;   // DF102 |  6  bits | u
    bool flag;           // DF103 |  1  bits | u
    char fit;            // DF137 |  1  bits | u

    /* parity | 24  bits */
    int32_t parity;      //      | 24  bits
} rtcm1019_pack;

const uint32_t tbl_CRC24Q[]={
    0x000000,0x864CFB,0x8AD50D,0x0C99F6,0x93E6E1,0x15AA1A,0x1933EC,0x9F7F17,
    0xA18139,0x27CDC2,0x2B5434,0xAD18CF,0x3267D8,0xB42B23,0xB8B2D5,0x3EFE2E,
    0xC54E89,0x430272,0x4F9B84,0xC9D77F,0x56A868,0xD0E493,0xDC7D65,0x5A319E,
    0x64CFB0,0xE2834B,0xEE1ABD,0x685646,0xF72951,0x7165AA,0x7DFC5C,0xFBB0A7,
    0x0CD1E9,0x8A9D12,0x8604E4,0x00481F,0x9F3708,0x197BF3,0x15E205,0x93AEFE,
    0xAD50D0,0x2B1C2B,0x2785DD,0xA1C926,0x3EB631,0xB8FACA,0xB4633C,0x322FC7,
    0xC99F60,0x4FD39B,0x434A6D,0xC50696,0x5A7981,0xDC357A,0xD0AC8C,0x56E077,
    0x681E59,0xEE52A2,0xE2CB54,0x6487AF,0xFBF8B8,0x7DB443,0x712DB5,0xF7614E,
    0x19A3D2,0x9FEF29,0x9376DF,0x153A24,0x8A4533,0x0C09C8,0x00903E,0x86DCC5,
    0xB822EB,0x3E6E10,0x32F7E6,0xB4BB1D,0x2BC40A,0xAD88F1,0xA11107,0x275DFC,
    0xDCED5B,0x5AA1A0,0x563856,0xD074AD,0x4F0BBA,0xC94741,0xC5DEB7,0x43924C,
    0x7D6C62,0xFB2099,0xF7B96F,0x71F594,0xEE8A83,0x68C678,0x645F8E,0xE21375,
    0x15723B,0x933EC0,0x9FA736,0x19EBCD,0x8694DA,0x00D821,0x0C41D7,0x8A0D2C,
    0xB4F302,0x32BFF9,0x3E260F,0xB86AF4,0x2715E3,0xA15918,0xADC0EE,0x2B8C15,
    0xD03CB2,0x567049,0x5AE9BF,0xDCA544,0x43DA53,0xC596A8,0xC90F5E,0x4F43A5,
    0x71BD8B,0xF7F170,0xFB6886,0x7D247D,0xE25B6A,0x641791,0x688E67,0xEEC29C,
    0x3347A4,0xB50B5F,0xB992A9,0x3FDE52,0xA0A145,0x26EDBE,0x2A7448,0xAC38B3,
    0x92C69D,0x148A66,0x181390,0x9E5F6B,0x01207C,0x876C87,0x8BF571,0x0DB98A,
    0xF6092D,0x7045D6,0x7CDC20,0xFA90DB,0x65EFCC,0xE3A337,0xEF3AC1,0x69763A,
    0x578814,0xD1C4EF,0xDD5D19,0x5B11E2,0xC46EF5,0x42220E,0x4EBBF8,0xC8F703,
    0x3F964D,0xB9DAB6,0xB54340,0x330FBB,0xAC70AC,0x2A3C57,0x26A5A1,0xA0E95A,
    0x9E1774,0x185B8F,0x14C279,0x928E82,0x0DF195,0x8BBD6E,0x872498,0x016863,
    0xFAD8C4,0x7C943F,0x700DC9,0xF64132,0x693E25,0xEF72DE,0xE3EB28,0x65A7D3,
    0x5B59FD,0xDD1506,0xD18CF0,0x57C00B,0xC8BF1C,0x4EF3E7,0x426A11,0xC426EA,
    0x2AE476,0xACA88D,0xA0317B,0x267D80,0xB90297,0x3F4E6C,0x33D79A,0xB59B61,
    0x8B654F,0x0D29B4,0x01B042,0x87FCB9,0x1883AE,0x9ECF55,0x9256A3,0x141A58,
    0xEFAAFF,0x69E604,0x657FF2,0xE33309,0x7C4C1E,0xFA00E5,0xF69913,0x70D5E8,
    0x4E2BC6,0xC8673D,0xC4FECB,0x42B230,0xDDCD27,0x5B81DC,0x57182A,0xD154D1,
    0x26359F,0xA07964,0xACE092,0x2AAC69,0xB5D37E,0x339F85,0x3F0673,0xB94A88,
    0x87B4A6,0x01F85D,0x0D61AB,0x8B2D50,0x145247,0x921EBC,0x9E874A,0x18CBB1,
    0xE37B16,0x6537ED,0x69AE1B,0xEFE2E0,0x709DF7,0xF6D10C,0xFA48FA,0x7C0401,
    0x42FA2F,0xC4B6D4,0xC82F22,0x4E63D9,0xD11CCE,0x575035,0x5BC9C3,0xDD8538
};

uint32_t crc24q(const uint8_t *buff, int32_t len)
{
    uint32_t crc=0;
    int i;
    for (i=0;i<len;i++) {
        uint32_t crc0 = (crc<<8)&0xFFFFFF;
        uint32_t crc1 = crc>>16;
        crc= crc0^tbl_CRC24Q[crc1^buff[i]];
    }
    return crc;
}

uint32_t bit32u(const uint8_t *buff, int32_t pos, int32_t len)
{
    uint32_t bits=0;
    for (int32_t i=pos;i<pos+len;i++) bits=(bits<<1)+((buff[i/8]>>(7-i%8))&1u);
    return bits;
}

int32_t bit32s(const uint8_t *buff, int32_t pos, int32_t len)
{
    uint32_t bits= bit32u(buff,pos,len);
    if (len<=0||32<=len||!(bits&(1u<<(len-1)))) return int32_t(bits);
    return int(bits|(~0u<<len)); /* extend sign */
}

bool is_rtcm3(const uint8_t* data, int32_t& data_size, uint32_t& tp) {

	if (*data != RTCM3PREAMB || data_size < 3+1+3)
		return false;
	
	int32_t len = bit32s(data, 14, 10);
	
	if (len<0 || len>5000 || data_size< 3+len+3)
		return false;

	uint32_t par = bit32u(data, 24+len*8, 24);
	uint32_t par_chk = crc24q(data, 3+len);

	if (par_chk != par)
		return false;

	tp = bit32u(data, 24, 12);
	data_size = 3+len+3;

	return tp>0;

}

int main(int argc, char *argv[]) {
    // open file
    string filename = "/home/ftn21/Documents/MAI/6sem/app_sredstva/read_rtcm/1019.rtcm";  
    ifstream rtcm_strm;
    rtcm_strm.open(filename, /*ios::in |*/ ios::binary);

    // ?open
    if (!rtcm_strm.is_open()) {
        cout << "error opening " << filename << endl;
    }
    else {
        // get length of file:
        rtcm_strm.seekg (0, rtcm_strm.end);
        int length = rtcm_strm.tellg();
        rtcm_strm.seekg (0, rtcm_strm.beg);
        cout << "fileLength is " << length << endl;
    }

    // read data
    rtcm1019_pack rtcm_msg;
    vector<uint8_t> buffer(istreambuf_iterator<char>(rtcm_strm), {});

    int32_t s = 67;
    uint32_t c = 1019;
    if (is_rtcm3(&buffer[0], s, c)) {
        cout << "the package is rtcm3. " << endl << endl;
    }
    else {
        cout << "the package NOT is rtcm3. " << endl << endl;
    }

    // rtcm read
    /* header | 24  bits*/
    rtcm_msg.preamble = bit32u(&buffer[0], 0, 8);
    rtcm_msg.length = bit32u(&buffer[0], 14, 10);

    /* data message | 488  bits*/
    rtcm_msg.type = bit32u(&buffer[0], 24, 12);
    rtcm_msg.satellite_id = bit32u(&buffer[0], 36, 6);
    rtcm_msg.week = bit32u(&buffer[0], 42, 10);
    rtcm_msg.sv_accuracy = bit32u(&buffer[0], 52, 4);
    rtcm_msg.code_on_l2 = bit32u(&buffer[0], 56, 2);  
    rtcm_msg.idot = pow(2, -43)*bit32s(&buffer[0], 58, 14) * pi; //4.7066350816749036e-11  /pi ?     
    rtcm_msg.iode = bit32u(&buffer[0], 72, 8);        
    rtcm_msg.t_oc = pow(2, 4)*bit32u(&buffer[0], 80, 16);        
    rtcm_msg.a_f2 = bit32s(&buffer[0], 96, 8);      
    rtcm_msg.a_f1 = pow(2, -43)*bit32s(&buffer[0], 104, 16);        
    rtcm_msg.a_f0 = pow(2, -31)*bit32s(&buffer[0], 120, 22);        
    rtcm_msg.idoc = bit32s(&buffer[0], 142, 10);        
    rtcm_msg.c_rs = pow(2, -5)*bit32s(&buffer[0], 152, 16);        
    rtcm_msg.delta_n = pow(2, -43)*bit32s(&buffer[0], 168, 16) * pi; 
    rtcm_msg.M0 = pow(2, -31)*bit32s(&buffer[0], 184, 32)  * pi;      
    rtcm_msg.c_uc = pow(2, -29)*bit32s(&buffer[0], 216, 16);        
    rtcm_msg.e = pow(2, -33)*bit32u(&buffer[0], 232, 32);           
    rtcm_msg.c_us = pow(2, -29)*bit32s(&buffer[0], 264, 16);        
    rtcm_msg.A_sqrt = pow(2, -19)*bit32u(&buffer[0], 280, 32);  //9.8813129168249309e-324    
    rtcm_msg.t_oe = pow(2, 4)*bit32u(&buffer[0], 312, 16);       
    rtcm_msg.c_ic = pow(2, -29)*bit32s(&buffer[0], 328, 16);       
    rtcm_msg.OMEGA0 = pow(2, -31)*bit32s(&buffer[0], 344, 32) * pi; 
    rtcm_msg.c_is = pow(2, -29)*bit32s(&buffer[0], 376, 16);      
    rtcm_msg.i0 = pow(2, -31)*bit32s(&buffer[0], 392, 32) * pi;  //0.31093034334480762        
    rtcm_msg.c_rc = pow(2, -5)*bit32s(&buffer[0], 424, 16);       
    rtcm_msg.omega = pow(2, -31)*bit32s(&buffer[0], 440, 32) * pi;    //0.18036843976005912   
    rtcm_msg.OMEGADOT = pow(2, -43)*bit32s(&buffer[0], 472, 24) *pi;   //-2.6769839678308927e-09
    rtcm_msg.t_GD = pow(2, -31)*bit32s(&buffer[0], 496, 8);        
    rtcm_msg.sv_health = bit32u(&buffer[0], 504, 6); 
    rtcm_msg.flag = bit32u(&buffer[0], 510, 1);        
    rtcm_msg.fit = bit32u(&buffer[0], 511, 1);   // 0 - curve-fit interval is 4 hours
                                                 //1 - curve-fit is greater than 4 hours  как кодировать? 4+0, 4+1 ?  
    /* parity | 24  bits*/
    int32_t crc_pos = 24 + 8*rtcm_msg.length;
    rtcm_msg.parity = bit32u(&buffer[0], crc_pos, 24); //crc in msg
    uint32_t crc = crc24q(&buffer[0], 64);             //calc crc

    //поправка по времени
    double t = 120000;
    double dTs = t - rtcm_msg.t_oc;
    for (int i = 0; i < 2; i++ ) {
        dTs -= rtcm_msg.a_f0 + rtcm_msg.a_f1*dTs + rtcm_msg.a_f2*dTs*dTs;
    }
    dTs = rtcm_msg.a_f0 + rtcm_msg.a_f1*dTs + rtcm_msg.a_f2*dTs*dTs;

    //individual satellite time
    double tbias = t - dTs;

    //time, elapsed since the reference epoch
    double tk = tbias - rtcm_msg.t_oe;

    double A = pow(rtcm_msg.A_sqrt, 2);
    double n0 = sqrt(GM/pow(A, 3));     //Computed mean motion
    double n = n0 + rtcm_msg.delta_n;       //Corrected mean motion

    double M = rtcm_msg.M0 + n*tk;        //Mean anomaly 0.14682866 rad

    double E = M;   //эксцентрическая аномалия
    double Ek = 0;
    for (int i = 0; i < 3; i++) {
        Ek = E;
        E = M + rtcm_msg.e*sin(Ek);
    }

    double cosv = (cos(E) - rtcm_msg.e) / (1 - rtcm_msg.e*cos(E));
    double v = acos(cosv); //true anomaly
    double Fi = v + rtcm_msg.omega; // v + omega

    double du = rtcm_msg.c_uc*cos(2*Fi) + rtcm_msg.c_us*sin(2*Fi);
    double u = Fi + du; //argument of latitude

    double dr = rtcm_msg.c_rc*cos(2*Fi) + rtcm_msg.c_rs*sin(2*Fi);
    double r = A*(1 - rtcm_msg.e*cos(E)) + dr; //radius-vector

    double di = rtcm_msg.c_ic*cos(2*Fi) + rtcm_msg.c_is*sin(2*Fi);
    double i = rtcm_msg.i0 + rtcm_msg.idot + di; //inclination

    double X_op = r*cos(u); //X position in the orbital plane
    double Y_op = r*sin(u); //Y position in the orbital plane

    double OMEGA = rtcm_msg.OMEGA0 + (rtcm_msg.OMEGADOT - omega_e)*tk - omega_e*rtcm_msg.t_oe; //longtitude of ascending node [rad]

    //geocentric sattelite coordinates ECEF
    double X_ecef = X_op*cos(OMEGA) - Y_op*sin(OMEGA)*cos(i);
    double Y_ecef = X_op*sin(OMEGA) + Y_op*cos(OMEGA)*cos(i);
    double Z_ecef = Y_op*sin(i);

    return 0;
}