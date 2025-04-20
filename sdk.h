// shot out to the alliedmodder's dota 2 sdk :)

#pragma once

#include "color.h"
#include <cstdint>
#include <steam/steam_api_common.h>
#include <steam/isteamgamecoordinator.h>
#include <network_connection.pb.h>
#include <dota_gcmessages_common_lobby.pb.h>
#include <gcsdk_gcmessages.pb.h>
#include <queue>

bool lieaboutbversion = true;
bool mustnotifygc = false;
int b_version;
int a_version = 6393;
CCallbackBase* gcmsgavailablecallback = nullptr;
HSteamPipe pipe = 0;
HSteamUser user = 0;
bool sentgchello = false;
bool lobbyinjected = false;
std::queue<std::string> gcmsgstoinject;
bool hookclientfuncs = true;

CSODOTALobby m_lobby;
CSODOTALobby lobby2;
CMsgSOIDOwner lobby_owner;
const uint64 lobby_id = 29273294585731458;
const uint64 match_id = 8261500427;
uint64 lobby_version = 29273294585731464;

// IAppSystem is a placeholder as we don't use any of it's functions
class IAppSystem {
public:
	virtual bool Connect(void* factory) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* pInterfaceName) = 0;
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual void PreShutdown() = 0;
	virtual const void* GetDependencies() = 0;
	virtual void GetTier() = 0;
	virtual void Reconnect(void* factory, const char* pInterfaceName) = 0;
	virtual bool IsSingleton() = 0;
	virtual void GetBuildType() = 0;
};

class CPlayerSlot {
public:
	CPlayerSlot(int slot) : m_Data(slot) {}

	int Get() const {
		return m_Data;
	}

	bool operator==(const CPlayerSlot& other) const {
		return other.m_Data == m_Data;
	}
	bool operator!=(const CPlayerSlot& other) const {
		return other.m_Data != m_Data;
	}

private:
	int m_Data;
};

class CPlayerUserId
{
public:
	CPlayerUserId(int index)
	{
		_index = index;
	}

	int Get() const
	{
		return _index;
	}

	bool operator==(const CPlayerUserId& other) const { return other._index == _index; }
	bool operator!=(const CPlayerUserId& other) const { return other._index != _index; }

private:
	unsigned short _index;
};


// ISource2GameClients
// Purpose: Player / Client related functions.
// Iface: Source2GameClients001
class ISource2GameClients : public IAppSystem {
	virtual void onclientconnected(CPlayerSlot slot, const char* name, const char* xuid, const char* networkid, const char* addr, bool fake) = 0;
	virtual bool clientconnect(CPlayerSlot slot, const char* pszName, uint64 xuid, const char* pszNetworkID, bool unk1, const char* pRejectReason) = 0;
	virtual void clientputinserver(CPlayerSlot slot, const char* pszName, int type, uint64 xuid) = 0;
	virtual void clientactive(CPlayerSlot slot, bool gameloaded, const char* pszName, uint64 xuid) = 0;
	virtual void clientfullyconnected(CPlayerSlot slot) = 0;
	virtual void clientdisconnected(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkId) = 0;
};

// ISource2Engine
// Purpose: Engine related stuff
class ISource2Engine : public IAppSystem {
public:
	virtual bool ispaused() = 0; // may be useful 
	virtual float gettimescale(void) const = 0; // isn't used
	virtual void* findorcreateworldsession(const char* pszWorldName, void* unk) = 0; // isn't used
	virtual void* getentitylumpfortemplate(const char*, bool, const char*, const char*) = 0; // isn't used
	virtual void* getstatsappid() const = 0; // isn't used
	virtual void* unk001(const char* pszFilename, void* pUnknown1, void* pUnknown2, void* pUnknown3) = 0;
	virtual void unk002() = 0;
};

