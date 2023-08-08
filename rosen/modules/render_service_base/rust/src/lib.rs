 /*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

//! Rust utils for cxx
#[cxx::bridge]
mod ffi{
    #![allow(dead_code)]
    extern "Rust"{
        //fn generate_random_value(min: f32, max: f32) -> f32;
        fn generate_value(start_value: f32, end_value: f32, start_time: i32, end_time: i32, current_time: i32) -> f32 ;
    }
}

// extern crate rand;
// use rand::Rng;

// fn generate_random_value(min: f32, max: f32) -> f32 {
//     let mut rng = rand::thread_rng();
//     rng.gen_range(min, max)
// }

fn generate_value(start_value: f32, end_value: f32, start_time: i32, end_time: i32, current_time: i32) -> f32 {
    let t = (current_time - start_time) as f32 / (end_time - start_time) as f32;
    let interpolation_value = start_value * (1.0 - t) + end_value * t;
    interpolation_value
}

