#include "SetRoomList.h"
#include "../RoomList.h"
#include "../ZRoom.h"
#include "../../BitConverter.h"

using namespace std;

SetRoomList::SetRoomList(ZRoom* nZRoom, std::vector<uint8_t> rawData, int rawDataIndex) : ZRoomCommand(nZRoom, rawData, rawDataIndex)
{
	int numRooms = rawData[rawDataIndex + 1];
	segmentOffset = BitConverter::ToInt32BE(rawData, rawDataIndex + 4) & 0x00FFFFFF;

	rooms = vector<RoomEntry*>();

	int32_t currentPtr = segmentOffset;

	for (int i = 0; i < numRooms; i++)
	{
		RoomEntry* entry = new RoomEntry(rawData, currentPtr);
		rooms.push_back(entry);

		currentPtr += 8;
	}
}

string SetRoomList::GenerateSourceCodePass1(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "%s 0x%02X, (u32)&roomList_%08X };", ZRoomCommand::GenerateSourceCodePass1(roomName).c_str(), rooms.size(), segmentOffset);
	sourceOutput = line;

	string declaration = "";

	sprintf(line, "RomFile roomList_%08X[] = \n{\n", segmentOffset);
	declaration += line;

	for (RoomEntry* entry : rooms)
	{
		string roomName = RoomList[entry->virtualAddressStart];
		sprintf(line, "\t{ (u32)_%sSegmentRomStart, (u32)_%sSegmentRomEnd },\n", roomName.c_str(), roomName.c_str());
		declaration += line;
	}

	declaration += "};\n";

	zRoom->declarations[segmentOffset] = declaration;

	return sourceOutput;
}

string SetRoomList::GenerateSourceCodePass2(string roomName)
{
	string sourceOutput = "";
	char line[2048];

	return sourceOutput;
}

string SetRoomList::GenerateExterns()
{
	string sourceOutput = "";
	char line[2048];

	sprintf(line, "extern RomFile roomList_%08X[];\n", segmentOffset);
	sourceOutput = line;

	return sourceOutput;
}

string SetRoomList::GetCommandCName()
{
	return "SCmdRoomList";
}

RoomCommand SetRoomList::GetRoomCommand()
{
	return RoomCommand::SetRoomList;
}

RoomEntry::RoomEntry(int32_t nVAS, int32_t nVAE)
{
	virtualAddressStart = nVAS;
	virtualAddressEnd = nVAE;
}

RoomEntry::RoomEntry(std::vector<uint8_t> rawData, int rawDataIndex) : RoomEntry(BitConverter::ToInt32BE(rawData, rawDataIndex + 0), BitConverter::ToInt32BE(rawData, rawDataIndex + 4))
{
}