// IVEngineServer2
// Purpose: Engine <-> Server communication
// Iface: Source2EngineToServer001
// There may be lots of useful functions here, but i suck at reversing them.
class IVEngineServer2 : public ISource2Engine {
public:
	virtual void getsteamuniverse() = 0; // unused
	virtual void ChangeLevel(const char* s1, const char* s2) = 0; // may be useful in the future
	virtual void dllmainxd() = 0; // )))
	virtual void unk001() = 0; // sub_18007BB20
	virtual void unk002() = 0; // sub_18007BB30
	virtual void unk003() = 0; // sub_18007BBC0
	virtual void precachedetail() = 0; // unused
	virtual void unk004() = 0; // sub_18007CD10
	virtual void unk005() = 0; // sub_18007CA50
	virtual void unk006() = 0; // sub_18007CAD0
	virtual void unk007() = 0; // sub_18007CAA0
	virtual void precachegeneric() = 0; // unused
	virtual void isgenericprecached() = 0; // unused
	virtual CPlayerUserId getplayeruserid(CPlayerSlot nSlot) = 0; // may be useful in the future
	virtual const char *getplayernetworkid(CPlayerSlot nslot) = 0; // unused, but why the hell not.
	virtual void getplayernetinfo() = 0; // unused
	virtual void unk008() = 0; // sub_18007C070
	virtual void unk009() = 0; // sub_18007C140
	virtual void unk010() = 0; // sub_18007C010
	virtual void servercommand(const char* cmd) = 0; // may be VERY useful
	virtual void clientcommand(CPlayerSlot slot, const char* cmd, ...) = 0; // may be VERY useful
	virtual void lightstyle() = 0; // unused
	virtual void clientprintf(CPlayerSlot slot, const char* msg) = 0; // may be VERY VERY VERY useful
	virtual bool islowviolence() = 0; // unused
	virtual bool sethltvchatban() = 0; // unused
	virtual bool isanyclientlowviolence() = 0; // unused
	virtual const char* getgamedir(void* cbufstring) = 0; // it maaaaay be useful, but i don't want to copy paste cbufferstring
	virtual CPlayerSlot createfakeclient(const char* netname) = 0; // wanna test this one out
	virtual char getclientconvarvalue(CPlayerSlot slot, const char* name) = 0; // may be useful
	virtual void logtofile(const char* msg) = 0; // meh
	virtual bool islogenabled() = 0; // something
	virtual bool issplitscreenplayer(CPlayerSlot slot) = 0; // unused
	virtual void getsplitscreenplayerattachtoedict() = 0; // unused
	virtual void getnumsplitscreenplayerattachedtoedit() = 0; // unused
	virtual void getsplitscreenplayerforedict() = 0; // unused
	virtual void unloadspawngroup() = 0; // unused
	virtual void setspawngroupdescription() = 0; // unused
	virtual bool isspawngrouploaded() = 0; // unused
	virtual bool isspawngrouploading() = 0; // unused
	virtual void makespawngroupactive() = 0; // unused
	virtual void syncspawngroup() = 0; // unused
	virtual void syncblocktillloaded() = 0; // unused
	virtual void settimescale(float scale) = 0; // unused
	virtual uint32 getappid() = 0; // used
	virtual uint64 getclientsteamid(CPlayerSlot slot) = 0; // im too lazy to define csteamid, hope this one will work out
	virtual void setgamestatsdata() = 0; // unused
	virtual void getgamestatsdata() = 0; // unused
	virtual void clientcommandkeyvalues(CPlayerSlot slot, void *pCommand) = 0; // we might use it in the veryyy far future
	virtual void setdedicatedserverbenchmarkmode(bool bench) = 0; // unused
	virtual bool isclientfullyauthenthificated(CPlayerSlot slot) = 0; // might be very useful
	virtual void getserverglobalvars() = 0; // unused
	virtual void setfakeclientconvarvalue(CPlayerSlot slot, const char* cvar, const char* value) = 0; // might be useful
	virtual void getsharededictchangeinfo() = 0; // i'm tired
	virtual void setachievementmgr() = 0; // unused
	virtual void getachmgr() = 0; // unused
	virtual bool getplayerinfo(CPlayerSlot slot, void*& info) = 0; // implement me: might be quite useful
	virtual uint64 getclientxuid(CPlayerSlot slot) = 0;
	virtual void getpvsforspawngroup() = 0; // unused
	virtual void findspawngroupbyname() = 0; // unused
	virtual CSteamID getserversteamid() = 0; // might be useful
	virtual int getbuildversion(void) const = 0; // used
	virtual bool isclientlowviolence(CPlayerSlot slot) = 0; // unused
	virtual void disconnectclient(CPlayerSlot slot, int reason) = 0; // used
};

class IServerGCLobby {
public:
	virtual bool haslobby() const = 0;
	virtual bool steamidallowedtoconnect(const char *steamId) const = 0;
	virtual void updateserverdetails(void) = 0;
	virtual bool shouldHibernate() = 0;
	virtual bool steamidallowedtop2pconnect(const char *steamId) const = 0;
	virtual bool lobbyallowscheats(void) const = 0;
};

