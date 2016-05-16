#ifndef _PS_INPUT_HEADER_
#define _PS_INPUT_HEADER_

#include "SharedShaderDefines.h"

SamplerState samplerLinear : register( s0 );
Texture2D txDiffuse : register( t0 );

#endif // _PS_INPUT_HEADER_