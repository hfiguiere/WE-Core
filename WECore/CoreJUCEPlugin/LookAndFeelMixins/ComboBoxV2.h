/*
 *	File:		ComboBoxV2.h
 *
 *	Version:	1.0.0
 *
 *	Created:	19/03/2019
 *
 *	This file is part of WECore.
 *
 *  WECore is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  WECore is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with WECore.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "General/CoreMath.h"

namespace WECore::LookAndFeelMixins {

    /**
     * V2 (December 2018) style lookandfeel combo box mixin.
     */
    template <typename BASE>
    class ComboBoxV2 : public BASE {

    public:
        ComboBoxV2() : _comboBoxFontName("Courier New") {}
        virtual ~ComboBoxV2() = default;

        /** @{ LookAndFeel overrides */
        inline virtual void drawComboBox(Graphics& g,
                                         int width,
                                         int height,
                                         const bool isButtonDown,
                                         int buttonX,
                                         int buttonY,
                                         int buttonW,
                                         int buttonH,
                                         ComboBox& box) override;

        inline virtual Font getComboBoxFont(ComboBox& comboBox) override;
        /** @} */

        void setComboBoxFontName(const char* fontName) { _comboBoxFontName = fontName; }

    private:
        const char* _comboBoxFontName;
    };

    template <typename BASE>
    void ComboBoxV2<BASE>::drawComboBox(Graphics& g,
                                        int /*width*/,
                                        int /*height*/,
                                        const bool /*isButtonDown*/,
                                        int buttonX,
                                        int buttonY,
                                        int buttonW,
                                        int buttonH,
                                        ComboBox& box) {

        g.setColour(box.findColour(ComboBox::arrowColourId));

        Path p;
        constexpr float LINE_WIDTH {0.5};
        const int arrowMarginX {buttonY / 4};
        const int arrowMarginY {buttonH / 3};
        const int arrowTipX {buttonX + (buttonW / 2)};
        const int arrowTipY {buttonY + buttonH - arrowMarginY};

        // Left side of arrow
        p.addLineSegment(Line<float>(buttonX + arrowMarginX,
                                    buttonY + arrowMarginY,
                                    arrowTipX,
                                    arrowTipY),
                        LINE_WIDTH);

        // Right side of arrow
        p.addLineSegment(Line<float>(buttonX + buttonW - arrowMarginX,
                                    buttonY + arrowMarginY,
                                    arrowTipX,
                                    arrowTipY),
                        LINE_WIDTH);

        g.strokePath(p, PathStrokeType(1));
    }

    template <typename BASE>
    Font ComboBoxV2<BASE>::getComboBoxFont(ComboBox& /*comboBox*/) {
        Font comboFont;
        comboFont.setTypefaceName(_comboBoxFontName);
        return comboFont;
    }
}
