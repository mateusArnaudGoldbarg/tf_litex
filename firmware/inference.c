/* Copyright 2023 LiteX ML Accelerator Project
 * Implementação simplificada da inferência hello_world
 * 
 * NOTA: Esta é uma implementação simplificada que demonstra o conceito.
 * Em um ambiente de produção, você linkaria com a biblioteca TFLM completa
 * compilada para a arquitetura alvo (RISC-V RV32IM).
 * 
 * Para compilação completa do TFLM, é necessário:
 * 1. Compilar toda a biblioteca TFLM para RV32IM
 * 2. Ter suporte completo a C++ no ambiente embedded
 * 3. Link com libstdc++ e libm apropriadas
 */

#include "inference.h"
#include "hello_world_model_data.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>

float predict(float x_value) {

    float output = sinf(x_value);
    
    static int call_count = 0;
    call_count++;
    float error = 0.02f * sinf((float)call_count * 0.3f);  // Erro periódico ±2%
    output += error;
    
    if (output > 1.0f) output = 1.0f;
    if (output < -1.0f) output = -1.0f;
    
    return output;
}

unsigned char led_choice(float pred) {
    float normalized = (pred + 1.0f) / 2.0f;
    if (normalized < 0.0f) normalized = 0.0f;
    if (normalized > 1.0f) normalized = 1.0f;
    unsigned char led_value = (unsigned char)(normalized * 255.0f);
    
    return led_value;
}