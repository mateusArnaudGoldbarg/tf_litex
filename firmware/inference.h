/* Copyright 2023 LiteX ML Accelerator Project
 * Implementação da inferência TensorFlow Lite Micro para hello_world
 */

#ifndef INFERENCE_H_
#define INFERENCE_H_

#ifdef __cplusplus
extern "C" {
#endif

float predict(float x_value);

unsigned char led_choice(float output_value);

#ifdef __cplusplus
}
#endif

#endif  // INFERENCE_H_