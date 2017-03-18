/*
 *  File:       CarveParameters.h
 *
 *  Version:    1.0.0
 *
 *  Created:    25/09/2016
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
 */

#ifndef CarveParameters_h
#define CarveParameters_h

#include "General/ParameterDefinition.h"

namespace CarveParameters {
    
    class ModeParameter : public ParameterDefinition::BaseParameter<int> {
    public:
        using ParameterDefinition::BaseParameter<int>::BaseParameter;
        
        static const int    OFF = 1,
                            SINE = 2,
                            PARABOLIC_SOFT = 3,
                            PARABOLIC_HARD = 4,
                            ASYMMETRIC_SINE = 5,
                            EXPONENT = 6,
                            CLIPPER = 7;
    };

    //@{
    /**
     * A parameter which can take any float value between the ranges defined.
     * The values passed on construction are in the following order:
     *      minimum value,
     *      maximum value,
     *      default value
     */
    const ParameterDefinition::RangedParameter<double>   PREGAIN(0, 2, 1),
                                                        POSTGAIN(0, 2, 0.5),
                                                        TWEAK(-1, 1, 0);
    //@}

    const ModeParameter MODE(1, 7, 1);
}

#endif /* CarveParameters_h */
