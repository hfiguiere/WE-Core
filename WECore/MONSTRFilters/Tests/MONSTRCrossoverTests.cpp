/*
 *  File:       MONSTRCrossoverTests.cpp
 *
 *  Version:    1.0.0
 *
 *  Created:    12/05/2017
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

#include <memory>

#include "catch.hpp"
#include "MONSTRFilters/MONSTRCrossover.h"
#include "General/CoreMath.h"
#include "MONSTRFilters/Tests/TestData.h"
#include "WEFilters/StereoWidthProcessor.h"

SCENARIO("MONSTRCrossover: Parameters can be set and retrieved correctly") {
    GIVEN("A new MONSTRCrossover object") {
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("Nothing is changed") {
            THEN("Parameters have their default values") {
                CHECK(mCrossover.getCrossoverLower() == Approx(100.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(5000.0f));

                CHECK(mCrossover.band1.getIsActive() == true);
                CHECK(mCrossover.band2.getIsActive() == true);
                CHECK(mCrossover.band2.getIsActive() == true);
            }
        }

        WHEN("All parameters are changed to unique values") {
            mCrossover.setCrossoverLower(41);
            mCrossover.setCrossoverUpper(3001);

            mCrossover.band1.setIsActive(1);

            THEN("They all get their correct unique values") {
                CHECK(mCrossover.getCrossoverLower() == Approx(41.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3001.0f));

                CHECK(mCrossover.band1.getIsActive() == 1);
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Parameters enforce their bounds correctly") {
    GIVEN("A new MONSTRCrossover object") {
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;

        WHEN("All parameter values are too low") {
            mCrossover.setCrossoverLower(39);
            mCrossover.setCrossoverUpper(2999);

            THEN("Parameters enforce their lower bounds") {
                CHECK(mCrossover.getCrossoverLower() == Approx(40.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3000.0f));
            }
        }

        WHEN("All parameter values are too high") {
            mCrossover.setCrossoverLower(39);
            mCrossover.setCrossoverUpper(2999);

            THEN("Parameters enforce their upper bounds") {
                CHECK(mCrossover.getCrossoverLower() == Approx(40.0f));
                CHECK(mCrossover.getCrossoverUpper() == Approx(3000.0f));
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Silence in = silence out") {
    GIVEN("A MONSTRCrossover, processors, and a buffer of silent samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.band1.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band2.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band3.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

        WHEN("The silence samples are processed") {
            // fill the buffer
            std::fill(leftBuffer.begin(), leftBuffer.end(), 0);
            std::fill(rightBuffer.begin(), rightBuffer.end(), 0);

            // do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The output is silence") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(0.0).margin(0.00001));
                    CHECK(rightBuffer[iii] == Approx(0.0).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Sine in = sine out") {
    GIVEN("A MONSTRCrossover, processors, and a buffer of sine samples") {
        std::vector<double> leftBuffer(1024);
        std::vector<double> rightBuffer(1024);
        const std::vector<double>& expectedOutput =
                TestData::MONSTR::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.band1.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band2.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band3.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        WHEN("The bands are all active with neutral processing") {
            // fill the buffer
            std::generate(leftBuffer.begin(),
                          leftBuffer.end(),
                          [iii = 0]() mutable {return std::sin(CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                    CHECK(rightBuffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }

        WHEN("The bands are all bypassed") {
            // bypass the bands
            mCrossover.band1.setIsActive(0);
            mCrossover.band2.setIsActive(0);
            mCrossover.band3.setIsActive(0);

            // fill the buffer
            std::generate(leftBuffer.begin(),
                          leftBuffer.end(),
                          [iii = 0]() mutable {return std::sin(CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );
            std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

            // do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                    CHECK(rightBuffer[iii] == Approx(expectedOutput[iii]).margin(0.00001));
                }
            }
        }
    }
}

SCENARIO("MONSTRCrossover: Small buffer") {
    GIVEN("A MONSTRCrossover and a buffer of sine samples smaller than the internal buffer") {
        std::vector<double> leftBuffer(100);
        std::vector<double> rightBuffer(100);
        const std::vector<double>& expectedOutputLeft =
                TestData::MONSTR::Data.at(Catch::getResultCapture().getCurrentTestName());
        const std::vector<double>& expectedOutputRight =
                TestData::MONSTR::Data.at(Catch::getResultCapture().getCurrentTestName());

        WECore::MONSTR::MONSTRCrossover<double> mCrossover;
        mCrossover.band1.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band2.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());
        mCrossover.band3.setEffectsProcessor(std::make_shared<WECore::StereoWidth::StereoWidthProcessor<double>>());

        // Set some parameters for the input signal
        constexpr size_t SAMPLE_RATE {44100};
        constexpr size_t SINE_FREQ {1000};
        constexpr double SAMPLES_PER_CYCLE {SAMPLE_RATE / SINE_FREQ};

        // fill the buffers
        std::generate(leftBuffer.begin(),
                      leftBuffer.end(),
                      [iii = 0]() mutable {return std::sin(CoreMath::LONG_TAU * (iii++ / SAMPLES_PER_CYCLE));} );
        std::copy(leftBuffer.begin(), leftBuffer.end() , rightBuffer.begin());

        WHEN("The bands are all active") {

            // do processing
            mCrossover.Process2in2out(&leftBuffer[0], &rightBuffer[0], leftBuffer.size());

            THEN("The expected output is produced") {
                for (size_t iii {0}; iii < leftBuffer.size(); iii++) {
                    CHECK(leftBuffer[iii] == Approx(expectedOutputLeft[iii]).margin(0.00001));
                    CHECK(rightBuffer[iii] == Approx(expectedOutputRight[iii]).margin(0.00001));
                }
            }
        }
    }
}
