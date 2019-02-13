#pragma once

#ifdef SNOWFALLENGINE_EXPORTS
#define SNOWFALLENGINE_API __declspec(dllexport)
#else
#define SNOWFALLENGINE_API __declspec(dllimport)
#endif