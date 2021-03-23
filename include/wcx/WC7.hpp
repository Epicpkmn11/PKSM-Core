/*
 *   This file is part of PKSM-Core
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#ifndef WC7_HPP
#define WC7_HPP

#include "enums/GameVersion.hpp"
#include "wcx/WCX.hpp"

namespace pksm
{
    class WC7 : public WCX
    {
    private:
        [[nodiscard]] int year(void) const override;
        [[nodiscard]] int month(void) const override;
        [[nodiscard]] int day(void) const override;
        void year(int v) override;
        void month(int v) override;
        void day(int v) override;
        [[nodiscard]] u32 rawDate() const;
        void rawDate(u32 v);

    protected:
        u8 data[264];
        int numItems = 0;

    public:
        static constexpr u16 length     = 264;
        static constexpr u16 lengthFull = 784;

        explicit WC7(u8* dt, bool full = false);

        [[nodiscard]] std::string_view extension(void) const override { return ".wc7"; }

        [[nodiscard]] Generation generation(void) const override;
        [[nodiscard]] bool bean(void) const override;
        [[nodiscard]] bool BP(void) const override;
        [[nodiscard]] bool item(void) const override;
        [[nodiscard]] bool pokemon(void) const override;
        [[nodiscard]] bool power(void) const override;
        [[nodiscard]] std::string title(void) const override;
        [[nodiscard]] u8 type(void) const override;
        [[nodiscard]] u16 ID(void) const override;
        [[nodiscard]] u16 object(void) const override;
        [[nodiscard]] u16 objectQuantity(void) const;
        // Multiple-item cards
        [[nodiscard]] u16 object(int index) const;
        [[nodiscard]] u16 objectQuantity(int index) const;
        [[nodiscard]] int items(void) const;
        [[nodiscard]] bool multiObtainable(void) const override;
        [[nodiscard]] u8 flags(void) const override;
        [[nodiscard]] u8 cardLocation(void) const override;
        [[nodiscard]] bool repeatable(void) const;
        [[nodiscard]] bool used(void) const override;
        [[nodiscard]] bool oncePerDay(void) const;

        // Pokemon properties
        [[nodiscard]] bool egg(void) const override;
        [[nodiscard]] bool shiny(void) const override;
        [[nodiscard]] Ability ability(void) const override;
        [[nodiscard]] u8 abilityType(void) const override;
        [[nodiscard]] Ball ball(void) const override;
        [[nodiscard]] u32 encryptionConstant(void) const;
        [[nodiscard]] u8 alternativeForm(void) const override;
        [[nodiscard]] Gender gender(void) const override;
        [[nodiscard]] u16 heldItem(void) const override;
        [[nodiscard]] Language language(void) const override;
        [[nodiscard]] u8 level(void) const override;
        [[nodiscard]] u8 metLevel(void) const override;
        [[nodiscard]] Move move(u8 index) const override;
        [[nodiscard]] Move relearnMove(u8 index) const;
        [[nodiscard]] Nature nature(void) const override;
        [[nodiscard]] std::string nickname(void) const override;
        [[nodiscard]] GameVersion version(void) const;
        [[nodiscard]] std::string otName(void) const override;
        [[nodiscard]] Gender otGender(void) const;
        [[nodiscard]] u8 otIntensity(void) const;
        [[nodiscard]] u8 otMemory(void) const;
        [[nodiscard]] u16 otTextvar(void) const;
        [[nodiscard]] u8 otFeeling(void) const;
        [[nodiscard]] u16 TID(void) const override;
        [[nodiscard]] u16 SID(void) const override;
        [[nodiscard]] u8 PIDType(void) const override;
        [[nodiscard]] u32 PID(void) const override;
        [[nodiscard]] Species species(void) const override;
        [[nodiscard]] u16 eggLocation(void) const override;
        [[nodiscard]] u16 metLocation(void) const override;
        [[nodiscard]] u8 contest(u8 index) const override;
        [[nodiscard]] u8 iv(Stat index) const override;
        [[nodiscard]] u8 ev(Stat index) const;
        [[nodiscard]] u16 additionalItem(void) const;
        [[nodiscard]] bool hasRibbon(Ribbon rib) const override;
        [[nodiscard]] bool ribbon(Ribbon rib) const override;

        [[nodiscard]] u16 formSpecies(void) const override;

        [[nodiscard]] int size(void) const override { return length; }
        [[nodiscard]] const u8* rawData(void) const override { return data; }
    };
}

#endif
