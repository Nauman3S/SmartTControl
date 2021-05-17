
#include "Controller.h"
#include "SoftwareStack.h"
#include "settings.h"
#include "intHandler.h"
#include "PIDController.h"
#include "zcdHandler.h"
#ifndef BUILTIN_LED
#define BUILTIN_LED  2  // backward compatibility
#endif

SoftwareStack ss;//SS instance

