/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
#include "UseCaseHandler.hpp"
#include "InputFiles.hpp"
#include "YoloFastestModel.hpp"
#include "UseCaseCommonUtils.hpp"
#include "DetectorPostProcessing.hpp"
#include "hal.h"

#include <inttypes.h>

namespace arm {
namespace app {

    /**
     * @brief           Presents inference results along using the data presentation
     *                  object.
     * @param[in]       platform           Reference to the hal platform object.
     * @param[in]       results            Vector of detection results to be displayed.
     * @return          true if successful, false otherwise.
     **/
    static bool PresentInferenceResult(hal_platform& platform,
                                    const std::vector<arm::app::object_detection::DetectionResult>& results);

    /* Object detection classification handler. */
    bool ObjectDetectionHandler(ApplicationContext& ctx, uint32_t imgIndex, bool runAll)
    {
        auto& platform = ctx.Get<hal_platform&>("platform");
        auto& profiler = ctx.Get<Profiler&>("profiler");

        constexpr uint32_t dataPsnImgDownscaleFactor = 1;
        constexpr uint32_t dataPsnImgStartX = 10;
        constexpr uint32_t dataPsnImgStartY = 35;

        constexpr uint32_t dataPsnTxtInfStartX = 150;
        constexpr uint32_t dataPsnTxtInfStartY = 40;

        platform.data_psn->clear(COLOR_BLACK);

        auto& model = ctx.Get<Model&>("model");

        /* If the request has a valid size, set the image index. */
        if (imgIndex < NUMBER_OF_FILES) {
            if (!SetAppCtxIfmIdx(ctx, imgIndex, "imgIndex")) {
                return false;
            }
        }
        if (!model.IsInited()) {
            printf_err("Model is not initialised! Terminating processing.\n");
            return false;
        }

        auto curImIdx = ctx.Get<uint32_t>("imgIndex");

        TfLiteTensor* inputTensor = model.GetInputTensor(0);

        if (!inputTensor->dims) {
            printf_err("Invalid input tensor dims\n");
            return false;
        } else if (inputTensor->dims->size < 3) {
            printf_err("Input tensor dimension should be >= 3\n");
            return false;
        }

        TfLiteIntArray* inputShape = model.GetInputShape(0);

        const uint32_t nCols = inputShape->data[arm::app::YoloFastestModel::ms_inputColsIdx];
        const uint32_t nRows = inputShape->data[arm::app::YoloFastestModel::ms_inputRowsIdx];
        const uint32_t nPresentationChannels = channelsImageDisplayed;

        /* Get pre/post-processing objects. */
        auto& postp = ctx.Get<object_detection::DetectorPostprocessing&>("postprocess");

        do {
            /* Strings for presentation/logging. */
            std::string str_inf{"Running inference... "};

            const uint8_t* curr_image = get_img_array(ctx.Get<uint32_t>("imgIndex"));

            /* Copy over the data and convert to grayscale */
            auto* dstPtr = static_cast<uint8_t*>(inputTensor->data.uint8);
            const size_t copySz = inputTensor->bytes < IMAGE_DATA_SIZE ?
                                inputTensor->bytes : IMAGE_DATA_SIZE;

            /* Copy of the image used for presentation, original images are read-only */
            std::vector<uint8_t> g_image_buffer(nCols*nRows*channelsImageDisplayed);
            if (nPresentationChannels == 3) {
                memcpy(g_image_buffer.data(),curr_image, nCols * nRows * channelsImageDisplayed);
            } else {
                image::RgbToGrayscale(curr_image, g_image_buffer.data(), nCols * nRows);
            }
            image::RgbToGrayscale(curr_image, dstPtr, copySz);

            /* Display this image on the LCD. */
            platform.data_psn->present_data_image(
                g_image_buffer.data(),
                nCols, nRows, nPresentationChannels,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

            /* If the data is signed. */
            if (model.IsDataSigned()) {
                image::ConvertImgToInt8(inputTensor->data.data, inputTensor->bytes);
            }

            /* Display message on the LCD - inference running. */
            platform.data_psn->present_data_text(str_inf.c_str(), str_inf.size(),
                                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            /* Run inference over this image. */
            info("Running inference on image %" PRIu32 " => %s\n", ctx.Get<uint32_t>("imgIndex"),
                get_filename(ctx.Get<uint32_t>("imgIndex")));

            if (!RunInference(model, profiler)) {
                return false;
            }

            /* Erase. */
            str_inf = std::string(str_inf.size(), ' ');
            platform.data_psn->present_data_text(str_inf.c_str(), str_inf.size(),
                                    dataPsnTxtInfStartX, dataPsnTxtInfStartY, 0);

            /* Detector post-processing*/
            std::vector<object_detection::DetectionResult> results;
            TfLiteTensor* modelOutput0 = model.GetOutputTensor(0);
            TfLiteTensor* modelOutput1 = model.GetOutputTensor(1);
            postp.RunPostProcessing(
                g_image_buffer.data(),
                nRows,
                nCols,
                modelOutput0,
                modelOutput1,
                results);

            platform.data_psn->present_data_image(
                g_image_buffer.data(),
                nCols, nRows, nPresentationChannels,
                dataPsnImgStartX, dataPsnImgStartY, dataPsnImgDownscaleFactor);

#if VERIFY_TEST_OUTPUT
            arm::app::DumpTensor(outputTensor);
#endif /* VERIFY_TEST_OUTPUT */

            if (!PresentInferenceResult(platform, results)) {
                return false;
            }

            profiler.PrintProfilingResult();

            IncrementAppCtxIfmIdx(ctx,"imgIndex");

        } while (runAll && ctx.Get<uint32_t>("imgIndex") != curImIdx);

        return true;
    }


    static bool PresentInferenceResult(hal_platform& platform,
                                       const std::vector<arm::app::object_detection::DetectionResult>& results)
    {
        platform.data_psn->set_text_color(COLOR_GREEN);

        /* If profiling is enabled, and the time is valid. */
        info("Final results:\n");
        info("Total number of inferences: 1\n");

        for (uint32_t i = 0; i < results.size(); ++i) {
            info("%" PRIu32 ") (%f) -> %s {x=%d,y=%d,w=%d,h=%d}\n", i,
                results[i].m_normalisedVal, "Detection box:",
                results[i].m_x0, results[i].m_y0, results[i].m_w, results[i].m_h );
        }

        return true;
    }

} /* namespace app */
} /* namespace arm */
