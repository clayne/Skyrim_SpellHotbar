#include "eventlistener.h"
#include "../logger/logger.h"
#include "../game_data/game_data.h"

namespace SpellHotbar::events {

    EventListener* EventListener::GetSingleton() {
        static EventListener instance;
        return &instance;
    }

    RE::BSEventNotifyControl EventListener::ProcessEvent(const RE::TESSpellCastEvent* event,
                                                         RE::BSTEventSource<RE::TESSpellCastEvent>*) {
        if (event) {
            if (event->object && (event->object->IsPlayerRef() || event->object->IsPlayer()))
            {
                auto form = RE::TESForm::LookupByID(event->spell);
                if (form) {
                    if (form->GetFormType() == RE::FormType::Spell) {
                        RE::SpellItem* spell = form->As<RE::SpellItem>();
                        if (spell) {
                            if (spell && spell->GetSpellType() == RE::MagicSystem::SpellType::kPower)
                            {
                                GameData::add_gametime_cooldown(event->spell, 24.0, false);
                            }
                            else if (spell->GetSpellType() == RE::MagicSystem::SpellType::kLesserPower)
                            {
                                RE::FormID id = spell->GetFormID();
                                if (SpellHotbar::GameData::spell_cast_info.contains(id)) {
                                    const auto& data = SpellHotbar::GameData::spell_cast_info.at(id);
                                    if (data.cooldown > 0.0f) {
                                        SpellHotbar::GameData::add_gametime_cooldown_with_timescale(id, data.cooldown, true);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl EventListener::ProcessEvent(const SKSE::ActionEvent* event, RE::BSTEventSource<SKSE::ActionEvent>*)
    {
        if (event->type == SKSE::ActionEvent::Type::kVoiceFire) {
            if (event->sourceForm && event->sourceForm->GetFormType() == RE::FormType::Shout) {
                RE::TESShout* shout = event->sourceForm->As<RE::TESShout>();
                if (shout && (shout->formFlags & RE::TESShout::RecordFlags::kTreatSpellsAsPowers))
                {
                    GameData::add_gametime_cooldown(shout->GetFormID(), 24.0, false);
                }
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }


}