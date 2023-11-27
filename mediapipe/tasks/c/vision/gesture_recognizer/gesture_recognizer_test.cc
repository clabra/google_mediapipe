/* Copyright 2023 The MediaPipe Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "mediapipe/tasks/c/vision/gesture_recognizer/gesture_recognizer.h"

#include <cstdint>
#include <cstdlib>
#include <string>

#include "absl/flags/flag.h"
#include "absl/strings/string_view.h"
#include "mediapipe/framework/deps/file_path.h"
#include "mediapipe/framework/formats/image.h"
#include "mediapipe/framework/port/gmock.h"
#include "mediapipe/framework/port/gtest.h"
#include "mediapipe/tasks/c/components/containers/landmark.h"
#include "mediapipe/tasks/cc/vision/utils/image_utils.h"

namespace {

using ::mediapipe::file::JoinPath;
using ::mediapipe::tasks::vision::DecodeImageFromFile;
using testing::HasSubstr;

constexpr char kTestDataDirectory[] = "/mediapipe/tasks/testdata/vision/";
constexpr char kModelName[] = "gesture_recognizer.task";
constexpr float kPrecision = 1e-4;
constexpr float kLandmarkPrecision = 1e-3;
constexpr int kIterations = 100;

std::string GetFullPath(absl::string_view file_name) {
  return JoinPath("./", kTestDataDirectory, file_name);
}

TEST(GestureRecognizerTest, ImageModeTest) {
  const auto image = DecodeImageFromFile(GetFullPath("fist.jpg"));
  ASSERT_TRUE(image.ok());

  const std::string model_path = GetFullPath(kModelName);
  GestureRecognizerOptions options = {
      /* base_options= */ {/* model_asset_buffer= */ nullptr,
                           /* model_asset_buffer_count= */ 0,
                           /* model_asset_path= */ model_path.c_str()},
      /* running_mode= */ RunningMode::IMAGE,
      /* num_hands= */ 1,
      /* min_hand_detection_confidence= */ 0.5,
      /* min_hand_presence_confidence= */ 0.5,
      /* min_tracking_confidence= */ 0.5,
      {/* display_names_locale= */ nullptr,
       /* max_results= */ -1,
       /* score_threshold= */ 0.0,
       /* category_allowlist= */ nullptr,
       /* category_allowlist_count= */ 0,
       /* category_denylist= */ nullptr,
       /* category_denylist_count= */ 0},
      {/* display_names_locale= */ nullptr,
       /* max_results= */ -1,
       /* score_threshold= */ 0.0,
       /* category_allowlist= */ nullptr,
       /* category_allowlist_count= */ 0,
       /* category_denylist= */ nullptr,
       /* category_denylist_count= */ 0}};

  void* recognizer =
      gesture_recognizer_create(&options, /* error_msg */ nullptr);
  EXPECT_NE(recognizer, nullptr);

  const auto& image_frame = image->GetImageFrameSharedPtr();
  const MpImage mp_image = {
      .type = MpImage::IMAGE_FRAME,
      .image_frame = {.format = static_cast<ImageFormat>(image_frame->Format()),
                      .image_buffer = image_frame->PixelData(),
                      .width = image_frame->Width(),
                      .height = image_frame->Height()}};

  GestureRecognizerResult result;
  gesture_recognizer_recognize_image(recognizer, &mp_image, &result,
                                     /* error_msg */ nullptr);

  // Expects to have the same number of hands detected.
  EXPECT_EQ(result.gestures_count, 1);
  EXPECT_EQ(result.handedness_count, 1);
  // Actual gesture with top score matches expected gesture.
  EXPECT_EQ(std::string{result.gestures[0][0].category_name}, "Closed_Fist");
  EXPECT_NEAR(result.gestures[0][0].score, 0.9000f, kPrecision);

  // Actual handedness matches expected handedness.
  EXPECT_EQ(std::string{result.handedness[0][0].category_name}, "Right");
  EXPECT_NEAR(result.handedness[0][0].score, 0.9893f, kPrecision);

  // Actual landmarks match expected landmarks.
  EXPECT_NEAR(result.hand_landmarks[0].landmarks[0].x, 0.477f,
              kLandmarkPrecision);
  EXPECT_NEAR(result.hand_landmarks[0].landmarks[0].y, 0.661f,
              kLandmarkPrecision);
  EXPECT_NEAR(result.hand_landmarks[0].landmarks[0].z, 0.0f,
              kLandmarkPrecision);
  EXPECT_NEAR(result.hand_world_landmarks[0].landmarks[0].x, -0.009f,
              kLandmarkPrecision);
  EXPECT_NEAR(result.hand_world_landmarks[0].landmarks[0].y, 0.082f,
              kLandmarkPrecision);
  EXPECT_NEAR(result.hand_world_landmarks[0].landmarks[0].z, 0.006f,
              kLandmarkPrecision);

  gesture_recognizer_close_result(&result);
  gesture_recognizer_close(recognizer, /* error_msg */ nullptr);
}

// TODO other tests

}  // namespace
