#include "pch.h"
#include "Room.h"
#include "Player.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
}

Room::~Room()
{
}

bool Room::HandleEnterPlayerLocked(PlayerRef player)
{
	WRITE_LOCK;

	bool success = EnterPlayer(player);

	// ���� ��ġ
	player->playerInfo->set_x(Utils::GetRandom(0.f, 500.f));
	player->playerInfo->set_y(Utils::GetRandom(0.f, 500.f));
	player->playerInfo->set_z(Utils::GetRandom(0.f, 500.f));
	player->playerInfo->set_yaw(Utils::GetRandom(0.f, 500.f));

	// ���� ����� ���� �÷��̾�� �˸���
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::PlayerInfo* playerInfo = new Protocol::PlayerInfo();
		playerInfo->CopyFrom(*player->playerInfo);
		enterGamePkt.set_allocated_player(playerInfo);
		//enterGamePkt.release_player();  <- playerinfo�� �ٷ� �Ⱦƹ��� ���

		SendBufferRef sendBuffer = ClientPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->session.lock())
			session->Send(sendBuffer);
	}

	// ���� ����� �ٸ� �÷��̾�� �˸���

	return success;
}

bool Room::EnterPlayer(PlayerRef player)
{
	// �ִٸ� ������ �ִ�
	if (_players.find(player->playerInfo->object_id()) != _players.end())
		return false;

	_players.insert(make_pair(player->playerInfo->object_id(), player));

	player->room.store(shared_from_this());

	return true;
}
