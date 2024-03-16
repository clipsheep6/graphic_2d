/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef UTIL_H
#define UTIL_H
#include <time.h>  

// 初始化随机数生成器  
void init_random_seed() {  
    srand((unsigned int)time(NULL));  
}  
  
// 生成一个归一化的无符号浮点数，范围在 [0, 1)  
float next_uscalar1() {  
    // 生成一个无符号整数  
    unsigned int random_int = rand();  
      
    // 将无符号整数转换为浮点数并归一化  
    float normalized_float = (float)random_int / (float)RAND_MAX;  
      
    return normalized_float;  
} 

#endif // LOG_COMMON_H
