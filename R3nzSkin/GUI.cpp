#include <algorithm>
#include <cstdio>
#include <ranges>
#include <string>
#include <vector>

#include "../includes/imgui/imgui.h"
#include "CheatManager.hpp"
#include "GUI.hpp"
#include "Memory.hpp"
#include "SkinDatabase.hpp"
#include "Utils.hpp"
#include "fnv_hash.hpp"

inline static void footer() noexcept {
  using namespace std::string_literals;
  static const auto buildText{"Last Build: "s + __DATE__ + " - " + __TIME__};
  ImGui::Separator();
  ImGui::textUnformattedCentered(buildText.c_str());
  ImGui::textUnformattedCentered("(C) 2021-2024 R3nzTheCodeGOD, NiceAesth");
}

static void changeTurretSkin(const std::int32_t skinId, const std::int32_t team) noexcept {
  if (skinId == -1) {
    return;
  }

  const auto turrets{cheatManager.memory->turretList};
  const auto playerTeam{cheatManager.memory->localPlayer->get_team()};

  for (auto i{0u}; i < turrets->length; ++i) {
    const auto turret{turrets->list[i]};
    if (turret->get_team() == team) {
      if (playerTeam == team) {
        turret->get_character_data_stack()->base_skin.skin = skinId * 2;
        turret->get_character_data_stack()->update(true);
      } else {
        turret->get_character_data_stack()->base_skin.skin = skinId * 2 + 1;
        turret->get_character_data_stack()->update(true);
      }
    }
  }
}

