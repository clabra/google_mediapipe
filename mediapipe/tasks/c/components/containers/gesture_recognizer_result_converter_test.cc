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

#include "mediapipe/tasks/c/components/containers/gesture_recognizer_result_converter.h"

#include "mediapipe/framework/port/gtest.h"
#include "mediapipe/tasks/c/vision/gesture_recognizer/gesture_recognizer_result.h"
#include "mediapipe/tasks/cc/vision/gesture_recognizer/gesture_recognizer_result.h"

namespace mediapipe::tasks::c::components::containers {

TEST(GestureRecognizerResultConverterTest, ConvertsCustomResult) {
  ::mediapipe::tasks::vision::gesture_recognizer::GestureRecognizerResult
      cpp_result;

  // Initialize gestures
  Classification classification_for_gestures;
  classification_for_gestures.set_index(0);
  classification_for_gestures.set_score(0.9f);
  classification_for_gestures.set_label("gesture_label_1");
  classification_for_gestures.set_display_name("gesture_display_name_1");
  ClassificationList gestures_list;
  *gestures_list.add_classification() = classification_for_gestures;
  cpp_result.gestures.push_back(gestures_list);

  // Initialize handedness
  Classification classification_for_handedness;
  classification_for_handedness.set_index(1);
  classification_for_handedness.set_score(0.8f);
  classification_for_handedness.set_label("handeness_label_1");
  classification_for_handedness.set_display_name("handeness_display_name_1");
  ClassificationList handedness_list;
  *handedness_list.add_classification() = classification_for_handedness;
  cpp_result.handedness.push_back(handedness_list);

  // Initialize hand_landmarks
  NormalizedLandmark normalized_landmark;
  normalized_landmark.set_x(0.1f);
  normalized_landmark.set_y(0.2f);
  normalized_landmark.set_z(0.3f);
  NormalizedLandmarkList normalized_landmark_list;
  *normalized_landmark_list.add_landmark() = normalized_landmark;
  cpp_result.hand_landmarks.push_back(normalized_landmark_list);

  // Initialize hand_world_landmarks
  Landmark landmark;
  landmark.set_x(1.0f);
  landmark.set_y(1.1f);
  landmark.set_z(1.2f);

  LandmarkList landmark_list;
  *landmark_list.add_landmark() = landmark;
  cpp_result.hand_world_landmarks.push_back(landmark_list);

  GestureRecognizerResult c_result;
  CppConvertToGestureRecognizerResult(cpp_result, &c_result);

  // Verify conversion of gestures
  EXPECT_NE(c_result.gestures, nullptr);
  EXPECT_EQ(c_result.gestures_count, cpp_result.gestures.size());

  for (uint32_t i = 0; i < c_result.gestures_count; ++i) {
    EXPECT_EQ(c_result.gestures_categories_counts[i],
              cpp_result.gestures[i].classification_size());
    for (uint32_t j = 0; j < c_result.gestures_categories_counts[i]; ++j) {
      auto gesture = cpp_result.gestures[i].classification(j);
      EXPECT_EQ(std::string(c_result.gestures[i][j].category_name),
                gesture.label());
      EXPECT_FLOAT_EQ(c_result.gestures[i][j].score, gesture.score());
    }
  }

  // Verify conversion of hand_landmarks
  EXPECT_NE(c_result.hand_landmarks, nullptr);
  EXPECT_EQ(c_result.hand_landmarks_count, cpp_result.hand_landmarks.size());

  for (uint32_t i = 0; i < c_result.hand_landmarks_count; ++i) {
    EXPECT_EQ(c_result.hand_landmarks[i].landmarks_count,
              cpp_result.hand_landmarks[i].landmark_size());
    for (uint32_t j = 0; j < c_result.hand_landmarks[i].landmarks_count; ++j) {
      const auto& landmark = cpp_result.hand_landmarks[i].landmark(j);
      EXPECT_FLOAT_EQ(c_result.hand_landmarks[i].landmarks[j].x, landmark.x());
      EXPECT_FLOAT_EQ(c_result.hand_landmarks[i].landmarks[j].y, landmark.y());
      EXPECT_FLOAT_EQ(c_result.hand_landmarks[i].landmarks[j].z, landmark.z());
    }
  }

  // Verify conversion of hand_world_landmarks
  EXPECT_NE(c_result.hand_world_landmarks, nullptr);
  EXPECT_EQ(c_result.hand_world_landmarks_count,
            cpp_result.hand_world_landmarks.size());
  for (uint32_t i = 0; i < c_result.hand_world_landmarks_count; ++i) {
    EXPECT_EQ(c_result.hand_world_landmarks[i].landmarks_count,
              cpp_result.hand_world_landmarks[i].landmark_size());
    for (uint32_t j = 0; j < c_result.hand_world_landmarks[i].landmarks_count;
         ++j) {
      const auto& landmark = cpp_result.hand_world_landmarks[i].landmark(j);
      EXPECT_FLOAT_EQ(c_result.hand_world_landmarks[i].landmarks[j].x,
                      landmark.x());
      EXPECT_FLOAT_EQ(c_result.hand_world_landmarks[i].landmarks[j].y,
                      landmark.y());
      EXPECT_FLOAT_EQ(c_result.hand_world_landmarks[i].landmarks[j].z,
                      landmark.z());
    }
  }

  CppCloseGestureRecognizerResult(&c_result);
}

}  // namespace mediapipe::tasks::c::components::containers
