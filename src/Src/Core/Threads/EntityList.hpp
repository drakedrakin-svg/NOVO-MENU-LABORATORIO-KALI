#pragma once
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <Core/Offsets.hpp>
#include <Core/Core.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

namespace Core
{
    namespace Threads
    {
        class cEntityList
        {
        private:
            std::mutex EntityListMutex;
        private:
            CPedInterFace* CPedInterFace = nullptr;
            CPedList* CPedList = nullptr;
        public:
            std::unordered_map<CPed*, Core::SDK::Game::EntityStruct> CachedEntities;
        public:
            void Update()
            {
                while (true)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    if (!g_MenuInfo.IsLogged && !g_Variables.g_bPassedByThisVerify)
                        continue;

                    if (!Core::SDK::Pointers::pReplayInterFace)
                        continue;

                    CPedInterFace = Core::SDK::Pointers::pReplayInterFace->InterfacePed();
                    CPedList = CPedInterFace->PedList();

                    if (!CPedInterFace || !CPedList)
                        continue;

                    size_t Size = Core::SDK::Game::EntityList.size();
                    size_t Index = 0;
                    static const int PedCount = CPedInterFace->MaxPed();

                    std::unordered_map<CPed*, bool> currentValidPeds;

                    {
                        std::lock_guard<std::mutex> Guard(EntityListMutex);

                        for (int i = 0; i < PedCount; i++)
                        {
                            CPed* CurrentPed = CPedList->Ped(i);

                            if (!CurrentPed)
                                continue;

                            currentValidPeds[CurrentPed] = true;

                            auto it = CachedEntities.find(CurrentPed);
                            Core::SDK::Game::EntityStruct Entity;

                            if (it == CachedEntities.end())
                            {
                                Entity.Id = CurrentPed->GetID();
                                Entity.Index = i;
                                Entity.Pos = CurrentPed->GetPos();
                                Entity.IsFriend = Core::SDK::Game::FriendMap[CurrentPed];
                                Entity.MaxHealth = CurrentPed->GetMaxHealth();
                                Entity.Health = CurrentPed->GetHealth();
                                Entity.Armor = CurrentPed->GetArmor();
                                Entity.PedType = CurrentPed->GetPedType();
                                Entity.Distance = CurrentPed->GetDistance(Core::SDK::Pointers::pLocalPlayer->GetPos(), Entity.Pos);

                                Entity.IsPlayer = (CurrentPed->GetPlayerInfo() != nullptr);

                                CachedEntities[CurrentPed] = Entity;
                            }
                            else
                            {
                                Entity = it->second;
                                Entity.Health = CurrentPed->GetHealth();
                                Entity.Armor = CurrentPed->GetArmor();
                                Entity.Pos = CurrentPed->GetPos();
                                Entity.IsFriend = Core::SDK::Game::FriendMap[CurrentPed];
                                Entity.Distance = CurrentPed->GetDistance(Core::SDK::Pointers::pLocalPlayer->GetPos(), Entity.Pos);
                                Entity.WeaponName = CurrentPed->GetWeaponManager()->GetWeaponInfo()->GetName();
                                Entity.PedType = CurrentPed->GetPedType();
                                Entity.IsPlayer = (CurrentPed->GetPlayerInfo() != nullptr);

                                CachedEntities[CurrentPed] = Entity;
                            }

                            auto& NetworkInfo = Core::Threads::g_UpdateNames.NetworkMap[Entity.Id];
                            Entity.NetworkInfo.UserName = Utils::StringToFirstUpperCase(CurrentPed->GetPlayerName(Entity.Id));

                            Entity.NetworkInfo.SteamId = NetworkInfo.SteamId;
                            Entity.NetworkInfo.DiscordId = NetworkInfo.DiscordId;
                            Entity.Ped = CurrentPed;

                            if (Index < Size)
                                Core::SDK::Game::EntityList[Index] = Entity;
                            else
                                Core::SDK::Game::EntityList.push_back(Entity);

                            Index++;
                        }

                        for (auto it = CachedEntities.begin(); it != CachedEntities.end(); ) {
                            if (currentValidPeds.find(it->first) == currentValidPeds.end()) {
                                it = CachedEntities.erase(it);
                            }
                            else {
                                ++it;
                            }
                        }
                    }

                    if (Index < Size)
                        Core::SDK::Game::EntityList.resize(Index);
                }
            }
        };

        inline cEntityList g_EntityList;
    }
}