void GUI::render() noexcept {
  std::call_once(set_font_scale, [&] {
    ImGui::GetIO().FontGlobalScale = cheatManager.config->fontScale;
  });

  const auto player{cheatManager.memory->localPlayer};
  const auto heroes{cheatManager.memory->heroList};
  static const auto my_team{player ? player->get_team() : 100};
  static int gear{player ? player->get_character_data_stack()->base_skin.gear : 0};

  static const auto vector_getter_skin = [](void *vec, const std::int32_t idx, const char **out_text) noexcept {
    const auto &vector{*static_cast<std::vector<SkinDatabase::skin_info> *>(vec)};
    if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) {
      return false;
    }
    *out_text = idx == 0 ? "Default" : vector.at(idx - 1).skin_name.c_str();
    return true;
  };

  static const auto vector_getter_ward_skin = [](void *vec, const std::int32_t idx, const char **out_text) noexcept {
    const auto &vector{*static_cast<std::vector<std::pair<std::int32_t, const char *>> *>(vec)};
    if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) {
      return false;
    }
    *out_text = idx == 0 ? "Default" : vector.at(idx - 1).second;
    return true;
  };

  static auto vector_getter_gear = [](void *vec, const std::int32_t idx, const char **out_text) noexcept {
    const auto &vector{*static_cast<std::vector<const char *> *>(vec)};
    if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) {
      return false;
    }
    *out_text = vector[idx];
    return true;
  };

  static auto vector_getter_default = [](void *vec, const std::int32_t idx, const char **out_text) noexcept {
    const auto &vector{*static_cast<std::vector<const char *> *>(vec)};
    if (idx < 0 || idx > static_cast<std::int32_t>(vector.size())) {
      return false;
    }
    *out_text = idx == 0 ? "Default" : vector.at(idx - 1);
    return true;
  };

  ImGui::Begin(
    "R3nzSkin", nullptr,
    ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_AlwaysAutoResize
  );
  {
    if (ImGui::BeginTabBar(
          "TabBar", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip
        )) {
      if (player) {
        if (ImGui::BeginTabItem("Local Player")) {
          auto &values{cheatManager.database
                         ->champion_skins[fnv::hash_runtime(player->get_character_data_stack()->base_skin.model.str)]};
          ImGui::Text("Player Skin Settings:");

          if (ImGui::Combo(
                "Current Skin", &cheatManager.config->current_combo_skin_index, vector_getter_skin,
                static_cast<void *>(&values), values.size() + 1
              )) {
            player->change_skin(
              values[(std::max)(0, cheatManager.config->current_combo_skin_index - 1)].model_name,
              values[(std::max)(0, cheatManager.config->current_combo_skin_index - 1)].skin_id
            );
          }

          const auto playerHash{fnv::hash_runtime(player->get_character_data_stack()->base_skin.model.str)};
          if (const auto it{std::ranges::find_if(
                cheatManager.database->special_skins,
                [&skin = player->get_character_data_stack()->base_skin.skin,
                 &ph   = playerHash](const SkinDatabase::specialSkin &x) noexcept -> bool {
                  return x.champHash == ph && (x.skinIdStart <= skin && x.skinIdEnd >= skin);
                }
              )};
              it != cheatManager.database->special_skins.end()) {
            const auto stack{player->get_character_data_stack()};
            gear = stack->base_skin.gear;

            if (ImGui::Combo(
                  "Current Gear", &gear, vector_getter_gear, static_cast<void *>(&it->gears), it->gears.size()
                )) {
              player->get_character_data_stack()->base_skin.gear = static_cast<std::int8_t>(gear);
              player->get_character_data_stack()->update(true);
            }
            ImGui::Separator();
          }

          if (ImGui::Combo(
                "Current Ward Skin", &cheatManager.config->current_combo_ward_index, vector_getter_ward_skin,
                static_cast<void *>(&cheatManager.database->ward_skins), cheatManager.database->ward_skins.size() + 1
              )) {
            cheatManager.config->current_ward_skin_index =
              cheatManager.config->current_combo_ward_index == 0
                ? -1
                : cheatManager.database->ward_skins.at(cheatManager.config->current_combo_ward_index - 1).first;
          }
          footer();
          ImGui::EndTabItem();
        }
      }

      static std::int32_t _heroes_count = heroes->length;
      if (_heroes_count > 1) {
        if (ImGui::BeginTabItem("Other Champs")) {
          ImGui::Text("Other Champion Skin Settings:");
          std::int32_t last_team{0};
          for (auto i{0u}; i < heroes->length; ++i) {
            const auto hero{heroes->list[i]};

            if (hero == player) {
              continue;
            }

            const auto champion_name_hash{fnv::hash_runtime(hero->get_character_data_stack()->base_skin.model.str)};
            if (champion_name_hash == FNV("PracticeTool_TargetDummy")) {
              _heroes_count = heroes->length - 1;
              continue;
            }

            const auto hero_team{hero->get_team()};
            const auto is_enemy{hero_team != my_team};

            if (last_team == 0 || hero_team != last_team) {
              if (last_team != 0) {
                ImGui::Separator();
              }
              if (is_enemy) {
                ImGui::Text(" Enemy Champions");
              } else {
                ImGui::Text(" Ally Champions");
              }
              last_team = hero_team;
            }

            auto &config_array{
              is_enemy ? cheatManager.config->current_combo_enemy_skin_index
                       : cheatManager.config->current_combo_ally_skin_index};
            const auto [first, second]{config_array.insert({champion_name_hash, 0})};

            std::snprintf(
              this->str_buffer, sizeof(this->str_buffer),
              cheatManager.config->usePlayerNames ? "Player name: [ %s ]##%X" : "Hero name: [ %s ]##%X",
              cheatManager.config->usePlayerNames ? hero->get_name()->c_str()
                                                  : hero->get_character_data_stack()->base_skin.model.str,
              reinterpret_cast<std::uintptr_t>(hero)
            );

            auto &values{cheatManager.database->champion_skins[champion_name_hash]};
            if (ImGui::Combo(
                  str_buffer, &first->second, vector_getter_skin, static_cast<void *>(&values), values.size() + 1
                )) {
              hero->change_skin(
                values[(std::max)(0, first->second - 1)].model_name, values[(std::max)(0, first->second - 1)].skin_id
              );
            }
          }
          footer();
          ImGui::EndTabItem();
        }
      }

      if (ImGui::BeginTabItem("Global Skins")) {
        ImGui::Text("Global Skin Settings:");
        if (ImGui::Combo(
              "Minion Skin:", &cheatManager.config->current_combo_minion_index, vector_getter_default,
              static_cast<void *>(&cheatManager.database->minion_skins), cheatManager.database->minion_skins.size() + 1
            )) {
          cheatManager.config->current_minion_skin_index = cheatManager.config->current_combo_minion_index - 1;
        }
        ImGui::Separator();
        if (ImGui::Combo(
              "Order Turret Skin:", &cheatManager.config->current_combo_order_turret_index, vector_getter_default,
              static_cast<void *>(&cheatManager.database->turret_skins), cheatManager.database->turret_skins.size() + 1
            )) {
          changeTurretSkin(cheatManager.config->current_combo_order_turret_index - 1, 100);
        }
        if (ImGui::Combo(
              "Chaos Turret Skin:", &cheatManager.config->current_combo_chaos_turret_index, vector_getter_default,
              static_cast<void *>(&cheatManager.database->turret_skins), cheatManager.database->turret_skins.size() + 1
            )) {
          changeTurretSkin(cheatManager.config->current_combo_chaos_turret_index - 1, 200);
        }
        ImGui::Separator();
        ImGui::Text("Jungle Mob Skin Settings:");
        for (auto &[name, name_hashes, skins] : cheatManager.database->jungle_mob_skins) {
          std::snprintf(str_buffer, 256, "%s Skin:", name);
          const auto [first, second]{
            cheatManager.config->current_combo_jungle_mob_skin_index.insert({name_hashes.front(), 0})};
          if (ImGui::Combo(
                str_buffer, &first->second, vector_getter_default, static_cast<void *>(&skins), skins.size() + 1
              )) {
            for (const auto &hash : name_hashes) {
              cheatManager.config->current_combo_jungle_mob_skin_index[hash] = first->second;
            }
          }
        }
        footer();
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Logger")) {
        cheatManager.logger->draw();
        ImGui::EndTabItem();
      }

      if (ImGui::BeginTabItem("Extras")) {
        ImGui::hotkey("Menu Key", cheatManager.config->menuKey);
        ImGui::Checkbox("Show Player Names", &cheatManager.config->usePlayerNames);
        ImGui::Checkbox("Quick Skin Change", &cheatManager.config->quickSkinChange);
        ImGui::hoverInfo(
          "Allows you to quickly change skin "
          "with the assigned key."
        );

        if (cheatManager.config->quickSkinChange) {
          ImGui::Separator();
          ImGui::hotkey("Previous Skin Key", cheatManager.config->previousSkinKey);
          ImGui::hotkey("Next Skin Key", cheatManager.config->nextSkinKey);
          ImGui::Separator();
        }

        if (player) {
          ImGui::InputText("Custom Nickname", player->get_name());
        }

        if (ImGui::Button("Remove Champion Skins")) {
          for (auto &enemy : cheatManager.config->current_combo_enemy_skin_index) {
            enemy.second = 1;
          }

          for (auto &ally : cheatManager.config->current_combo_ally_skin_index) {
            ally.second = 1;
          }

          for (auto i{0u}; i < heroes->length; ++i) {
            const auto hero{heroes->list[i]};
            if (hero != player) {
              hero->change_skin(hero->get_character_data_stack()->base_skin.model.str, 0);
            }
          }
        }
        ImGui::hoverInfo(
          "Remove the skins of all champions "
          "except for the player's."
        );

        if (ImGui::Button("Random Skins")) {
          for (auto i{0u}; i < heroes->length; ++i) {
            const auto hero{heroes->list[i]};
            const auto championHash{fnv::hash_runtime(hero->get_character_data_stack()->base_skin.model.str)};

            if (championHash == FNV("PracticeTool_TargetDummy")) {
              continue;
            }

            const auto skinCount{cheatManager.database->champion_skins[championHash].size()};
            auto &skinDatabase{cheatManager.database->champion_skins[championHash]};
            auto &config{
              (hero->get_team() != my_team) ? cheatManager.config->current_combo_enemy_skin_index
                                            : cheatManager.config->current_combo_ally_skin_index};

            if (hero == player) {
              cheatManager.config->current_combo_skin_index = random(1ull, skinCount);
              hero->change_skin(
                skinDatabase[cheatManager.config->current_combo_skin_index - 1].model_name,
                skinDatabase[cheatManager.config->current_combo_skin_index - 1].skin_id
              );
            } else {
              auto &data{config[championHash]};
              data = random(1ull, skinCount);
              hero->change_skin(skinDatabase[data - 1].model_name, skinDatabase[data - 1].skin_id);
            }
          }
        }
        ImGui::hoverInfo("Sets a random skin for all champions.");

        ImGui::SliderFloat("Font Scale", &cheatManager.config->fontScale, 1.0f, 2.0f, "%.3f");
        if (ImGui::GetIO().FontGlobalScale != cheatManager.config->fontScale) {
          ImGui::GetIO().FontGlobalScale = cheatManager.config->fontScale;
        }
        ImGui::hoverInfo("Changes the menu font scale.");

        if (ImGui::Button("Force Close")) {
          cheatManager.hooks->uninstall();
        }
        ImGui::hoverInfo("You will be returned to the reconnect screen.");
        ImGui::Text("FPS: %.0f FPS", ImGui::GetIO().Framerate);
        footer();
        ImGui::EndTabItem();
      }
    }
  }
  ImGui::End();
}
