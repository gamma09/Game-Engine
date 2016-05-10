#pragma once

#include <d3d11_1.h>
#include <GameAssert.h>

#define GameCheckFatalDx( expr, msg ) { HRESULT hr = (expr); GameCheckFatal( SUCCEEDED( hr ), (msg) ); }