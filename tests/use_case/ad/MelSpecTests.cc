/*
 * Copyright (c) 2021 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "AdMelSpectrogram.hpp"
#include <limits>
#include <algorithm>
#include <catch.hpp>

/* First 1024 samples from test wav. */
const std::vector<int16_t> testWav1 = std::vector<int16_t>{
    490,495,445,403,390,259,126,146,
    175,134,232,243,166,145,123,33,
    -61,-4,8,-115,-281,-292,-210,-133,
    -98,-142,-229,-356,-415,-438,-443,-396,
    -377,-297,-85,122,172,16,-197,-351,
    -484,-408,-378,-405,-399,-335,-180,-141,
    -124,-108,-46,37,141,234,264,218,
    147,164,132,111,125,73,2,36,
    107,113,93,6,-40,-153,-273,-282,
    -291,-298,-389,-446,-394,-324,-333,-385,
    -485,-548,-690,-718,-660,-704,-690,-601,
    -549,-641,-637,-513,-469,-366,-227,-269,
    -348,-408,-486,-570,-638,-666,-730,-746,
    -710,-634,-543,-461,-281,-156,-130,-126,
    -144,-118,-23,103,132,37,-69,-86,
    -234,-360,-366,-330,-248,-268,-282,-169,
    -190,-152,-151,-145,-133,-205,-263,-397,
    -558,-656,-668,-718,-779,-828,-856,-817,
    -761,-759,-722,-772,-873,-983,-962,-897,
    -843,-788,-750,-677,-555,-447,-373,-218,
    -182,-230,-204,-174,-144,-127,-231,-199,
    -127,-194,-250,-183,-189,-254,-249,-337,
    -417,-459,-513,-505,-481,-402,-344,-284,
    -281,-441,-450,-423,-327,-119,102,197,
    208,173,102,103,165,131,15,75,
    283,365,322,391,303,287,372,406,
    493,577,640,681,577,498,524,511,
    476,425,380,315,337,339,408,603,
    749,745,672,654,588,520,523,544,
    557,632,636,565,491,413,368,252,
    136,33,1,-26,-152,-258,-98,18,
    1,-18,-99,-117,-109,-228,-295,-349,
    -334,-337,-441,-373,-279,-202,-204,-219,
    -119,149,410,489,564,623,683,642,
    707,872,932,862,833,862,894,784,
    637,559,507,394,306,420,510,484,
    519,526,599,789,959,1052,1063,1030,
    860,697,603,530,475,463,468,461,
    609,641,534,482,435,329,239,216,
    185,82,88,106,60,26,-43,-127,
    -220,-262,-317,-259,-172,-175,-271,-217,
    -196,-164,8,144,150,134,60,13,
    57,-58,-115,-171,-282,-310,-298,-106,
    42,-101,-172,-181,-249,-326,-262,-132,
    -56,-82,-71,-88,-196,-325,-426,-413,
    -411,-317,-191,-172,-195,-292,-328,-191,
    -88,-60,21,-63,-175,-135,-64,-83,
    -163,-279,-440,-536,-403,-308,-236,-132,
    -95,-69,-73,-21,13,133,185,251,
    238,88,-66,-134,-175,-231,-219,-151,
    -213,-328,-340,-374,-459,-601,-556,-395,
    -248,-205,-174,-227,-402,-493,-464,-483,
    -588,-564,-463,-493,-505,-416,-378,-313,
    -215,-192,-192,-59,18,-40,-66,-60,
    -143,-263,-213,-224,-265,-249,-237,-227,
    -418,-504,-573,-699,-679,-577,-500,-570,
    -538,-416,-444,-415,-294,-300,-427,-423,
    -299,-279,-279,-187,-137,-123,60,230,
    227,277,356,413,440,418,477,594,
    697,729,586,561,653,570,590,628,
    497,357,366,470,591,576,458,439,
    417,431,447,349,304,241,294,406,
    484,516,587,598,566,465,380,347,
    316,391,429,409,216,69,57,76,
    150,101,93,113,90,41,-28,-15,
    -2,47,208,261,333,362,239,301,
    422,431,426,434,482,510,480,407,
    244,53,-108,-234,-275,-302,-304,-207,
    -117,-181,-214,-248,-203,-52,5,-14,
    24,-9,-154,-186,-82,-23,-62,-165,
    -174,-190,-368,-414,-316,-301,-180,41,
    116,214,319,408,416,157,-100,-40,
    118,248,310,301,302,387,458,414,
    301,261,233,111,33,39,65,56,
    9,-92,-87,-98,-172,-196,-186,-18,
    -14,-57,-111,-178,-278,-304,-358,-359,
    -362,-464,-528,-400,-355,-284,-189,-240,
    -253,-216,-319,-490,-621,-684,-758,-860,
    -883,-877,-847,-787,-766,-852,-727,-481,
    -339,-282,-266,-405,-414,-286,-225,-204,
    -330,-488,-412,-292,-254,-290,-372,-436,
    -545,-564,-413,-360,-344,-389,-430,-340,
    -248,-271,-343,-383,-414,-409,-272,-223,
    -215,-123,-10,-4,-6,-27,-11,78,
    169,226,139,-19,16,100,54,-75,
    -117,-103,-77,-277,-598,-644,-602,-509,
    -396,-232,-227,-208,-153,-146,-205,-223,
    -108,-55,-26,-8,-42,-178,-298,-320,
    -254,-146,-135,-262,-370,-331,-337,-394,
    -265,-53,136,309,354,312,345,303,
    275,338,287,269,346,329,319,327,
    199,118,251,296,243,111,90,150,
    104,163,274,278,242,135,93,138,
    5,-154,-206,-270,-334,-356,-251,-96,
    -78,-123,-80,-93,-160,-217,-214,-154,
    -42,128,228,243,307,465,492,425,
    381,382,425,530,518,484,560,654,
    659,663,723,717,672,652,542,507,
    471,468,579,573,459,313,262,310,
    284,235,331,361,275,207,104,35,
    35,89,136,192,218,161,89,64,
    116,175,159,95,96,242,350,248,
    170,64,-35,-136,-202,-271,-307,-290,
    -257,-219,-206,-185,-216,-213,-184,-135,
    -165,-141,-25,-31,-28,-98,-247,-162,
    10,35,-16,-113,-139,-127,-58,-100,
    -166,-320,-406,-462,-604,-594,-650,-538,
    -427,-365,-196,-117,-120,-102,-66,-122,
    -211,-235,-202,-135,-40,-10,-38,-150,
    -286,-223,-50,93,149,86,184,128,
    113,163,13,-53,-135,-100,-72,-75,
    -73,-118,-150,-197,-224,-131,-59,-109,
    -92,-129,-189,-220,-166,-173,-114,-8,
    26,-27,-38,50,109,143,161,209,
    266,289,384,397,312,203,5,-64,
    -14,6,56,67,19,-43,-112,-46,
    -74,-101,-83,-115,-142,-207,-274,-292,
    -299,-236,-181,-188,-48,60,6,-76,
    -8,115,188,260,236,143,44,-30,
    -17,31,37,-16,-28,87,210,276,
    372,365,302,270,137,-8,-142,-246,
    -279,-259,-203,-241,-278,-254,-245,-177,
    -77,-8,-47,-159,-295,-412,-414,-414,
    -566,-533,-255,-82,-10,222,358,336,
    355,360,303,237,267,224,244,434,
    422,372,404,464,559,538,446,294,
    217,60,-82,-150,-144,-162,-250,-263,
    -222,-148,-81,-134,-134,-106,-27,-71,
};

