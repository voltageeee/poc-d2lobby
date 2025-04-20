#include "pch.h"
#include "MinHook.h"
#include <stdio.h>
#include "sdk.h"
#include <inttypes.h>
#include <base_gcmessages.pb.h>
#include <dota_gcmessages_server.pb.h>
#include <gcsystemmsgs.pb.h>
#include <dota_gcmessages_msgid.pb.h>
#include <dota_gcmessages_client_match_management.pb.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/descriptor.pb.h>

const char* prefix = "[voltage_dota_fixups]";
bool injectedobj = false;

using namespace google::protobuf;
void TryParseUnknownObject(const std::string& data) {
    const DescriptorPool* pool = DescriptorPool::generated_pool();
    DynamicMessageFactory factory;

    std::vector<std::string> matches;

    for (int i = 0; i < pool->FindFileByName("dota_gcmessages_common_lobby.proto")->message_type_count(); ++i) {
        const Descriptor* desc = pool->FindFileByName("dota_gcmessages_common_lobby.proto")->message_type(i);
        const Message* prototype = factory.GetPrototype(desc);
        std::unique_ptr<Message> msgg(prototype->New());

        if (msgg->ParseFromString(data)) {
            msg("Match: %s\n", desc->full_name());
            msg("dbg string: %s\n", msgg->ShortDebugString());
            matches.push_back(desc->full_name());
        }
    }

    if (matches.empty()) {
        msg("no match\n");
    }
}

bool injectgcmsg(std::string msg) {
    gcmsgstoinject.push(msg);
}

bool sendlobbysoupdate() {
    CMsgSOMultipleObjects objs;
    auto obj = objs.add_objects_modified();
    obj->set_type_id(2004);
    m_lobby.SerializeToString(obj->mutable_object_data());
    objs.set_version(lobby_version);
    objs.set_service_id(0);
    objs.mutable_owner_soid()->set_type(3);
    objs.mutable_owner_soid()->set_id(lobby_id);

    uint32 emsg = k_ESOMsg_UpdateMultiple | 0x80000000;
    int headerSize = 0;

    std::string message;
    message.append((const char*)&emsg, sizeof(uint32));
    message.append((const char*)&headerSize, sizeof(int));
    objs.AppendToString(&message);

    msg("[Stub] Adding SOUpdateMultiple message to the queue\n");

    injectgcmsg(message);
}

void handleconnectedplayers(const CMsgConnectedPlayers& msg) {
    for (auto& connected : msg.connected_players()) {
        for (auto& p : *m_lobby.mutable_all_members()) {
            if (p.id() == connected.steam_id()) {
                p.set_leaver_status(DOTA_LEAVER_NONE);
                p.set_hero_id(connected.hero_id());
                break;
            }
        }
    }

    m_lobby.set_first_blood_happened(msg.first_blood_happened());
    m_lobby.set_state(CSODOTALobby_State_RUN);
    m_lobby.set_game_state(DOTA_GAMERULES_STATE_WAIT_FOR_PLAYERS_TO_LOAD);

    sendlobbysoupdate();
}

