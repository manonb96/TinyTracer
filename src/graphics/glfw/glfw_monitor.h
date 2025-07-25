#pragma once
#include "../../utils/precomp.h"

struct GLFWmonitor;
struct GLFWwindow;

GLFWmonitor* GetPrimaryMonitor();
int2 GetMonitorPosition(gsl::not_null<GLFWmonitor*> monitor);
int2 GetMonitorWorkAreaSize(gsl::not_null<GLFWmonitor*> monitor);
void MoveWindowToMonitor(gsl::not_null<GLFWwindow*> window, gsl::not_null<GLFWmonitor*> monitor);