#pragma once

#ifndef stringify
#define stringify(x) __stringify(x)
#define __stringify(x) #x
#endif