bool injectgclobby() {
    if (lobbyinjected) {
        return true;
    }

    {
        CSteamID serverid = enginetoserver->getserversteamid();

        m_lobby.set_lobby_id(lobby_id);
        auto* pMember = m_lobby.add_all_members();
        pMember->set_id(76561198969757101);
        pMember->set_team(DOTA_GC_TEAM_GOOD_GUYS);
        pMember->set_slot(1);
        pMember->set_leaver_status(DOTA_LEAVER_DISCONNECTED);
        m_lobby.add_member_indices(0);
        m_lobby.set_game_mode(DOTA_GAMEMODE_TURBO);
        m_lobby.set_state(CSODOTALobby_State_UI);
        m_lobby.set_lobby_type(CSODOTALobby_LobbyType_CASUAL_MATCH);
        m_lobby.set_game_state(DOTA_GAMERULES_STATE_INIT);
        m_lobby.set_allow_cheats(false);
        m_lobby.set_fill_with_bots(true);
        m_lobby.set_cm_pick(DOTA_CM_RANDOM);
        m_lobby.set_match_id(match_id);
        m_lobby.set_allow_spectating(false);
        m_lobby.set_bot_difficulty_radiant(BOT_DIFFICULTY_UNFAIR);
        m_lobby.set_leagueid(0);
        m_lobby.set_series_type(0);
        m_lobby.set_radiant_series_wins(0);
        m_lobby.set_dire_series_wins(0);
        m_lobby.set_allchat(false);
        m_lobby.set_dota_tv_delay(LobbyDotaTV_10);
        m_lobby.set_lan(false);
        m_lobby.set_pause_setting(LobbyDotaPauseSetting_Unlimited);
        m_lobby.set_bot_difficulty_dire(BOT_DIFFICULTY_UNFAIR);
        m_lobby.set_bot_radiant(0);
        m_lobby.set_bot_dire(0);
        m_lobby.set_selection_priority_rules(k_DOTASelectionPriorityRules_Manual);

        lobby2.set_lobby_id(0);
        lobby2.set_match_id(0);
        lobby2.set_lan(false);
        lobby2.set_dota_tv_delay(LobbyDotaTV_10);

        CSODOTAStaticLobby lStatic;
        lStatic.add_all_members();
        CSODOTAServerLobby lServer;
        lServer.add_all_members();
        CSODOTAServerStaticLobby lSStatic;
        lSStatic.add_all_members();

        CMsgSOCacheSubscribed sub;
        auto* pObject = sub.add_objects();
        auto* pObject2 = sub.add_objects();
        auto* pObject3 = sub.add_objects();
        auto* pObject4 = sub.add_objects();
        auto* pObject5 = sub.add_objects();
        pObject->set_type_id(2004);
        pObject2->set_type_id(2013);
        pObject3->set_type_id(2014);
        pObject4->set_type_id(2015);
        pObject5->set_type_id(2016);

        sub.mutable_owner_soid()->set_type(3);
        sub.mutable_owner_soid()->set_id(lobby_id);
        sub.set_version(149599865);

        std::string data;
        std::string data1;
        std::string data2;
        std::string data3;
        std::string data4;
        m_lobby.SerializeToString(&data);
        pObject->add_object_data(data);

        lobby2.SerializeToString(&data1);
        pObject2->add_object_data(data1);

        lStatic.SerializeToString(&data2);
        pObject3->add_object_data(data2);

        lServer.SerializeToString(&data3);
        pObject4->add_object_data(data3);

        lSStatic.SerializeToString(&data4);
        pObject5->add_object_data(data4);

        lobby_owner.set_id(lobby_id);
        lobby_owner.set_type(3);

        uint32 emsg = k_ESOMsg_CacheSubscribed | 0x80000000;
        int header = 0;

        std::string message;
        message.append((const char*)&emsg, sizeof(uint32));
        message.append((const char*)&header, sizeof(int));
        sub.AppendToString(&message);

        msg("adding CacheSubscribed to queue\n");
        injectgcmsg(message);
    }

    {
        m_lobby.set_state(CSODOTALobby_State_SERVERASSIGN);

        CMsgSOSingleObject obj;
        obj.set_type_id(2004);
        obj.set_version(149599865);
        obj.set_service_id(0);
        obj.mutable_owner_soid()->set_type(3);
        obj.mutable_owner_soid()->set_id(lobby_id);

        m_lobby.SerializeToString(obj.mutable_object_data());

        uint32 emsg = k_ESOMsg_Create | 0x80000000;
        int headerSize = 0;

        std::string message;
        message.append((const char*)&emsg, sizeof(uint32));
        message.append((const char*)&headerSize, sizeof(int));
        obj.AppendToString(&message);

        msg("adding SOCreate message to the queue\n");
        injectgcmsg(message);
    }

    {
        m_lobby.set_state(CSODOTALobby_State_SERVERSETUP);
        m_lobby.set_connect("localhost");


        CMsgSOMultipleObjects objs;
        auto* obj = objs.add_objects_modified();
        obj->set_type_id(2004);
        m_lobby.SerializeToString(obj->mutable_object_data());

        objs.set_version(149599865);
        objs.set_service_id(0);
        objs.mutable_owner_soid()->set_type(3);
        objs.mutable_owner_soid()->set_id(lobby_id);

        uint32 emsg = k_ESOMsg_UpdateMultiple | 0x80000000;
        int headerSize = 0;

        std::string message;
        message.append((const char*)&emsg, sizeof(uint32));
        message.append((const char*)&headerSize, sizeof(int));
        objs.AppendToString(&message);

        msg("[Stub] Adding SOUpdateMultiple message to the queue\n");
        injectgcmsg(message);
    }

    lobbyinjected = true;
    return true;
}


