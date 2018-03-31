/*
 *  File:       SongbirdFormantFilter.h
 *
 *  Version:    1.0.0
 *
 *  Created:    16/07/2016
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

#ifndef SONGBIRDFORMANTFILTER_H_INCLUDED
#define SONGBIRDFORMANTFILTER_H_INCLUDED

#include "SongbirdFilters/Formant.h"
#include "WEFilters/TPTSVFilter.h"
#include <vector>

/**
 * A class containing a vector of bandpass filters to produce a vowel sound.
 *
 * Supports only mono audio processing. For stereo processing, you must create
 * two objects of this type. (Do not reuse a single object for both channels)
 *
 * To use this class, simply call setFormants, reset, and process as necessary.
 *
 * @see setFormants - must be called before performing any processing
 * @see Formant     - Formant objects are required for operation of this class
 */
class SongbirdFormantFilter {
public:
    /**
     * Creates and stores the appropriate number of filters.
     *
     * @param   numFormants The number of formant peaks the filter should produce
     *                      (therefore the number of bandpass filters it will need to
     *                      contain), defaults to 5
     */
    SongbirdFormantFilter(int numFormants = 5) {
        for (int iii {0}; iii < numFormants; iii++) {
            TPTSVFilter tempFilter;
            tempFilter.setMode(TPTSVFilterParameters::FILTER_MODE.PEAK);
            tempFilter.setQ(15);
            _filters.push_back(tempFilter);
        }
    }
    
    /**
     * Default.
     */
    virtual ~SongbirdFormantFilter() = default;
    
    /**
     * Applies the filtering to a mono buffer of samples.
     * Expect seg faults or other memory issues if arguements passed are incorrect.
     *
     * @param   inSamples   Pointer to the first sample of the buffer
     * @param   numSamples  Number of samples in the buffer
     */
    void process(double* inSamples, size_t numSamples) {

        // If the buffer we've been passed is bigger than our static internal buffer, then we need
        // to break it into chunks
        const size_t numBuffersRequired {static_cast<size_t>(
            std::ceil(static_cast<double>(numSamples) / INTERNAL_BUFFER_SIZE)
            )};

        for (size_t bufferNumber {0}; bufferNumber < numBuffersRequired; bufferNumber++) {

            // Calculate how many samples need to be processed in this chunk
            const size_t numSamplesRemaining {numSamples - (bufferNumber * INTERNAL_BUFFER_SIZE)};
            const size_t numSamplesToCopy {std::min(numSamplesRemaining,
                                            static_cast<size_t>(INTERNAL_BUFFER_SIZE))};

            // Empty the output buffer
            std::fill(_outputBuffer, &_outputBuffer[numSamplesToCopy], 0);

            // Get a pointer to the start of this chunk
            double* const bufferInputStart {&inSamples[bufferNumber * INTERNAL_BUFFER_SIZE]};

            // Perform the filtering for each formant peak
            for (size_t filterNumber {0}; filterNumber < _filters.size(); filterNumber++) {

                // Copy the input samples to the temp buffer
                std::copy(bufferInputStart, &bufferInputStart[numSamplesToCopy], _tempInputBuffer);

                _filters[filterNumber].processBlock(_tempInputBuffer, numSamplesToCopy);

                // Add the processed samples to the output buffer
                for (size_t iii {0}; iii < numSamplesToCopy; iii++) {
                    _outputBuffer[iii] += _tempInputBuffer[iii];
                }
            }

            // Write the buffer to output
            std::copy(_outputBuffer, &_outputBuffer[numSamplesToCopy], bufferInputStart);
        }
    }
    
    /**
     * Sets the properties of each bandpass filter contained in the object.
     *
     * @param   formants    A vector of Formants, the size of which must equal the 
     *                      number of bandpass filters in the object, as a single Formant
     *                      is applied to single bandpass filter in a one-to-one fashion
     *
     * @return  A boolean value, true if the formants have been applied to the filters
     *          correctly, false if the operation failed
     *
     * @see     Formant - This object is used to as a convenient container of all the
     *                    parameters which can be supplied to a bandpass filter.
     */
    bool setFormants(std::vector<Formant> formants) {
        bool retVal {false};
        
        // if the correct number of formants have been supplied,
        // apply them to each filter in turn
        if (_filters.size() == formants.size()) {
            retVal = true;
            
            for (size_t iii {0}; iii < _filters.size(); iii++) {
                _filters[iii].setCutoff(formants[iii].frequency);
                
                double gainAbs = pow(10, formants[iii].gaindB / 20.0);
                _filters[iii].setGain(gainAbs);
            }
        }
        
        return retVal;
    }
    
    /**
     * Sets the sample rate which the filters will be operating on.
     */
    void setSampleRate(double val) {
        for (TPTSVFilter& filter : _filters) {
            filter.setSampleRate(val);
        }
    }
    
    /**
     * Resets all filters.
     * Call this whenever the audio stream is interrupted (ie. the playhead is moved)
     */
    void reset() {
        for (TPTSVFilter& filter : _filters) {
            filter.reset();
        }
    }
    
private:
    std::vector<TPTSVFilter> _filters;

    static constexpr unsigned int INTERNAL_BUFFER_SIZE = 512;

    double _outputBuffer[INTERNAL_BUFFER_SIZE];
    double _tempInputBuffer[INTERNAL_BUFFER_SIZE];
};



#endif  // SONGBIRDFORMANTFILTER_H_INCLUDED
