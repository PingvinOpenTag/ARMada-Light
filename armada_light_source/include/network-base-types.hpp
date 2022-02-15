#ifndef RF_BASE_TYPES_HPP_
#define RF_BASE_TYPES_HPP_

#include <stdint.h>
#include <functional>
#include <initializer_list>


using PackageId = uint16_t;



struct DeviceAddress
{
	constexpr static uint8_t size = 3;
	uint8_t address[size];

	DeviceAddress(uint8_t a0 = 1, uint8_t a1 = 1, uint8_t a2 = 1)
		{ address[0] = a0; address[1] = a1, address[2] = a2; }

	void convertFromString(const char* str);

	// Operators
	inline bool operator==(const DeviceAddress& other) const
	{
		for(int i=0; i<size; i++)
			if (address[i] != other.address[i])
				return false;
		return true;
	}

	inline bool operator!=(const DeviceAddress& other) const
	{
		return not (*this == other);
	}

	inline bool operator<(const DeviceAddress& other) const
	{
		for(int i=0; i<size; i++)
		{
			if (address[i] < other.address[i])
				return true;
			if (address[i] > other.address[i])
				return false;
		}
		return false;
	}

	inline DeviceAddress& operator=(const std::initializer_list<uint8_t>& args)
	{
		int i=0;
		for (auto v: args)
		{
			address[i++] = v;
			if (i == 3) break;
		}
		return *this;
	}
};



struct PackageTimings
{
	constexpr static uint32_t defaultTimeout = 10000000;
	constexpr static uint32_t defaultResendTime = 200000;
	constexpr static uint32_t defaultResendTimeDelta = 100000;

	PackageTimings(
			bool _infiniteResend = false,
			uint32_t _timeout = defaultTimeout,
			uint32_t _resendTime = defaultResendTime,
			uint32_t _resendTimeDelta = defaultResendTimeDelta
			) : infiniteResend(_infiniteResend), timeout(_timeout), resendTime(_resendTime), resendTimeDelta(_resendTimeDelta)
	{ }

	bool infiniteResend = false;
	uint32_t timeout = defaultTimeout;
	uint32_t resendTime = defaultResendTime;
	uint32_t resendTimeDelta = defaultResendTimeDelta;
};

using ReceivePackageCallback = std::function<void(DeviceAddress /*sender*/, uint8_t* /*payload*/, uint16_t /*payload length*/)>;
using PackageSendingDoneCallback = std::function<void(PackageId /*package_id*/, bool /*was successfully sent*/)>;



#pragma pack(push, 1)
struct PackageDetails
{
	PackageDetails(uint16_t id, uint8_t ack = 0, uint16_t ttl = 0) :
		packageId(id),
		TTL(ttl),
		needAck(ack)
	{}

	PackageDetails() :
		packageId(0),
		TTL(0),
		needAck(0)
	{
	}

	PackageId packageId;
	uint8_t TTL : 7;
	uint8_t needAck : 1;
};
#pragma pack(pop)



#pragma pack(push, 1)
struct PackageHeader
{
	DeviceAddress sender;
	DeviceAddress target;
	PackageDetails details;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Package
{
	DeviceAddress sender;
	DeviceAddress target;
	PackageDetails details;

	/// payloadLength is 23 bytes
	constexpr static uint16_t packageLength = 32;//IRadioPhysicalDevice::defaultRFPhysicalPayloadSize;
	constexpr static uint16_t payloadLength /*23*/ = packageLength /*32*/ - sizeof(sender)/*3*/ - sizeof(target)/*3*/ - sizeof(details)/*3*/;

	uint8_t payload[payloadLength];
};
static_assert(sizeof(Package) == Package::packageLength, "Network layer package size is bad");
#pragma pack(pop)



#pragma pack(push, 1)
	struct AckPayload
	{
		constexpr static uint16_t acknoledgementCode = 0xFFFF;
		uint8_t size = sizeof(uint16_t);
		uint16_t operationCode = acknoledgementCode;
		uint16_t packageId;
		bool isAck() { return operationCode == acknoledgementCode; }
	};
#pragma pack(pop)

using Time = portTickType;//uint64_t;

struct WaitingPackage
{
	Time wasCreated = 0;
	Time nextTransmission = 0;
	bool isBroadcast = false;
	PackageTimings timings;
	PackageSendingDoneCallback callback;
	Package package;
};


enum rf_data_type{
	PING,
	STATUS_DATA,
	NICKNAME
};

using trf_data_type = rf_data_type;



#pragma pack(push, 1)
struct hit_confirmation{
	uint8_t need_to_send:1;//нужно ли отправлять подтверждение
	uint8_t in_game :1;//в игре или вне игры
	uint8_t team_color:2; //цвет команды
	uint8_t gun_damage:4;//нанесённый урон
	uint8_t player_id;//идентификатор игрока
};

#pragma pack(pop)


using thit_confirmation = hit_confirmation;



#pragma pack(push, 1)

struct  status_data_payload{//������ 25 ����
	 trf_data_type type; //��� ������������ ������						8 ���
	 uint8_t team_color :2;//���� �������								2 ����
	 uint8_t gun_damage:4;//����, ������� ������� ������				4 ����
	 uint8_t in_game :1;//� ���� ��� ��� ����							1 ���
	 uint8_t bluetooth_is_connected:1;//���������� �� ������-�������	1 ���
	 uint8_t player_id;//������������� ������							8 ���
	 uint8_t health;//��������											8 ���
	 uint16_t battery_voltage;//���������� �� ������� (� ������������)	16 ���
	 uint16_t clips;//���������� �����									16 ���
	 unsigned char nickname[13];//�������								15*8 ���
//	 trx_packet hit_data;//												3*8 ���
	 thit_confirmation  hit_data;//										2*8 ���

};

#pragma pack(pop)

using tstatus_payload = status_data_payload;




#endif