void __cdecl hkRunCallbacks() {
    o_runcallbacks();

    if (!gcmsgstoinject.empty()) {
        const std::string& msg = gcmsgstoinject.front();

        GCMessageAvailable_t cmsg;
        cmsg.m_nMessageSize = static_cast<uint32_t>(msg.size());

        msgcol(Color(255, 255, 0), "%s - [hkRunCallbacks] - notifying GC about available message of size %u\n", prefix, cmsg.m_nMessageSize);
        gcmsgavailablecallback->Run(&cmsg);
    }
}

void __fastcall hkonclientconnected(void* thisptr, CPlayerSlot slot, const char* name, const char* xuid, const char* networkid, const char* addr, bool fake) {
    msgcol(Color(0, 204, 204), "%s - [hkOnClientConnected] - Client %s (%d) is attempting to connect. IP: %s\n", prefix, name, slot.Get(), addr);
    o_onclientconnected(thisptr, slot, name, xuid, networkid, addr, fake);
}

void __fastcall hkclientdisconnect(void* thisptr, CPlayerSlot slot, ENetworkDisconnectionReason reason, const char* pszName, UINT64 xuid, const char* pszNetworkId) {
    msgcol(Color(0, 204, 204), "%s - [hkClientDisconnect] - Client %s has disconnected: %d\n", prefix, pszName, reason);
    o_clientdisconnect(thisptr, slot, reason, pszName, xuid, pszNetworkId);
}

int hkgetbuildver(void) {
    if (lieaboutbversion && b_version == a_version) {
        msgcol(Color(0, 204, 204), "%s - [hkGetBuildVersion] - Wanted to lie about the bVersion, but bVersion == aVersion (%d)\n", prefix, o_getbuildversion());
        return b_version;
    }
    else if (lieaboutbversion) {
        msgcol(Color(0, 204, 204), "%s - [hkGetBuildVersion] - Lying about the bVersion. We're on version %d, GC wants %d\n", prefix, o_getbuildversion(), a_version);
        return a_version;
    }

    return b_version;
}

EGCResults __fastcall hkSendMessage(void* thisptr, uint32_t msgType, const void* data, uint32_t size) {
    int realmsg = msgType & ~0x80000000;

    if (realmsg == k_EMsgGCGameServerInfo) {
        msg("intercepted outgoing k_EMsgGCGameServerInfo\n");
        CMsgGameServerInfo message;
        MessageFromBuffer(message, data, size);
        uint32 bver = message.build_version();

        msg("dbg bver: %" PRIu32 "\n", bver);

        sentgchello = true;
    }

    if (realmsg == k_EMsgGCConnectedPlayers) {
        msg("intercepted outgoing k_EMsgGCConnectedPlayers\n");
        CMsgConnectedPlayers message;
        MessageFromBuffer(message, data, size);
        handleconnectedplayers(message);

        return k_EGCResultOK;
    }

    return o_sendmessage(thisptr, msgType, data, size);
}

