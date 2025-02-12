#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "ObjectUtils.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

// 직접 컨텐츠 작업자가
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	//TODO : Log
	return false;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	// TODO : DB에서 Account, 유저 정보를 긁어온다
	Protocol::S_LOGIN loginPkt;

	for (int32 i = 0; i < 3; i++)
	{
		Protocol::PlayerInfo* player = loginPkt.add_players();
		player->set_x(0);
		player->set_y(0);
		player->set_z(0);
		player->set_yaw(0);
	}

	loginPkt.set_success(true);
	SEND_PACKET(loginPkt);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	// 플레이어 생성
	PlayerRef player = ObjectUtils::CreatePlayer(static_pointer_cast<GameSession>(session));

	// 방에 입장
	GRoom->HandleEnterPlayerLocked(player);

	return true;
}

bool Handle_C_LEAVE_GAME(PacketSessionRef& session, Protocol::C_LEAVE_GAME& pkt)
{
	auto gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->player.load();
	if (player == nullptr)
		return false;

	RoomRef room = player->room.load().lock();
	if (room == nullptr)
		return false;

	room->HandleLeavePlayerLocked(player);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{

	return true;
}