/* Golden log mel spec output for test wav. */
const std::vector<float> testWavMelSpec {
        -8.601085, -10.563560, -13.791912, -12.356619, -16.892878,
        -16.913876, -15.695299, -21.848980, -21.193371, -18.772688,
        -21.795116, -20.008236, -22.413673, -25.162649, -24.091856,
        -24.936411, -19.341146, -23.534576, -29.052885, -26.562546,
        -25.046455, -29.586889, -30.115177, -32.281334, -29.806450,
        -30.398304, -26.682615, -27.397421, -31.224312, -31.033779,
        -36.314369, -29.530331, -28.428139, -30.097546, -34.101303,
        -32.660480, -34.229076, -34.668293, -35.140759, -34.104649,
        -34.141472, -36.514408, -37.655891, -33.590931, -40.532566,
        -39.105091, -39.600319, -40.239834, -41.356224, -41.103714,
        -39.861557, -41.827553, -41.275696, -42.203575, -42.689217,
        -46.495552, -46.704731, -45.560322, -47.423828, -50.672031,
        -51.387669, -53.410839, -54.899536, -55.807552,
};


arm::app::audio::AdMelSpectrogram GetMelSpecInstance() {
    int frameLenSamples = 1024;
    return arm::app::audio::AdMelSpectrogram(frameLenSamples);
}

template <class T>
void TestQuntisedMelSpec() {
    float quantScale = 0.1410219967365265;
    int quantOffset = 11;
    std::vector<T> melSpecOutput = GetMelSpecInstance().MelSpecComputeQuant<T>(testWav1, quantScale, quantOffset);

    long min_val = std::numeric_limits<T>::min();
    long max_val = std::numeric_limits<T>::max();

    for (size_t i = 0; i < testWavMelSpec.size(); i++){
        long TestWavMelSpec = (std::lround((testWavMelSpec[i] / quantScale) + quantOffset));
        T quantizedTestWavMelSpec = static_cast<T>(std::max(min_val, std::min(TestWavMelSpec, max_val)));

        REQUIRE(quantizedTestWavMelSpec  == Approx(melSpecOutput[i]).margin(1));
    }
}

template void TestQuntisedMelSpec<int8_t>();
template void TestQuntisedMelSpec<uint8_t>();
template void TestQuntisedMelSpec<int16_t>();

TEST_CASE("Mel Spec calculation") {

    SECTION("FP32") {
        auto melSpecOutput = GetMelSpecInstance().ComputeMelSpec(testWav1);
        REQUIRE_THAT( melSpecOutput, Catch::Approx( testWavMelSpec ).margin(0.1) );
    }

    SECTION("int8_t") {
        TestQuntisedMelSpec<int8_t>();
    }

    SECTION("uint8_t") {
        TestQuntisedMelSpec<uint8_t>();
    }

    SECTION("int16_t") {
        TestQuntisedMelSpec<int16_t>();
    }
}