EGCResults __fastcall hkRetrieveMessage(void* thisptr, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize) {
    if (!gcmsgstoinject.empty()) {
        msg("server is retrieving for our message\n");

        const std::string& msg_ = gcmsgstoinject.front();

        uint32_t size = (uint32_t)msg_.size();
        *punMsgType = *(uint32_t*)msg_.data();
        *pcubMsgSize = size;

        if (cubDest < size) {
            msg("cubDest < size\n");
            return k_EGCResultBufferTooSmall;
        }

        memcpy(pubDest, msg_.data(), size);
        gcmsgstoinject.pop();

        return k_EGCResultOK;
    }

    EGCResults result = o_retrievemessage(thisptr, punMsgType, pubDest, cubDest, pcubMsgSize);

    int realmsg = *punMsgType & ~0x80000000;
    msg("%d\n", realmsg);

    /* if (realmsg == k_ESOMsg_CacheSubscribed) {
        msg("got cache subscribe\n");

        CMsgSOCacheSubscribed msgg;

        MessageFromBuffer(msgg, pubDest, cubDest);
        if (msgg.objects(0).type_id() != 2004 || msgg.objects_size() < 5 || injectedobj) {
            return result;
        }
        const auto& serverlobby = msgg.objects(1);
        if (serverlobby.object_data_size() > 0) {
            const std::string& raw_data = serverlobby.object_data(0);

            CSODOTALobby parsed_lobby;
            if (parsed_lobby.ParseFromArray(raw_data.data(), raw_data.size())) {
                msg("%d\n", parsed_lobby.lobby_id());
            }
            else {
                msg("Failed to parse CSODOTAServerLobby!\n");
            }
        }
    } */

    if (result == k_EGCResultOK && punMsgType && pcubMsgSize) {
        msgcol(Color(0, 204, 204), "%s - [hkRetrieveMessage] - incoming gc message: type: 0x%X (%u), size: %u\n", prefix, *punMsgType, *punMsgType, *pcubMsgSize);
    }
    else {
        msgcol(Color(255, 100, 100), "%s - [hkRetrieveMessage] - failed to retrieve an incoming gc message. result: %d\n", prefix, result);
    }

    return result;
}

bool __fastcall hkIsMessageAvailable(void* thisptr, uint32_t* pcubMsgSize) {
    if (!gcmsgstoinject.empty()) {
        msg("server is checking for a message available and we have one.\n");
        const std::string& mess = gcmsgstoinject.front();

        uint32_t size = (uint32_t)mess.size();
        *pcubMsgSize = size;

        return true;
    }

    bool result = o_ismsgavl(thisptr, pcubMsgSize);
        
    if (result) {
        msgcol(Color(0, 204, 204), "%s - [hkIsMessageAvailable] - server found a gc message of size %u\n", prefix, *pcubMsgSize);
    }
    else {
        msgcol(Color(0, 204, 204), "%s - [hkIsMessageAvailable] - server failed to find a gc message of size %u\n", prefix, *pcubMsgSize);
    }

    return result;
}

void __fastcall hkRunMsgAvailable(void* pThis, void* pvParam, bool bIOFailure, SteamAPICall_t hSteamAPICall) {
    auto msg = static_cast<GCMessageAvailable_t*>(pvParam);
    if (msg->m_nMessageSize > 10000 || msg->m_nMessageSize < 0) {
        msgcol(Color(0, 204, 255), "%s - [hkRunMsgAvailable] - steam api has informed us about an enormously large message. ignoring.\n", prefix);
        return o_msgavailablecallback(pThis, pvParam, bIOFailure, hSteamAPICall);
    }
    msgcol(Color(0, 255, 255), "%s - [hkRunMsgAvailable] - steam api is informing server about a message being available. size: %d\n", prefix, msg->m_nMessageSize);

    o_msgavailablecallback(pThis, pvParam, bIOFailure, hSteamAPICall);
}

