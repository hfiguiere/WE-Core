/*
 *  File:       TPTSVFilterTests.cpp
 *
 *  Version:    1.0.0
 *
 *  Created:    09/05/2017
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

#include "catch.hpp"
#include "CoreTestLib.h"
#include "WEFilters/TPTSVFilter.h"

SCENARIO("TPTSVFilter: Parameters can be set and retrieved correctly") {
    GIVEN("A new TPTSVFilter object") {
        TPTSVFilter mFilter;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mFilter.getMode() == 1);
                CHECK(CoreTestLib::compareFloats(mFilter.getCutoff(), 20.0));
                CHECK(CoreTestLib::compareFloats(mFilter.getQ(), 0.5));
                CHECK(CoreTestLib::compareFloats(mFilter.getGain(), 1.0));
            }
        }

        WHEN("All parameters are changed to unique values") {
            mFilter.setMode(2);
            mFilter.setCutoff(21);
            mFilter.setQ(0.6);
            mFilter.setGain(1.1);

            THEN("They all get their correct unique values") {
                CHECK(mFilter.getMode() == 2);
                CHECK(CoreTestLib::compareFloats(mFilter.getCutoff(), 21.0));
                CHECK(CoreTestLib::compareFloats(mFilter.getQ(), 0.6));
                CHECK(CoreTestLib::compareFloats(mFilter.getGain(), 1.1));
            }
        }
    }
}

SCENARIO("TPTSVFilter: Parameters enforce their bounds correctly") {
    GIVEN("A new TPTSVFilter object") {
        TPTSVFilter mFilter;
        
        WHEN("All parameter values are too low") {
            mFilter.setMode(-1);
            mFilter.setCutoff(19);
            mFilter.setQ(0);
            mFilter.setGain(-1);
            
            THEN("Parameters enforce their lower bounds") {
                CHECK(mFilter.getMode() == 1);
                CHECK(CoreTestLib::compareFloats(mFilter.getCutoff(), 20.0));
                CHECK(CoreTestLib::compareFloats(mFilter.getQ(), 0.1));
                CHECK(CoreTestLib::compareFloats(mFilter.getGain(), 0.0));
            }
        }

        WHEN("All parameter values are too high") {
            mFilter.setMode(5);
            mFilter.setCutoff(20001);
            mFilter.setQ(21);
            mFilter.setGain(3);
            
            THEN("Parameters enforce their upper bounds") {
                CHECK(mFilter.getMode() == 4);
                CHECK(CoreTestLib::compareFloats(mFilter.getCutoff(), 20000.0));
                CHECK(CoreTestLib::compareFloats(mFilter.getQ(), 20.0));
                CHECK(CoreTestLib::compareFloats(mFilter.getGain(), 2.0));
            }
        }
    }
}

SCENARIO("TPTSVFilter: Silence in = silence out") {
    GIVEN("A TPTSVFilter and a buffer of silent samples") {
        std::vector<double> buffer(1024);
        TPTSVFilter mFilter;
        
        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(buffer.begin(), buffer.end(), 0);
            
            // do processing
            mFilter.processBlock(&buffer[0], buffer.size());
            
            THEN("The output is silence") {
                for (size_t iii {0}; iii < buffer.size(); iii++) {
                    CHECK(CoreTestLib::compareFloats(buffer[iii], 0.0));
                }
            }
        }
    }
}
