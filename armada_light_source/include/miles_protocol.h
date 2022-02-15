#ifndef __MILES_PROTOCOL_H
#define __MILES_PROTOCOL_H

#define IR_START_BIT_DURATION 2400	// ������������ �����-���� (� �������������)
#define IR_ONE_BIT_DURATION 1200	// ������������ ����, ���������������� �������� (� �������������)
#define IR_ZERO_BIT_DURATION 600	// ������������ ����, ���������������� ���� (� �������������)
#define IR_SPACE_DURATION 600		// ������������ ����, ���������������� ��������� ����� ������ (� �������������)

#define IR_TOL                      20 //���������� ����������� ��� ������ �� ������ (� ���������

// ���� ������ ������ ���������� �������� � �������� ��������� � ���������
#define checkVal(var,val,tol) (var>(val*(100-tol)/100) && var<(val*(100+tol)/100))

//#define SAFE_TIME 50
//#define DEFAULT_SHOCK_TIME 100


//---------------------------------------------------------------------//

enum Team_Color {	//��������� ������������� ��� ��� ������ � ������ �������
	Red, 	//00 = Red
	Blue, 	//01 = Blue
	Yellow, //10 = Yellow
	Green}; //11 = Green

typedef enum Team_Color tteam_color;

//---------------------------------------------------------------------//
//��������� ������������� ���
//��� ������ � "������"
enum GunDamage {

	Damage_1,  //0000 = 1
	Damage_2,  //0001 = 2
	Damage_4,  //0010 = 4
	Damage_5,  //0011 = 5
	Damage_7,  //0100 = 7
	Damage_10, //0101 = 10
	Damage_15, //0110 = 15
	Damage_17, //0111 = 17
	Damage_20, //1000 = 20
	Damage_25, //1001 = 25
	Damage_30, //1010 = 30
	Damage_35, //1011 = 35
	Damage_40, //1100 = 40
	Damage_50, //1101 = 50
	Damage_75, //1110 = 75
	Damage_100 //1111 = 100
	};


typedef enum GunDamage tgun_damage;


#define Add_Health 0x80 //��������� �������� �����
#define Add_Rounds 0x81 //��������� �������� ��������
#define Change_color 0xA9//��������� ������� ���� �������
#define Command 0x83 //���������-�������
#define Valid_value 0xE8 //���������� �������� ������������ ����� ���������
#define SYSTEM_DATA 0x87 //��������� ������
#define CLONING_DATA 0x01 //������ ��� ������������
#define TAG_INIT_DATA 0x02//������ ��� ��������� ����
#define IR_POWER_INCREMENT 0x18
#define IR_POWER_DECREMENT 0x19
#define IR_DUTY_CYCLE_INCREMENT 0x23
#define IR_DUTY_CYCLE_DECREMENT 0x24
#define ANOMALY 0xA1
#define RADIATION 0xA0
#define SET_ACTIVE_PRESET 0xA8
#define SET_IR_DUTY_CYCLE 0x84





#endif /* __MILES_PROTOCOL_H */
