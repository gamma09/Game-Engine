// This file MUST use header guards instead of pragma once, as it is included by both HLSL and C++ files
#ifndef _SHARED_SHADER_DEFINES_H_
#define _SHARED_SHADER_DEFINES_H_

#ifdef _HLSL

#define CONSTANT_BUFFER_REGISTER( num ) b ## num
#define SRV_REGISTER( num ) t ## num
#define UAV_REGISTER( num ) u ## num
#define SAMPLER_REGISTER( num ) s ## num

#else

#define CONSTANT_BUFFER_REGISTER( num ) num
#define SRV_REGISTER( num ) num
#define UAV_REGISTER( num ) num
#define SAMPLER_REGISTER( num ) num

#endif

// Compute shader registers
#define CS_VIEW_PROJECTION_REGISTER  CONSTANT_BUFFER_REGISTER( 0 )
#define CS_HIERARCHY_REGISTER        CONSTANT_BUFFER_REGISTER( 1 )
#define CS_BIND_POSE_REGISTER        CONSTANT_BUFFER_REGISTER( 2 )
#define CS_MODEL_DATA_REGISTER       CONSTANT_BUFFER_REGISTER( 3 )

#define CS_PREV_FRAME_REGISTER    SRV_REGISTER( 0 )
#define CS_NEXT_FRAME_REGISTER    SRV_REGISTER( 1 )
#define CS_OUTPUT_BONES_REGISTER  UAV_REGISTER( 0 )

// Shared vertex shader registers
#define VS_INV_PROJECTION_REGISTER  CONSTANT_BUFFER_REGISTER( 0 )
#define VS_BONE_MATRICES_REGISTER   SRV_REGISTER( 0 )

// Lit texture vertex shader registers
#define VS_LIT_TEXTURE_LIGHT_REGISTER  CONSTANT_BUFFER_REGISTER( 1 )

// Shared pixel shader registers
#define PS_SAMPLER_REGISTER SAMPLER_REGISTER( 0 )
#define PS_DIFFUSE_TEXTURE_REGISTER SRV_REGISTER( 0 )


#define MAX_BONES 128


#endif