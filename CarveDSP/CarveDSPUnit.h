/*
 *	File:		CarveDSPUnit.h
 *
 *	Version:	2.0.0
 *
 *	Created:	09/09/2015
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

#include "CarveParameters.h"
#include "General/CoreMath.h"

/**
 * A class for applying waveshaping functions to samples.
 *
 * To use this class, use the setter and getter methods to manipulate parameters,
 * and call the process method to process individual samples.
 *
 * Each sample is processed with no dependency on the previous samples, therefore a
 * single object can be reused for multiple audio streams if so desired.
 *
 * Internally relies on the parameters provided in CarveParameters.h
 *
 * The process method must be called once for each sample you wish to process:
 * @code
 * CarveDSPUnit unit;
 * unit.setMode(WECore::Carve::Parameters::MODE.SINE);
 * ...
 * set any other parameters you need
 * ...
 * 
 * for (size_t iii {0}; iii < buffer.size(); iii++) {
 *     buffer[iii] = unit.process(buffer[iii]);
 * }
 * @endcode
 */

namespace WECore::Carve {

    class CarveDSPUnit {
    public:
        /**
         * Sets all parameters to their default values.
         */
        CarveDSPUnit() : _preGain(Parameters::PREGAIN.defaultValue),
                         _postGain(Parameters::POSTGAIN.defaultValue),
                         _tweak(Parameters::TWEAK.defaultValue),
                         _mode(Parameters::MODE.defaultValue) { }
        
        virtual ~CarveDSPUnit() {}
        
        /** @name Setter Methods */
        /** @{ */
        
        /**
         * Sets the wave shape which will be applied to the signal.
         *
         * @param[in]   val Value the mode should be set to
         *
         * @see         ModeParameter for valid values
         */
        void setMode(int val) { _mode = Parameters::MODE.BoundsCheck(val); }
        
        /**
         * Sets the gain to be applied to the signal before processing.
         * More pre-gain = more distortion.
         *
         * @param[in]   val Pre-gain value that should be used
         *
         * @see         PREGAIN for valid values
         */
        void setPreGain(double val) { _preGain = Parameters::PREGAIN.BoundsCheck(val); }
        
        /**
         * Sets the gain to be applied to the signal after processing.
         * More post-gain = more volume.
         *     
         * @param[in]   val Post-gain value that should be used
         *
         * @see         POSTGAIN for valid values
         */
        void setPostGain(double val) { _postGain = Parameters::POSTGAIN.BoundsCheck(val); }
        
        /**
         * Sets the tweak value to be applied to the signal during processing.
         * This behaves differently for each mode, and modifies the shape of
         * the wave applied to the signal
         *
         * @param[in]   val Tweak value that should be used
         *
         * @see         TWEAK for valid values
         */
        void setTweak(double val) { _tweak = Parameters::TWEAK.BoundsCheck(val); }
        
        /** @} */
        
        /** @name Getter Methods */
        /** @{ */
        
        /**
         * @see     setMode
         */
        int getMode() { return _mode; }
        
        /**
         * @see     setPreGain
         */
        double getPreGain() { return _preGain; }
        
        /**
         * @see     setPostGain
         */
        double getPostGain() { return _postGain; }
        
        /**
         * @see     setTweak
         */
        double getTweak() { return _tweak; }
        
        /** @} */
        
        /**
         * Performs the processing on the sample, by calling the appropriate
         * private processing methods.
         *
         * @param[in]   inSample    The sample to be processed
         *
         * @return      The value of inSample after processing
         */
        double process(double inSample) const;
        
    private:
        double  _preGain,
                _postGain,
                _tweak;
        
        int _mode;

        double _processSine(double inSample) const;
        
        double _processParabolicSoft(double inSample) const;
        
        double _processParabolicHard(double inSample) const;
        
        double _processAsymmetricSine(double inSample) const;
        
        double _processExponent(double inSample) const;
        
        double _processClipper(double inSample) const;
    };

    double CarveDSPUnit::process(double inSample) const {
        switch (_mode) {
            case Parameters::MODE.OFF:
                return 0;
                
            case Parameters::MODE.SINE:
                return _processSine(inSample);
                
            case Parameters::MODE.PARABOLIC_SOFT:
                return _processParabolicSoft(inSample);
                
            case Parameters::MODE.PARABOLIC_HARD:
                return _processParabolicHard(inSample);
                
            case Parameters::MODE.ASYMMETRIC_SINE:
                return _processAsymmetricSine(inSample);
                
            case Parameters::MODE.EXPONENT:
                return _processExponent(inSample);
                
            case Parameters::MODE.CLIPPER:
                return _processClipper(inSample);
                
            default:
                return _processSine(inSample);
        }
    }

    double CarveDSPUnit::_processSine(double inSample) const {
        return  (
                    (((1 - std::abs(_tweak/2)) * sin(CoreMath::DOUBLE_PI * inSample * _preGain)))
                    + ((_tweak/2) * sin(4 * CoreMath::DOUBLE_PI * inSample * _preGain))
                )
                * _postGain;
    }

    double CarveDSPUnit::_processParabolicSoft(double inSample) const {
        return (
                CoreMath::DOUBLE_PI * inSample * _preGain * ((4 * _tweak)
                - sqrt(4 * pow(inSample * CoreMath::DOUBLE_PI * _preGain, 2))) * 0.5
            )
            * _postGain;
    }

    double CarveDSPUnit::_processParabolicHard(double inSample) const {
        return  (
                    ((1 - std::abs(_tweak/10)) * (atan(_preGain * 4 * CoreMath::DOUBLE_PI * inSample) / 1.5))
                    + ((_tweak/10) * sin(CoreMath::DOUBLE_PI * inSample * _preGain))
                )
                * _postGain;
    }

    double CarveDSPUnit::_processAsymmetricSine(double inSample) const {
        return (
                cos(CoreMath::DOUBLE_PI * inSample * (_tweak + 1))
                * atan(4 * CoreMath::DOUBLE_PI * inSample * _preGain)
            )
            * _postGain;
    }

    double CarveDSPUnit::_processExponent(double inSample) const {
        return  (
                    sin(-0.25 * pow(2 * CoreMath::DOUBLE_E, (inSample * _preGain + 1.5)))
                )
                * _postGain;
    }

    double CarveDSPUnit::_processClipper(double inSample) const {
        inSample *= CoreMath::DOUBLE_PI * _preGain;
        
        return (
                sin(0.5 * inSample) +
                0.3 * sin(1.5 * inSample) +
                0.15 * sin(2.5 * inSample) *
                0.075 * sin(3.5 * inSample) +
                0.0375 * sin(4.5 * inSample) +
                0.01875 * sin(5.5 * inSample) +
                0.009375 * sin(6.5 * inSample)
            )
            * _postGain / 1.5;
    }
}