void __cdecl hkGetNextCallback(class CCallbackBase *pCallback, int icallback) {
    if (icallback == 1701) {
        msgcol(Color(0, 204, 204), "%s - [hkGetNextCallback] - got the GCMessageAvailable callback -> %p\n", prefix, pCallback);
        gcmsgavailablecallback = pCallback;
        msgcol(Color(0, 204, 204), "%s - [hkGetNextCallback] - saved the GCMessagaAvailable callback. hooking pCallback->Run()\n", prefix);
        void** vtable = *(void***)(pCallback);
        void* pRun = vtable[1];

        if (MH_CreateHook(pRun, &hkRunMsgAvailable, (void**)&o_msgavailablecallback) != MH_OK) {
            msgcol(Color(0, 204, 204), "%s - [hkGetNextCallback] - failed to create hook for GCMessageAvailable callback. won't be able to inform the GC about new messages.\n", prefix);
            return o_registercallback(pCallback, icallback);
        }

        if (MH_EnableHook(pRun) != MH_OK) {
            msgcol(Color(0, 204, 204), "%s - [hkGetNextCallback] - failed to enable hook for GCMessageAvailable callback. won't be able to inform the GC about new messages.\n", prefix);
            return o_registercallback(pCallback, icallback);
        }

        msgcol(Color(255, 0, 0), "%s - [hkGetNextCallback] - installed the GCMessageAvailabel callback hook.\n", prefix);

    }

    o_registercallback(pCallback, icallback);
}

void __cdecl hkFreeLastCallback(class CCallbackBase* pCallback) {
    o_unregistercallback(pCallback);
}

void __fastcall hkGameServerSteamAPIActivated(void* thisptr) {
    pipe = gethsteampipe();
    user = gethsteamuser();

    msgcol(Color(200, 100, 50), "%s - got hSteamUser (%d) and hSteamPipe (%d)\n", prefix, user, pipe);

    void* gcinterface = getisteaminterface(user, "SteamGameCoordinator001");
    gccl = static_cast<ISteamGameCoordinator*>(gcinterface);
    void** vtable_gc = *(void***)gccl;

    msgcol(Color(200, 100, 50), "%s - got steamgcinterface -> %p\n", prefix, gccl);

    DWORD oldProtection;
    VirtualProtect(&vtable_gc[0], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);
    o_sendmessage = (SendMessageFn)vtable_gc[0];
    vtable_gc[0] = (void*)hkSendMessage;
    VirtualProtect(&vtable_gc[0], sizeof(void*), oldProtection, &oldProtection);

    msgcol(Color(200, 100, 50), "%s - hooked gccl->SendMessage(). new addr -> %p\n", prefix, vtable_gc[0]);

    VirtualProtect(&vtable_gc[2], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);
    o_retrievemessage = (RetrieveMessageFn)vtable_gc[2];
    vtable_gc[2] = (void*)hkRetrieveMessage;
    VirtualProtect(&vtable_gc[2], sizeof(void*), oldProtection, &oldProtection);

    msgcol(Color(200, 100, 50), "%s - hooked gccl->RetrieveMessage(). new addr -> %p\n", prefix, vtable_gc[2]);

    VirtualProtect(&vtable_gc[1], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtection);
    o_ismsgavl = (IsMessageAvailableFn)vtable_gc[1];
    vtable_gc[1] = (void*)hkIsMessageAvailable;
    VirtualProtect(&vtable_gc[1], sizeof(void*), oldProtection, &oldProtection);
    msgcol(Color(200, 100, 50), "%s - hooked gccl->IsMessageAvailable(). new addr -> %p\n", prefix, vtable_gc[1]);

    o_gameserversteamapiactivated(thisptr);
}