class ISource2Server : public IAppSystem {
public:
	virtual bool unk001() const = 0;
	virtual void setglobals(void* pGlobals) = 0;
	virtual void gamecreatenetworkingtables(void) = 0;
	virtual void writesignonmsg(const char* buf) = 0;
	virtual void preworldupd(bool simulating) = 0;
	virtual void getentity2networktables(void) const = 0;
	virtual void* getentinfo() = 0;
	virtual void applygamesetings(void *pKV) = 0;
	virtual void gameframe(bool simulating, bool bFirstTick, bool bLastTick) = 0;
	virtual bool shouldhidefrommasterserver(bool bServerHasPassword) = 0;
	virtual void getmmtags(char* buf, size_t bufSize) = 0;
	virtual void serverhibernationupd(bool bHibernating) = 0;
	virtual IServerGCLobby* getservergclobby() = 0;
	virtual void getmatchmakinggamedata(void* buf) = 0;
	virtual bool shouldtimeoutclient(int nUserID, float flTimeSinceLastReceived) = 0;
	virtual void printstatus(void* nPlayerEntityIndex, void* output) = 0;
	virtual int	 getservergamedllflags(void) const = 0;
	virtual void gettaggedconvarlist(void* pCvarTagList) = 0;
	virtual void* getallserverclasses(void) = 0;
	virtual const char* getactiveworldname(void) const = 0;
	virtual bool ispaused(void) const = 0;
	virtual bool getnavmeshdata(void* pNavMeshData) = 0;
	virtual void setnavmeshdata(const void* navMeshData) = 0;
	virtual void registernavlistener(void* pNavListener) = 0;
	virtual void unregisternavlistener(void* pNavListener) = 0;
	virtual void* getspawndebuginterface(void) = 0;
	virtual void* unknown1(void) = 0;
	virtual void* gettoolgamesimulationapi(void) = 0;
	virtual void getanimationactivitylist(void* activityList) = 0;
	virtual void getanimationeventlist(void* eventList) = 0;
	virtual void filterplayercounts(int* pInOutHumans, int* pInOutHumansSlots, int* pInOutBots) = 0;
	virtual void gameserversteamapiactivated(void) = 0;
};

using CreateInterfaceFn = void* (__fastcall*)(const char* name, int* returnCode);
using OnClientConnectedFn = void(__fastcall*)(void* thisptr, CPlayerSlot userid, const char* name, const char* xuid, const char* networkid, const char* addr, bool fake);
using ClientDisconnect = void(__fastcall*)(void* thisptr, CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, uint64 xuid, const char* pszNetworkId);
using GetBuildVersion = int(void);
using GetBuildVersionFn = GetBuildVersion*;
using GetHSteamPipe = int(*)();
using GetHSteamUser = int(*)();
using GameServerSteamAPIActivatedFn = void(__thiscall*)(void* thisptr);
using SendMessageFn = EGCResults(__fastcall*)(void* thisptr, uint32_t punMsgType, const void* pubData, uint32_t cubData);
using RetrieveMessageFn = EGCResults(__fastcall*)(void* thisptr, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
using IsMessageAvailableFn = bool(__fastcall*)(void* thisptr, uint32_t* pcubMsgSize);
using SteamAPI_RegisterCallbackFn = void(__cdecl*)(class CCallbackBase *pCallback, int icallback);
using SteamAPI_UnregisterCallbackFn = void(__cdecl*)(class CCallbackBase *pCallback);
using GCSDK_MsgAvailableCallback = void(__fastcall*)(void* thisptr, void* pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall);
using SteamGameServer_RunCallbacksFn = void(__cdecl*)();
using SteamAPI_InitFn = void(__cdecl*)();
using SteamAPI_GetHSteamPipe_t = HSteamPipe(__cdecl*)();
using SteamAPI_GetHSteamUser_t = HSteamUser(__cdecl*)();
typedef void(__fastcall* LobbyUpdatedFn)(void* pServerGCSystem, void* pLobbyObject);
typedef void* (__cdecl* GetISteamGenericInterfaceFn)(HSteamUser, const char*);
typedef void(__cdecl* MsgFn)(const char* fmt, ...);
typedef void(__cdecl* MsgColFn)(const Color& color, const char* format, ...);

MsgFn msg = nullptr;
MsgColFn msgcol = nullptr;
CreateInterfaceFn getinterface_s = nullptr;
CreateInterfaceFn getinterface_e = nullptr;
OnClientConnectedFn o_onclientconnected = nullptr;
ClientDisconnect o_clientdisconnect = nullptr;
GetBuildVersionFn o_getbuildversion = nullptr;
GameServerSteamAPIActivatedFn o_gameserversteamapiactivated = nullptr;
GetISteamGenericInterfaceFn getisteaminterface = nullptr;
GetHSteamPipe gethsteampipe = nullptr;
GetHSteamUser gethsteamuser = nullptr;
SendMessageFn o_sendmessage = nullptr;
RetrieveMessageFn o_retrievemessage = nullptr;
IsMessageAvailableFn o_ismsgavl = nullptr;
SteamAPI_RegisterCallbackFn o_registercallback = nullptr;
SteamAPI_UnregisterCallbackFn o_unregistercallback = nullptr;
GCSDK_MsgAvailableCallback o_msgavailablecallback = nullptr;
SteamGameServer_RunCallbacksFn o_runcallbacks = nullptr;
LobbyUpdatedFn o_lobby_updated = nullptr;
SteamAPI_InitFn o_steamapiinit = nullptr;

ISteamGameCoordinator* gccl = nullptr;
IVEngineServer2* enginetoserver = nullptr;

template<typename T>
inline bool MessageFromBuffer(T& msg, const void* pubData, uint32 cubData)
{
	int headerSize = *(int*)((intptr_t)pubData + 4);
	return msg.ParseFromArray((const void*)((intptr_t)pubData + 8 + headerSize), cubData - headerSize - 8);
}