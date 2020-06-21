/*
 *   This file is part of PKSM-Core
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#include "sav/Sav4.hpp"
#include "pkx/PK4.hpp"
#include "utils/crypto.hpp"
#include "utils/endian.hpp"
#include "utils/i18n.hpp"
#include "utils/utils.hpp"
#include "wcx/PGT.hpp"

namespace pksm
{
    Sav4::CountType Sav4::compareCounters(u32 c1, u32 c2)
    {
        if (c1 == 0xFFFFFFFF && c2 == 0)
        {
            return CountType::SECOND;
        }
        else if (c1 > c2)
        {
            return CountType::FIRST;
        }
        else if (c2 > c1)
        {
            return CountType::SECOND;
        }
        else
        {
            return CountType::SAME;
        }
    }

    void Sav4::GBO(void)
    {
        int ofs = GBOOffset;

        u32 major1 = LittleEndian::convertTo<u32>(&data[ofs]), major2 = LittleEndian::convertTo<u32>(&data[ofs + 0x40000]),
            minor1 = LittleEndian::convertTo<u32>(&data[ofs + 4]), minor2 = LittleEndian::convertTo<u32>(&data[ofs + 0x40004]);

        CountType cmp = compareCounters(major1, major2);
        if (cmp == CountType::FIRST)
        {
            gbo = 0;
        }
        else if (cmp == CountType::SECOND)
        {
            gbo = 0x40000;
        }
        else
        {
            cmp = compareCounters(minor1, minor2);
            if (cmp == CountType::SECOND)
            {
                gbo = 0x40000;
            }
            else
            {
                gbo = 0;
            }
        }
    }

    void Sav4::SBO(void)
    {
        int ofs = SBOOffset;

        u32 major1 = LittleEndian::convertTo<u32>(&data[ofs]), major2 = LittleEndian::convertTo<u32>(&data[ofs + 0x40000]),
            minor1 = LittleEndian::convertTo<u32>(&data[ofs + 4]), minor2 = LittleEndian::convertTo<u32>(&data[ofs + 0x40004]);

        CountType cmp = compareCounters(major1, major2);
        if (cmp == CountType::FIRST)
        {
            sbo = 0;
        }
        else if (cmp == CountType::SECOND)
        {
            sbo = 0x40000;
        }
        else
        {
            cmp = compareCounters(minor1, minor2);
            if (cmp == CountType::SECOND)
            {
                sbo = 0x40000;
            }
            else
            {
                sbo = 0;
            }
        }
    }

    void Sav4::resign(void)
    {
        u16 cs;
        // start, end, chkoffset
        int general[3] = {
            0x0, game == Game::DP ? 0xC0EC : game == Game::Pt ? 0xCF18 : 0xF618, game == Game::DP ? 0xC0FE : game == Game::Pt ? 0xCF2A : 0xF626};
        int storage[3] = {game == Game::DP ? 0xC100 : game == Game::Pt ? 0xCF2C : 0xF700,
            game == Game::DP ? 0x1E2CC : game == Game::Pt ? 0x1F0FC : 0x21A00, game == Game::DP ? 0x1E2DE : game == Game::Pt ? 0x1F10E : 0x21A0E};

        cs = pksm::crypto::ccitt16(&data[gbo + general[0]], general[1] - general[0]);
        LittleEndian::convertFrom<u16>(&data[gbo + general[2]], cs);

        cs = pksm::crypto::ccitt16(&data[sbo + storage[0]], storage[1] - storage[0]);
        LittleEndian::convertFrom<u16>(&data[sbo + storage[2]], cs);
    }

    u16 Sav4::TID(void) const { return LittleEndian::convertTo<u16>(&data[Trainer1 + 0x10]); }
    void Sav4::TID(u16 v) { LittleEndian::convertFrom<u16>(&data[Trainer1 + 0x10], v); }

    u16 Sav4::SID(void) const { return LittleEndian::convertTo<u16>(&data[Trainer1 + 0x12]); }
    void Sav4::SID(u16 v) { LittleEndian::convertFrom<u16>(&data[Trainer1 + 0x12], v); }

    GameVersion Sav4::version(void) const { return game == Game::DP ? GameVersion::D : game == Game::Pt ? GameVersion::Pt : GameVersion::HG; }
    void Sav4::version(GameVersion) {}

    Gender Sav4::gender(void) const { return Gender{data[Trainer1 + 0x18]}; }
    void Sav4::gender(Gender v) { data[Trainer1 + 0x18] = u8(v); }

    Language Sav4::language(void) const { return Language(data[Trainer1 + 0x19]); }
    void Sav4::language(Language v) { data[Trainer1 + 0x19] = u8(v); }

    std::string Sav4::otName(void) const { return StringUtils::transString45(StringUtils::getString4(data.get(), Trainer1, 8)); }
    void Sav4::otName(const std::string& v) { StringUtils::setString4(data.get(), StringUtils::transString45(v), Trainer1, 8); }

    u32 Sav4::money(void) const { return LittleEndian::convertTo<u32>(&data[Trainer1 + 0x14]); }
    void Sav4::money(u32 v) { LittleEndian::convertFrom<u32>(&data[Trainer1 + 0x14], v); }

    u32 Sav4::BP(void) const { return LittleEndian::convertTo<u16>(&data[Trainer1 + 0x20]); } // Returns Coins @ Game Corner
    void Sav4::BP(u32 v) { LittleEndian::convertFrom<u32>(&data[Trainer1 + 0x20], v); }

    u8 Sav4::badges(void) const
    {
        u8 badgeBits = data[Trainer1 + 0x1A];
        u8 ret       = 0;
        for (size_t i = 0; i < sizeof(badgeBits) * 8; i++)
        {
            ret += (badgeBits & (1 << i)) ? 1 : 0;
        }
        if (game == Game::HGSS)
        {
            badgeBits = data[Trainer1 + 0x1F];
            for (size_t i = 0; i < sizeof(badgeBits) * 8; i++)
            {
                ret += (badgeBits & (1 << i)) ? 1 : 0;
            }
        }
        return ret;
    }

    u16 Sav4::playedHours(void) const { return LittleEndian::convertTo<u16>(&data[Trainer1 + 0x22]); }
    void Sav4::playedHours(u16 v) { LittleEndian::convertFrom<u16>(&data[Trainer1 + 0x22], v); }

    u8 Sav4::playedMinutes(void) const { return data[Trainer1 + 0x24]; }
    void Sav4::playedMinutes(u8 v) { data[Trainer1 + 0x24] = v; }

    u8 Sav4::playedSeconds(void) const { return data[Trainer1 + 0x25]; }
    void Sav4::playedSeconds(u8 v) { data[Trainer1 + 0x25] = v; }

    u8 Sav4::currentBox(void) const
    {
        int ofs = game == Game::HGSS ? boxOffset(maxBoxes(), 0) : Box - 4;
        return data[ofs];
    }

    void Sav4::currentBox(u8 v)
    {
        int ofs   = game == Game::HGSS ? boxOffset(maxBoxes(), 0) : Box - 4;
        data[ofs] = v;
    }

    u32 Sav4::boxOffset(u8 box, u8 slot) const
    {
        return Box + PK4::BOX_LENGTH * box * 30 + (game == Game::HGSS ? box * 0x10 : 0) + slot * PK4::BOX_LENGTH;
    }
    u32 Sav4::partyOffset(u8 slot) const { return Party + slot * PK4::PARTY_LENGTH; }

    std::unique_ptr<PKX> Sav4::pkm(u8 slot) const { return PKX::getPKM<Generation::FOUR>(&data[partyOffset(slot)], true); }

    void Sav4::pkm(const PKX& pk, u8 slot)
    {
        if (pk.generation() == Generation::FOUR)
        {
            auto pk4 = pk.partyClone();
            pk4->encrypt();
            std::copy(pk4->rawData(), pk4->rawData() + pk4->getLength(), &data[partyOffset(slot)]);
        }
    }

    std::unique_ptr<PKX> Sav4::pkm(u8 box, u8 slot) const { return PKX::getPKM<Generation::FOUR>(&data[boxOffset(box, slot)]); }

    void Sav4::pkm(const PKX& pk, u8 box, u8 slot, bool applyTrade)
    {
        if (pk.generation() == Generation::FOUR)
        {
            auto pkm = pk.clone();
            if (applyTrade)
            {
                trade(*pkm);
            }

            std::copy(pkm->rawData(), pkm->rawData() + PK4::BOX_LENGTH, &data[boxOffset(box, slot)]);
        }
    }

    void Sav4::trade(PKX& pk, const Date& date) const
    {
        if (pk.egg() && (otName() != pk.otName() || TID() != pk.TID() || SID() != pk.SID() || gender() != pk.otGender()))
        {
            pk.metLocation(2002);
            pk.metDate(date);
        }
    }

    void Sav4::cryptBoxData(bool crypted)
    {
        for (u8 box = 0; box < maxBoxes(); box++)
        {
            for (u8 slot = 0; slot < 30; slot++)
            {
                std::unique_ptr<PKX> pk4 = PKX::getPKM<Generation::FOUR>(&data[boxOffset(box, slot)], false, true);
                if (!crypted)
                {
                    pk4->encrypt();
                }
            }
        }
    }

    bool Sav4::giftsMenuActivated(void) const { return (data[gbo + 72] & 1) == 1; }

    void Sav4::giftsMenuActivated(bool v)
    {
        data[gbo + 72] &= 0xFE;
        data[gbo + 72] |= v ? 1 : 0;
    }

    void Sav4::mysteryGift(WCX& wc, int& pos)
    {
        if (wc.generation() == Generation::FOUR)
        {
            giftsMenuActivated(true);
            PGT* pgt                            = (PGT*)&wc;
            data[WondercardFlags + (2047 >> 3)] = 0x80;
            std::copy(pgt->rawData(), pgt->rawData() + PGT::length, &data[WondercardData + pos * PGT::length]);
            pos++;
            if (game == Game::DP)
            {
                static constexpr u32 dpSlotActive = 0xEDB88320;
                const int ofs                     = WondercardFlags + 0x100;
                LittleEndian::convertFrom<u32>(&data[ofs + 4 * pos], dpSlotActive);
            }
        }
    }

    std::string Sav4::boxName(u8 box) const
    {
        return StringUtils::transString45(StringUtils::getString4(data.get(), boxOffset(18, 0) + box * 0x28 + (game == Game::HGSS ? 0x8 : 0), 9));
    }

    void Sav4::boxName(u8 box, const std::string& name)
    {
        StringUtils::setString4(data.get(), StringUtils::transString45(name), boxOffset(18, 0) + box * 0x28 + (game == Game::HGSS ? 0x8 : 0), 9);
    }

    int adjustWallpaper(int value, int shift)
    {
        // Pt's  Special Wallpapers 1-8 are shifted by +0x8
        // HG/SS Special Wallpapers 1-8 (Primo Phrases) are shifted by +0x10
        if (value >= 0x10) // special
            return value + shift;
        return value;
    }

    u8 Sav4::boxWallpaper(u8 box) const
    {
        int offset = boxOffset(maxBoxes(), 0);
        if (game == Game::HGSS)
            offset += 0x8;
        offset += (maxBoxes() * 0x28) + box;

        int v = data[offset];

        if (game != Game::DP)
            v = adjustWallpaper(v, -(game == Game::Pt ? 0x8 : 0x10));
        return v;
    }
    void Sav4::boxWallpaper(u8 box, u8 v)
    {
        if (game != Game::DP)
            v = adjustWallpaper(v, game == Game::Pt ? 0x8 : 0x10);

        int offset = boxOffset(maxBoxes(), 0);
        if (game == Game::HGSS)
            offset += 0x8;
        offset += (maxBoxes() * 0x28) + box;

        if (offset < 0 || box > maxBoxes())
            return;
        data[offset] = v;
    }

    u8 Sav4::partyCount(void) const { return data[Party - 4]; }
    void Sav4::partyCount(u8 v) { data[Party - 4] = v; }

    void Sav4::dex(const PKX& pk)
    {
        if (!(availableSpecies().count(pk.species()) > 0) || pk.egg())
        {
            return;
        }

        static constexpr int brSize = 0x40;
        int bit                     = u16(pk.species()) - 1;
        u8 mask                     = (u8)(1 << (bit & 7));
        int ofs                     = PokeDex + (bit >> 3) + 0x4;

        /* 4 BitRegions with 0x40*8 bits
         * Region 0: Caught (Captured/Owned) flags
         * Region 1: Seen flags
         * Region 2/3: Toggle for gender display
         * 4 possible states: 00, 01, 10, 11
         * 00 - 1Seen: Male Only
         * 01 - 2Seen: Male First, Female Second
         * 10 - 2Seen: Female First, Male Second
         * 11 - 1Seen: Female Only
         * (bit1 ^ bit2) + 1 = forms in dex
         * bit2 = male/female shown first toggle */

        // Set the species() Owned Flag
        data[ofs + brSize * 0] |= mask;

        // Check if already Seen
        if ((data[ofs + brSize * 1] & mask) == 0) // Not seen
        {
            data[ofs + brSize * 1] |= mask; // Set seen
            u8 gr = pk.genderType();
            switch (gr)
            {
                case 255: // Genderless
                case 0:   // Male Only
                    data[ofs + brSize * 2] &= ~mask;
                    data[ofs + brSize * 3] &= ~mask;
                    break;
                case 254: // Female Only
                    data[ofs + brSize * 2] |= mask;
                    data[ofs + brSize * 3] |= mask;
                    break;
                default: // Male or Female
                    bool m = (data[ofs + brSize * 2] & mask) != 0;
                    bool f = (data[ofs + brSize * 3] & mask) != 0;
                    if (m || f) // bit already set?
                        break;
                    u8 gender = u8(pk.gender()) & 1;
                    data[ofs + brSize * 2] &= ~mask; // unset
                    data[ofs + brSize * 3] &= ~mask; // unset
                    gender ^= 1;                     // Set OTHER gender seen bit so it appears second
                    data[ofs + brSize * (2 + gender)] |= mask;
                    break;
            }
        }

        int formOffset        = PokeDex + 4 + (brSize * 4) + 4;
        std::vector<u8> forms = getForms(pk.species());
        if (forms.size() > 0)
        {
            if (pk.species() == Species::Unown)
            {
                for (u8 i = 0; i < 0x1C; i++)
                {
                    u8 val = data[formOffset + 4 + i];
                    if (val == pk.alternativeForm())
                        break; // already set
                    if (val != 0xFF)
                        continue; // keep searching

                    data[formOffset + 4 + i] = (u8)pk.alternativeForm();
                    break; // form now set
                }
            }
            else if (pk.species() == Species::Pichu && game == Game::HGSS)
            {
                u8 form = pk.alternativeForm() == 1 ? 2 : u8(pk.gender());
                checkInsertForm(forms, form);
                setForms(forms, pk.species());
            }
            else
            {
                checkInsertForm(forms, pk.alternativeForm());
                setForms(forms, pk.species());
            }
        }

        int dpl = 0;
        if (game == Game::DP)
        {
            Species DPLangSpecies[] = {Species::Ekans, Species::Pikachu, Species::Psyduck, Species::Ponyta, Species::Staryu, Species::Magikarp,
                Species::Wobbuffet, Species::Heracross, Species::Sneasel, Species::Teddiursa, Species::Houndour, Species::Wingull, Species::Slakoth,
                Species::Roselia};
            for (int i = 0; i < 14; i++)
            {
                if (pk.species() == DPLangSpecies[i])
                {
                    dpl = i + 1;
                    break;
                }
            }
            if (dpl == 0)
                return;
        }

        // Set the Language
        int languageFlags = formOffset + (game == Game::HGSS ? 0x3C : 0x20);
        int lang          = u8(pk.language()) - 1;
        switch (lang) // invert ITA/GER
        {
            case 3:
                lang = 4;
                break;
            case 4:
                lang = 3;
                break;
        }
        if (lang > 5)
            lang = 0;                 // no KOR+
        lang = (lang < 0) ? 1 : lang; // default English
        data[languageFlags + (game == Game::DP ? dpl : u16(pk.species()))] |= (u8)(1 << lang);
    }

    int Sav4::dexSeen(void) const
    {
        int ret                     = 0;
        static constexpr int brSize = 0x40;
        int ofs                     = PokeDex + 0x4;
        for (const auto& spec : availableSpecies())
        {
            u16 i   = u16(spec);
            int bit = i - 1;
            int bd  = bit >> 3;
            int bm  = bit & 7;

            if ((1 << bm & data[ofs + bd + brSize]) != 0)
            {
                ret++;
            }
        }
        return ret;
    }

    int Sav4::dexCaught(void) const
    {
        int ret = 0;
        int ofs = PokeDex + 0x4;
        for (const auto& spec : availableSpecies())
        {
            u16 i   = u16(spec);
            int bit = i - 1;
            int bd  = bit >> 3;
            int bm  = bit & 7;

            if ((1 << bm & data[ofs + bd]) != 0)
            {
                ret++;
            }
        }
        return ret;
    }

    bool Sav4::checkInsertForm(std::vector<u8>& forms, u8 formNum)
    {
        for (size_t i = 0; i < forms.size(); i++)
        {
            if (forms[i] == formNum)
            {
                return false;
            }
        }

        std::vector<u8> dummy(forms.size(), 0xFF);
        if (std::equal(forms.begin(), forms.end(), dummy.begin()))
        {
            forms[0] = formNum;
            return true;
        }

        // insert at first empty
        u8 index = 255;
        for (size_t i = 0; i < forms.size(); i++)
        {
            if (forms[i] == 255)
            {
                index = i;
                break;
            }
        }

        if (index == 255)
        {
            return false;
        }

        forms[index] = formNum;
        return true;
    }

    std::vector<u8> Sav4::getForms(Species species)
    {
        static constexpr u8 brSize = 0x40;
        if (species == Species::Deoxys)
        {
            u32 val = (u32)data[PokeDex + 0x4 + 1 * brSize - 1] | data[PokeDex + 0x4 + 2 * brSize - 1] << 8;
            return getDexFormValues(val, 4, 4);
        }

        int formOffset = PokeDex + 4 + 4 * brSize + 4;
        switch (species)
        {
            case Species::Shellos:
                return getDexFormValues(data[formOffset + 0], 1, 2);
            case Species::Gastrodon:
                return getDexFormValues(data[formOffset + 1], 1, 2);
            case Species::Burmy:
                return getDexFormValues(data[formOffset + 2], 2, 3);
            case Species::Wormadam:
                return getDexFormValues(data[formOffset + 3], 2, 3);
            case Species::Unown:
            {
                int ofs = formOffset + 4;
                std::vector<u8> forms(0x1C);
                std::copy(data.get() + ofs, data.get() + ofs + 0x1C, forms.begin());
                return forms;
            }
            default:
                break;
        }

        if (game == Game::DP)
            return {};

        int languageFlags = formOffset + (game == Game::HGSS ? 0x3C : 0x20);
        int formOffset2   = languageFlags + 0x1F4;
        switch (species)
        {
            case Species::Rotom:
                return getDexFormValues(LittleEndian::convertTo<u32>(data.get() + formOffset2), 3, 6);
            case Species::Shaymin:
                return getDexFormValues(data[formOffset2 + 4], 1, 2);
            case Species::Giratina:
                return getDexFormValues(data[formOffset2 + 5], 1, 2);
            case Species::Pichu:
                if (game == Game::HGSS)
                    return getDexFormValues(data[formOffset2 + 6], 2, 3);
            default:
                break;
        }

        return {};
    }

    std::vector<u8> Sav4::getDexFormValues(u32 v, u8 bitsPerForm, u8 readCt)
    {
        std::vector<u8> forms(readCt);

        u8 n1 = 0xFF >> (8 - bitsPerForm);
        for (int i = 0; i < readCt; i++)
        {
            u8 val   = (u8)(v >> (i * bitsPerForm)) & n1;
            forms[i] = n1 == val && bitsPerForm > 1 ? 255 : val;
        }

        if (bitsPerForm == 1 && forms[0] == forms[1] && forms[0] == 1)
        {
            forms[0] = forms[1] = 255;
        }

        return forms;
    }

    void Sav4::setForms(std::vector<u8> forms, Species species)
    {
        static constexpr u8 brSize = 0x40;
        if (species == Species::Deoxys)
        {
            u32 newval                           = setDexFormValues(forms, 4, 4);
            data[PokeDex + 0x4 + 1 * brSize - 1] = (u8)(newval & 0xFF);
            data[PokeDex + 0x4 + 2 * brSize - 1] = (u8)((newval >> 8) & 0xFF);
        }

        int formOffset = PokeDex + 4 + 4 * brSize + 4;
        switch (species)
        {
            case Species::Shellos:
                data[formOffset + 0] = (u8)setDexFormValues(forms, 1, 2);
                return;
            case Species::Gastrodon:
                data[formOffset + 1] = (u8)setDexFormValues(forms, 1, 2);
                return;
            case Species::Burmy:
                data[formOffset + 2] = (u8)setDexFormValues(forms, 2, 3);
                return;
            case Species::Wormadam:
                data[formOffset + 3] = (u8)setDexFormValues(forms, 2, 3);
                return;
            case Species::Unown:
            {
                int ofs = formOffset + 4;
                int len = forms.size();
                forms.resize(0x1C);
                for (size_t i = len; i < forms.size(); i++)
                    forms[i] = 0xFF;
                std::copy(forms.begin(), forms.end(), data.get() + ofs);
                return;
            }
            default:
                break;
        }

        if (game == Game::DP)
            return;

        int languageFlags = formOffset + (game == Game::HGSS ? 0x3C : 0x20);
        int formOffset2   = languageFlags + 0x1F4;
        switch (species)
        {
            case Species::Rotom:
            {
                auto values = LittleEndian::convertFrom(setDexFormValues(forms, 3, 6));
                for (size_t i = 0; i < values.size(); i++)
                {
                    data[formOffset2 + i] = values[i];
                }
                return;
            }
            case Species::Shaymin:
            {
                data[formOffset2 + 4] = (u8)setDexFormValues(forms, 1, 2);
                return;
            }
            case Species::Giratina:
            {
                data[formOffset2 + 5] = (u8)setDexFormValues(forms, 1, 2);
                return;
            }
            case Species::Pichu:
            {
                if (game == Game::HGSS)
                {
                    data[formOffset2 + 6] = (u8)setDexFormValues(forms, 2, 3);
                    return;
                }
            }
            default:
                break;
        }
    }

    u32 Sav4::setDexFormValues(std::vector<u8> forms, u8 bitsPerForm, u8 readCt)
    {
        int n1 = 0xFF >> (8 - bitsPerForm);
        u32 v  = 0xFFFFFFFF << (readCt * bitsPerForm);
        for (size_t i = 0; i < forms.size(); i++)
        {
            int val = forms[i];
            if (val == 255)
                val = n1;

            v |= (u32)(val << (bitsPerForm * i));
        }
        return v;
    }

    std::unique_ptr<PKX> Sav4::emptyPkm() const { return PKX::getPKM<Generation::FOUR>(nullptr); }

    int Sav4::currentGiftAmount(void) const
    {
        u8 t;
        bool empty;
        for (t = 0; t < maxWondercards(); t++)
        {
            empty = true;
            for (u32 j = 0; j < PGT::length; j++)
            {
                if (data[WondercardData + t * PGT::length + j] != 0)
                {
                    empty = false;
                    break;
                }
            }

            if (empty)
            {
                break;
            }
        }
        return t;
    }

    std::unique_ptr<WCX> Sav4::mysteryGift(int pos) const { return std::make_unique<PGT>(data.get() + WondercardData + pos * PGT::length); }

    void Sav4::item(const Item& item, Pouch pouch, u16 slot)
    {
        Item4 inject = (Item4)item;
        auto write   = inject.bytes();
        switch (pouch)
        {
            case Pouch::NormalItem:
                std::copy(write.begin(), write.end(), &data[PouchHeldItem + slot * 4]);
                break;
            case Pouch::KeyItem:
                std::copy(write.begin(), write.end(), &data[PouchKeyItem + slot * 4]);
                break;
            case Pouch::TM:
                std::copy(write.begin(), write.end(), &data[PouchTMHM + slot * 4]);
                break;
            case Pouch::Mail:
                std::copy(write.begin(), write.end(), &data[MailItems + slot * 4]);
                break;
            case Pouch::Medicine:
                std::copy(write.begin(), write.end(), &data[PouchMedicine + slot * 4]);
                break;
            case Pouch::Berry:
                std::copy(write.begin(), write.end(), &data[PouchBerry + slot * 4]);
                break;
            case Pouch::Ball:
                std::copy(write.begin(), write.end(), &data[PouchBalls + slot * 4]);
                break;
            case Pouch::Battle:
                std::copy(write.begin(), write.end(), &data[BattleItems + slot * 4]);
                break;
            default:
                return;
        }
    }

    std::unique_ptr<Item> Sav4::item(Pouch pouch, u16 slot) const
    {
        switch (pouch)
        {
            case Pouch::NormalItem:
                return std::make_unique<Item4>(&data[PouchHeldItem + slot * 4]);
            case Pouch::KeyItem:
                return std::make_unique<Item4>(&data[PouchKeyItem + slot * 4]);
            case Pouch::TM:
                return std::make_unique<Item4>(&data[PouchTMHM + slot * 4]);
            case Pouch::Mail:
                return std::make_unique<Item4>(&data[MailItems + slot * 4]);
            case Pouch::Medicine:
                return std::make_unique<Item4>(&data[PouchMedicine + slot * 4]);
            case Pouch::Berry:
                return std::make_unique<Item4>(&data[PouchBerry + slot * 4]);
            case Pouch::Ball:
                return std::make_unique<Item4>(&data[PouchBalls + slot * 4]);
            case Pouch::Battle:
                return std::make_unique<Item4>(&data[BattleItems + slot * 4]);
            default:
                return nullptr;
        }
    }

    std::vector<std::pair<Sav::Pouch, int>> Sav4::pouches(void) const
    {
        return {{Pouch::NormalItem, game == Game::DP ? 161 : game == Game::Pt ? 162 : 162},
            {Pouch::KeyItem, game == Game::DP ? 37 : game == Game::Pt ? 40 : 38}, {Pouch::TM, game == Game::DP ? 100 : game == Game::Pt ? 100 : 100},
            {Pouch::Mail, game == Game::DP ? 12 : game == Game::Pt ? 12 : 12}, {Pouch::Medicine, game == Game::DP ? 38 : game == Game::Pt ? 38 : 38},
            {Pouch::Berry, game == Game::DP ? 64 : game == Game::Pt ? 64 : 64}, {Pouch::Ball, game == Game::DP ? 15 : game == Game::Pt ? 15 : 24},
            {Pouch::Battle, game == Game::DP ? 13 : game == Game::Pt ? 13 : 13}};
    }

    std::map<Sav::Pouch, std::vector<int>> Sav4::validItems() const
    {
        return {
            {Pouch::NormalItem,
                {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
                    101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 135, 136, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
                    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
                    251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276,
                    277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302,
                    303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327}},
            {Pouch::KeyItem, {434, 435, 437, 444, 445, 446, 447, 450, 456, 464, 465, 466, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
                                 480, 481, 482, 483, 484, 501, 502, 503, 504, 532, 533, 534, 535, 536}},
            {Pouch::TM,
                {328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353,
                    354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379,
                    380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405,
                    406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427}},
            {Pouch::Mail, {137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148}},
            {Pouch::Medicine, {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46,
                                  47, 48, 49, 50, 51, 52, 53, 54}},
            {Pouch::Berry, {149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172,
                               173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196,
                               197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212}},
            {Pouch::Ball, {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 492, 493, 494, 495, 496, 497, 498, 499, 500}},
            {Pouch::Battle, {55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67}}};
    }
}