DWORD WINAPI Init(LPVOID) {
    HMODULE tier0 = GetModuleHandleA("tier0.dll");
    HMODULE server = GetModuleHandleA("server.dll");
    HMODULE engine = GetModuleHandleA("engine2.dll");
    HMODULE steam_api = GetModuleHandleA("steam_api64.dll");
    if (!tier0 || !server || !engine || !steam_api)
    {
        MessageBoxA(NULL, "tier0.dll / server.dll / engine2.dll // steam_api64.dll isn't loaded.", "Error", MB_OK);
        return 1;
    }

    msg = (MsgFn)GetProcAddress(tier0, (LPCSTR)328);
    msgcol = (MsgColFn)GetProcAddress(tier0, (LPCSTR)327);
    
    if (msg && msgcol) {
        msgcol(Color(0, 200, 100), "------------------------------------------\n");
        msgcol(Color(0, 200, 100), "%s - msg -> %p, msgcol -> %p\n", prefix, msg, msgcol);
    }
    else {
        MessageBoxA(NULL, "Init() -> MsgCol init failed", "Error", MB_OK);
        return 1;
    }

    msgcol(Color(150, 50, 0), "%s - Initializing server.dll...\n", prefix);
    msgcol(Color(150, 50, 0), "%s - server.dll: %p\n", prefix, server);
    msgcol(Color(150, 50, 0), "%s - steam_api64.dll: %p\n", prefix, steam_api);

    getinterface_s = (CreateInterfaceFn)GetProcAddress(server, "CreateInterface");

    if (getinterface_s) {
        msgcol(Color(0, 200, 100), "%s - sCreateInterface -> %p\n", prefix, getinterface_s);
    }
    else {
        msgcol(Color(0, 200, 100), "%s - failed to grab CreateInterface\n", prefix);
        msgcol(Color(255, 0, 0), "%s - Init -> FAILED\n", prefix);
        return 1;
    }

    void* iface_gc = getinterface_s("Source2GameClients001", nullptr);
    ISource2GameClients* s2gc = static_cast<ISource2GameClients*>(iface_gc);
    void** vtable_s2gc = *(void***)s2gc;

    if (!s2gc) {
        msgcol(Color(0, 200, 100), "%s - failed to grab ISource2GameClients\n", prefix);
        msgcol(Color(255, 0, 0), "%s - Init -> FAILED\n", prefix);
        return 1;
    }

    o_onclientconnected = (OnClientConnectedFn)vtable_s2gc[11];
    o_clientdisconnect = (ClientDisconnect)vtable_s2gc[16];

    msgcol(Color(0, 200, 100), "%s - oOnClientConnected -> %p\n", prefix, o_onclientconnected);
    msgcol(Color(0, 200, 100), "%s - oClientDisconnect -> %p\n", prefix, o_clientdisconnect);

    DWORD oldProtect;
    VirtualProtect(&vtable_s2gc[11], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
    vtable_s2gc[11] = (void*)hkonclientconnected;
    VirtualProtect(&vtable_s2gc[11], sizeof(void*), oldProtect, &oldProtect);

    VirtualProtect(&vtable_s2gc[16], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
    vtable_s2gc[16] = (void*)hkclientdisconnect;
    VirtualProtect(&vtable_s2gc[16], sizeof(void*), oldProtect, &oldProtect);

    msgcol(Color(0, 200, 100), "%s - Hooked OnClientConnected -> %p\n", prefix, vtable_s2gc[11]);
    msgcol(Color(0, 200, 100), "%s - Hooked ClientConnect -> %p\n", prefix, vtable_s2gc[12]);
    msgcol(Color(0, 200, 100), "%s - Hooked ClientDisconnect -> %p\n", prefix, vtable_s2gc[16]);

    void* iface_srv = getinterface_s("Source2Server001", nullptr);
    ISource2Server* srv = static_cast<ISource2Server*>(iface_srv);
    void** vtable_srv = *(void***)srv;

    msgcol(Color(0, 200, 100), "%s - iface_srv -> %p, srv -> %p\n", prefix, iface_srv, srv);
    msgcol(Color(0, 200, 100), "%s - vtable_srv -> %p\n", prefix, vtable_srv);

    msgcol(Color(0, 200, 100), "%s - Server.dll Init -> Success!\n", prefix);

    msgcol(Color(150, 50, 0), "%s - Initializing engine2.dll...\n", prefix);

    getinterface_e = (CreateInterfaceFn)GetProcAddress(engine, "CreateInterface");

    if (getinterface_e) {
        msgcol(Color(0, 200, 100), "%s - eCreateInterface -> %p\n", prefix, getinterface_e);
    }
    else {
        msgcol(Color(0, 200, 100), "%s - failed to grab eCreateInterface\n", prefix);
        msgcol(Color(255, 0, 0), "%s - Init -> FAILED\n", prefix);
        return 1;
    }

    void* iface_ets = getinterface_e("Source2EngineToServer001", nullptr);
    enginetoserver = static_cast<IVEngineServer2*>(iface_ets);
    void** vtable_ets = *(void***)enginetoserver;

    if (!enginetoserver) {
        msgcol(Color(0, 200, 100), "%s - failed to grab IVEngineServer2\n", prefix);
        msgcol(Color(255, 0, 0), "%s - Init -> FAILED\n", prefix);
        return 1;
    }

    if (!vtable_ets) {
        msgcol(Color(0, 200, 100), "%s - failed to grab vtable_ets\n", prefix);
        msgcol(Color(255, 0, 0), "%s - Init -> FAILED\n", prefix);
        return 1;
    }

    msgcol(Color(0, 200, 100), "%s - IVEngineServer2 -> %p\n", prefix, enginetoserver);
    msgcol(Color(0, 200, 100), "%s - vtable_ets -> %p\n", prefix, vtable_ets);

    msgcol(Color(0, 200, 100), "%s - unhooked getbuildversion -> %p\n", prefix, vtable_ets[78]);

    VirtualProtect(&vtable_ets[78], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
    o_getbuildversion = (GetBuildVersionFn)vtable_ets[78];
    vtable_ets[78] = (GetBuildVersion*)hkgetbuildver;
    VirtualProtect(&vtable_ets[78], sizeof(void*), oldProtect, &oldProtect);

    msgcol(Color(0, 200, 100), "%s - hooked getbuildversion -> %p\n", prefix, vtable_ets[78]);

    msgcol(Color(0, 200, 100), "%s - we're on %d\n", prefix, o_getbuildversion());

    IServerGCLobby* p_lobby = srv->getservergclobby();
    void** p_lobby_vtable = *(void***)p_lobby;

    msgcol(Color(0, 200, 100), "%s - p_lobby_vtable -> %p\n", prefix, p_lobby_vtable);

    msgcol(Color(200, 100, 50), "%s - trying to initialize steam api. steam_api64 -> %p\n", prefix, steam_api);

    getisteaminterface = (GetISteamGenericInterfaceFn)GetProcAddress(steam_api, "SteamInternal_FindOrCreateGameServerInterface");

    if (!getisteaminterface) {
        msgcol(Color(255, 0, 0), "%s - failed to init steam, getisteamgenericinterface not found.\n", prefix);
        return 1;
    }

    msgcol(Color(200, 100, 50), "%s - got getisteamgenericinterface -> %p\n", prefix, getisteaminterface);

    gethsteampipe = (GetHSteamPipe)GetProcAddress(steam_api, "SteamGameServer_GetHSteamPipe");

    if (!gethsteampipe) {
        msgcol(Color(255, 0, 0), "%s - failed to init steam, gethsteampipe not found.\n", prefix);
        return 1;
    }

    msgcol(Color(200, 100, 50), "%s - got gethsteampipe -> %p\n", prefix, gethsteampipe);

    gethsteamuser = (GetHSteamUser)GetProcAddress(steam_api, "SteamGameServer_GetHSteamUser");

    if (!gethsteamuser) {
        msgcol(Color(255, 0, 0), "%s - failed to init steam, gethsteamuser not found.\n", prefix);
        return 1;
    }

    msgcol(Color(200, 100, 50), "%s - got gethsteamuser -> %p\n", prefix, gethsteamuser);

    msgcol(Color(200, 100, 50), "%s - trying to hook gameserversteamapiactivated\n", prefix);

    msgcol(Color(200, 100, 50), "%s - gameserversteamapiactivated_unhooked -> %p\n", prefix, vtable_srv[42]);

    VirtualProtect(&vtable_srv[42], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
    o_gameserversteamapiactivated = (GameServerSteamAPIActivatedFn)vtable_srv[42];
    vtable_srv[42] = (void*)hkGameServerSteamAPIActivated;
    VirtualProtect(&vtable_srv[42], sizeof(void*), oldProtect, &oldProtect);

    msgcol(Color(200, 100, 50), "%s - gameserversteamapiactivated -> %p\n", prefix, vtable_srv[42]);

    void* registercallback = GetProcAddress(steam_api, "SteamAPI_RegisterCallback");
    void* unregistercallback = GetProcAddress(steam_api, "SteamAPI_UnregisterCallback");
    void* runcallbacks = GetProcAddress(steam_api, "SteamGameServer_RunCallbacks");

    msgcol(Color(200, 100, 50), "%s - hooking registercallback, runcallbacks and unregistercallback - > %p | %p\n", prefix, registercallback, unregistercallback);

    MH_Initialize();

    if (MH_CreateHook(registercallback, &hkGetNextCallback, reinterpret_cast<void**>(&o_registercallback)) != MH_OK) {
        msgcol(Color(200, 100, 50), "%s - failed to create getnextcallback hook\n", prefix);
    }
    else {
        msgcol(Color(200, 100, 50), "%s - created getnextcallback hook\n", prefix);
    }

    if (MH_EnableHook(registercallback) != MH_OK) {
        msgcol(Color(200, 100, 50), "%s - failed to install getnextcallback hook\n", prefix);
    }
    else {
        msgcol(Color(200, 100, 50), "%s - installed getnextcallback hook!\n", prefix);
    }

    if (MH_CreateHook(unregistercallback, &hkFreeLastCallback, reinterpret_cast<void**>(&o_unregistercallback)) != MH_OK) {
        msgcol(Color(200, 100, 50), "%s - failed to create freelastcallback hook\n", prefix);
    }
    else {
        msgcol(Color(200, 100, 50), "%s - created getnextcallback hook\n", prefix);
    }

    if (MH_EnableHook(unregistercallback) != MH_OK) {
        msgcol(Color(200, 100, 50), "%s - failed to install freelastcallback hook\n", prefix);
    }
    else {
        msgcol(Color(200, 100, 50), "%s - installed freelastcallback hook!\n", prefix);
    }

    if (MH_CreateHook(runcallbacks, &hkRunCallbacks, reinterpret_cast<void**>(&o_runcallbacks)) != MH_OK) {
        msgcol(Color(200, 100, 50), "%s - failed to create runcallbacks hook\n", prefix);
    }
    else {
        msgcol(Color(200, 100, 50), "%s - created runcallbacks hook\n", prefix);
    }

    if (MH_EnableHook(runcallbacks) != MH_OK) {
        msgcol(Color(200, 100, 50), "%s - failed to install runcallbacks hook\n", prefix);
    }
    else {
        msgcol(Color(200, 100, 50), "%s - installed runcallbacks hook!\n", prefix);
    }

    msgcol(Color(0, 255, 0), "%s - Global Init -> Success!\n", prefix);

    msgcol(Color(0, 200, 100), "------------------------------------------\n");

    msgcol(Color(0, 200, 100), "%s - waiting for the server to send hello() to gc...\n", prefix);

    while (!sentgchello) {
        Sleep(100);
    }

    msgcol(Color(0, 200, 100), "%s - sentgchello == true, waiting 2 secs...\n", prefix);

    Sleep(2000);

    msgcol(Color(0, 200, 100), "%s - trying to inject a lobby object.\n", prefix);

    injectgclobby();

    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, Init, nullptr, 0, nullptr);
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

